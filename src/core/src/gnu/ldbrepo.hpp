/*
 * PipeDB
 * 
 * Copyright (C) 2014 Jean-Manuel CABA
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *      http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#pragma once

#include "blokrepository.h"

#include <leveldb/db.h>
#include <leveldb/slice.h>
#include <leveldb/status.h>
#include <leveldb/filter_policy.h>
#include <leveldb/write_batch.h>
#include <leveldb/cache.h>

namespace ibs {

class LdbRepo: public BlockRepository {
    public:
        LdbRepo(const std::string& path) :
        BlockRepository(), 
        hasCache(createCache), 
        hasBloomFilter(createBloomFilter), 
        isOpen(false), 
        db(), 
        cache(), 
        filter(), 
        writeOptions(), 
        readOptions(), 
        options(), 
        leveldbPath(path), 
        isCompacting(false) {
            options.create_if_missing = true;
            options.compression = leveldb::kSnappyCompression;
            // improve read performance using a cache
            assert(isOpen == false);
            cache.reset(leveldb::NewLRUCache(64 << 20));
            assert(NULL != cache.get());
            options.block_cache = cache.get();
            this->setCacheSize(cacheSize);
            // Use a bloom filters to reduce the disk lookups.
            filter.reset(leveldb::NewBloomFilterPolicy(100));
            assert(NULL != filter.get());
            options.filter_policy = filter.get();
            options.write_buffer_size = 64 << 20;
        }

        virtual ~LdbRepo() {
            // release allocated memory
            options.block_cache = NULL;
            options.filter_policy = NULL;
            close();
            db.reset();
            cache.reset();
            filter.reset();
        }

        virtual bool opened() {
            return isOpen;
        }

        virtual Return close() {
            // delete the associated leveldb instance to close
            isOpen = false;
            db.reset();
            isCompacting = false;
            return Return::OK;
        }

        virtual bool closed() {
            return ! isOpen;
        }

        virtual StatusCode open() {
            if (isOpen.exchange(true) == true) {
                return StatusCode::NotSupported();
            }
            else {
                leveldb::DB* dbToAllocate = NULL;
                leveldb::Status status;
                leveldb::DB::Open(options, leveldbPath, &dbToAllocate);

                if (status.ok() == false) {
                    if (dbToAllocate != NULL) {
                        delete dbToAllocate;
                        dbToAllocate = NULL;
                    }
                    close();
                    return fromStatus(status);
                }
                else {
                    assert(status.ok());
                    if (dbToAllocate == NULL) {
                        close();
                        return StatusCode::NotSupported();
                    }
                    // pass the memory to the class
                    // in the smart pointer
                    db.reset(dbToAllocate);
                    return StatusCode::OK();
                }
            }
        }

        virtual void close() {
            ;
        }

        virtual bool isClosed() {
            return !isOpen;
        }

        virtual Return put(const Key&& key, const InputBlock&& value) {
            if (isOpen) {
                return fromStatus(db->Put(wOptions, toSlice(key), toSlice(value)));
            }
            else {
                return StatusCode::NotSupported();
            }
        }

        virtual Return get(const Key&& key, OutputBlock& value) {
            if (isOpen) {
                return fromStatus(db->Get(rOptions, toSlice(key), value));
            }
            else {
                return StatusCode::NotSupported();
            }
        }

        virtual Return drop(const Key&& key) {
            if (isOpen) {
                return fromStatus(db->Delete(wOptions, toSlice(key)));
            }
            else {
                return StatusCode::NotSupported();
            }
        }

        virtual bool included(const Key&& key) {
            if (isOpen) {
                std::string value;
                leveldb::Status status = db->Get(rOptions, toSlice(key), &value);
                return status.ok();
            }
            else {
                return false;
            }
        }

        virtual bool excluded(const Key&& key) {
            return ! included(key);
        }

        virtual Return erase()  {
            if (isOpen) {
                return StatusCode::NotSupported();
            }
            else {
                leveldb::Status status;
                status = leveldb::DestroyDB(leveldbPath, options);
                FileTools::removeDirectory(leveldbPath);
                return fromStatus(status);
            }
        }

    private:
        std::atomic<bool> isOpen; /** If the database is open */
        std::unique_ptr<leveldb::DB> db; /** LevelDb instance. */
        std::unique_ptr<leveldb::Cache> cache; /** LevelDb read block cache */
        std::unique_ptr<const leveldb::FilterPolicy> filter; /** LevelDb filter */
        leveldb::WriteOptions wOptions; /** Default write options. */
        leveldb::ReadOptions rOptions; /** Default read options. */
        leveldb::Options options; /** Leveldb options. Used at initialization time. */
        const std::string leveldbPath; /** file system location for the levelDB database. */
        std::atomic<bool> isCompacting; /** If manual compaction is pending */
};

} /* namespace ibs */

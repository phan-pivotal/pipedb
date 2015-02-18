/*
 * PipeDB
 * 
 * Copyright (C) 2014-2015 Jean-Manuel CABA
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

#include "blockrepository.hpp"
#include "tools.hpp"

#include <leveldb/db.h>
#include <leveldb/slice.h>
#include <leveldb/status.h>
#include <leveldb/filter_policy.h>
#include <leveldb/write_batch.h>
#include <leveldb/cache.h>

namespace pipedb {

class LdbRepo: public BlockRepository {
    public:
        LdbRepo(const std::string& path) :
        BlockRepository(), 
        isOpen(false), 
        db(), 
        cache(), 
        filter(), 
        writeOptions(), 
        readOptions(), 
        options(), 
        leveldbPath(path) {
            options.create_if_missing = true;
            options.compression = leveldb::kSnappyCompression;
            // improve read performance using a cache
            assert(isOpen == false);
            cache.reset(leveldb::NewLRUCache(64 << 20));
            assert(NULL != cache.get());
            options.block_cache = cache.get();
            // use a bloom filters to reduce the disk lookups.
            filter.reset(leveldb::NewBloomFilterPolicy(100));
            assert(NULL != filter.get());
            options.filter_policy = filter.get();
	    // improve write performance using a write buffer
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

        virtual bool closed() {
            return ! isOpen;
        }

        virtual Return close() {
            // delete the associated leveldb instance to close
            isOpen = false;
            db.reset();
            return Return::OK;
        }

        virtual Return open() {
            if (isOpen.exchange(true) == true) {
                return Return::NOT_SUPPORTED;
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
                        return Return::NOT_SUPPORTED;
                    }
                    // pass the memory to the class
                    // in the smart pointer
                    db.reset(dbToAllocate);
                    return Return::OK;
                }
            }
        }

        virtual Return put(const Key&& key, const InputBlock&& value) {
            if (isOpen) {
	      return fromStatus(db->Put(writeOptions, toSlice(std::move(key)), toSlice(std::move(value))));
            }
            else {
                return Return::NOT_SUPPORTED;
            }
        }

        virtual Return get(const Key&& key, OutputBlock& value) {
            if (isOpen) {
	      std::string data;
	      Return state = fromStatus(db->Get(readOptions, toSlice(std::move(key)), &data));
	      if(state.success()) {
		value.set_data(data);
	      }
	      return state;
            }
            else {
                return Return::NOT_SUPPORTED;
            }
        }

        virtual Return drop(const Key&& key) {
            if (isOpen) {
	      return fromStatus(db->Delete(writeOptions, toSlice(std::move(key))));
            }
            else {
                return Return::NOT_SUPPORTED;
            }
        }

        virtual bool included(const Key&& key) {
            if (isOpen) {
                std::string value;
                leveldb::Status status = db->Get(readOptions, toSlice(std::move(key)), &value);
                return status.ok();
            }
            else {
                return false;
            }
        }

        virtual bool excluded(const Key&& key) {
	  return isOpen && ! included(std::move(key));
        }

        virtual Return erase()  {
            if (isOpen) {
                return Return::NOT_SUPPORTED;
            }
            else {
                leveldb::Status status;
                status = leveldb::DestroyDB(leveldbPath, options);
                Tools::remove_all(leveldbPath);
                return fromStatus(status);
            }
        }
    protected:
  
        Return fromStatus(const leveldb::Status& st) noexcept {
	  if (st.ok()) {
	    return Return::OK;
	  }
	  else if (st.IsCorruption()) {
	    return Return::BACKEND_ERROR;
	  }
	  else if (st.IsIOError()) {
	    return Return::BACKEND_ERROR;
	  }
	  else if (st.IsNotFound()) {
	    return Return::KEY_NOT_PRESENT;
	  }
	  else {
	    return Return::INTERNAL_ERROR;
	  }
	}

        leveldb::Slice toSlice(const Chunk* b) noexcept {
	  assert(b != NULL);
	  return leveldb::Slice(b->get_data(), b->get_size());
	}

        leveldb::Slice toSlice(const Key&& b) noexcept {
	  return leveldb::Slice(b.get_data(), b.get_size());
	}

        leveldb::Slice toSlice(const InputBlock&& b) noexcept {
	  return leveldb::Slice(b.get_data(), b.get_size());
	}

        leveldb::Slice toSlice(const OutputBlock&& b) noexcept {
	  return leveldb::Slice(b.get_data(), b.get_size());
	}
  
    private:
        std::atomic<bool> isOpen; /** If the database is open */
        std::unique_ptr<leveldb::DB> db; /** LevelDb instance. */
        std::unique_ptr<leveldb::Cache> cache; /** LevelDb read block cache */
        std::unique_ptr<const leveldb::FilterPolicy> filter; /** LevelDb filter */
        leveldb::WriteOptions writeOptions; /** Default write options. */
        leveldb::ReadOptions readOptions; /** Default read options. */
        leveldb::Options options; /** Leveldb options. Used at initialization time. */
        const std::string leveldbPath; /** file system location for the levelDB database. */
};

} /* namespace ibs */

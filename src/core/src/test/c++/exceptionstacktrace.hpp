#pragma once

#include <sys/types.h>
#include <execinfo.h>
#include <signal.h>
#include <exception>
#include <iostream>
#include <map>
#include <string>
#include <fstream>

namespace pipedb_testing {

// Exception handling back_trace like the following IBM article :
// http://www.ibm.com/developerworks/library/l-cppexcep/index.html

// Class to register a signal handler and translate the signal to a C++ exception
template<class SignalExceptionClass> class SignalTranslator {
    public:
        SignalTranslator() {
            signal(SignalExceptionClass::GetSignalNumber(), SignalHandler);
        }

        static void SignalHandler(int) {
            throw SignalExceptionClass();
        }
};

class ExceptionTracer {
    public:
        ExceptionTracer() {
            void* array[25];
            int nSize = backtrace(array, 25);
            char** symbols = backtrace_symbols(array, nSize);

            for (int i = 0; i < nSize; i++) {
              std::cerr << symbols[i] << std::endl;
            }

            free(symbols);
        }
    private:
        static bool redTrace;
};

class SegmentationFaultException: public ExceptionTracer, public std::exception {
    public:
        static int GetSignalNumber() {
            return SIGSEGV;
        }
};

class BusErrorException: public ExceptionTracer, public std::exception {
    public:
        static int GetSignalNumber() {
            return SIGBUS;
        }
};

class AbortException: public ExceptionTracer, public std::exception {
    public:
        static int GetSignalNumber() {
            return SIGABRT;
        }
};

class FloatingPointException: public ExceptionTracer, public std::exception {
    public:
        static int GetSignalNumber() {
            return SIGFPE;
        }
};

extern SignalTranslator<SegmentationFaultException> g_objSegmentationFaultTranslator;
extern SignalTranslator<AbortException> g_objAbortTranslator;
extern SignalTranslator<FloatingPointException> g_objFPETranslator;

class ExceptionHandler {
    public:
        ExceptionHandler() {
            std::set_terminate(Handler);
        }

        static void Handler();
};

extern ExceptionHandler g_objHandler;

}

SignalTranslator<SegmentationFaultException> g_objSegmentationFaultTranslator;
SignalTranslator<BusErrorException> g_objBusErrorTranslator;
SignalTranslator<AbortException> g_objAbortTranslator;
SignalTranslator<FloatingPointException> g_objFPETranslator;

ExceptionHandler g_objHandler;

void ExceptionHandler::Handler() {
    try {
        throw;
    }
    catch (SegmentationFaultException &) {
      std::cerr << "Segmentation fault not treated in tests" << std::endl;
    }
    catch (AbortException &) {
      std::cerr << "Abort not treated in tests" << std::endl;
    }
    exit(-1);
}

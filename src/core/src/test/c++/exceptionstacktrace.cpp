#include "exceptionTracer.h"
#include <fstream>

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

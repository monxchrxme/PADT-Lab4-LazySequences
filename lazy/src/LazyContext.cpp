#include "LazyContext.hpp"

LazyContext::LazyContext() {
}

LazyContext::~LazyContext() {
    for (int i = 0; i < allocatedObjects.get_length(); ++i) { 
        delete allocatedObjects.get(i);
    }
}
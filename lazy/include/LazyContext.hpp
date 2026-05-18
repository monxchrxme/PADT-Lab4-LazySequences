#pragma once

#include "IAllocatedObject.hpp"
#include "mutable_array_sequence.hpp" 
#include <utility>

class LazyContext {
private:
    MutableArraySequence<IAllocatedObject*> allocatedObjects;

public:
    LazyContext();
    
    // Запрещаем копирование контекста
    LazyContext(const LazyContext&) = delete;
    LazyContext& operator=(const LazyContext&) = delete;

    ~LazyContext();

    template<typename T, typename... Args>
    T* Allocate(Args&&... args) {
        // Perfect Forwarding для передачи без копирования 
        T* newObj = new T(std::forward<Args>(args)...);
        
        allocatedObjects.append(newObj);
        
        return newObj;
    }
};
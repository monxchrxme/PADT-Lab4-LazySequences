#pragma once

// Базовый класс для всех объектов, управляемых LazyContext
class IAllocatedObject {
public:
    virtual ~IAllocatedObject() = default;
};
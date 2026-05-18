#include <gtest/gtest.h>
#include "LazyContext.hpp"

// Глобальный счетчик для отслеживания удаленных объектов
static int g_destroyed_count = 0;

class MemorySpy : public IAllocatedObject {
public:
    MemorySpy() {}
    ~MemorySpy() override {
        g_destroyed_count++;
    }
};

TEST(LazyContextTest, MemoryCleanup) {
    g_destroyed_count = 0; 

    {
        // Создаем контекст (симулируем область видимости функции)
        LazyContext ctx;

        ctx.Allocate<MemorySpy>();
        ctx.Allocate<MemorySpy>();
        ctx.Allocate<MemorySpy>();

        EXPECT_EQ(g_destroyed_count, 0);
    } 
    // Здесь ctx выходит из области видимости, вызывается его деструктор
    // Контекст должен сам удалить все 3 объекта MemorySpy

    EXPECT_EQ(g_destroyed_count, 3);
}

// Тестируем, что фабрика возвращает рабочие указатели
class DummyData : public IAllocatedObject {
public:
    int value;
    DummyData(int v) : value(v) {}
};

TEST(LazyContextTest, AllocationReturnsValidPointer) {
    LazyContext ctx;
    DummyData* data = ctx.Allocate<DummyData>(42);
    
    ASSERT_NE(data, nullptr);
    EXPECT_EQ(data->value, 42);
}
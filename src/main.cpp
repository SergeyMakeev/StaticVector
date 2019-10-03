#include <new>
#include "staticvector.h"

static const int kNumberOfElements = 64;
static const int kNumberOfElementsForTest1 = 128;
static const int kNumberOfPassesForTest2 = 8192;


static int numberOfAllocations = 0;


void* operator new(std::size_t sz)
{
    numberOfAllocations++;
    void *ptr = std::malloc(sz);
    if (ptr)
        return ptr;
    else
        throw std::bad_alloc{};
}

void operator delete(void* ptr) noexcept
{
    std::free(ptr);
}

struct CustomData
{
    int value;

    explicit CustomData(int v)
        : value(v)
    {
    }
};


template<typename T>
int test1(T& container)
{
    assert(container.size() == 0);

    // test
    numberOfAllocations = 0;
    for (int i = 0; i < kNumberOfElementsForTest1; i++)
    {
        container.emplace_back(i);
    }

    // validation
    for (int i = 0; i < kNumberOfElementsForTest1; i++)
    {
        assert(container[i].value == i);
    }

    return numberOfAllocations;
}

template<typename T>
int test3(T& container)
{
    assert(container.size() == 0);

    // test
    numberOfAllocations = 0;
    for (int i = 0; i < kNumberOfElements; i++)
    {
        container.emplace_back(i);
    }

    // validation
    for (int i = 0; i < kNumberOfElements; i++)
    {
        assert(container[i].value == i);
    }

    return numberOfAllocations;
}


template<typename T>
int test2(T& container)
{
    static const int kNumberOfPreallocatedElements = kNumberOfElements - 2;

    assert(container.size() == 0);

    // test
    numberOfAllocations = 0;
    for (int i = 0; i < kNumberOfPreallocatedElements; i++)
    {
        container.emplace_back(i);
    }

    // intentionally add and remove 4 elements to cross the boundary between static/dynamic
    for (int i = kNumberOfElements; i < kNumberOfPassesForTest2; i++)
    {
        container.emplace_back(i);
        container.emplace_back(i);
        container.emplace_back(-i);
        container.emplace_back(-i);
        container.erase(container.begin());
        container.erase(container.begin());
        container.erase(container.begin());
        container.erase(container.begin());
    }

    // validation
    assert(container.size() == kNumberOfPreallocatedElements);

    int expectedValue = (kNumberOfPassesForTest2 - 1);
    for (int i = 0; i < kNumberOfPreallocatedElements; i+=4)
    {
        int idx = (int)container.size() - 1 - i;

        if (idx < 0)
            break;
        assert(container[idx].value == -expectedValue);

        if ((idx - 1) < 0)
            break;
        assert(container[idx - 1].value == -expectedValue);

        if ((idx - 2) < 0)
            break;
        assert(container[idx - 2].value == expectedValue);

        if ((idx - 3) < 0)
            break;
        assert(container[idx - 3].value == expectedValue);

        expectedValue--;
    }

    return numberOfAllocations;
}


int main()
{
    int allocCount = 0;

    // Info + Test 1
    {
        std::vector<CustomData> testStandard;
        stdext::static_vector<CustomData, kNumberOfElements> testStatic;

        const int sizeOfStandard = sizeof(testStandard);
        const int sizeOfStatic = sizeof(testStatic);
        printf("sizeof(std::vector<int>) = %d bytes\n", sizeOfStandard);
        printf("sizeof(stdext::static_vector<int, %d>) = %d bytes, stoarge = %d bytes\n", kNumberOfElements, sizeOfStatic, (int)(sizeof(CustomData) * kNumberOfElements));
        printf("Overhead from the static_vector = %d bytes + embedded storage size %d bytes\n", (int)(sizeOfStatic - sizeOfStandard - sizeof(CustomData) * kNumberOfElements), (int)(sizeof(CustomData) * kNumberOfElements));

        printf("\nTest1\n");
        printf("--------------\n");

        allocCount = test1(testStandard);
        printf("Standard: Number of allocations for %d insertions = %d\n", kNumberOfElementsForTest1, allocCount);

        allocCount = test1(testStatic);
        printf("Static: Number of allocations for %d insertions = %d\n", kNumberOfElementsForTest1, numberOfAllocations);
    }

    // Test 2
    {
        printf("\nTest2\n");
        printf("--------------\n");

        std::vector<CustomData> testStandard2;
        stdext::static_vector<CustomData, kNumberOfElements> testStatic2;

        allocCount = test2(testStandard2);
        printf("Standard: Number of allocations %d\n", allocCount);

        allocCount = test2(testStatic2);
        printf("Static: Number of allocations %d\n", allocCount);

    }

    // Test 3
    {
        printf("\nTest3\n");
        printf("--------------\n");

        std::vector<CustomData> testStandard3;
        stdext::static_vector<CustomData, kNumberOfElements> testStatic3;

        allocCount = test3(testStandard3);
        printf("Standard: Number of allocations %d\n", allocCount);

        allocCount = test3(testStatic3);
        printf("Static: Number of allocations %d\n", allocCount);

    }

    return 0;
}
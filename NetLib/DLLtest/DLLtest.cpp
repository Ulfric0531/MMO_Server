#include "pch.h"
#include "Export.h"
#pragma comment(lib, "winmm.lib")
#include <vector>
#include <timeapi.h>

#define MASSIVE_COUNT (100000000)
#define THREAD_COUNT (8)

void PoolMassiveAllocTest();

void PoolMassiveDeleteTest();

unsigned int PoolRandomTest(void* ptr);

void HeapMassiveAllocTest();

void HeapMassiveDeleteTest();

int HeapRandomTest();

struct result_t
{
    int count;
};

struct A
{
    int a;
    int b;
    int c;
    int d;
    int e;

};

struct B
{
    double a;
    int b;
    int c;
};

using namespace std;

vector<A*> v1;
vector<A*> v2;

int g_Pattern[1024] = { 0, };


HANDLE g_ThreadArgs[THREAD_COUNT];
HANDLE g_CompleteEventList[THREAD_COUNT];
result_t g_Result[THREAD_COUNT];
int g_Id[THREAD_COUNT];

int main()
{
    srand(1);
    for (int i = 0; i < 1024; ++i)
    {
        g_Pattern[i] = rand() % 2;
    }

    for (int i = 0; i < THREAD_COUNT; ++i)
    {
        unsigned int threadId = 0;
        g_CompleteEventList[i] = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        g_Id[i] = i;
        g_ThreadArgs[i] = (HANDLE)_beginthreadex(nullptr, 0, PoolRandomTest, g_Id + i, 0, &threadId);

    }

    WaitForMultipleObjects(THREAD_COUNT, g_CompleteEventList, TRUE, INFINITE);

    int result = 0;
    for (int i = 0; i < THREAD_COUNT; ++i)
    {
        result += g_Result[i].count;
    }

    cout << result << endl;
}

void PoolMassiveAllocTest()
{
    for (int i = 0; i < MASSIVE_COUNT; ++i)
    {
        A* element = (A*)Allocate(sizeof(A));
        v1.push_back(element);
    }
}

void PoolMassiveDeleteTest()
{
    for (int i = 0; i < MASSIVE_COUNT; ++i)
    {
        A* element = v1.back();
        v1.pop_back();
        Release(element);
    }
}

unsigned int PoolRandomTest(void* ptr)
{
    vector<A*> vec1;
    int id = *(int*)ptr;
    long start = timeGetTime();
    int count = 0;
    int patternIndex = 0;
    while (timeGetTime() - start <= 10000)
    //while (true)
    {
        int ptn = g_Pattern[patternIndex % 1024];
        if (ptn == 0)
        {
            A* element = (A*)Allocate(sizeof(A));
            vec1.push_back(element);
            ++count;
            ++patternIndex;
        }
        else
        {
            if (vec1.empty())
            {
                ++patternIndex;
                continue;
            }
            A* element = vec1.back();
            vec1.pop_back();
            Release(element);
            ++count;
            ++patternIndex;
        }
    }
    g_Result[id].count = count;
    SetEvent(g_CompleteEventList[id]);

    return 0;
}


void HeapMassiveAllocTest()
{
    for (int i = 0; i < MASSIVE_COUNT; ++i)
    {
        A* element = new A;
        v1.push_back(element);
    }
}

void HeapMassiveDeleteTest()
{
    for (int i = 0; i < MASSIVE_COUNT; ++i)
    {
        A* element = v1.back();
        v1.pop_back();
        delete element;
    }
}

int HeapRandomTest()
{
    return 0;
}


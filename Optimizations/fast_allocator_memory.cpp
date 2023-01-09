#define fast_allocator_memory 60'000'000

#ifdef fast_allocator_memory

int allocatorPos = 0;
char allocatorMemory[int(fast_allocator_memory)];

inline void* operator new (size_t n) {
    char* res = allocatorMemory + allocatorPos;
    allocatorPos += n;
    return (void*)res;
}

inline void operator delete (void*) noexcept {}

#endif // fast_allocator_memory

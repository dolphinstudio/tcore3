#ifndef __blockallocator_h__
#define __blockallocator_h__

#include <cstdint>
#include <string>
#include <unordered_map>

namespace tlib {
    class blockallocator {
        static const int kChunkSize = 16 * 1024;
        static const int kMaxBlockSize = 640;
        static const int kBlockSizes = 14;
        static const int kChunkArrayIncrement = 128;

    public:
        blockallocator();
        ~blockallocator();

    public:
        void* allocate(int size, const char * file, const int line);
        void Free(void *p, int size, const char * file, const int line);
        void free(void *p, int size);
        void clear();
        void clearfree();

    private:
        std::unordered_map<void *, int> _allocamap;
        int             _alloc_size_record;
        int             num_chunk_count_;
        int             num_chunk_space_;
        struct Chunk*   chunks_;
        struct Block*   free_lists_[kBlockSizes];
        static int      block_sizes_[kBlockSizes];
        static uint8_t  s_block_size_lookup_[kMaxBlockSize + 1];
        static bool     s_block_size_lookup_initialized_;
    };

#define baallocate(ba, size) ba->allocate(size, __FILE__, __LINE__)
#define bafree(ba, p, size) ba->Free(p, size, __FILE__, __LINE__)
}

#endif
#ifndef __tpool_h__
#define __tpool_h__

#include "tools.h"
#include <list>
#include <algorithm>
#ifdef _DEBUG
#include <map>
#endif //_DEBUG
using namespace std;

namespace tlib {
    template <typename T, s32 chunk_count = 1, s32 chunk_size = 64>
    class tpool {
#ifdef _DEBUG
        std::map<T *, std::string> _map;
#endif //_DEBUG

        enum {
            IN_USE,
            IS_FREE,
        };

        struct chunklist;
        struct chunk {
            s8 buffer[sizeof(T)];
            chunklist * parent;
            chunk * prev;
            chunk * next;
            s32 len;
            s8 state;
#ifdef _DEBUG
            char file[128];
            s32 line;
#endif
        };

        struct chunklist {
            s32 count;
            chunklist * prev;
            chunklist * next;
            chunk chunks[chunk_size];
        };

    public:
        tpool() : _head(nullptr), _list_head(nullptr), _chunk_count(0) {
            alloc_chunk(chunk_count);
        }

        ~tpool() {
            while (_list_head != nullptr) {
                auto tmp = _list_head;
                _list_head = _list_head->next;

                FREE(tmp);
            }
        }

#ifdef _DEBUG
        T * Create(const char * file, const s32 line) {
            chunk * chunk = create(file, line);
#else
        T * Create() {
            chunk * chunk = create();
#endif
            T * t = new(chunk->buffer) T();
#ifdef _DEBUG
//             string debug = file;
//             debug.append(":").append(tools::intAsString(line));
//             _map.insert(std::make_pair(t, debug));
#endif //_DEBUG
            return t;
        }

        template<typename... Args>
#ifdef _DEBUG
        T * Create(const char * file, const s32 line, Args... args) {
            chunk * chunk = create(file, line);
#else
        T * Create(Args... args) {
            chunk * chunk = create();
#endif
            T * t = new(chunk->buffer) T(args...);
#ifdef _DEBUG
//             string debug = file;
//             debug.append(":").append(tools::intAsString(line));
//             _map.insert(std::make_pair(t, debug));
#endif //_DEBUG
            return t;
        }

#ifdef _DEBUG
        void Recover(T * t, const char * file, const s32 line) {
            //printf("recover %llx %s:%d\n", t, file, line);
            //_map.erase(_map.find(t));
#else
        void Recover(T * t) {
#endif //_DEBUG
            t->~T();
            recover((chunk *)t);
        }

        s32 count() { return _chunk_count * chunk_size; }

    private:
#ifdef _DEBUG
        chunk * create(const char * file, const s32 line) {
#else
        chunk * create() {
#endif
            chunk * ret = nullptr;
            if (_head == nullptr)
                alloc_chunk(1);

            ret = _head;
            remove(_head);
            tassert(ret->state == IS_FREE && ret->len == sizeof(chunk), "chunk invalid");
            ++ret->parent->count;
            ret->state = IN_USE;
#ifdef _DEBUG
            safesprintf(ret->file, sizeof(ret->file), "%s", file);
            ret->line = line;
#endif
            return ret;
        }

        void recover(chunk * _chunk) {
            tassert(_chunk->state == IN_USE && _chunk->len == sizeof(chunk), "recover invalid chunk");
            tassert(_chunk->parent->count > 0, "chunk list error");
            --_chunk->parent->count;
            _chunk->state = IS_FREE;
            if (_chunk->parent->count == 0 && _chunk_count > chunk_count)
                free_chunk_list(_chunk->parent);
            else
                add(_chunk);
        }

        void alloc_chunk(s32 count) {
            for (int i = 0; i < count; ++i)
            {
                chunklist * new_list = (chunklist*)MALLOC(sizeof(chunklist));
                tassert(new_list, "new chunk faild");
                arrange_chunk_list(new_list);


                new_list->next = _list_head;
                if (_list_head != nullptr)
                    _list_head->prev = new_list;
                _list_head = new_list;
            }

            _chunk_count += count;
        }

        inline void free_chunk_list(chunklist * chunk_list) {
            for (int i = 0; i < chunk_size; ++i) {
                tassert(chunk_list->chunks[i].state == IS_FREE, "free chunk list but child chunk is not free");
                remove(&(chunk_list->chunks[i]));
            }

            if (chunk_list->next != nullptr)
                chunk_list->next->prev = chunk_list->prev;

            if (chunk_list->prev != nullptr)
                chunk_list->prev->next = chunk_list->next;

            if (_list_head == chunk_list)
                _list_head = chunk_list->next;

            FREE(chunk_list);
            --_chunk_count;
        }

        inline void arrange_chunk_list(chunklist * chunk_list) {
            chunk_list->prev = nullptr;
            chunk_list->next = nullptr;
            chunk_list->count = 0;
            for (int i = 0; i < chunk_size; ++i) {
                chunk_list->chunks[i].parent = chunk_list;
                chunk_list->chunks[i].state = IS_FREE;
                chunk_list->chunks[i].len = sizeof(chunk);
                chunk_list->chunks[i].prev = nullptr;
                chunk_list->chunks[i].next = nullptr;

                add(&(chunk_list->chunks[i]));
            }
        }

        inline void add(chunk * chunk) {
            chunk->next = _head;
            if (_head != nullptr)
                _head->prev = chunk;
            _head = chunk;
        }

        inline void remove(chunk * chunk) {
            if (chunk->next != nullptr)
                chunk->next->prev = chunk->prev;

            if (chunk->prev != nullptr)
                chunk->prev->next = chunk->next;

            if (_head == chunk)
                _head = chunk->next;

            chunk->next = nullptr;
            chunk->prev = nullptr;
        }

    private:
        chunk * _head;
        chunklist * _list_head;
        s32 _chunk_count;
        };
    }

#ifdef _DEBUG
#define create_from_pool(pool, ...) pool.Create(__FILE__, __LINE__, ##__VA_ARGS__)
#define recover_to_pool(pool, p) pool.Recover((p), __FILE__, __LINE__)
#else
#define create_from_pool(pool, ...) pool.Create( __VA_ARGS__ )
#define recover_to_pool(pool, p) pool.Recover(p)
#endif
#endif //__tpool_h__

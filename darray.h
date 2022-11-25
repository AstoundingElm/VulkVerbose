#pragma once
/*
#include "defines.h"

Memory layout
u64 capacity = number elements that can be held
u64 length = number of elements currently contained
u64 stride = size of each element in bytes
void* elements

#define FALSE 0
#define TRUE 1
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void* platform_allocate(u64 size, b8 aligned) {
        return malloc(size);
}
void platform_free(void* block, b8 aligned) {
        free(block);
}
void* platform_zero_memory(void* block, u64 size) {
        return memset(block, 0, size);
}
void* platform_copy_memory(void* dest, const void* source, u64 size) {
        return memcpy(dest, source, size);
}
void* platform_set_memory(void* dest, i32 value, u64 size) {
        return memset(dest, value, size);
}

typedef enum memory_tag {
        // For temporary use. Should be assigned one of the below or have a new tag created.
        MEMORY_TAG_UNKNOWN,
        MEMORY_TAG_ARRAY,
        MEMORY_TAG_DARRAY,
        MEMORY_TAG_DICT,
        MEMORY_TAG_RING_QUEUE,
        MEMORY_TAG_BST,
        MEMORY_TAG_STRING,
        MEMORY_TAG_APPLICATION,
        MEMORY_TAG_JOB,
        MEMORY_TAG_TEXTURE,
        MEMORY_TAG_MATERIAL_INSTANCE,
        MEMORY_TAG_RENDERER,
        MEMORY_TAG_GAME,
        MEMORY_TAG_TRANSFORM,
        MEMORY_TAG_ENTITY,
        MEMORY_TAG_ENTITY_NODE,
        MEMORY_TAG_SCENE,
        
        MEMORY_TAG_MAX_TAGS
} memory_tag;

struct memory_stats {
        u64 total_allocated;
        u64 tagged_allocations[MEMORY_TAG_MAX_TAGS];
};

static const char* memory_tag_strings[MEMORY_TAG_MAX_TAGS] = {
        "UNKNOWN    ",
        "ARRAY      ",
        "DARRAY     ",
        "DICT       ",
        "RING_QUEUE ",
        "BST        ",
        "STRING     ",
        "APPLICATION",
        "JOB        ",
        "TEXTURE    ",
        "MAT_INST   ",
        "RENDERER   ",
        "GAME       ",
        "TRANSFORM  ",
        "ENTITY     ",
        "ENTITY_NODE",
        "SCENE      "};

static struct memory_stats stats;

void initialize_memory() {
        platform_zero_memory(&stats, sizeof(stats));
}

void shutdown_memory() {
}

void* kallocate(u64 size, memory_tag tag) {
        if (tag == MEMORY_TAG_UNKNOWN) {
                //KWARN("kallocate called using MEMORY_TAG_UNKNOWN. Re-class this allocation.");
        }
        
        stats.total_allocated += size;
        stats.tagged_allocations[tag] += size;
        
        // TODO: Memory alignment
        void* block = platform_allocate(size, FALSE);
        platform_zero_memory(block, size);
        return block;
}

void kfree(void* block, u64 size, memory_tag tag) {
        if (tag == MEMORY_TAG_UNKNOWN) {
                //KWARN("kfree called using MEMORY_TAG_UNKNOWN. Re-class this allocation.");
        }
        
        stats.total_allocated -= size;
        stats.tagged_allocations[tag] -= size;
        
        // TODO: Memory alignment
        platform_free(block, FALSE);
}

void* kzero_memory(void* block, u64 size) {
        return platform_zero_memory(block, size);
}

void* kcopy_memory(void* dest, const void* source, u64 size) {
        return platform_copy_memory(dest, source, size);
}

void* kset_memory(void* dest, i32 value, u64 size) {
        return platform_set_memory(dest, value, size);
}

char* get_memory_usage_str() {
        const u64 gib = 1024 * 1024 * 1024;
        const u64 mib = 1024 * 1024;
        const u64 kib = 1024;
        
        char buffer[8000] = "System memory use (tagged):\n";
        u64 offset = strlen(buffer);
        for (u32 i = 0; i < MEMORY_TAG_MAX_TAGS; ++i) {
                char unit[4] = "XiB";
                float amount = 1.0f;
                if (stats.tagged_allocations[i] >= gib) {
                        unit[0] = 'G';
                        amount = stats.tagged_allocations[i] / (float)gib;
                } else if (stats.tagged_allocations[i] >= mib) {
                        unit[0] = 'M';
                        amount = stats.tagged_allocations[i] / (float)mib;
                } else if (stats.tagged_allocations[i] >= kib) {
                        unit[0] = 'K';
                        amount = stats.tagged_allocations[i] / (float)kib;
                } else {
                        unit[0] = 'B';
                        unit[1] = 0;
                        amount = (float)stats.tagged_allocations[i];
                }
                
                i32 length = snprintf(buffer + offset, 8000, "  %s: %.2f%s\n", memory_tag_strings[i], amount, unit);
                offset += length;
        }
        char* out_string = strdup(buffer);
        return out_string;
}

enum {
        DARRAY_CAPACITY,
        DARRAY_LENGTH,
        DARRAY_STRIDE,
        DARRAY_FIELD_LENGTH
};

KAPI void* _darray_create(u64 length, u64 stride);
KAPI void _darray_destroy(void* array);

KAPI u64 _darray_field_get(void* array, u64 field);
KAPI void _darray_field_set(void* array, u64 field, u64 value);

KAPI void* _darray_resize(void* array);

KAPI void* _darray_push(void* array, const void* value_ptr);
KAPI void _darray_pop(void* array, void* dest);

KAPI void* _darray_pop_at(void* array, u64 index, void* dest);
KAPI void* _darray_insert_at(void* array, u64 index, void* value_ptr);

#define DARRAY_DEFAULT_CAPACITY 1
#define DARRAY_RESIZE_FACTOR 2

#define darray_create(type) \
_darray_create(DARRAY_DEFAULT_CAPACITY, sizeof(type))

#define darray_reserve(type, capacity) \
_darray_create(capacity, sizeof(type))

#define darray_destroy(array) _darray_destroy(array);

#define darray_push(array, value)           \
{                                       \
typeof(value) temp = value;         \
array = _darray_push(array, &temp); \
}
// NOTE: could use __auto_type for temp above, but intellisense
// for VSCode flags it as an unknown type. typeof() seems to
// work just fine, though. Both are GNU extensions.

#define darray_pop(array, value_ptr) \
_darray_pop(array, value_ptr)

#define darray_insert_at(array, index, value)           \
{                                                   \
typeof(value) temp = value;                     \
array = _darray_insert_at(array, index, &temp); \
}

#define darray_pop_at(array, index, value_ptr) \
_darray_pop_at(array, index, value_ptr)

#define darray_clear(array) \
_darray_field_set(array, DARRAY_LENGTH, 0)

#define darray_capacity(array) \
_darray_field_get(array, DARRAY_CAPACITY)

#define darray_length(array) \
_darray_field_get(array, DARRAY_LENGTH)

#define darray_stride(array) \
_darray_field_get(array, DARRAY_STRIDE)

#define darray_length_set(array, value) \
_darray_field_set(array, DARRAY_LENGTH, value)



void* _darray_create(u64 length, u64 stride) {
        u64 header_size = DARRAY_FIELD_LENGTH * sizeof(u64);
        u64 array_size = length * stride;
        u64* new_array = kallocate(header_size + array_size, MEMORY_TAG_DARRAY);
        kset_memory(new_array, 0, header_size + array_size);
        new_array[DARRAY_CAPACITY] = length;
        new_array[DARRAY_LENGTH] = 0;
        new_array[DARRAY_STRIDE] = stride;
        return (void*)(new_array + DARRAY_FIELD_LENGTH);
}

void _darray_destroy(void* array) {
        u64* header = (u64*)array - DARRAY_FIELD_LENGTH;
        u64 header_size = DARRAY_FIELD_LENGTH * sizeof(u64);
        u64 total_size = header_size + header[DARRAY_CAPACITY] * header[DARRAY_STRIDE];
        kfree(header, total_size, MEMORY_TAG_DARRAY);
}

u64 _darray_field_get(void* array, u64 field) {
        u64* header = (u64*)array - DARRAY_FIELD_LENGTH;
        return header[field];
}

void _darray_field_set(void* array, u64 field, u64 value) {
        u64* header = (u64*)array - DARRAY_FIELD_LENGTH;
        header[field] = value;
}

void* _darray_resize(void* array) {
        u64 length = darray_length(array);
        u64 stride = darray_stride(array);
        void* temp = _darray_create(
                                    (DARRAY_RESIZE_FACTOR * darray_capacity(array)),
                                    stride);
        kcopy_memory(temp, array, length * stride);
        
        _darray_field_set(temp, DARRAY_LENGTH, length);
        _darray_destroy(array);
        return temp;
}

void* _darray_push(void* array, const void* value_ptr) {
        u64 length = darray_length(array);
        u64 stride = darray_stride(array);
        if (length >= darray_capacity(array)) {
                array = _darray_resize(array);
        }
        
        u64 addr = (u64)array;
        addr += (length * stride);
        kcopy_memory((void*)addr, value_ptr, stride);
        _darray_field_set(array, DARRAY_LENGTH, length + 1);
        return array;
}

void _darray_pop(void* array, void* dest) {
        u64 length = darray_length(array);
        u64 stride = darray_stride(array);
        u64 addr = (u64)array;
        addr += ((length - 1) * stride);
        kcopy_memory(dest, (void*)addr, stride);
        _darray_field_set(array, DARRAY_LENGTH, length - 1);
}

void* _darray_pop_at(void* array, u64 index, void* dest) {
        u64 length = darray_length(array);
        u64 stride = darray_stride(array);
        if (index >= length) {
                //KERROR("Index outside the bounds of this array! Length: %i, index: %index", length, index);
                return array;
        }
        
        u64 addr = (u64)array;
        kcopy_memory(dest, (void*)(addr + (index * stride)), stride);
        
        // If not on the last element, snip out the entry and copy the rest inward.
        if (index != length - 1) {
                kcopy_memory(
                             (void*)(addr + (index * stride)),
                             (void*)(addr + ((index + 1) * stride)),
                             stride * (length - index));
        }
        
        _darray_field_set(array, DARRAY_LENGTH, length - 1);
        return array;
}

void* _darray_insert_at(void* array, u64 index, void* value_ptr) {
        u64 length = darray_length(array);
        u64 stride = darray_stride(array);
        if (index >= length) {
                //KERROR("Index outside the bounds of this array! Length: %i, index: %index", length, index);
                return array;
        }
        if (length >= darray_capacity(array)) {
                array = _darray_resize(array);
        }
        
        u64 addr = (u64)array;
        
        // If not on the last element, copy the rest outward.
        if (index != length - 1) {
                kcopy_memory(
                             (void*)(addr + ((index + 1) * stride)),
                             (void*)(addr + (index * stride)),
                             stride * (length - index));
        }
        
        // Set the value at the index
        kcopy_memory((void*)(addr + (index * stride)), value_ptr, stride);
        
        _darray_field_set(array, DARRAY_LENGTH, length + 1);
        return array;
}*/
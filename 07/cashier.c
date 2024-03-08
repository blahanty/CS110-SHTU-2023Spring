#include "cashier.h"
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

/* calculate log_2(x) */
uint64_t log2i(uint64_t x) {
    /* initialization */
    uint64_t retval = 0, y = x;
    while (y != 1) {
        y /= 2;
        /* update result */
        ++retval;
    }
    return retval;
}

struct cashier *cashier_init(struct cache_config config) {
    /* allocate memory */
    struct cashier *new_cache = (struct cashier *) malloc(sizeof(struct cashier));
    /* allocate fails */
    if (!new_cache)
        return NULL;
    /* initialization */
    new_cache->config = config;
    new_cache->size = config.line_size * config.lines;
    /* offset */
    new_cache->offset_bits = log2i(config.line_size);
    new_cache->offset_mask = (1 << new_cache->offset_bits) - 1;
    /* index */
    new_cache->index_bits = log2i(config.lines / config.ways);
    new_cache->index_mask = ((1 << (new_cache->index_bits + new_cache->offset_bits)) - 1) & (~new_cache->offset_mask);
    /* tag */
    new_cache->tag_bits = config.address_bits - new_cache->offset_bits - new_cache->index_bits;
    new_cache->tag_mask = ~(new_cache->offset_mask + new_cache->index_mask);
    printf("(%lu\n",new_cache->tag_mask);
    /* allocate memory */
    new_cache->lines = (struct cache_line *) calloc(config.lines, sizeof(struct cache_line));
    /* allocate fails */
    if (!new_cache->lines) {
        free(new_cache);
        return NULL;
    }
    /* initialization */
    struct cache_line new_cache_line;
    new_cache_line.valid = false;
    new_cache_line.dirty = false;
    new_cache_line.tag = 0;
    new_cache_line.last_access = 0;
    /* initialization */
    for (size_t i = 0; i < config.lines; ++i) {
        /* allocate memory */
        new_cache_line.data = (uint8_t *) calloc(config.line_size, sizeof(uint8_t));
        /* allocate fails */
        if (!new_cache_line.data) {
            /* deallocate bytes */
            for (size_t j = 0; j < i; ++j)
                free(new_cache->lines[j].data);
            /* deallocate lines */
            free(new_cache->lines);
            /* deallocate cashier */
            free(new_cache);
            return NULL;
        }
        /* initialization */
        new_cache->lines[i] = new_cache_line;
    }
    return new_cache;
}

void cashier_release(struct cashier *cache) {
    for (size_t j = 0; j < cache->config.ways; ++j) {
        for (size_t i = 0; i < cache->config.lines / cache->config.ways; ++i) {
            /* index */
            int k = i * cache->config.ways + j;
            /* evict */
            if (cache->lines[k].valid) {
                before_eviction(i, cache->lines + k);
                /* write back */
                if (cache->lines[k].dirty)
                    for (size_t l = 0; l < cache->config.line_size; ++l)
                        mem_write((cache->lines[k].tag << (cache->offset_bits + cache->index_bits)) +
                                  (i << cache->offset_bits) + l, cache->lines[k].data[l]);
            }
            free(cache->lines[k].data);
        }
    }
    /* deallocate all */
    free(cache->lines);
    free(cache);
}

bool cashier_read(struct cashier *cache, uint64_t addr, uint8_t *byte) {
    /* extract offset, index and tag from index */
    uint64_t o = addr & cache->offset_mask;
    uint64_t i = (addr & cache->index_mask) >> cache->offset_bits;
    uint64_t t = (addr & cache->tag_mask) >> (cache->offset_bits + cache->index_bits);
    /* initialization */
    size_t min_last_access = 1145141919810, min_last_access_index = 0;
    size_t k = i * cache->config.ways;
    /* try to hit */
    for (size_t j = 0; j < cache->config.ways; ++j, ++k) {
        if (cache->lines[k].valid) {
            /* hit */
            if (cache->lines[k].tag == t) {
                *byte = cache->lines[k].data[o];
                cache->lines[k].last_access = get_timestamp();
                return true;
            } else {
                /* get minimum last_access for LRU */
                if (cache->lines[k].last_access < min_last_access) {
                    min_last_access = cache->lines[k].last_access;
                    min_last_access_index = k;
                }
            }
        } else {
            /* empty block */
            /* miss */
            /* to read to this block */
            min_last_access_index = k;
            break;
        }
    }
    /* miss */
    /* nonempty block */
    if (cache->lines[min_last_access_index].valid) {
        before_eviction(i, cache->lines + min_last_access_index);
        /* write back */
        if (cache->lines[min_last_access_index].dirty)
            for (size_t l = 0; l < cache->config.line_size; ++l)
                mem_write((cache->lines[min_last_access_index].tag << (cache->offset_bits + cache->index_bits)) +
                          (addr & cache->index_mask) + l, cache->lines[min_last_access_index].data[l]);
    }
    /* (re)initialization */
    cache->lines[min_last_access_index].valid = true;
    cache->lines[min_last_access_index].dirty = false;
    cache->lines[min_last_access_index].tag = t;
    cache->lines[min_last_access_index].last_access = get_timestamp();
    /* read from memory */
    for (size_t l = 0; l < cache->config.line_size; ++l)
        cache->lines[min_last_access_index].data[l] = mem_read((addr & (cache->index_mask + cache->tag_mask)) + l);
    /* read to *byte */
    *byte = cache->lines[min_last_access_index].data[o];
    return false;
}

bool cashier_write(struct cashier *cache, uint64_t addr, uint8_t byte) {
    /* extract offset, index and tag from index */
    uint64_t o = addr & cache->offset_mask;
    uint64_t i = (addr & cache->index_mask) >> cache->offset_bits;
    uint64_t t = (addr & cache->tag_mask) >> (cache->offset_bits + cache->index_bits);
    /* initialization */
    size_t min_last_access = 1145141919810, min_last_access_index = 0;
    size_t k = i * cache->config.ways;
    /* try to hit */
    for (size_t j = 0; j < cache->config.ways; ++j, ++k) {
        if (cache->lines[k].valid) {
            /* hit */
            if (cache->lines[k].tag == t) {
                cache->lines[k].data[o] = byte;
                cache->lines[k].dirty = true;
                cache->lines[k].last_access = get_timestamp();
                return true;
            } else {
                /* get minimum last_access for LRU */
                if (cache->lines[k].last_access < min_last_access) {
                    min_last_access = cache->lines[k].last_access;
                    min_last_access_index = k;
                }
            }
        } else {
            /* empty block */
            /* miss */
            /* to write to this block */
            min_last_access_index = k;
            break;
        }
    }
    /* miss */
    /* nonempty block */
    if (cache->lines[min_last_access_index].valid) {
        before_eviction(i, cache->lines + min_last_access_index);
        /* write back */
        if (cache->lines[min_last_access_index].dirty)
            for (size_t l = 0; l < cache->config.line_size; ++l)
                mem_write((cache->lines[min_last_access_index].tag << (cache->offset_bits + cache->index_bits)) +
                          (addr & cache->index_mask) + l, cache->lines[min_last_access_index].data[l]);
    }
    /* (re)initialization */
    cache->lines[min_last_access_index].valid = true;
    cache->lines[min_last_access_index].dirty = true;
    cache->lines[min_last_access_index].tag = t;
    cache->lines[min_last_access_index].last_access = get_timestamp();
    /* read from memory */
    for (size_t l = 0; l < cache->config.line_size; ++l)
        cache->lines[min_last_access_index].data[l] = mem_read((addr & (cache->index_mask + cache->tag_mask)) + l);
    /* write from byte */
    cache->lines[min_last_access_index].data[o] = byte;
    return false;
}

#include <assert.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "cashier.h"

// Hacking the grader is acceptable.
// You may try to get accepted by exploiting it.
//
// This **won't** lead to any penalty as long as you follow the academic
// integrity rules.
#define SECRET_MSG "YOU DON'T HAVE THE CORRECT MESSAGE"

uint8_t rand_u8(void);

// You may use `stdout` or `stderr` for debugging,
// the grader will filter out them using `SECRET_MSG`.
#define log(...)                                                               \
  do {                                                                         \
    printf("%s[%" PRIu64 "]: ", SECRET_MSG, get_timestamp());                  \
    printf(__VA_ARGS__);                                                       \
    putchar('\n');                                                             \
  } while (0)

static uint64_t ts = 0;
void inc_timestamp() { ts++; }
uint64_t get_timestamp() { return ts; }

uint8_t mem_read(uint64_t addr) {
  log("mem read at %" PRIu64 "\n", addr);
  return *(uint8_t *)addr;
}
void mem_write(uint64_t addr, uint8_t byte) {
  log("mem write at %" PRIu64 "\n", addr);
  *(uint8_t *)addr = byte;
}

uint8_t rand_u8() { return (uint8_t)rand(); }

void before_eviction(uint64_t set_index, struct cache_line *victim) {
  log("evict set=%" PRIu64 ",tag=%" PRIu64, set_index, victim->tag);
}

struct cashier *cache;
// memory write wrapper
static void write(uint8_t *addr, uint8_t byte) {
  inc_timestamp();
  bool hit = cashier_write(cache, (uint64_t)addr, byte);
  log("write %4u to %p: %s", byte, addr, hit ? "HIT" : "MISS");
}
// memory read wrapper
static uint8_t read(uint8_t *addr) {
  inc_timestamp();
  uint8_t byte = 0;
  bool hit = cashier_read(cache, (uint64_t)addr, &byte);
  log("read  %4u from %p: %s", byte, addr, hit ? "HIT" : "MISS");
  return byte;
}

// you can try to make a few more traces.
// a dot product trace is given for your reference
void dot_test(struct cache_config config, uint64_t N) {
  // pre
  uint64_t sz = (N / config.line_size + 1) * config.line_size;
  uint8_t *a = aligned_alloc(config.line_size, sz);
  uint8_t *b = aligned_alloc(config.line_size, sz);
  uint8_t *dot = aligned_alloc(config.line_size, sz);

  // test
  cache = cashier_init(config);
  log("fill array with random data");
  for (uint64_t i = 0; i < N; i++) {
    write(a + i, rand_u8());
    write(b + i, rand_u8());
  }
  log("compute dot product");
  for (uint64_t i = 0; i < N; i++) {
    log("adding the %" PRIu64 "-th coordinate", i);
    uint8_t ai = read(a + i);
    uint8_t bi = read(b + i);
    uint8_t doti = read(dot);
    write(dot, doti + ai * bi);
  }
  log("finished, release cache");
  cashier_release(cache);

  // post
  for (uint64_t i = 0; i < N; i++)
    log("dot product result %u", dot[i]);
  free(a), free(b), free(dot);
}

int main() {
  struct cache_config config = (struct cache_config){
      .line_size = 4, .lines = 16, .ways = 4, .address_bits = 64};
  uint64_t n = 500;
  dot_test(config, n);
  return 0;
};

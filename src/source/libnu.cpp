#if !defined(_WIN32)
#include <execinfo.h>
#include <unistd.h>
#endif

#include <atomic>
#include <heaplayers.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#if defined(__APPLE__)
#include <malloc/malloc.h>
#elif defined(linux)
#include <malloc.h>
#endif

#include <cstddef>

#include "common.hpp"

#if defined(__APPLE__)
#include "macinterpose.h"
#endif

// For use by the replacement printf routines (see
// https://github.com/mpaland/printf)

extern "C" void _putchar(char ch) { ::write(1, (void *)&ch, 1); }

#include "wrappers/generic-memalign.cpp"

static bool in_malloc { false };
static bool in_free { false };
static unsigned long actual_malloc_count { 0 };
static unsigned long actual_free_count { 0 };

extern "C" {

  static unsigned long sampled_malloc_count { 0 };
  static unsigned long total_sampled_count { 0 };

  void allocationIntensityTimer(int)
  {
    total_sampled_count++;
    if (in_malloc || in_free) {
      sampled_malloc_count++;
    }
    if (total_sampled_count % 100 == 0) {
      fprintf(stderr, "malloc ratio = %lf (%lu / %lu); mallocs=%lu, frees=%lu\n",
	      (double) sampled_malloc_count / (double) total_sampled_count,
	      sampled_malloc_count,
	      total_sampled_count,
	      actual_malloc_count,
	      actual_free_count);
    }
  }
}

// Malloc replacement functions

extern "C" {

  static std::atomic<bool> initialized = false;

  static void initialize_me()
  {
    signal(SIGALRM, allocationIntensityTimer);
    struct itimerval tm;
    tm.it_value.tv_sec = 0;
    tm.it_value.tv_usec = 2000;
    tm.it_interval.tv_sec = 0;
    tm.it_interval.tv_usec = 2000;
    setitimer(ITIMER_REAL, &tm, nullptr);
    //      setitimer(ITIMER_VIRTUAL, &tm, nullptr);
    ///      signal(SIGVTALRM, handleTimer);
    // signal(SIGVTALRM, allocationIntensityTimer);
  }
  
  void * __attribute__((always_inline)) xxmalloc(size_t sz) {
    if (!initialized) {
      initialized = true;
      initialize_me();
    }
    actual_malloc_count++;
    in_malloc = true;
    auto ptr = ::malloc(sz);
    in_malloc = false;
    return ptr;
  }
  
  void __attribute__((always_inline)) xxfree(void * ptr) {
    if (!initialized) {
      initialized = true;
      initialize_me();
    }
    actual_free_count++;
    in_free = true;
    ::free(ptr);
    in_free = false;
  }
  
  decltype(::malloc_size(nullptr)) xxmalloc_usable_size(void * ptr) {
    return ::malloc_size(ptr);
  }

  void * xxmemalign(size_t alignment, size_t size) {
    return generic_xxmemalign(alignment, size);
  }
  
  void xxmalloc_lock() {}
  void xxmalloc_unlock() {}
};

MAC_INTERPOSE(xxmalloc, malloc);

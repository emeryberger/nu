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
#else
#include <malloc.h>
#include <sys/time.h>
#endif

#include <cstddef>

#include "common.hpp"

#if defined(__APPLE__)
#include "macinterpose.h"
#else
#include <dlfcn.h>
#endif

// For use by the replacement printf routines (see
// https://github.com/mpaland/printf)

extern "C" void _putchar(char ch) { ::write(1, (void *)&ch, 1); }

#include "wrappers/generic-memalign.cpp"

static bool in_malloc { false };
static bool in_free { false };
static unsigned long actual_malloc_count { 0 };
static unsigned long actual_free_count { 0 };
static unsigned long sampled_malloc_count { 0 };
static unsigned long total_sampled_count { 0 };
static unsigned long malloc_hist[32];
static unsigned long free_hist[32];
static unsigned long footprint { 0 };

template <typename T>
void printHist(T& hist) {
  for (int i = 0; i < sizeof(hist)/sizeof(decltype(hist[0])); i++) {
    char toPrint = '_';
    if (hist[i]) {
      if (ilog2(hist[i]) >= 26) {
	toPrint = 'A' + ilog2(hist[i]);
      } else {
	toPrint = 'a' + ilog2(hist[i]);
      }
    }
    fprintf(stderr, "%c", toPrint);
  }
}

extern "C" {

 
  void allocationIntensityTimer(int)
  {
    total_sampled_count++;
    if (in_malloc || in_free) {
      sampled_malloc_count++;
    }
    if (total_sampled_count % 100 == 0) {
      fprintf(stderr, "malloc ratio = %lf (%lu / %lu); mallocs=%lu, frees=%lu (footprint=%lu)\n",
	      (double) sampled_malloc_count / (double) total_sampled_count,
	      sampled_malloc_count,
	      total_sampled_count,
	      actual_malloc_count,
	      actual_free_count,
	      footprint);
      fprintf(stderr, "malloc histogram: ");
      printHist(malloc_hist);
      fprintf(stderr, "\n");
      fprintf(stderr, "free histogram:   ");
      printHist(free_hist);
      fprintf(stderr, "\n");
    }
  }
}

// Malloc replacement functions

extern "C" {

  void * my_dlsym(void * handle, const char * symbol);
  
  static std::atomic<bool> initialized = false;
  static decltype(::malloc)* _malloc{0};
  static decltype(::free)* _free{0};
#if defined(__APPLE__)
  static decltype(::malloc_size)* _malloc_usable_size{0};
#else
  static decltype(::malloc_usable_size)* _malloc_usable_size{0};
#endif
  
  static void initialize_me()
  {
#if defined(__APPLE__)
    _malloc = ::malloc;
    _free = ::free;
    _malloc_usable_size = ::malloc_size;
#else
    _malloc = (decltype(::malloc) *) my_dlsym(RTLD_NEXT, "malloc");
    _free = (decltype(::free) *) my_dlsym(RTLD_NEXT, "free");
    _malloc_usable_size = (decltype(::malloc_usable_size) *) my_dlsym(RTLD_NEXT, "malloc_usable_size");
#endif
    signal(SIGALRM, allocationIntensityTimer);
    struct itimerval tm;
    tm.it_value.tv_sec = 0;
    tm.it_value.tv_usec = 10000;
    tm.it_interval.tv_sec = 0;
    tm.it_interval.tv_usec = 10000;
    setitimer(ITIMER_REAL, &tm, nullptr);
    initialized = true;
  }
  
#if defined(__APPLE__)
  decltype(::malloc_size(nullptr)) xxmalloc_usable_size(void * ptr) {
    return _malloc_usable_size(ptr);
  }
#else
  decltype(::malloc_usable_size(nullptr)) xxmalloc_usable_size(void * ptr) {
    return _malloc_usable_size(ptr);
  }
#endif
  
  void * __attribute__((always_inline)) xxmalloc(size_t sz) {
    if (!initialized) {
      initialize_me();
    }
    actual_malloc_count++;
    in_malloc = true;
    const auto ptr = _malloc(sz);
    in_malloc = false;
    const auto actual_sz = ilog2(::xxmalloc_usable_size(ptr) >> 3);
    malloc_hist[actual_sz]++;
    footprint += actual_sz;
    return ptr;
  }
  
  void __attribute__((always_inline)) xxfree(void * ptr) {
    if (!initialized) {
      initialize_me();
    }
    actual_free_count++;
    const auto actual_sz = ilog2(::xxmalloc_usable_size(ptr) >> 3);
    free_hist[actual_sz]++;
    in_free = true;
    _free(ptr);
    in_free = false;
    footprint -= actual_sz;
  }

  void * xxmemalign(size_t alignment, size_t size) {
    return generic_xxmemalign(alignment, size);
  }
  
  void xxmalloc_lock() {}
  void xxmalloc_unlock() {}
};

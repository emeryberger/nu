#if !defined(_WIN32)
#include <execinfo.h>
#include <unistd.h>
#endif

#include <atomic>
#include <heaplayers.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#include <cstddef>

#include "common.hpp"

#include <iostream>

void decode_histogram(const std::string& malloc_hist) {
  std::string s = "";
  for (unsigned long i = 0; i < 32; i++) {
    auto ch = malloc_hist.c_str()[i];
    int v;
    if (ch == '_') {
      v = 0;
    } else {
      if ((ch >= 'a') && (ch <= 'z')) {
	v = 1 << ((int) ch - (int) 'a');
      } else {
	v = 1 << ((int) ch - (int) 'A' + 26);
      }
    }
    std::cout << "size\t" << (8UL << i) << " = " << v << std::endl;
  }
}

int main()
{
  std::string malloc_hist;
  std::cin >> malloc_hist;
  decode_histogram(malloc_hist);
  return 0;
}

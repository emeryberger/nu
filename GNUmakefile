LIBNAME = nu
C_SOURCES = src/source/libnu.cpp

.PHONY: vendor-deps

# CXXFLAGS = -std=c++17 -g -O0 # FIXME
CXXFLAGS = -std=c++17 -g -O3 -DNDEBUG -D_REENTRANT=1 -pipe -fno-builtin-malloc -fvisibility=hidden
CXX = g++

INCLUDES  = -Isrc -Isrc/include
INCLUDES := $(INCLUDES) -Ivendor/Heap-Layers -Ivendor/Heap-Layers/wrappers -Ivendor/Heap-Layers/utility
INCLUDES := $(INCLUDES) -Ivendor/printf

ifeq ($(shell uname -s),Darwin)
  LIBFILE := lib$(LIBNAME).dylib
  WRAPPER := vendor/Heap-Layers/wrappers/macwrapper.cpp
  ifneq (,$(filter $(shell uname -p),arm arm64))  # this means "if arm or arm64"
    ARCH := -arch arm64 
  else
    ARCH := -arch x86_64
  endif
  CXXFLAGS := $(CXXFLAGS) -flto -ftls-model=initial-exec -ftemplate-depth=1024 $(ARCH) -compatibility_version 1 -current_version 1 -dynamiclib
  SED_INPLACE = -i ''

else # non-Darwin
  LIBFILE := lib$(LIBNAME).so
  WRAPPER := vendor/Heap-Layers/wrappers/gnuwrapper.cpp
  INCLUDES := $(INCLUDES) -I/usr/include/nptl 
  CXXFLAGS := $(CXXFLAGS) -fPIC -shared -Bsymbolic
  RPATH_FLAGS :=
  SED_INPLACE = -i

endif

SRC := src/source/lib$(LIBNAME).cpp $(WRAPPER) vendor/printf/printf.cpp

OUTDIR=.

all: $(OUTDIR)/$(LIBFILE)

$(OUTDIR)/$(LIBFILE): vendor/Heap-Layers $(SRC) $(C_SOURCES) GNUmakefile
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SRC) -o $(OUTDIR)/$(LIBFILE) -ldl -lpthread

clean:
	rm -f $(OUTDIR)/$(LIBFILE) $(LIBNAME)/*.so $(LIBNAME)/*.dylib
	rm -rf $(OUTDIR)/$(LIBFILE).dSYM

$(WRAPPER) : vendor/Heap-Layers

vendor/Heap-Layers:
	mkdir -p vendor && cd vendor && git clone https://github.com/emeryberger/Heap-Layers

vendor/printf/printf.cpp:
	mkdir -p vendor && cd vendor && git clone https://github.com/mpaland/printf
	cd vendor/printf && ln -s printf.c printf.cpp
	sed $(SED_INPLACE) -e 's/^#define printf printf_/\/\/&/' vendor/printf/printf.h

vendor-deps: vendor/Heap-Layers vendor/printf/printf.cpp

clang-format:
	-clang-format -i $(C_SOURCES) --style=google

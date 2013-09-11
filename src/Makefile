#
# phi4 build definition
#
# Author: Adam Strzelecki <adam.strzelecki@uj.edu.pl>
#
# You may control build settings using: make VAR=value ...
#
#   CC=compiler name i.e. CC=icc, CC=gcc, CC=clang (automatically sets CXX)
#   DIM=(2 or 3) dimensions
#   NCOMP=(>=1) number_of_components
#   FLOAT=(double or float) defaults to double, float on SIMD
#   SIMD=(4 or 8) size of SIMD vector (enables SIMD computing)
#   INS=(sse, avx or avx2) instruction set, default native
#   FAST_INDEX=1 fast index (avoid memory index) size only power of 2
#
# Note: output executable will have suffix indicating current settings.

CC    := cc
DIM   := 3
NCOMP := 1
ARCH  := native
O     := 3
ifdef SIMD
FLOAT := float
else
FLOAT := double
endif
SED   := sed
MV    := mv

# Architecture
CPPFLAGS += -march=$(ARCH)

# ICC specif flags
ifeq ($(CC),icc)
CPPFLAGS += -fno-alias -g \
            -vec-report \
            -fp-model fast=2 \
            -opt-ra-region-strategy=block

# change O3 to Intels -fast flavor
ifeq ($(O),3)
CFLAGS   += -fast
else
CFLAGS   += -O$(O)
endif

# GCC & Clang flags
else
CPPFLAGS += -O$(O) \
            -g \
            -funsafe-math-optimizations \
            -ffast-math
endif

# non-ICC use ICC svml on SIMD
ifdef SIMD
LDFLAGS  += -lsvml
endif

CXX := $(subst cc,c++,$(subst gcc,g++,$(subst icc,icpc,$(CC))))

# define executable suffix
SUFFIX := -$(NCOMP)nc-$(DIM)d-$(FLOAT)

# SIMD option
ifneq ($(SIMD),)
SUFFIX := $(SUFFIX)-vec$(SIMD)
SRC_SUFFIX := _simd
CPPFLAGS += -DSIMD=$(SIMD)
ifneq ($(CACHE),)
SUFFIX   := $(SUFFIX)-cache
CPPFLAGS += -DCACHE=1
endif
endif

# OpenMP option
ifneq ($(OMP),)
OPENMP := $(OMP)
endif
ifneq ($(OPENMP),)
SUFFIX   := $(SUFFIX)-omp
CXXFLAGS += -fopenmp
LDFLAGS  += -fopenmp
endif

# Fast index option
ifneq ($(FIDX),)
FAST_INDEX := 1
endif
ifneq ($(FAST_INDEX),)
CPPFLAGS += -DFAST_INDEX=1
SUFFIX   := $(SUFFIX)-fidx
endif

# Custom compiler
ifneq ($(CC),cc)
SUFFIX := $(SUFFIX)-$(CC)
CC_SUFFIX := -$(CC)
endif

# Custom architecture
ifneq ($(ARCH),native)
SUFFIX := $(SUFFIX)-$(ARCH)
endif

# Custom optimization
ifneq ($(O),3)
SUFFIX := $(SUFFIX)-o$(O)
endif

# Old SIMD wrapper
ifneq ($(OLD),)
SUFFIX := $(SUFFIX)-old
CPPFLAGS += -DOLD
endif

LDFLAGS  += -g
CPPFLAGS += -MMD \
            -DDIM=$(DIM) \
            -DFLOAT=$(FLOAT) \
            -DNCOMP=$(NCOMP) \
            -DNAME=phi4$(SUFFIX)

EXEC      = ../phi4$(SUFFIX)
MAIN      = phi4$(SUFFIX).o
OBJECTS   = sweep$(SRC_SUFFIX)$(SUFFIX).o \
            random$(SRC_SUFFIX)$(SUFFIX).o
ifneq ($(SIMD),)
OBJECTS  += simd$(CC_SUFFIX).o
endif
TOBJECTS  = $(subst .cpp,.o,$(wildcard *_test.cpp)) gtest-all.o
DEPEND    = $(subst .o,.d,$(MAIN)) \
            $(subst .o,.d,$(OBJECTS)) \
            $(subst .o,.d,$(TESTS_OBJECTS))

all: $(EXEC)

$(EXEC): $(MAIN) $(OBJECTS)
	$(CXX) -o $(EXEC) $^ $(LDFLAGS)

dsym: $(EXEC)
	dsymutil $(EXEC)

-include $(DEPEND)

%$(SUFFIX).o: %.cpp Makefile
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

%$(CC_SUFFIX).o: %.cpp Makefile
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

%$(SUFFIX).o: %.c Makefile
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

test: $(OBJECTS) $(TESTS_OBJECTS)
	$(CXX) $(LDFLAGS) -o $@ $^ -lpthread

clean:
	rm -f $(MAIN) $(OBJECTS) $(TOBJECTS) $(DEPEND)

clean-all:
	rm -rf *.o *.d *.s
	find .. -perm +ugo=x -name 'phi4-*' -delete

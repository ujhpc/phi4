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
INS   := arch=native
O     := 3
ifdef SIMD
FLOAT := float
else
FLOAT := double
endif
SED   := sed
MV    := mv

# Instruction set
CPPFLAGS += -m$(INS)

# ICC specif flags
ifeq ($(CC),icc)
CPPFLAGS += -fno-alias -g \
            -vec-report \
            -fp-model fast=2 \
            -opt-ra-region-strategy=block
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

ifdef OMP
ifndef OPENMP
OPENMP := $(OMP)
endif
endif

# define executable suffix
SUFFIX := -$(NCOMP)nc-$(DIM)d-$(FLOAT)
ifdef SIMD
SUFFIX := $(SUFFIX)-vec$(SIMD)
SIMD_SUFFIX := _simd
endif
ifdef CACHE
SUFFIX := $(SUFFIX)-cache
endif
ifdef OPENMP
SUFFIX := $(SUFFIX)-omp
endif
ifneq ($(FIDX),)
FAST_INDEX := 1
endif
ifneq ($(FAST_INDEX),)
CPPFLAGS += -DFAST_INDEX=1
SUFFIX   := $(SUFFIX)-fidx
endif
ifneq ($(CC),cc)
SUFFIX := $(SUFFIX)-$(CC)
CC_SUFFIX := -$(CC)
endif
ifneq ($(O),3)
SUFFIX := $(SUFFIX)-o$(O)
endif
ifneq ($(INS),arch=native)
SUFFIX := $(SUFFIX)-$(INS)
endif
ifneq ($(OLD),)
SUFFIX := $(SUFFIX)-old
CPPFLAGS += -DOLD
endif

ifdef OPENMP
CXXFLAGS += -fopenmp
LDFLAGS  += -fopenmp
endif

LDFLAGS  += -g
CPPFLAGS += -MMD \
            -DDIM=$(DIM) \
            -DFLOAT=$(FLOAT) \
            -DNCOMP=$(NCOMP) \
            -DNAME=phi4$(SUFFIX)
ifdef SIMD
CPPFLAGS += -DSIMD=$(SIMD)
ifdef CACHE
CPPFLAGS += -DCACHE=1
endif
endif

EXEC      = ../phi4$(SUFFIX)
MAIN      = phi4$(SUFFIX).o
OBJECTS   = sweep$(SIMD_SUFFIX)$(SUFFIX).o \
            random$(SIMD_SUFFIX)$(SUFFIX).o
ifdef SIMD
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

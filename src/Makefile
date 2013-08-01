#
# phi4 build definition
#
# Author: Adam Strzelecki <adam.strzelecki@uj.edu.pl>
#
# You may control build settings using: make VAR=value ...
#
#   CC=compiler name i.e. CC=icc, CC=gcc, CC=clang (automatically sets CXX)
#   DIM=dimensions (2 or 3)
#   NCOMP=number_of_components (>=1)
#   FLOAT=float_type (double or float) (defaults to double, float on SIMD)
#   SIMD=(4 or 8) size of SIMD vector (enabled SIMD computing)
#
# Note: output executable will have suffix indicating current settings.

CC    := cc
DIM   := 3
NCOMP := 1
ifdef SIMD
FLOAT := float
else
FLOAT := double
endif
SED   := sed
MV    := mv

# ICC
ifeq ($(CC),icc)
CPPFLAGS += -fast -fno-alias -g \
            -vec-report \
            -fp-model fast=2 \
            -opt-ra-region-strategy=block
else
# Clang
ifneq ($(findstring clang,$(CC)),)
CXX      := $(subst clang,clang++,$(CC))
CPPFLAGS += -g \
            -mavx \
            -O3 \
            -funsafe-math-optimizations \
            -ffast-math
else
# generic GCC compatible compiler
CPPFLAGS += -g \
            -march=native \
            -O3 \
            -funsafe-math-optimizations \
            -ffast-math
endif
# non-ICC use ICC svml on SIMD
ifdef SIMD
LDFLAGS  += -lsvml
endif
endif

CXX := $(subst cc,c++,$(subst gcc,g++,$(subst icc,icpc,$(CC))))

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
ifneq ($(CC),cc)
SUFFIX := $(SUFFIX)-$(CC)
CC_SUFFIX := -$(CC)
endif

ifdef OPENMP
ifeq ($(CC),icc)
CXXFLAGS += -fopenmp
LDFLAGS  += -fopenmp
else
CXXFLAGS += -openmp
LDFLAGS  += -openmp
endif
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

ifdef FIDX
CPPFLAGS += -DFAST_INDEX=1
SUFFIX   += -fidx
endif

EXEC      = phi4$(SUFFIX)
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
	rm -rf *.o *.d *.s phi4-*

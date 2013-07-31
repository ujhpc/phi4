CC       := cc
DIM      := 3
NCOMP    := 1
FLOAT    := double
SIMD     := 1

ifeq ($(CC),icc)
# ICC
CXX      := icpc
CFLAGS   += -DGTEST_HAS_TR1_TUPLE=1
CXXFLAGS += -DGTEST_HAS_TR1_TUPLE=0
CPPFLAGS += -fast -fno-alias -g \
	-vec-report \
	-fp-model fast=2 \
	-opt-ra-region-strategy=block
else
ifneq ($(findstring clang,$(CC)),)
# Clang
CXX      := $(subst clang,clang++,$(CC))
CPPFLAGS += -g -mavx -O3 -funsafe-math-optimizations -ffast-math
else
# generic GCC compatible compiler
CXX      := $(subst cc,c++,$(subst gcc,g++,$(CC)))
CPPFLAGS += -g -march=native -O3 -funsafe-math-optimizations -ffast-math
endif
# non-ICC suffix
CC_SUFFIX := -$(CC)
LDFLAGS   += -lsvml
endif

SED := sed
MV  := mv

ifneq ($(SIMD),1)
SIMD_SUFFIX := -vec$(SIMD)
endif

ifdef OPENMP
OMP_SUFFIX := -omp
ifneq ($(CC), icc)
CXXFLAGS += -fopenmp
LDFLAGS  += -fopenmp
else
CXXFLAGS += -openmp
LDFLAGS  += -openmp
SUFFIX   += -$(CC)
endif
endif

SUFFIX       := -$(NCOMP)nc-$(DIM)d-$(FLOAT)$(SIMD_SUFFIX)$(OMP_SUFFIX)$(CC_SUFFIX)
LDFLAGS      += -g -lpopt
CPPFLAGS     += -MMD -DDIM=$(DIM) -DFLOAT=$(FLOAT) -DNCOMP=$(NCOMP) -DSIMD=$(SIMD)
VPATH         = $(HOME)/downloads/gtest-1.6.0/src

EXEC          = phi4$(SUFFIX)
MAIN          = phi4$(SUFFIX).o
OBJECTS       = sweep$(SUFFIX).o random$(SUFFIX).o file$(CC_SUFFIX).o
ifneq ($(SIMD),1)
OBJECTS      += simd$(CC_SUFFIX).o
endif
TESTS_OBJECTS = $(subst .cpp,.o,$(wildcard *_test.cpp)) gtest-all.o
DEPEND        = $(subst .o,.d,$(MAIN)) $(subst .o,.d,$(OBJECTS)) $(subst .o,.d,$(TESTS_OBJECTS))

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
	rm -f $(MAIN) $(OBJECTS) $(TESTS_OBJECTS) $(DEPEND)

clean-all:
	rm -rf *.trace *.odcpp *.dSYM *.o *.d *.s phi4-*

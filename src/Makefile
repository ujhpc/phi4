CC    := cc
DIM   := 3
NCOMP := 1
FLOAT := double

SED   := sed
MV    := mv

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
# non-ICC use ICC svml on SIMD
ifdef SIMD
LDFLAGS  += -lsvml
endif
endif

# define executable suffix
SUFFIX := -$(NCOMP)nc-$(DIM)d-$(FLOAT)
ifdef SIMD
SUFFIX := $(SUFFIX)-vec$(SIMD)
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

LDFLAGS  += -g -lpopt
CPPFLAGS += -MMD -DDIM=$(DIM) -DFLOAT=$(FLOAT) -DNCOMP=$(NCOMP) -DNAME=phi4$(SUFFIX)
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
OBJECTS   = sweep$(SUFFIX).o file$(CC_SUFFIX).o
ifdef SIMD
OBJECTS  += simd$(CC_SUFFIX).o random_simd$(SUFFIX).o
else
OBJECTS  += random$(SUFFIX).o
endif
TOBJECTS  = $(subst .cpp,.o,$(wildcard *_test.cpp)) gtest-all.o
DEPEND    = $(subst .o,.d,$(MAIN)) $(subst .o,.d,$(OBJECTS)) $(subst .o,.d,$(TESTS_OBJECTS))

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
	rm -rf *.trace *.odcpp *.dSYM *.o *.d *.s phi4-*

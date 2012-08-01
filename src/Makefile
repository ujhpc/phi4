CC       := icc
DIM      := 3
NCOMP    := 1
FLOAT    := double

ifneq ($(CC), icc)
CPPFLAGS += -O3 -g
else
CXX      := icpc
CFLAGS   += -DGTEST_HAS_TR1_TUPLE=1
CXXFLAGS += -DGTEST_HAS_TR1_TUPLE=0
CPPFLAGS += -fast -g
endif

SED := sed
MV  := mv

INCLUDE_DIRS+=-I${HOME}/local/include
LIB_DIRS+=-L${HOME}/local/lib

ARCH=sse3
CXXFLAGS+=-m${ARCH}
LDFLAGS+=-m${ARCH}

LDFLAGS+=${LIB_DIRS}
CXXFLAGS+=${INCLUDE_DIRS}

SUFFIX := -$(NCOMP)nc-$(DIM)d-$(FLOAT)-no-omp
ifdef OPENMP
SUFFIX := -$(NCOMP)nc-$(DIM)d-$(FLOAT)-omp
ifneq ($(CC), icc)
CXXFLAGS += -fopenmp
LDFLAGS  += -fopenmp
else
CXXFLAGS += -openmp
LDFLAGS  += -openmp
endif
endif


LDFLAGS      += -g -lpopt
CPPFLAGS     += -MMD -DDIM=$(DIM) -DFLOAT=$(FLOAT) -DNCOMP=$(NCOMP)
VPATH         = $(HOME)/downloads/gtest-1.6.0/src

EXEC          = phi4$(SUFFIX)
MAIN          = phi4$(SUFFIX).o
OBJECTS       = sweep$(SUFFIX).o random$(SUFFIX).o file.o
TESTS_OBJECTS = $(subst .cpp,.o,$(wildcard *_test.cpp)) gtest-all.o
DEPEND        = $(subst .o,.d,$(MAIN)) $(subst .o,.d,$(OBJECTS)) $(subst .o,.d,$(TESTS_OBJECTS))

$(EXEC): $(MAIN) $(OBJECTS)
	$(CXX) $(LDFLAGS) -o $(EXEC) $^

dsym: $(EXEC)
	dsymutil $(EXEC)

-include $(DEPEND)

%$(SUFFIX).o: %.cpp
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) -o $@ $<

test: $(OBJECTS) $(TESTS_OBJECTS) 
	$(CXX) $(LDFLAGS) -o test $^ -lpthread

clean:
	rm -f $(MAIN) $(OBJECTS) $(TESTS_OBJECTS) $(DEPEND)

clean-all:
	rm -rf *.trace *.odcpp *.dSYM *.o *.d *.s phi4-*-omp*

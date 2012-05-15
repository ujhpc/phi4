
ifeq "$(strip $(COMPILER))" "gcc"
CC=g++
CXX=g++
CXXFLAGS+=-O3 -g
else
CC=icc -DGTEST_HAS_TR1_TUPLE=0
CXX=icpc -DGTEST_HAS_TR1_TUPLE=0
CXXFLAGS+=-fast -g
endif


LD=$(CXX)
DP=$(CXX) -MM

SED=sed
MV=mv

SUFFIX=-no-omp
ifdef OPENMP
SUFFIX=-omp
ifeq "$(strip $(COMPILER))" "gcc"
CXXFLAGS+=-fopenmp
LDFLAGS +=-fopenmp
else
CXXFLAGS+=-openmp
LDFLAGS +=-openmp
endif
endif


LDFLAGS+=-lboost_program_options

VPATH=$(HOME)/downloads/gtest-1.6.0/src

CXXFLAGS +=-MMD

EXEC=phi4$(SUFFIX)
MAIN=phi4$(SUFFIX).o
OBJECTS=sweep$(SUFFIX).o random$(SUFFIX).o
TESTS_OBJECTS=$(subst .cpp,.o,$(wildcard *_test.cpp)) gtest-all.o

DEPEND=$(subst .o,.d,$(MAIN)) $(subst .o,.d,$(OBJECTS)) $(subst .o,.d,$(TESTS_OBJECTS))

$(EXEC): $(MAIN) $(OBJECTS)
	$(LD) $(LDFLAGS) -o $(EXEC) $^

-include $(DEPEND)

%$(SUFFIX).o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<

test: $(OBJECTS) $(TESTS_OBJECTS) 
	$(LD) $(LDFLAGS) -o test  $^  -lpthread

clean:
	rm -f *.o *.d
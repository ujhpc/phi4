CC=icc -DGTEST_HAS_TR1_TUPLE=0
CXX=icpc -DGTEST_HAS_TR1_TUPLE=0
LD=icpc
DP=icpc -MM
SED=sed
MV=mv

PREP_C =  perl -pe 's!(${notdir $*})\.o[ :]*!${dir $*}$$1.o $@ : !g' > $@

#  $(SED) 's,\($$(notdir $2)\) *:,$$(dir $2) $3: ,' > $3.tmp

define make-depend
  $(DP) $(CFLAGS) $(CXXFLAGS) $1 | \
  perl -pe 's!(${notdir $2})[ :]*!${dir $2}$$1 $3 : !g' > $3.tmp
  $(MV) $3.tmp $3
endef

%.o: %.cpp
	$(call make-depend,$<,$@,$(subst .o,.d,$@))
	$(CXX) -c $(CXXFLAGS) -o $@ $<



CXXFLAGS+=-fast 


MAIN=phi4.o
OBJECTS= sweep.o random.o
TESTS_OBJECTS=$(subst .cpp,.o,$(wildcard *_test.cpp)) gtest-all.o

DEPEND=$(subst .o,.d,$(MAIN)) $(subst .o,.d,$(OBJECTS)) $(subst .o,.d,$(TESTS_OBJECTS))

-include $(DEPEND)

build: $(MAIN) $(OBJECTS)
	$(LD) -o phi4 $^	

test: $(OBJECTS) $(TESTS_OBJECTS) 
	$(LD) -o test  $^  -lpthread
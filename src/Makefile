CC=icc
CXX=icpc 
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



OBJECTS=phi4.o sweep.o random.o

DEPEND=$(subst .o,.d,$(OBJECTS))

-include $(DEPEND)

build: $(OBJECTS)
	$(LD) -o phi4 $^	

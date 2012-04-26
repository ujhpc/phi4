CC=icc
CXX=icpc 
LD=icpc
DP=icpc -MM
SED=sed
MV=mv

define make-depend
  $(DP) $(CFLAGS) $(CXXFLAGS) $1 | \
  $(SED) 's,\($$(notdir $2)\) *:,$$(dir $2) $3: ,' > $3.tmp
  $(MV) $3.tmp $3
endef

%.o: %.cpp
	$(call make-depend,$<,$@,$(subst .o,.d,$@))
	$(CXX) -c $(CXXFLAGS) -o $@ $<



CXXFLAGS+=-fast 



OBJECTS=phi4.o sweep.o
DEPEND=$(subst .o,.d,$(OBJECTS))

-include $(DEPEND)

build: $(OBJECTS)
	$(LD) -o phi4 $^	

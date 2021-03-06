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
#   FAST_INDEXER=1 fast indexer (no memory lookup) size only power of 2
#   SVML_FIX=1 fix for older SVML libs (ICC<13) missing symbols
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
OS    := $(shell uname -s)
NAME  := phi4

COMPILERS := gcc-4.9 gcc-4.8 clang

# Architecture
CPPFLAGS += -march=$(ARCH)

# ICC specif flags
ifeq ($(findstring $(CC),icc),icc)
CPPFLAGS += -O$(O) -fno-alias -g \
            -fp-model fast=2
# GCC & Clang flags
else
CPPFLAGS += -O$(O) -g \
            -funsafe-math-optimizations \
            -ffast-math
# non-ICC use ICC svml on SIMD
ifneq ($(SIMD),)
LDFLAGS  += -lsvml
endif
ifeq ($(findstring $(CC),clang),clang)
CPPFLAGS += -fomit-frame-pointer
# fix Clang on Linux compilation
ifeq ($(OS),Linux)
CPPFLAGS += -D__extern_always_inline=inline
endif
endif
endif

# link to rt to get realtime clock
ifeq ($(OS),Linux)
LDFLAGS  += -lrt
endif

CXX := $(subst cc,c++,$(subst gcc,g++,$(subst icc,icpc,$(subst clang,clang++,$(CC)))))

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
FAST_INDEXER := 1
endif
ifneq ($(FAST_INDEXER),)
CPPFLAGS += -DFAST_INDEXER=1
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

# Legacy SIMD
ifneq ($(OLD),)
LEGACY := 1
endif
ifneq ($(LEGACY),)
SUFFIX := $(SUFFIX)-legacy
CPPFLAGS += -DLEGACY=1
endif

LDFLAGS  += -g
CPPFLAGS += -MMD \
            -DDIM=$(DIM) \
            -DFLOAT=$(FLOAT) \
            -DNCOMP=$(NCOMP) \
            -DNAME=$(NAME)$(SUFFIX)

OBJDIR   := .obj$(SUFFIX)
EXECDIR  := ..
EXEC     := $(EXECDIR)/$(NAME)$(SUFFIX)
MAIN     := $(OBJDIR)/$(NAME).o
OBJECTS  := $(OBJDIR)/sweep$(SRC_SUFFIX).o \
            $(OBJDIR)/random$(SRC_SUFFIX).o
ifneq ($(SVML_FIX),)
ifneq ($(SIMD),)
ifeq ($(OS),Linux)
OBJECTS  += $(OBJDIR)/libsvml_wrapper_linux$(CC_SUFFIX).o
endif
endif
endif
DEPEND   := $(patsubst %.o,%.d,$(MAIN)) \
            $(patsubst %.o,%.d,$(OBJECTS))

all: $(EXEC)

time:
	@echo -n "$(CC)	"
	@echo -n "SIMD=$(SIMD)	"
	@$(EXEC) --time -t 1000

define recurse
$1:
	@for cc in $$(COMPILERS); do \
		$$(MAKE) --no-print-directory CC=$$$$cc FIDX=1 SIMD=4 ARCH=corei7 $2; \
		$$(MAKE) --no-print-directory CC=$$$$cc FIDX=1 SIMD=4 ARCH=corei7 LEGACY=1 $2; \
		$$(MAKE) --no-print-directory CC=$$$$cc FIDX=1 SIMD=8 ARCH=corei7-avx $2; \
		$$(MAKE) --no-print-directory CC=$$$$cc FIDX=1 SIMD=8 ARCH=corei7-avx LEGACY=1 $2; \
		$$(MAKE) --no-print-directory CC=$$$$cc FIDX=1 SIMD=8 ARCH=core-avx2 $2; \
	done
endef

$(eval $(call recurse,time-all,time))
$(eval $(call recurse,build-all,))

$(EXEC): $(MAIN) $(OBJECTS)
	$(CXX) -o $(EXEC) $^ $(LDFLAGS)

dsym: $(EXEC)
	dsymutil $(EXEC)

-include $(DEPEND)

$(OBJDIR):
	mkdir -p $(OBJDIR)

ifneq ($(LEGACY),)
$(OBJDIR)/%.o: legacy/%.cpp Makefile $(OBJDIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<
endif

$(OBJDIR)/%.o: %.cpp Makefile $(OBJDIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

$(OBJDIR)/%.o: %.c Makefile $(OBJDIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(MAIN) $(OBJECTS) $(DEPEND)
	test -d $(OBJDIR) && rmdir $(OBJDIR) || true

clean-all:
	find . \( -name '*.o' -o -name '*.d' -o -name '*.s' \) -delete
	find . -name '.obj*' -type d -delete
	find .. -perm +0111 -name '$(NAME)-*' -delete
	test -d $(OBJDIR) && rmdir $(OBJDIR) || true

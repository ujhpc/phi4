// NOTE: This file is only for Linux and it is
// temporary solution for missing symbols when
// linking to SVML Intel library.

extern "C" {
#if !__INTEL_COMPILER&& __linux__
// We relay on Intel's svml library,
// unfortunatelly on older ICC it is crippled
// and requires some built-in function generated
// by ICC linker, if we are not using ICC let's
// pretend we have valid function.
int __intel_cpu_indicator = 0;
// CPU dispatcher function
void __intel_cpu_indicator_init(void) {
  // CPU_AVX
  // __intel_cpu_indicator = 0x20000;
  // CPU_SSE42
  __intel_cpu_indicator = 0x8000;
  // CPU_SSE4
  // __intel_cpu_indicator = 0x2000;
  // CPU_SSSE3
  // __intel_cpu_indicator = 0x1000;
  // CPU_SSE3
  __intel_cpu_indicator = 0x800;
  // CPU_SSE2 && !(CPU_SSE2_IS_SLOW) )
  // _intel_cpu_indicator = 0x200;
  // CPU_SSE
  // __intel_cpu_indicator = 0x80;
  // CPU_MMX2
  // __intel_cpu_indicator = 8;
  // __intel_cpu_indicator = 1;
}
#endif
}

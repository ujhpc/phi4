// NOTE: This file is only for Linux and it is
// temporary solution for missing symbols when
// linking to old SVML Intel library (ICC<13).

extern "C" {
#if !defined(__INTEL_COMPILER) && defined(__linux__)
// We relay on Intel's svml library,
// unfortunatelly on older ICC it is crippled
// and requires some built-in function generated
// by ICC linker, if we are not using ICC let's
// pretend we have valid function.
#if defined __AVX2__
int __intel_cpu_indicator = 0x40000;
#elif defined __AVX__
int __intel_cpu_indicator = 0x20000;
#elif defined __SSE4_2__
int __intel_cpu_indicator = 0x8000;
#elif defined __SSE4_1__
int __intel_cpu_indicator = 0x2000;
#elif defined __SSSE3__
int __intel_cpu_indicator = 0x1000;
#elif defined __SSE3__
int __intel_cpu_indicator = 0x800;
#elif defined __SSE2_MATH__  // !(CPU_SSE2_IS_SLOW)
int _intel_cpu_indicator = 0x200;
#elif defined __SSE__
int __intel_cpu_indicator = 0x80;
#elif defined __MMX__
int __intel_cpu_indicator = 8;
#else
int __intel_cpu_indicator = 1;
#endif

// CPU dispatcher function (dummy)
void __intel_cpu_indicator_init(void) {}
#endif
}

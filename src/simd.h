// Vector type wrappers for C++
//
// (c) 2012 Adam Strzelecki
//
// Created: Jun 29, 2012
//
// Description:
//   Both GCC and Clang define complete set of arithmetic, shift, bit and
//   comparison operators for vector extended types using
//   attribute((vector_size(...))). However GCC does not define shift and
//   comparison operators in C++ mode, Intel CC does not define them at all.
//   None define reliable single element and all elements constructors or index
//   accessors.
//   This class is drop in replecement for that, addressing above shortcommings.

#define HAS_CMP __clang__
#define HAS_SHIFT __clang__
#define HAS_BIT __clang__

#if __i386__ || __x86_64__
  #include <immintrin.h>
#endif

#define ARRAY(n, ...) __VA_ARGS__[n]
#define INDEX(n, ...) __VA_ARGS__##n
#define SCALAR(n, ...) __VA_ARGS__

#define REPn(n) REP##n
#define REP(n, f, ...) REPn(n)(f, __VA_ARGS__)
#define REP1(f, ...) f(0, __VA_ARGS__)
#define REP2(f, ...) f(0, __VA_ARGS__), f(1, __VA_ARGS__)
#define REP4(f, ...)                                                          \
  f(0, __VA_ARGS__), f(1, __VA_ARGS__), f(2, __VA_ARGS__), f(3, __VA_ARGS__)
#define REP8(f, ...)                                                          \
  f(0, __VA_ARGS__), f(1, __VA_ARGS__), f(2, __VA_ARGS__), f(3, __VA_ARGS__), \
      f(4, __VA_ARGS__), f(5, __VA_ARGS__), f(6, __VA_ARGS__),                \
      f(7, __VA_ARGS__)

template <typename type, int ncomp> struct simd {};

#define DEF(storage_type, mask_type, ncomp)                                   \
  template <> struct simd<storage_type, ncomp> { \
  typedef simd<storage_type, ncomp> this_class; \
  typedef storage_type scalar; \
  typedef storage_type type                                                   \
      __attribute__((vector_size(ncomp * sizeof(storage_type)))); \
  typedef mask_type mask_scalar; \
  typedef mask_type mask_vector                                               \
      __attribute__((vector_size(ncomp * sizeof(storage_type)))); \
  typedef simd<mask_type, ncomp> mask; \
  typedef this_class float_class; \
  type v; \
  simd<storage_type, ncomp>() {} \
  simd<storage_type, ncomp>(type s) : v(s) {} \
  simd<storage_type, ncomp>(scalar s) : v((type) { REP(ncomp, SCALAR, s) }) {} \
  simd<storage_type, ncomp>(REP(ncomp, INDEX, scalar s))                      \
      : v((type) { REP(ncomp, INDEX, s) }) {} \
  simd<storage_type, ncomp>(scalar* p) : v(*(type*)p) {} \
  scalar& operator[](int i) { return ((scalar*)&v)[i]; }                      \
  };                                                                          \
  template <> struct simd<mask_type, ncomp> { \
  typedef simd<mask_type, ncomp> this_class; \
  typedef mask_type scalar; \
  typedef mask_type type                                                      \
      __attribute__((vector_size(ncomp * sizeof(mask_type)))); \
  typedef mask_type mask_scalar; \
  typedef mask_type mask_vector                                               \
      __attribute__((vector_size(ncomp * sizeof(mask_type)))); \
  typedef simd<mask_type, ncomp> mask; \
  typedef simd<storage_type, ncomp> float_class; \
  type v; \
  simd<mask_type, ncomp>() {} \
  simd<mask_type, ncomp>(type s) : v(s) {} \
  simd<mask_type, ncomp>(scalar s) : v((type) { REP(ncomp, SCALAR, s) }) {} \
  simd<mask_type, ncomp>(REP(ncomp, INDEX, scalar s))                         \
      : v((type) { REP(ncomp, INDEX, s) }) {} \
  simd<mask_type, ncomp>(scalar* p) : v(*(type*)p) {} \
  scalar& operator[](int i) { return ((scalar*)&v)[i]; } \
  float_class operator()(const float_class& mv) {                             \
    return float_class((float_class::type)((type)mv.v & v));                  \
  }

#if __SIZEOF_INT__ == 4
  #define INT32 int
#elif __SIZEOF_LONG__ == 4
  #define INT32 long
#endif
#if __SIZEOF_LONG_LONG__ == 8
  #define INT64 long long
#endif
#if !defined INT32 || !defined INT64
  #include <stdint.h>
#endif
#if !defined INT32
  #define INT32 int32_t
#endif
#if !defined INT64
  #define INT64 int64_t
#endif

#define TPL template <typename storage_type, int ncomp>
#define TPL1 template <int ncomp>
#define VEC simd<storage_type, ncomp>
#define VECS simd<float, ncomp>
#define VECD simd<double, ncomp>
#define VEC4S simd<float, 4>
#define VEC8S simd<float, 8>
#define VEC2D simd<double, 2>
#define VEC4D simd<double, 4>
#define VEC4_32 simd<INT32, 4>
#define VEC8_32 simd<INT32, 8>
#define VEC2_64 simd<INT64, 2>
#define VEC4_64 simd<INT64, 4>
#define TYPE typename simd<storage_type, ncomp>::type
#define MASKVEC typename simd<storage_type, ncomp>::mask
#define MASK typename simd<storage_type, ncomp>::mask_vector
#define MASKSLR typename simd<storage_type, ncomp>::mask_scalar
#define TYPES typename simd<float, ncomp>::type
#define MASKVECS typename simd<float, ncomp>::mask
#define MASKS typename simd<float, ncomp>::mask_vector
#define MASKSLRS typename simd<float, ncomp>::mask_scalar
#define TYPED typename simd<double, ncomp>::type
#define MASKVECD typename simd<double, ncomp>::mask
#define MASKD typename simd<double, ncomp>::mask_vector
#define MASKSLRD typename simd<double, ncomp>::mask_scalar

// special blend operators aka IIF
DEF(float, INT32, 4)
#if __SSE4_1__
float_class operator()(const float_class& tv, const float_class& fv) {
  return float_class(_mm_blendv_ps(fv.v, tv.v, (float_class::type)v));
}
#elif __SSE__
float_class operator()(const float_class& tv, const float_class& fv) {
  return float_class((float_class::type)((~v & (type)fv.v) | (v & (type)tv.v)));
}
#endif
#if __SSE2__
operator float_class() { return float_class(_mm_cvtepi32_ps((__m128i)v)); }
#endif
}
;
DEF(float, INT32, 8)
#if __AVX__
float_class operator()(const float_class& tv, const float_class& fv) {
  return float_class(_mm256_blendv_ps(fv.v, tv.v, (float_class::type)v));
}
operator float_class() { return float_class(_mm256_cvtepi32_ps((__m256i)v)); }
#endif
}
;
DEF(double, INT64, 2)
}
;
DEF(double, INT64, 4)
}
;

// arithmetic operators
TPL VEC operator+(const VEC& a, const VEC& b) { return VEC(a.v + b.v); }
TPL VEC operator-(const VEC& a, const VEC& b) { return VEC(a.v - b.v); }
TPL VEC operator*(const VEC& a, const VEC& b) { return VEC(a.v * b.v); }
TPL VEC operator/(const VEC& a, const VEC& b) { return VEC(a.v / b.v); }

TPL void operator+=(VEC& a, const VEC& b) { a.v += b.v; }
TPL void operator-=(VEC& a, const VEC& b) { a.v -= b.v; }
TPL void operator*=(VEC& a, const VEC& b) { a.v *= b.v; }
TPL void operator/=(VEC& a, const VEC& b) { a.v /= b.v; }

// bit operators
TPL VEC operator~(const VEC& v) { return VEC(~v.v); }
TPL VEC operator^(const VEC& a, const VEC& b) { return VEC(a.v ^ b.v); }
TPL VEC operator&(const VEC& a, const VEC& b) { return VEC(a.v & b.v); }
TPL VEC operator|(const VEC& a, const VEC& b) { return VEC(a.v | b.v); }

// comparison operators
#if HAS_CMP
TPL MASKVEC operator<(const VEC& a, const VEC& b) { return MASKVEC(a.v < b.v); }
TPL MASKVEC operator<=(const VEC& a, const VEC& b) {
  return MASKVEC(a.v <= b.v);
}
TPL MASKVEC operator>=(const VEC& a, const VEC& b) {
  return MASKVEC(a.v >= b.v);
}
TPL MASKVEC operator>(const VEC& a, const VEC& b) { return MASKVEC(a.v > b.v); }
TPL MASKVEC operator==(const VEC& a, const VEC& b) {
  return MASKVEC(a.v == b.v);
}
TPL MASKVEC operator!=(const VEC& a, const VEC& b) {
  return MASKVEC(a.v != b.v);
}
#else
#if __SSE__
#if defined(__INTEL_COMPILER)
  #pragma warning(push)
  #pragma warning(disable : 171)  // warning #171: invalid type conversion
  #pragma warning(disable : 2496)  // warning #2496: conversion between \
                                   // incompatible vector types
#endif  // __INTEL_COMPILER
static inline VEC4_32 operator<(const VEC4S& a, const VEC4S& b) {
  return VEC4_32((__v4si)_mm_cmplt_ps(a.v, b.v));
}
static inline VEC4_32 operator<=(const VEC4S& a, const VEC4S& b) {
  return VEC4_32((__v4si)_mm_cmple_ps(a.v, b.v));
}
static inline VEC4_32 operator>=(const VEC4S& a, const VEC4S& b) {
  return VEC4_32((__v4si)_mm_cmpge_ps(a.v, b.v));
}
static inline VEC4_32 operator>(const VEC4S& a, const VEC4S& b) {
  return VEC4_32((__v4si)_mm_cmpgt_ps(a.v, b.v));
}
static inline VEC4_32 operator==(const VEC4S& a, const VEC4S& b) {
  return VEC4_32((__v4si)_mm_cmpeq_ps(a.v, b.v));
}
static inline VEC4_32 operator!=(const VEC4S& a, const VEC4S& b) {
  return VEC4_32((__v4si)_mm_cmpneq_ps(a.v, b.v));
}

static inline VEC2_64 operator<(const VEC2D& a, const VEC2D& b) {
  return VEC2_64((__v2di)_mm_cmplt_pd(a.v, b.v));
}
static inline VEC2_64 operator<=(const VEC2D& a, const VEC2D& b) {
  return VEC2_64((__v2di)_mm_cmple_pd(a.v, b.v));
}
static inline VEC2_64 operator>=(const VEC2D& a, const VEC2D& b) {
  return VEC2_64((__v2di)_mm_cmpge_pd(a.v, b.v));
}
static inline VEC2_64 operator>(const VEC2D& a, const VEC2D& b) {
  return VEC2_64((__v2di)_mm_cmpgt_pd(a.v, b.v));
}
static inline VEC2_64 operator==(const VEC2D& a, const VEC2D& b) {
  return VEC2_64((__v2di)_mm_cmpeq_pd(a.v, b.v));
}
static inline VEC2_64 operator!=(const VEC2D& a, const VEC2D& b) {
  return VEC2_64((__v2di)_mm_cmpneq_pd(a.v, b.v));
}
#endif
#if __AVX__
static inline VEC8_32 operator<(const VEC8S& a, const VEC8S& b) {
  return VEC8_32((__v8si)_mm256_cmp_ps(a.v, b.v, _CMP_LT_OS));
}
static inline VEC8_32 operator<=(const VEC8S& a, const VEC8S& b) {
  return VEC8_32((__v8si)_mm256_cmp_ps(a.v, b.v, _CMP_LE_OS));
}
static inline VEC8_32 operator>=(const VEC8S& a, const VEC8S& b) {
  return VEC8_32((__v8si)_mm256_cmp_ps(a.v, b.v, _CMP_GE_OS));
}
static inline VEC8_32 operator>(const VEC8S& a, const VEC8S& b) {
  return VEC8_32((__v8si)_mm256_cmp_ps(a.v, b.v, _CMP_GT_OS));
}
static inline VEC8_32 operator==(const VEC8S& a, const VEC8S& b) {
  return VEC8_32((__v8si)_mm256_cmp_ps(a.v, b.v, _CMP_EQ_OS));
}
static inline VEC8_32 operator!=(const VEC8S& a, const VEC8S& b) {
  return VEC8_32((__v8si)_mm256_cmp_ps(a.v, b.v, _CMP_NEQ_OS));
}

static inline VEC4_64 operator<(const VEC4D& a, const VEC4D& b) {
  return VEC4_64((__v4di)_mm256_cmp_pd(a.v, b.v, _CMP_LT_OS));
}
static inline VEC4_64 operator<=(const VEC4D& a, const VEC4D& b) {
  return VEC4_64((__v4di)_mm256_cmp_pd(a.v, b.v, _CMP_LE_OS));
}
static inline VEC4_64 operator>=(const VEC4D& a, const VEC4D& b) {
  return VEC4_64((__v4di)_mm256_cmp_pd(a.v, b.v, _CMP_GE_OS));
}
static inline VEC4_64 operator>(const VEC4D& a, const VEC4D& b) {
  return VEC4_64((__v4di)_mm256_cmp_pd(a.v, b.v, _CMP_GT_OS));
}
static inline VEC4_64 operator==(const VEC4D& a, const VEC4D& b) {
  return VEC4_64((__v4di)_mm256_cmp_pd(a.v, b.v, _CMP_EQ_OS));
}
static inline VEC4_64 operator!=(const VEC4D& a, const VEC4D& b) {
  return VEC4_64((__v4di)_mm256_cmp_pd(a.v, b.v, _CMP_NEQ_OS));
}
#if defined(__INTEL_COMPILER)
  #pragma warning(pop)
#endif  // __INTEL_COMPILER
#endif
#endif

// shift operators
#if HAS_SHIFT
TPL VEC operator<<(const VEC& v, const MASKVEC& s) {
  return VEC((TYPE)((MASK)v.v << s.v));
}
TPL VEC operator>>(const VEC& v, const MASKVEC& s) {
  return VEC((TYPE)((MASK)v.v >> s.v));
}
TPL VEC operator<<(const VEC& v, const int s) {
  return VEC((TYPE)((MASK)v.v << s));
}
TPL VEC operator>>(const VEC& v, const int s) {
  return VEC((TYPE)((MASK)v.v >> s));
}
#elif __SSE2__
static inline VEC4_32 operator<<(const VEC4_32& v, const VEC4_32& s) {
  return VEC4_32((__v4si)_mm_sll_epi32((__m128i)v.v, (__m128i)s.v));
}
static inline VEC4_32 operator>>(const VEC4_32& v, const VEC4_32& s) {
  return VEC4_32((__v4si)_mm_srl_epi32((__m128i)v.v, (__m128i)s.v));
}
static inline VEC4_32 operator<<(const VEC4_32& v, const int s) {
  return VEC4_32((__v4si)_mm_slli_epi32((__m128i)v.v, s));
}
static inline VEC4_32 operator>>(const VEC4_32& v, const int s) {
  return VEC4_32((__v4si)_mm_srli_epi32((__m128i)v.v, s));
}
static inline VEC2_64 operator<<(const VEC2_64& v, const VEC2_64& s) {
  return VEC2_64(_mm_sll_epi64(v.v, s.v));
}
static inline VEC2_64 operator>>(const VEC2_64& v, const VEC2_64& s) {
  return VEC2_64(_mm_srl_epi64(v.v, s.v));
}
static inline VEC2_64 operator<<(const VEC2_64& v, const int s) {
  return VEC2_64(_mm_slli_epi64(v.v, s));
}
static inline VEC2_64 operator>>(const VEC2_64& v, const int s) {
  return VEC2_64(_mm_srli_epi64(v.v, s));
}
#endif

#if __SSE__
static inline VEC4S min(const VEC4S& a, const VEC4S& b) {
  return VEC4S(_mm_min_ps(a.v, b.v));
}
static inline VEC2D min(const VEC2D& a, const VEC2D& b) {
  return VEC2D(_mm_min_pd(a.v, b.v));
}
static inline VEC4S max(const VEC4S& a, const VEC4S& b) {
  return VEC4S(_mm_max_ps(a.v, b.v));
}
static inline VEC2D max(const VEC2D& a, const VEC2D& b) {
  return VEC2D(_mm_max_pd(a.v, b.v));
}
extern "C" {
  __m128 __svml_logf4(__m128);
  __m128d __svml_logd2(__m128d);
}
static inline VEC4S log(const VEC4S& v) { return VEC4S(__svml_logf4(v.v)); }
static inline VEC2D log(const VEC2D& v) { return VEC2D(__svml_logd2(v.v)); }
#endif
#if __AVX__
static inline VEC8S min(const VEC8S& a, const VEC8S& b) {
  return VEC8S(_mm256_min_ps(a.v, b.v));
}
static inline VEC4D min(const VEC4D& a, const VEC4D& b) {
  return VEC4D(_mm256_min_pd(a.v, b.v));
}
static inline VEC8S max(const VEC8S& a, const VEC8S& b) {
  return VEC8S(_mm256_max_ps(a.v, b.v));
}
static inline VEC4D max(const VEC4D& a, const VEC4D& b) {
  return VEC4D(_mm256_max_pd(a.v, b.v));
}
extern "C" {
  __m256 __svml_logf8(__m256);
  __m256d __svml_logd4(__m256d);
}
static inline VEC8S log(const VEC8S& v) { return VEC8S(__svml_logf8(v.v)); }
static inline VEC4D log(const VEC4D& v) { return VEC4D(__svml_logd4(v.v)); }
#endif

// undefine all used macros (and make them thus private)
#undef ARRAY
#undef SCALAR

#ifndef __SIMD_H__REP
#undef REPn
#undef REP
#undef REP1
#undef REP2
#undef REP4
#undef REP8
#endif

#undef TPL
#undef TPL1
#undef VEC
#undef VECS
#undef VECD
#undef VEC4_32
#undef VEC8_32
#undef VEC2_64
#undef VEC4_64
#undef TYPE
#undef MASKVEC
#undef MASK
#undef MASKSLR
#undef TYPES
#undef MASKVECS
#undef MASKS
#undef MASKSLRS
#undef TYPED
#undef MASKVECD
#undef MASKD
#undef MASKSLRD

#undef HAS_CMP
#undef HAS_SHIFT

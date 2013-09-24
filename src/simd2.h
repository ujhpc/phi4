// Vector type wrappers for C++ (version 2.0)
//
// (c) 2012-2013 Adam Strzelecki
//
// Updated: Sep 10, 2013
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

#pragma once

#if __i386__ || __x86_64__
#include <immintrin.h>
#endif

// Helper type for itegral <-> float mapping with same size
template <typename Type> struct simd_integral_of {
  typedef Type type;
  typedef Type utype;
};
template <> struct simd_integral_of<float> {
  typedef int type;
  typedef unsigned int utype;
};
template <> struct simd_integral_of<double> {
  typedef long long type;
  typedef unsigned long long utype;
};
template <> struct simd_integral_of<int> {
  typedef int type;
  typedef unsigned int utype;
};
template <> struct simd_integral_of<long long> {
  typedef long long type;
  typedef unsigned long long utype;
};
template <typename Type> struct simd_float_of {
  typedef Type type;
};
template <> struct simd_float_of<int> {
  typedef float type;
};
template <> struct simd_float_of<long long> {
  typedef double type;
};

// Clang workaround: http://llvm.org/bugs/show_bug.cgi?id=16986
#if __clang__
#define __simd_type(T, N) __attribute__((ext_vector_type(N))) T
#else
#define __simd_type(T, N) __attribute__((vector_size(sizeof(T) * N))) T
#endif

// Argument replication macros
#define __simd_repn(n) __simd_rep##n
#define __simd_rep(n, f, ...) __simd_repn(n)(f, __VA_ARGS__)
#define __simd_rep1(f, ...) f(0, __VA_ARGS__)
#define __simd_rep2(f, ...) f(0, __VA_ARGS__), f(1, __VA_ARGS__)
#define __simd_rep4(f, ...) \
  f(0, __VA_ARGS__), f(1, __VA_ARGS__), f(2, __VA_ARGS__), f(3, __VA_ARGS__)
#define __simd_rep8(f, ...)                                                   \
  f(0, __VA_ARGS__), f(1, __VA_ARGS__), f(2, __VA_ARGS__), f(3, __VA_ARGS__), \
      f(4, __VA_ARGS__), f(5, __VA_ARGS__), f(6, __VA_ARGS__),                \
      f(7, __VA_ARGS__)
#define __simd_rep16(f, ...)                                                  \
  f(0, __VA_ARGS__), f(1, __VA_ARGS__), f(2, __VA_ARGS__), f(3, __VA_ARGS__), \
      f(4, __VA_ARGS__), f(5, __VA_ARGS__), f(6, __VA_ARGS__),                \
      f(7, __VA_ARGS__), f(8, __VA_ARGS__), f(9, __VA_ARGS__),                \
      f(10, __VA_ARGS__), f(11, __VA_ARGS__), f(12, __VA_ARGS__),             \
      f(13, __VA_ARGS__), f(14, __VA_ARGS__), f(15, __VA_ARGS__)

#define __simd_array(n, ...) __VA_ARGS__[n]
#define __simd_iarray(n, i, ...) __VA_ARGS__[i[n]]
#define __simd_index(n, ...) __VA_ARGS__##n
#define __simd_scalar(n, ...) __VA_ARGS__

#ifdef __INTEL_COMPILER
#pragma warning(push)
// warning #597: "simd<T, N>::operator simd<float, 2>() const
// will not be called for implicit or explicit conversions
#pragma warning(disable : 597)
// warning #171: invalid type conversion ...
#pragma warning(disable : 171)
#endif

/// Main SIMD wrapper template type for vector
template <typename T, int N> struct simd {
  /// Number of vector components
  static const int vector_size = sizeof(T) * N;
  typedef T scalar_t;
  typedef typename simd_integral_of<T>::type iscalar_t;
  typedef typename simd_integral_of<T>::utype uiscalar_t;
  typedef typename simd_float_of<T>::type fscalar_t;
  typedef __simd_type(scalar_t, N) vector_t;
  typedef __simd_type(iscalar_t, N) ivector_t;
  typedef __simd_type(fscalar_t, N) fvector_t;
  typedef simd<scalar_t, N> type;
  typedef simd<iscalar_t, N> itype;
  typedef simd<fscalar_t, N> ftype;
  vector_t v;

  /// Default constructor returning zero
  simd();
  /// Constructor taking native representation
  simd(vector_t a) : v(a) {}

  // Constructors taking vector size number of arguments
  simd(__simd_rep(2, __simd_index, T s));
  simd(__simd_rep(4, __simd_index, T s));
  simd(__simd_rep(8, __simd_index, T s));
  simd(__simd_rep(16, __simd_index, T s));

  // We need dummy here otherwise GCC will not allow us override contructor even
  // vector and scalar types are different.
  simd(scalar_t, char dummy = 0);
  /// Load from memory constructor
  simd(scalar_t* ptr) : v(*(vector_t*)ptr) {}
  /// Unsigned to signed constructor
  simd(uiscalar_t u, char dummy = 0) { v = simd<iscalar_t, N>((iscalar_t)u).v; }
  /// Gather
  simd(const scalar_t* ptr,
       const itype& index,
       const itype& mask = itype(-1),
       const simd& def = simd((scalar_t)0)) {
    for (int i = 0; i < N; ++i)
      (*this)[i] = ptr[index[i]];
  }

  scalar_t& operator[](const int i) { return ((scalar_t*)&v)[i]; }
  scalar_t operator[](const int i) const { return ((scalar_t*)&v)[i]; }

  simd operator+(const simd a) const { return simd(v + a.v); }
  simd operator-(const simd a) const { return simd(v - a.v); }
  simd operator*(const simd a) const { return simd(v * a.v); }
  simd operator/(const simd a) const { return simd(v / a.v); }

  simd operator~() const { return simd(~v); }
  simd operator|(const simd& a) const { return simd(v | a.v); }
  simd operator&(const simd& a) const { return simd(v & a.v); }
  simd operator^(const simd& a) const { return simd(v ^ a.v); }

  simd operator<<(const int shift) const { return simd(v << shift); }
  simd operator>>(const int shift) const { return simd(v >> shift); }
  simd operator<<(const simd& shift) const { return simd(v << shift.v); }
  simd operator>>(const simd& shift) const { return simd(v >> shift.v); }

  itype operator!=(const simd& a) const { return itype(v != a); }
  itype operator<(const simd& a) const { return itype(v < a); }
  itype operator>(const simd& a) const { return itype(v > a); }
  itype operator<=(const simd& a) const { return itype(v <= a); }
  itype operator>=(const simd& a) const { return itype(v >= a); }
  itype operator==(const simd& a) const { return itype(v == a); }
  simd operator!() const { return simd(!v); }

  operator itype() const;
  operator ftype() const;

  // Masking operator
  ftype operator()(const ftype& tval, const ftype& fval) const { return tval; }
  ftype operator()(const ftype& tval) const { return (*this)(tval, ftype()); }

// Assign operators
#define __simd_aop(T, O)     \
  simd& operator O##=(T a) { \
    v = (operator O(a)).v;   \
    return *this;            \
  }
  __simd_aop(const simd&, +)
  __simd_aop(const simd&, -)
  __simd_aop(const simd&, *)
  __simd_aop(const simd&, / )

  __simd_aop(const simd&, | )
  __simd_aop(const simd&, &)
  __simd_aop(const simd&, ^)
// Workaround for GCC <= 4.6
#if __GNUC__ < 4 || __GNUC__ == 4 && __GNUC_MINOR__ <= 6
#undef __simd_aop
#define __simd_aop(T, O) \
  simd& operator O##=(T a) { return *this; }
#endif
  __simd_aop(const int, << )
  __simd_aop(const int, >> )
  __simd_aop(const simd&, >> )
  __simd_aop(const simd&, << )
#undef __simd_aop
};
#undef __simd_type

// This is mumbo-jumbo generating scalar to vector constructors.
// Thanks to that we can get simd<int, 4>(1) -> {1, 1, 1, 1}
#define __simd_ctor_(T, N)                                                  \
  template <>                                                               \
  inline simd<T, N>::simd()                                                 \
      : v((simd<T, N>::vector_t) { __simd_rep(N, __simd_scalar, (T)0) }) {} \
  template <>                                                               \
  inline simd<T, N>::simd(T s, char)                                        \
      : v((simd<T, N>::vector_t) { __simd_rep(N, __simd_scalar, s) }) {}    \
  template <>                                                               \
  inline simd<T, N>::simd(__simd_rep(N, __simd_index, T s))                 \
      : v((simd<T, N>::vector_t) { __simd_rep(N, __simd_index, s) }) {}
#define __simd_ctor(TI, T, N) __simd_ctor_(T, N) __simd_ctor_(TI, N)
// single precision
__simd_ctor(int, float, 4)
#ifdef __AVX__
__simd_ctor(int, float, 8)
#endif
// double precision
__simd_ctor(long long, double, 2)
#ifdef __AVX__
__simd_ctor(long long, double, 4)
#endif
#undef __simd_ctor_
#undef __simd_ctor

// Add gather constructors
#ifdef __AVX2__
#define __simd_gather_impl(T, TF, N, I, ...) \
  (simd<T, N>::vector_t) I(__VA_ARGS__)
#else
#define __simd_gather_impl(T, TF, N, I, ...) \
  (simd<T, N>::vector_t) { __simd_rep(N, __simd_iarray, index, ptr) }
#endif

#define __simd_gather_(T, TF, N, I, ...)                   \
  template <>                                              \
  inline simd<T, N>::simd(const simd<T, N>::scalar_t* ptr, \
                          const simd<T, N>::itype& index,  \
                          const simd<T, N>::itype& mask,   \
                          const simd<T, N>& def)           \
      : v(__simd_gather_impl(T, TF, N, I, __VA_ARGS__)) {}
#define __simd_gather(TI, T, N, I, ...)   \
  __simd_gather_(T, T, N, I, __VA_ARGS__) \
      __simd_gather_(TI, T, N, I, __VA_ARGS__)
// single precision
__simd_gather(int,
              float,
              4,
              _mm_mask_i32gather_ps,
              (__m128)def.v,
              (const float*)ptr,
              (__m128i)index.v,
              (__m128)mask.v,
              4)
#ifdef __AVX__
__simd_gather(int,
              float,
              8,
              _mm256_mask_i32gather_ps,
              (__m256)def.v,
              (const float*)ptr,
              (__m256i)index.v,
              (__m256)mask.v,
              8)
#endif
// double precision
__simd_gather(long long,
              double,
              2,
              _mm_mask_i64gather_pd,
              (__m128d)def.v,
              (const double*)ptr,
              (__m128i)index.v,
              (__m128d)mask.v,
              2)
#ifdef __AVX__
__simd_gather(long long,
              double,
              4,
              _mm256_mask_i64gather_pd,
              (__m256d)def.v,
              (const double*)ptr,
              (__m256i)index.v,
              (__m256d)mask.v,
              4)
#endif
#undef __simd_gather
#undef __simd_gather_impl
#undef __simd_scatter
#undef __simd_gather_scatter

// Drop replication macros
#ifndef __simd_use_rep
#undef __simd_rep
#undef __simd_repn
#undef __simd_rep1
#undef __simd_rep2
#undef __simd_rep4
#undef __simd_rep8
#undef __simd_rep16
#undef __simd_array
#undef __simd_parray
#undef __simd_index
#undef __simd_scalar
#endif

// This section defines missing operators using X86 intrinsics
#define __simd_has_cmp __clang__
#define __simd_has_shift __clang__
#define __simd_has_bitop __clang__

// Add missing comparsion type built-ins
#if !__simd_has_cmp
#define __simd_op_(T, N, O, V, I, ...)                                         \
  template <>                                                                  \
  inline simd<T, N>::itype simd<T, N>::operator O(const simd<T, N>& a) const { \
    return simd<T, N>::itype((V)I(__VA_ARGS__));                               \
  }
#define __simd_op(T, N, O, V, I) __simd_op_(T, N, O, V, I, v, a.v)
__simd_op(float, 4, <, __v4si, _mm_cmplt_ps)
__simd_op(float, 4, <=, __v4si, _mm_cmple_ps)
__simd_op(float, 4, >=, __v4si, _mm_cmpge_ps)
__simd_op(float, 4, >, __v4si, _mm_cmpgt_ps)
__simd_op(float, 4, ==, __v4si, _mm_cmpeq_ps)
__simd_op(float, 4, !=, __v4si, _mm_cmpneq_ps)
#undef __simd_op
#if __AVX__
#define __simd_op(T, N, O, V, I, X) __simd_op_(T, N, O, V, I, v, a.v, X)
__simd_op(float, 8, <, __v8si, _mm256_cmp_ps, _CMP_LT_OS)
__simd_op(float, 8, <=, __v8si, _mm256_cmp_ps, _CMP_LE_OS)
__simd_op(float, 8, >=, __v8si, _mm256_cmp_ps, _CMP_GE_OS)
__simd_op(float, 8, >, __v8si, _mm256_cmp_ps, _CMP_GT_OS)
__simd_op(float, 8, ==, __v8si, _mm256_cmp_ps, _CMP_EQ_OS)
__simd_op(float, 8, !=, __v8si, _mm256_cmp_ps, _CMP_NEQ_OS)
#undef __simd_op
#endif
#undef __simd_op_
#endif

// Add bit shift type built-ins
#if !__simd_has_shift
// Shift by scalar
#define __simd_op(T, N, O, V, IV, I)                                          \
  template <> inline simd<T, N> simd<T, N>::operator O(const T shift) const { \
    return simd<T, N>((V)I((IV)v, shift));                                    \
  }                                                                           \
  template <> inline simd<T, N>& simd<T, N>::operator O##=(const T shift) {   \
    v = ((V)I((IV)v, shift));                                                 \
    return *this;                                                             \
  }
__simd_op(int, 4, <<, __v4si, __m128i, _mm_slli_epi32)
__simd_op(int, 4, >>, __v4si, __m128i, _mm_srli_epi32)
#if __AVX2__
__simd_op(int, 8, <<, __v8si, __m256i, _mm256_slli_epi32)
__simd_op(int, 8, >>, __v8si, __m256i, _mm256_srli_epi32)
#endif
#undef __simd_op
// Shift by vector
#define __simd_op(T, N, O, V, IV, S, I)                                     \
  template <>                                                               \
  inline simd<T, N> simd<T, N>::operator O(const simd<T, N>& shift) const { \
    return simd<T, N>((V)I((IV)v, S));                                      \
  }
__simd_op(int, 4, <<, __v4si, __m128i, (__m128i)shift.v, _mm_sll_epi32)
__simd_op(int, 4, >>, __v4si, __m128i, (__m128i)shift.v, _mm_srl_epi32)
#if __AVX2__
// FIXME: this is probably incorrect since shift argument is 128-bit!
__simd_op(int, 8, <<, __v8si, __m256i, *((__m128i*)&shift.v), _mm256_sll_epi32)
__simd_op(int, 8, >>, __v8si, __m256i, *((__m128i*)&shift.v), _mm256_srl_epi32)
#endif
#undef __simd_op
#endif

// Conversion
#define __simd_cast(F, T, N, V, I)                             \
  template <> inline simd<F, N>::operator simd<T, N>() const { \
    return simd<T, N>((simd<T, N>::vector_t)I((V)v));          \
  }
#if __SSE2__
__simd_cast(int, float, 4, __m128i, _mm_cvtepi32_ps)
__simd_cast(float, int, 4, __m128, _mm_cvtps_epi32)
#endif
#if __AVX__
__simd_cast(int, float, 8, __m256i, _mm256_cvtepi32_ps)
__simd_cast(float, int, 8, __m256, _mm256_cvtps_epi32)
#endif
#undef __simd_cast

// Blend (conditional assign) operator
#define __simd_op(T, IT, N, I)                                            \
  template <>                                                             \
  inline simd<T, N> simd<IT, N>::operator()(const simd<T, N>& tv,         \
                                            const simd<T, N>& fv) const { \
    return simd<T, N>(I);                                                 \
  }
#if __SSE4_1__
__simd_op(float, int, 4, _mm_blendv_ps(fv.v, tv.v, (__m128)v))
__simd_op(double, long long, 2, _mm_blendv_pd(fv.v, tv.v, (__m128d)v))
#elif __SSE__
__simd_op(float, int, 4, (__m128)((~v&(__v4si)fv.v) | (v&(__v4si)tv.v)))
#endif
#if __AVX__
__simd_op(float, int, 8, _mm256_blendv_ps(fv.v, tv.v, (__m256)v))
__simd_op(double, long long, 4, _mm256_blendv_pd(fv.v, tv.v, (__m256d)v))
#endif
#undef __simd_op

// Finally some helper functions
#define __simd_fun(F, T, N, I) \
  inline simd<T, N> F(const simd<T, N>& a) { return simd<T, N>(I(a.v)); }
// SVML
extern "C" {
__m128 __svml_logf4(__m128);
__m128d __svml_logd2(__m128d);
#if __AVX__
__m256 __svml_logf8(__m256);
__m256d __svml_logd4(__m256d);
#endif
}
__simd_fun(log, float, 4, __svml_logf4)
__simd_fun(log, double, 2, __svml_logd2)
#if __AVX__
__simd_fun(log, float, 8, __svml_logf8)
__simd_fun(log, double, 4, __svml_logd4)
#endif
#undef __simd_fun

#define __simd_fun2(F, T, N, I)                                   \
  inline simd<T, N> F(const simd<T, N>& a, const simd<T, N>& b) { \
    return simd<T, N>(I(a.v, b.v));                               \
  }
__simd_fun2(min, float, 4, _mm_min_ps)
__simd_fun2(max, float, 4, _mm_max_ps)
__simd_fun2(min, double, 2, _mm_min_pd)
__simd_fun2(max, double, 2, _mm_max_pd)
#if __AVX__
__simd_fun2(min, float, 8, _mm256_min_ps)
__simd_fun2(max, float, 8, _mm256_max_ps)
__simd_fun2(min, double, 4, _mm256_min_pd)
__simd_fun2(max, double, 4, _mm256_max_pd)
#endif
#undef __simd_fun2

#undef __simd_has_cmp
#undef __simd_has_shift
#undef __simd_has_bitop

#if defined(__INTEL_COMPILER)
#pragma warning(pop)
#endif

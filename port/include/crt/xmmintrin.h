/* Portable SSE/MMX intrinsic subset for the native guest build.

   Recovered source keeps the retail build's `<xmmintrin.h>` usage, including
   MSVC's `m128_f32`/`m64_u16` union members. This header provides the same
   types and the intrinsics the recovered code uses, written from the public
   Intel intrinsic semantics in plain C so the ARM guest can compile them.
   Approximation instructions (`rsqrtss`) are computed exactly; MXCSR is kept
   as an emulated register without changing host floating-point behavior. */
#ifndef H2_PORT_XMMINTRIN_H
#define H2_PORT_XMMINTRIN_H

#if !H2_NATIVE_PORT
#error "port/include/crt/xmmintrin.h is only for the native port build"
#endif

typedef union __declspec(align(16)) __m128
{
	float m128_f32[4];
	unsigned long long m128_u64[2];
	signed char m128_i8[16];
	short m128_i16[8];
	int m128_i32[4];
	long long m128_i64[2];
	unsigned char m128_u8[16];
	unsigned short m128_u16[8];
	unsigned int m128_u32[4];
} __m128;

typedef union __declspec(align(8)) __m64
{
	unsigned long long m64_u64;
	float m64_f32[2];
	signed char m64_i8[8];
	short m64_i16[4];
	int m64_i32[2];
	long long m64_i64;
	unsigned char m64_u8[8];
	unsigned short m64_u16[4];
	unsigned int m64_u32[2];
} __m64;

#define H2_SSE static inline __attribute__((always_inline))

#define _MM_SHUFFLE(z, y, x, w) (((z) << 6) | ((y) << 4) | ((x) << 2) | (w))

#define _MM_HINT_T0 1
#define _MM_HINT_T1 2
#define _MM_HINT_T2 3
#define _MM_HINT_NTA 0

#define _mm_prefetch(address, hint) __builtin_prefetch((char const *)(address))

H2_SSE float h2_sse_sqrtf(float value) { return __builtin_sqrtf(value); }

H2_SSE __m128 h2_sse_make(float x, float y, float z, float w)
{
	__m128 result;
	result.m128_f32[0] = x;
	result.m128_f32[1] = y;
	result.m128_f32[2] = z;
	result.m128_f32[3] = w;
	return result;
}

H2_SSE __m128 _mm_setzero_ps(void) { return h2_sse_make(0.0f, 0.0f, 0.0f, 0.0f); }
H2_SSE __m128 _mm_set_ss(float a) { return h2_sse_make(a, 0.0f, 0.0f, 0.0f); }
H2_SSE __m128 _mm_set_ps1(float a) { return h2_sse_make(a, a, a, a); }
H2_SSE __m128 _mm_set1_ps(float a) { return h2_sse_make(a, a, a, a); }
H2_SSE __m128 _mm_set_ps(float z, float y, float x, float w) { return h2_sse_make(w, x, y, z); }
H2_SSE __m128 _mm_setr_ps(float w, float x, float y, float z) { return h2_sse_make(w, x, y, z); }

H2_SSE __m128 _mm_load_ps(float const *p) { return h2_sse_make(p[0], p[1], p[2], p[3]); }
H2_SSE __m128 _mm_loadu_ps(float const *p) { return h2_sse_make(p[0], p[1], p[2], p[3]); }
H2_SSE __m128 _mm_load_ss(float const *p) { return h2_sse_make(p[0], 0.0f, 0.0f, 0.0f); }
H2_SSE __m128 _mm_load1_ps(float const *p) { return h2_sse_make(p[0], p[0], p[0], p[0]); }
#define _mm_load_ps1 _mm_load1_ps

H2_SSE void _mm_store_ps(float *p, __m128 a)
{
	p[0] = a.m128_f32[0];
	p[1] = a.m128_f32[1];
	p[2] = a.m128_f32[2];
	p[3] = a.m128_f32[3];
}
#define _mm_storeu_ps _mm_store_ps
H2_SSE void _mm_store_ss(float *p, __m128 a) { p[0] = a.m128_f32[0]; }
H2_SSE float _mm_cvtss_f32(__m128 a) { return a.m128_f32[0]; }

#define H2_SSE_PACKED(name, expression) \
	H2_SSE __m128 name(__m128 a, __m128 b) \
	{ \
		__m128 r; \
		for (int i = 0; i < 4; i++) \
		{ \
			float x = a.m128_f32[i]; \
			float y = b.m128_f32[i]; \
			r.m128_f32[i] = (expression); \
		} \
		return r; \
	}
#define H2_SSE_SCALAR(name, expression) \
	H2_SSE __m128 name(__m128 a, __m128 b) \
	{ \
		__m128 r = a; \
		float x = a.m128_f32[0]; \
		float y = b.m128_f32[0]; \
		r.m128_f32[0] = (expression); \
		return r; \
	}

H2_SSE_PACKED(_mm_add_ps, x + y)
H2_SSE_PACKED(_mm_sub_ps, x - y)
H2_SSE_PACKED(_mm_mul_ps, x * y)
H2_SSE_PACKED(_mm_div_ps, x / y)
/* minps/maxps return the second operand when either is NaN or both are zero. */
H2_SSE_PACKED(_mm_min_ps, x < y ? x : y)
H2_SSE_PACKED(_mm_max_ps, x > y ? x : y)
H2_SSE_SCALAR(_mm_add_ss, x + y)
H2_SSE_SCALAR(_mm_sub_ss, x - y)
H2_SSE_SCALAR(_mm_mul_ss, x * y)
H2_SSE_SCALAR(_mm_div_ss, x / y)
H2_SSE_SCALAR(_mm_min_ss, x < y ? x : y)
H2_SSE_SCALAR(_mm_max_ss, x > y ? x : y)

H2_SSE __m128 _mm_sqrt_ss(__m128 a)
{
	a.m128_f32[0] = h2_sse_sqrtf(a.m128_f32[0]);
	return a;
}
H2_SSE __m128 _mm_sqrt_ps(__m128 a)
{
	for (int i = 0; i < 4; i++)
		a.m128_f32[i] = h2_sse_sqrtf(a.m128_f32[i]);
	return a;
}
H2_SSE __m128 _mm_rsqrt_ss(__m128 a)
{
	a.m128_f32[0] = 1.0f / h2_sse_sqrtf(a.m128_f32[0]);
	return a;
}
H2_SSE __m128 _mm_rsqrt_ps(__m128 a)
{
	for (int i = 0; i < 4; i++)
		a.m128_f32[i] = 1.0f / h2_sse_sqrtf(a.m128_f32[i]);
	return a;
}
H2_SSE __m128 _mm_rcp_ss(__m128 a)
{
	a.m128_f32[0] = 1.0f / a.m128_f32[0];
	return a;
}
H2_SSE __m128 _mm_rcp_ps(__m128 a)
{
	for (int i = 0; i < 4; i++)
		a.m128_f32[i] = 1.0f / a.m128_f32[i];
	return a;
}

#define H2_SSE_BITWISE(name, expression) \
	H2_SSE __m128 name(__m128 a, __m128 b) \
	{ \
		__m128 r; \
		for (int i = 0; i < 4; i++) \
		{ \
			unsigned int x = a.m128_u32[i]; \
			unsigned int y = b.m128_u32[i]; \
			r.m128_u32[i] = (expression); \
		} \
		return r; \
	}

H2_SSE_BITWISE(_mm_and_ps, x & y)
H2_SSE_BITWISE(_mm_andnot_ps, ~x & y)
H2_SSE_BITWISE(_mm_or_ps, x | y)
H2_SSE_BITWISE(_mm_xor_ps, x ^ y)

#define H2_SSE_COMPARE(name, expression) \
	H2_SSE __m128 name(__m128 a, __m128 b) \
	{ \
		__m128 r; \
		for (int i = 0; i < 4; i++) \
		{ \
			float x = a.m128_f32[i]; \
			float y = b.m128_f32[i]; \
			r.m128_u32[i] = (expression) ? 0xffffffffu : 0u; \
		} \
		return r; \
	}

H2_SSE_COMPARE(_mm_cmpeq_ps, x == y)
H2_SSE_COMPARE(_mm_cmplt_ps, x < y)
H2_SSE_COMPARE(_mm_cmple_ps, x <= y)
H2_SSE_COMPARE(_mm_cmpgt_ps, x > y)
H2_SSE_COMPARE(_mm_cmpge_ps, x >= y)
H2_SSE_COMPARE(_mm_cmpneq_ps, !(x == y))

H2_SSE int _mm_movemask_ps(__m128 a)
{
	return (int)((a.m128_u32[0] >> 31) | ((a.m128_u32[1] >> 31) << 1) |
		((a.m128_u32[2] >> 31) << 2) | ((a.m128_u32[3] >> 31) << 3));
}

#define _mm_shuffle_ps(a, b, imm) h2_sse_shuffle((a), (b), (imm))
H2_SSE __m128 h2_sse_shuffle(__m128 a, __m128 b, unsigned int imm)
{
	return h2_sse_make(a.m128_f32[imm & 3], a.m128_f32[(imm >> 2) & 3],
		b.m128_f32[(imm >> 4) & 3], b.m128_f32[(imm >> 6) & 3]);
}
H2_SSE __m128 _mm_unpacklo_ps(__m128 a, __m128 b)
{
	return h2_sse_make(a.m128_f32[0], b.m128_f32[0], a.m128_f32[1], b.m128_f32[1]);
}
H2_SSE __m128 _mm_unpackhi_ps(__m128 a, __m128 b)
{
	return h2_sse_make(a.m128_f32[2], b.m128_f32[2], a.m128_f32[3], b.m128_f32[3]);
}
H2_SSE __m128 _mm_movelh_ps(__m128 a, __m128 b)
{
	return h2_sse_make(a.m128_f32[0], a.m128_f32[1], b.m128_f32[0], b.m128_f32[1]);
}
H2_SSE __m128 _mm_movehl_ps(__m128 a, __m128 b)
{
	return h2_sse_make(b.m128_f32[2], b.m128_f32[3], a.m128_f32[2], a.m128_f32[3]);
}
H2_SSE __m128 _mm_move_ss(__m128 a, __m128 b)
{
	a.m128_f32[0] = b.m128_f32[0];
	return a;
}

#define _MM_TRANSPOSE4_PS(row0, row1, row2, row3) \
	do \
	{ \
		__m128 h2_t0 = _mm_unpacklo_ps((row0), (row1)); \
		__m128 h2_t1 = _mm_unpacklo_ps((row2), (row3)); \
		__m128 h2_t2 = _mm_unpackhi_ps((row0), (row1)); \
		__m128 h2_t3 = _mm_unpackhi_ps((row2), (row3)); \
		(row0) = _mm_movelh_ps(h2_t0, h2_t1); \
		(row1) = _mm_movehl_ps(h2_t1, h2_t0); \
		(row2) = _mm_movelh_ps(h2_t2, h2_t3); \
		(row3) = _mm_movehl_ps(h2_t3, h2_t2); \
	} while (0)

/* MXCSR has no host effect; recovered code only saves, masks and restores it. */
extern unsigned int h2_sse_emulated_csr;
H2_SSE unsigned int _mm_getcsr(void) { return h2_sse_emulated_csr; }
H2_SSE void _mm_setcsr(unsigned int value) { h2_sse_emulated_csr = value; }

H2_SSE void _mm_empty(void) { }

#define H2_MMX_LANES(name, expression) \
	H2_SSE __m64 name(__m64 a, __m64 b) \
	{ \
		__m64 r; \
		for (int i = 0; i < 4; i++) \
		{ \
			short x = a.m64_i16[i]; \
			short y = b.m64_i16[i]; \
			r.m64_i16[i] = (short)(expression); \
		} \
		return r; \
	}

H2_MMX_LANES(_mm_add_pi16, x + y)
H2_MMX_LANES(_mm_sub_pi16, x - y)
H2_MMX_LANES(_mm_cmpgt_pi16, x > y ? -1 : 0)
H2_SSE __m64 _mm_and_si64(__m64 a, __m64 b)
{
	__m64 r;
	r.m64_u64 = a.m64_u64 & b.m64_u64;
	return r;
}

#undef H2_SSE_PACKED
#undef H2_SSE_SCALAR
#undef H2_SSE_BITWISE
#undef H2_SSE_COMPARE
#undef H2_MMX_LANES

#endif

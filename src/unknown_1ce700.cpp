#include "unknown_11c920.h"
#include "unknown_1cec30.h"
#include <math.h>
// @flags /O2 /arch:SSE /Gr

PRIVATE __forceinline void function_1ce701(__m128 *arg_0)
{
 __m128 local_0 = *arg_0;
 __m128 local_1 = _mm_mul_ps(local_0, local_0);
 __m128 local_2 = _mm_add_ss(_mm_shuffle_ps(local_1, local_1, 0xaa),
  _mm_add_ss(_mm_shuffle_ps(local_1, local_1, 0x55), local_1));
 __m128 local_3 = _mm_rsqrt_ss(local_2);
 __m128 local_4 = _mm_sub_ss(_mm_set_ss(3.0f), _mm_mul_ss(_mm_mul_ss(local_2, local_3), local_3));
 __m128 local_5 = _mm_mul_ss(_mm_mul_ss(_mm_set_ss(0.5f), local_3), local_4);
 *arg_0 = _mm_mul_ps(_mm_shuffle_ps(local_5, local_5, 0), local_0);
}

struct s_1ce700
{
 hkRigidBody *field_0;
 hkRigidBody *field_4;
 __m128 field_10;
 __m128 field_20;
 __m128 field_30;
 __m128 field_40;
 __m128 field_50;
 __m128 field_60;
 void function_1ce700(hkRigidBody *arg_1, hkRigidBody *arg_2,
  __m128 const *arg_3, __m128 const *arg_4, __m128 const *arg_5,
  __m128 const *arg_6, __m128 const *arg_7, __m128 const *arg_8);
};

// @retail 0x1ce700
void s_1ce700::function_1ce700(hkRigidBody *arg_1, hkRigidBody *arg_2,
 __m128 const *arg_3, __m128 const *arg_4, __m128 const *arg_5,
 __m128 const *arg_6, __m128 const *arg_7, __m128 const *arg_8)
{
 hkRigidBody *const *local_0 = &arg_2;
 field_0 = arg_1;
 field_4 = *local_0;
 field_10 = *arg_3;
 field_20 = *arg_4;
 field_30 = *arg_5;
 function_1ce701(&field_30);
 field_40 = *arg_6;
 function_1ce701(&field_40);
 field_50 = *arg_7;
 function_1ce701(&field_50);
 field_60 = *arg_8;
 function_1ce701(&field_60);
}

struct s_1ce2f0
{
 hkRigidBody *field_0;
 hkRigidBody *field_4;
 __m128 field_10;
 __m128 field_20;
 __m128 field_30;
 __m128 field_40;
 __m128 field_50;
 __m128 field_60;
 __m128 field_70;
 void function_1ce2f0(hkRigidBody *arg_1, hkRigidBody *arg_2,
  __m128 const *arg_3, __m128 const *arg_4, __m128 const *arg_5,
  __m128 const *arg_6, __m128 const *arg_7, __m128 const *arg_8);
};

PRIVATE __forceinline __m128 function_1ce2f1(__m128 arg_1, __m128 arg_0)
{
 __m128 local_0 = _mm_mul_ps(_mm_shuffle_ps(arg_1, arg_1, 0xd2), _mm_shuffle_ps(arg_0, arg_0, 0xc9));
 __m128 local_1 = _mm_mul_ps(_mm_shuffle_ps(arg_1, arg_1, 0xc9), _mm_shuffle_ps(arg_0, arg_0, 0xd2));
 return _mm_sub_ps(local_1, local_0);
}

// @retail 0x1ce2f0
void s_1ce2f0::function_1ce2f0(hkRigidBody *arg_1, hkRigidBody *arg_2,
 __m128 const *arg_3, __m128 const *arg_4, __m128 const *arg_5,
 __m128 const *arg_6, __m128 const *arg_7, __m128 const *arg_8)
{
 hkRigidBody *const *local_0 = &arg_2;
 field_0 = arg_1;
 field_4 = *local_0;
 field_10 = *arg_3;
 field_20 = *arg_4;
 field_30 = *arg_5;
 field_40 = *arg_6;
 function_1ce701(&field_30);
 function_1ce701(&field_40);
 field_60 = *arg_7;
 function_1ce701(&field_60);
 __m128 const *const *local_1 = &arg_8;
 field_70 = **local_1;
 function_1ce701(&field_70);
 field_50 = function_1ce2f1(field_60, field_30);
}

struct s_1ce4f0
{
 hkRigidBody *field_0;
 hkRigidBody *field_4;
 __m128 field_10;
 __m128 field_20;
 __m128 field_30;
 __m128 field_40;
 __m128 field_50;
 void function_1ce4f0(hkRigidBody *arg_1, hkRigidBody *arg_2,
  __m128 const *arg_3, __m128 const *arg_4, __m128 const *arg_5, __m128 const *arg_6);
};

// @retail 0x1ce4f0
void s_1ce4f0::function_1ce4f0(hkRigidBody *arg_1, hkRigidBody *arg_2,
 __m128 const *arg_3, __m128 const *arg_4, __m128 const *arg_5, __m128 const *arg_6)
{
 hkRigidBody *const *local_0 = &arg_2;
 field_0 = arg_1;
 field_4 = *local_0;
 field_10 = *arg_3;
 field_20 = *arg_4;
 __m128 local_1 = *arg_5;
 field_30 = *arg_6;
 function_1ce701(&local_1);
 function_1ce701(&field_30);
 real local_2 = fabsf(((real *)&local_1)[0]);
 real local_3 = fabsf(((real *)&local_1)[1]);
 real local_4 = fabsf(((real *)&local_1)[2]);
 long local_5 = 0, local_6 = 1, local_7 = 2;

 if (local_2 > local_3)
 {
  local_6 = 0;
  local_5 = 1;
  local_2 = local_3;
 }
 if (local_2 > local_4)
 {
  local_7 = local_5;
  local_5 = 2;
 }
 real *local_9 = (real *)&field_40;
 local_9[local_5] = 0.0f;
 local_9[local_6] = ((real *)&local_1)[local_7];
 local_9[local_7] = -((real *)&local_1)[local_6];
 function_1ce701(&field_40);
 field_50 = function_1ce2f1(local_1, field_40);
}

struct s_2da8c0
{
 hkRotation field_0;
 void function_2da8c0(hkRotation const *arg_0, hkRotation const *arg_1);
};

struct s_1ce970
{
 hkRigidBody *field_0;
 hkRigidBody *field_4;
 __m128 field_10;
 __m128 field_20;
 __m128 field_30;
 __m128 field_40;
 __m128 field_50;
 s_2da8c0 field_60;
 void function_1ce970(hkRigidBody *arg_1, hkRigidBody *arg_2,
  __m128 const *arg_3, __m128 const *arg_4, __m128 const *arg_5,
  __m128 const *arg_6, __m128 const *arg_7, __m128 const *arg_8);
};

// @retail 0x1ce970
void s_1ce970::function_1ce970(hkRigidBody *arg_1, hkRigidBody *arg_2,
 __m128 const *arg_3, __m128 const *arg_4, __m128 const *arg_5,
 __m128 const *arg_6, __m128 const *arg_7, __m128 const *arg_8)
{
 hkRigidBody *const *local_0 = &arg_2;
 field_0 = arg_1;
 field_4 = *local_0;
 field_10 = *arg_3;
 field_20 = *arg_4;
 field_30 = *arg_5;
 field_40 = *arg_6;
 function_1ce701(&field_30);
 function_1ce701(&field_40);
 __m128 local_1 = *arg_7;
 field_50 = *arg_8;
 function_1ce701(&local_1);
 function_1ce701(&field_50);
 __m128 local_2 = function_1ce2f1(field_40, field_50);
 hkRotation local_3, local_4;
 local_3.m_col0.m_quad = _mm_load_ps((real const *)&field_30);
 local_3.m_col1.m_quad = local_1;
 local_3.m_col2.m_quad = function_1ce2f1(field_30, local_1);
 local_4.m_col0.set(((real *)&field_40)[0], ((real *)&field_50)[0], ((real *)&local_2)[0]);
 local_4.m_col1.set(((real *)&field_40)[1], ((real *)&field_50)[1], ((real *)&local_2)[1]);
 local_4.m_col2.set(((real *)&field_40)[2], ((real *)&field_50)[2], ((real *)&local_2)[2]);
 field_60.function_2da8c0(&local_3, &local_4);
}

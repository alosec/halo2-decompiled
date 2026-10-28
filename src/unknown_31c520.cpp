// @flags /O2 /GL-
// Term cursors and the word records written to the solver op stream.
#include <xmmintrin.h>
struct s_linear_term_31c530
{
	__m128 linear, angular_a, angular_b;
	s_linear_term_31c530 *offset(long index);
};
struct s_angular_term_31c520
{
	__m128 a, b;
};
struct s_angular_cursor_31c520 : s_angular_term_31c520
{
	s_angular_cursor_31c520 *offset(long index);
};
struct s_ops_output_31f5e0
{
	void *term;
	unsigned long *ops;
};
struct s_ops_pair_31f5e0 { unsigned long a, b; };
typedef void (__cdecl *solver_callback_320290)();
struct s_ops_cursors_31e740
{
	__m128 *term;
	unsigned char *ops;
};
struct s_ops_header_31e770
{
	unsigned long opcode;
	__m128 *term;
	long result_offset;
	long body_a;
	long body_b;
	long body_b_offset;
};
struct s_ops_input_31e770 { long body_a, body_b; };
typedef char assert_angular_term_31c520[sizeof(s_angular_term_31c520) == 32 ? 1 : -1];
typedef char assert_linear_term_31c530[sizeof(s_linear_term_31c530) == 48 ? 1 : -1];
typedef char assert_ops_header_31e770[sizeof(s_ops_header_31e770) == 24 ? 1 : -1];

// @retail 0x31c520
s_angular_cursor_31c520 *s_angular_cursor_31c520::offset(long index)
{
	return this + index;
}
// @retail 0x31c530
s_linear_term_31c530 *s_linear_term_31c530::offset(long index)
{
	return this + index;
}
// @retail 0x31f5e0
void __cdecl function_31f5e0(s_ops_pair_31f5e0 const *pair, s_ops_output_31f5e0 *output)
{
	s_ops_pair_31f5e0 values = *pair;
	unsigned long *ops = output->ops;
	void *term = output->term;
	ops[3] = values.a;
	ops[4] = values.b;
	ops[1] = (unsigned long)term;
	ops[0] = 0x11801;
	ops[2] = 0;
	ops[5] = 0;
	output->ops = ops + 6;
}
// @retail 0x320290
void *__cdecl function_320290(solver_callback_320290 callback, long bytes,
	void const *input, s_ops_output_31f5e0 *output)
{
	unsigned long *ops = output->ops;
	ops[0] = ((bytes + 8) << 8) | 0x2000e;
	ops[1] = (unsigned long)callback;
	unsigned char *payload = (unsigned char *)(ops + 2);
	output->ops = (unsigned long *)(payload + bytes);
	return payload;
}
// @retail 0x31e740
void __cdecl function_31e740(float factor, __m128 const *value, s_ops_cursors_31e740 const *output, long index)
{
	(output->term - index * 3)->m128_f32[3] = factor * value->m128_f32[3];
}
// @retail 0x31e770
void __cdecl function_31e770(s_ops_input_31e770 const *input, s_ops_cursors_31e740 *output, long result_offset, long body_b_offset)
{
	s_ops_header_31e770 *ops = (s_ops_header_31e770 *)output->ops;
	__m128 *term = output->term;
	long a = input->body_a;
	long b = input->body_b;
	ops->body_a = a;
	ops->result_offset = result_offset;
	ops->body_b = b;
	ops->body_b_offset = body_b_offset;
	ops->opcode = 0x11801;
	ops->term = term;
	output->ops = (unsigned char *)(ops + 1);
}

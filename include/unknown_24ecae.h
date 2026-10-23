#ifndef UNKNOWN_24ECAE_H
#define UNKNOWN_24ECAE_H

#include "unknown_13927e.h"

struct s_24ecae_placement
{
	real forward_i, forward_j;
	point2f position;
	real size;
	bool hidden;
	byte unknown15;
	bool alternate;
	bool fading;
	real fade;
	real unknown1c;
	real scale;
};

struct s_24ecae_kind
{
	long unknown00;
	long bitmap_index;
	long unknown08;
	long shader_index;
	short sequence_index;
	short alternate_sequence;
	short hidden_sequence;
	short unknown16;
};

struct s_24f0fc_kind
{
	long unknown00;
	long bitmap_index;
	long unknown08;
	long shader_index;
	byte unknown10[0x20 - 0x10];
	long border_bitmap;
};

struct s_24ecae_globals
{
	byte unknown00[0x408];
	long label_count;
	s_24f0fc_kind *labels;
	long image_count;
	s_24ecae_kind *images;
};

struct s_24ecae_sprite
{
	byte unknown00[8];
	box2f bounds;
	byte unknown18[0x20 - 0x18];
};

struct s_24ecae_sequence
{
	byte unknown00[0x20];
	short first_bitmap;
	byte unknown22[0x34 - 0x22];
	long sprite_count;
	s_24ecae_sprite *sprites;
};

struct s_24ecae_bitmap
{
	byte unknown00[4];
	short width, height;
	byte unknown08[8];
	short field_10, field_12;
	byte unknown14[0x74 - 0x14];
};

struct s_24ecae_bitmap_group
{
	byte unknown00[0x3c];
	long sequence_count;
	s_24ecae_sequence *sequences;
	long bitmap_count;
	s_24ecae_bitmap *bitmaps;
};

struct s_4e6950
{
	color3f field_0;
	point3f field_c;
	real value18, value1c, value20, value24;
	byte unknown28[0x3c - 0x28];
	real value3c;
	byte unknown40[0x70 - 0x40];
	real user_values[4];
};

extern s_4e6950 g_4e6950;
extern real g_4e69c0[4];

struct s_float_rect;
struct s_widget_quad_2b11;
void function_22a664(s_widget_quad_2b11 const *, s_float_rect const *, long, long, long);
void function_1392c5(long);

#endif

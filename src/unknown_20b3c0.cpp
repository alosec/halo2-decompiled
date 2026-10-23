// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_20B3C0.CPP: the length of a sound permutation in seconds (an
   outside function the looping sound code in 0x189ee0, 0x18a240 and 0x18bf90
   needs) */

#include "unknown_11c920.h"
#include "globals.h"

/* a sound tag's definition, as this file reads it */
struct s_sound_duration_definition
{
	byte unknown00[0x10];
	long duration;
};

real function_218c60(long definition_index, long pitch_range_index, long permutation_index);
real sound_definition_permutation_duration(long definition_index, long pitch_range_index, long permutation_index, real pitch);
bool function_138820();

// @retail 0x20b3c0
real sound_permutation_reference_duration(long definition_index, s_sound_permutation_reference const *reference)
{
	real result = 0.0f;

	if (definition_index != NONE)
	{
		s_sound_duration_definition *definition = (s_sound_duration_definition *)g_4e3b44[definition_index & 0xffff].data;

		if (reference->permutation_index != NONE && reference->pitch_range_index != NONE)
		{
			if (function_138820())
			{
				result = function_218c60(definition_index, reference->pitch_range_index, reference->permutation_index);
			}
			else
			{
				result = sound_definition_permutation_duration(definition_index, reference->pitch_range_index, reference->permutation_index, 0.0f);
			}
		}
		else
		{
			result = (real)definition->duration;
		}
	}
	return result * 0.001f;
}

/* a sound tag's pitch ranges, as the permutation choice reads them */
struct s_sound_choice_definition
{
	byte unknown00[8];
	short first_pitch_range_index;
	char pitch_range_count;
};

struct s_set_ref;
struct s_animation_state;
struct s_animation_ref;
short function_219110(s_set_ref *ref, short offset, dword *used_mask, short previous, dword *seed, bool *all_used, bool mirror);
long function_219430(s_animation_state *state, s_animation_ref *ref);
long log2_ceiling_plus_one(dword value);

extern byte *g_51e9ec;
bool g_4f55e1;

#define BIT_VECTOR_TEST(vector, bit) ((((dword *)(vector))[(bit) >> 5] & (1 << ((bit) & 31))) != 0)
#define BIT_VECTOR_SET(vector, bit, value) ((value) ? (((dword *)(vector))[(bit) >> 5] |= (1 << ((bit) & 31))) : (((dword *)(vector))[(bit) >> 5] &= ~(1 << ((bit) & 31))))
#define SET_BIT(flags, bit, value) ((value) ? ((flags) |= (1 << (bit))) : ((flags) &= ~(1 << (bit))))

/* a number in [lower, upper) from the second random seed */
// @retail 0x20b100
long sound_random_range(short lower, short upper)
{
	dword *seed = &g_4e7408->seed;

	*seed = *seed * 0x19660d + 0x3c6ef35f;
	return (short)(lower + (((dword)(upper - lower) * (*seed >> 16)) >> 16));
}

/* chooses the permutation of a sound with one pitch range to play next,
   keeping the permutations played and the last one chosen in the bits of
   g_51e9ec */
// @retail 0x20b140
void sound_choose_permutation(long definition_index, s_sound_permutation_reference *reference, bool *all_used)
{
	if (all_used)
	{
		*all_used = false;
	}

	if (definition_index != NONE)
	{
		s_sound_choice_definition *definition = (s_sound_choice_definition *)g_4e3b44[definition_index & 0xffff].data;

		if (definition->pitch_range_count == 1)
		{
			s_permutation_set *set = &g_51ebd4->sets[definition->first_pitch_range_index];
			long count = set->count;
			long bit_index = function_219430((s_animation_state *)set, (s_animation_ref *)definition);

			if (count == 1)
			{
				reference->permutation_index = 0;
				if (all_used)
				{
					*all_used = true;
				}
			}
			else if (bit_index != NONE)
			{
				dword used_mask = 0;
				dword previous = 0;
				long previous_bits = (short)log2_ceiling_plus_one(count);
				long i;
				bool mirror;

				for (i = 0; i < count; i++)
				{
					SET_BIT(used_mask, i, BIT_VECTOR_TEST(g_51e9ec, bit_index + i));
				}
				for (i = 0; i < previous_bits; i++)
				{
					SET_BIT(previous, i, BIT_VECTOR_TEST(g_51e9ec, bit_index + count + i));
				}

				mirror = g_4e6948->state == 1 ? g_4f55e1 : false;
				reference->permutation_index = (char)function_219110((s_set_ref *)definition, 0, &used_mask, (short)previous, &g_4e7408->seed, all_used, mirror);

				for (i = 0; i < previous_bits; i++)
				{
					BIT_VECTOR_SET(g_51e9ec, bit_index + count + i, reference->permutation_index & (1 << i));
				}
				for (i = 0; i < count; i++)
				{
					BIT_VECTOR_SET(g_51e9ec, bit_index + i, used_mask & (1 << i));
				}
			}
			else
			{
				reference->permutation_index = (char)sound_random_range(0, (short)count);
			}
			reference->pitch_range_index = 0;
			return;
		}
	}
	reference->pitch_range_index = NONE;
	reference->permutation_index = NONE;
}

// @retail 0x20afb0
short function_20afb0(long definition_index, dword *mask)
{
	(void)&mask;
	short result = 0;
	if (definition_index != NONE)
	{
		s_sound_choice_definition *definition = (s_sound_choice_definition *)g_4e3b44[definition_index & 0xffff].data;
		if (definition->pitch_range_count == 1)
		{
			s_permutation_set *set = &g_51ebd4->sets[definition->first_pitch_range_index];
			long count = set->count;
			long bit_index = function_219430((s_animation_state *)set, (s_animation_ref *)definition);
			dword value = 0;
			for (long i = 0; i < count && i < 32; i++)
				SET_BIT(value, i, BIT_VECTOR_TEST(g_51e9ec, bit_index + i));
			*mask = value;
			result = (short)count;
		}
	}
	return result;
}

// @retail 0x20b050
bool function_20b050(long definition_index, dword mask)
{
	(void)&mask;
	bool result = false;
	if (definition_index != NONE)
	{
		s_sound_choice_definition *definition = (s_sound_choice_definition *)g_4e3b44[definition_index & 0xffff].data;
		if (definition->pitch_range_count == 1)
		{
			s_permutation_set *set = &g_51ebd4->sets[definition->first_pitch_range_index];
			long count = set->count;
			long bit_index = function_219430((s_animation_state *)set, (s_animation_ref *)definition);
			for (long i = 0; i < count && i < 32; i++)
				BIT_VECTOR_SET(g_51e9ec, bit_index + i, mask & (1 << i));
			result = true;
		}
	}
	return result;
}

#pragma inline_depth(0)
// @retail 0x20b5c0
bool function_20b5c0(long definition_index)
{
	bool result = false;
	if (definition_index != NONE)
	{
		s_sound_choice_definition *definition = (s_sound_choice_definition *)g_4e3b44[definition_index & 0xffff].data;
		if (definition->pitch_range_count == 1)
		{
			s_permutation_set *set = &g_51ebd4->sets[definition->first_pitch_range_index];
			long bit_index = function_219430((s_animation_state *)set, (s_animation_ref *)definition);
			if (bit_index != NONE)
			{
				long count = set->count;
				long offset = count + (short)log2_ceiling_plus_one(count);
				long local_0 = bit_index + offset;
				for (long i = 0; i < 5; i++, local_0++)
				{
					if (BIT_VECTOR_TEST(g_51e9ec, local_0))
					{
						result = true;
						break;
					}
				}
			}
		}
	}
	return result;
}
#pragma inline_depth(255)



struct s_tag_iterator
{
    long unknown00;
    long unknown04;
    long datum_index;
    long next_index;
    long group_tag;
};
long function_122c70(s_tag_iterator *iterator);

struct s_sound_timer_flags
{
    byte unknown00[0xa];
    word unused : 5;
    word timed : 1;
    word remaining : 10;
};

// @retail 0x20b660
void function_20b660(void)
{
    real period_real = g_510c54->field_2_3 * 56.25f;
    long period;
    __asm
    {
        fld period_real
        fistp period
    }
    if (g_4e6948->state == 1 && g_510c54->game_time % (short)period == 0)
    {
        s_tag_iterator iterator;
        iterator.next_index = 0;
        iterator.group_tag = 'snd!';
        long index;
        while ((index = function_122c70(&iterator)) != NONE)
        {
            byte *globals = (byte *)g_4e034c;
            byte *definition = g_4e3b44[index & 0xffff].bytes;
            byte *sound_globals = *(long *)(globals + 0xc0) ? *(byte **)(globals + 0xc4) : NULL;
            byte *tag = g_4e3b44[*(long *)(sound_globals + 4) & 0xffff].bytes;
            byte *category = *(byte **)(tag + 4) + (signed char)definition[2] * 0x5c;
            if (TEST_FIELD_BIT(((s_sound_timer_flags *)category)->timed) && definition[0xa] == 1)
            {
                s_permutation_set *set = &g_51ebd4->sets[*(short *)(definition + 8)];
                long bit_index = function_219430((s_animation_state *)set, (s_animation_ref *)definition);
                if (bit_index != NONE)
                {
                    long count = set->count;
                    short const &width = (short)log2_ceiling_plus_one(count);
                    long offset = bit_index + count + width;
                    short timer = 0;
                    for (long i = 0; i < 5; i++)
                        SET_BIT(timer, i, BIT_VECTOR_TEST(g_51e9ec, offset + i));
                    if (timer > 0)
                        timer--;
                    for (long i = 0; i < 5; i++)
                    {
                        long bit = offset + i;
                        BIT_VECTOR_SET(g_51e9ec, bit, timer & (1 << i));
                    }
                }
            }
        }
    }
}

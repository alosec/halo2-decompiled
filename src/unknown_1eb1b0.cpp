// @flags /O2 /Ob1 /Gr
/* UNKNOWN_1EB1B0.CPP: the shape blocks of a physics model */

#include "unknown_11c920.h"
#include "impacts.h"

// @retail 0x1eb1b0
s_impact_tag_block *physics_model_shape_block_get(
	byte *physics_model,
	s_physics_model_shape_key const *key,
	long *element_size)
{
	s_impact_tag_block *block;

	switch (key->type)
	{
	case 0:
		block = (s_impact_tag_block *)(physics_model + 0xb0);
		*element_size = 0x78;
		break;
	case 1:
		block = (s_impact_tag_block *)(physics_model + 0xe8);
		*element_size = 0x84;
		break;
	case 2:
		block = (s_impact_tag_block *)(physics_model + 0xb8);
		*element_size = 0x94;
		break;
	case 3:
		block = (s_impact_tag_block *)(physics_model + 0xf8);
		*element_size = 0x7c;
		break;
	case 4:
		block = (s_impact_tag_block *)(physics_model + 0xf0);
		*element_size = 0x78;
		break;
	case 5:
		block = (s_impact_tag_block *)(physics_model + 0x100);
		*element_size = 0x84;
		break;
	default:
		__assume(0);
	}
	return block;
}

struct s_physics_constraint_iterator
{
	byte *physics;
	short type;
	short index;
};

// @retail 0x1eb110
void function_1eb110(s_physics_constraint_iterator *iterator)
{
	byte *volatile element;
	iterator->type = 0;
	iterator->index = 0;
	s_physics_model_shape_key *key = (s_physics_model_shape_key *)&iterator->type;
	for (;;)
	{
		long size;
		s_impact_tag_block *block = physics_model_shape_block_get(iterator->physics, key, &size);
		if (key->index < block->count)
		{
			byte *value = block->address + key->index * size;
			element = value;
			if (value)
				break;
		}
		if (key->type >= 5)
			break;
		key->type++;
		iterator->index = 0;
	}
}

// @retail 0x1eb160
void function_1eb160(s_physics_constraint_iterator *iterator)
{
	byte *volatile element;
	iterator->index++;
	s_physics_model_shape_key *key = (s_physics_model_shape_key *)&iterator->type;
	for (;;)
	{
		long size;
		s_impact_tag_block *block = physics_model_shape_block_get(iterator->physics, key, &size);
		if (key->index < block->count)
		{
			byte *value = block->address + key->index * size;
			element = value;
			if (value)
				break;
		}
		if (key->type >= 5)
			break;
		key->type++;
		iterator->index = 0;
	}
}

struct s_shape_block_table
{
	byte field_0[0x48];
	s_impact_tag_block shapes[6];
	byte field_78[0x90 - 0x78];
	s_impact_tag_block block90;
	byte field_98[8];
	s_impact_tag_block blocka0;
	byte field_a8[0x108 - 0xa8];
	s_impact_tag_block block108;
};

// @retail 0x1eaf30
void *function_1eaf30(short type, s_shape_block_table *table, short index)
{
	void *result = NULL;
	switch (type)
	{
	case 0: result = (byte *)table->shapes[0].address + index * 0x80 + 0x30; break;
	case 5: result = (byte *)table->shapes[1].address + index * 0xb0 + 0x20; break;
	case 1: result = (byte *)table->shapes[2].address + index * 0x50 + 0x20; break;
	case 2: result = (byte *)table->shapes[3].address + index * 0x90 + 0x40; break;
	case 3: result = (byte *)table->shapes[4].address + index * 0x60 + 0x20; break;
	case 4: result = (byte *)table->shapes[5].address + index * 0x100 + 0x20; break;
	case 14: result = (byte *)table->block90.address + index * 0x38; break;
	case 15: result = (byte *)table->blocka0.address + index * 0x14; break;
	case 6: result = (byte *)table->block108.address + index * 0x20; break;
	}
	return result;
}

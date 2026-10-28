// @flags /O2 /GL-
// Havok (third-party physics library): three objects that each add a word to
// one of their lists.
#include "growable_array_2d91d0.h"
#include "description_312430.h"

#include "lane_b_constraint_entities.h"

ELEMENTS_2D91D0(pointers_30be00, void *);

// @retail 0x30bdc0
void class_30bdc0::function_30bdc0(long value)
{
	words.append(value);
}

struct s_1d1540
{
	char unknown0[0x70];
	pointers_30be00 pointers;

	void function_30be00(void *arg_0);
};

// @retail 0x30be00
void s_1d1540::function_30be00(void *arg_0)
{
	pointers.append(arg_0);
}

struct s_47f048_object
{
	char unknown0[0x60];
	words_30bdc0 words;

	void function_30be40(long value);
};

// @retail 0x30be40
void s_47f048_object::function_30be40(long value)
{
	words.append(value);
}

// sets the object from the description, with the byte and the word it adds
// @retail 0x30be80
void class_30bd90::function_30be80(description_30bd90 const *description)
{
	function_3124c0(description);
	field_48 = description->field_18;
	field_5c = description->field_1a;
}

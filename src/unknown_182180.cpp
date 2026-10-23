// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_182180.CPP: Havok shape queries */

#include "unknown_11c920.h"

/* Havok's collision shape: the virtual slots the game calls (the others are
   placeholders) */
class hkShape
{
public:
	virtual void slot00(void) {}
	virtual void slot01(void) {}
	virtual void slot02(void) {}
	virtual void slot03(void) {}
	virtual void slot04(void) {}
	virtual long kind_code(void) const { return 0; }
};

// @retail 0x182180
bool function_182180(hkShape const *shape)
{
	bool result = false;

	switch (shape->kind_code())
	{
	case 1:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
	case 24:
		result = true;
		break;
	}
	return result;
}

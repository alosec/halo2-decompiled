// @flags /O2 /GL- /GX
// Havok (third-party physics library): destroying the axis-aligned box
// volume that contact queries add to the world (0x30b480 builds it): its
// list of bodies found goes, then the listened object under it.
#include "listened_object_315950.h"

class class_30b510 : public class_315950
{
public:
	char unknown54[0x80 - 0x54];
	words_315950 bodies;

	virtual ~class_30b510();
};

// @retail 0x30b510
class_30b510::~class_30b510()
{
}

// @retail 0x30b5c0 deleting class_30b510

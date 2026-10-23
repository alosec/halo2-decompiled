#ifndef CONTACT_VOLUME_30BB50_H
#define CONTACT_VOLUME_30BB50_H
// Havok (third-party physics library): the transform contact-query volume as
// Havok's own code sees it, a reference-counted object of 0xd0 bytes that its
// allocator hands out with tag 0x2c, over the listened object (0x315950).
#include "listened_object_315950.h"

struct c_contact_query_transform_info;

class c_contact_query_transform_volume : public class_315950
{
public:
	c_contact_query_transform_volume(c_contact_query_transform_info const *info);
	virtual ~c_contact_query_transform_volume();

	REFERENCED_NEW_2D88A0(0x2c)

	char unknown54[0xc0 - 0x54];
	words_315950 bodies;
	char unknowncc[0xd0 - 0xcc];
};

#endif

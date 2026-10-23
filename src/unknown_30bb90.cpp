// @flags /O2 /GL- /GX
// Havok (third-party physics library): destroying the transform contact-query
// volume: its list of bodies found goes, then the listened object under it.
#include "contact_volume_30bb50.h"

// @retail 0x30bb90
c_contact_query_transform_volume::~c_contact_query_transform_volume()
{
}

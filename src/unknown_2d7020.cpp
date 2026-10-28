// @flags /O2 /GL-
// Overflow stack pages save the previous four-word stack state before payload.
#include "unknown_146a20.h"
#include "globals.h"
#include <string.h>

// @retail 0x2d7020
void *c_havok_abstract_allocator::method9(long size)
{
	long capacity = size > 0x1000 ? size : 0x1000;
	char *page = (char *)((c_havok_abstract_allocator *)g_480118)->allocate16(capacity + 16, 0x12);
	memcpy(page, m_unknown08, sizeof(m_unknown08));
	char *payload = page + 16;
	m_unknown08[3] = (long)payload;
	m_unknown08[0] = (long)(payload + size);
	m_unknown08[1] = capacity - size;
	m_unknown08[2] = (long)page;
	return payload;
}

// @retail 0x2d7090
void c_havok_abstract_allocator::method10(void *block, long size)
{
	char *page = (char *)m_unknown08[3] - 16;
	long capacity = m_unknown08[1] + size + 16;
	memcpy(m_unknown08, page, sizeof(m_unknown08));
	((c_havok_abstract_allocator *)g_480118)->deallocate16(page, capacity, 0x12);
}

#ifdef H2_NATIVE_PORT

#include "unknown_1cec30.h"

#include <stddef.h>

void __cdecl function_2d91d0(void *array, long element_size);

static_assert(sizeof(s_havok_entity_property) == 8, "Havok property stride must match retail");
static_assert(offsetof(hkEntity, m_properties) == 0x30,
    "Havok entity property pointer offset must match retail");
static_assert(offsetof(hkEntity, m_property_count) == 0x34,
    "Havok entity property count offset must match retail");
static_assert(offsetof(hkEntity, m_property_capacity) == 0x38,
    "Havok entity property capacity offset must match retail");

// @retail 0x3123a0
s_havok_property_word hkEntity::entity_property_erase(dword key)
{
    for (long index = 0; index < m_property_count; ++index)
    {
        if (m_properties[index].m_key == key)
        {
            s_havok_property_word result = m_properties[index].m_value;
            --m_property_count;
            for (long move = index; move < m_property_count; ++move)
                m_properties[move] = m_properties[move + 1];
            return result;
        }
    }
    return s_havok_property_word(0);
}

// @retail 0x312530
void hkEntity::entity_property_put(dword key, s_havok_property_word value)
{
    for (long index = 0; index < m_property_count; ++index)
    {
        if (m_properties[index].m_key == key)
            return;
    }
    if (m_property_count == (long)(m_property_capacity & 0x7fffffffU))
        function_2d91d0(&m_properties, sizeof(s_havok_entity_property));
    m_properties[m_property_count].m_key = key;
    m_properties[m_property_count].m_value = value;
    ++m_property_count;
}

#endif

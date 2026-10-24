// @flags /O2 /Gr /arch:SSE
// BLOCKED-BY 0x182b90: enabling this body loses matched 0x1cfd10 and 0x277ec0 in the full image: once this
// self-aligning (16-byte frame) caller reaches 0x1cfd10, our 0x182b90 takes its fourth argument on the stack
// instead of in ebx; any 16-aligned caller does it, and 0x182b90's own frame is not the cause (it is retail's
// since round 27; build/codex-report.md, rounds 26 and 27).
#if H2_WHOLE_IMAGE /* parked upstream for byte matching; the whole image runs it */
#include "unknown_1cec30.h"

struct s_2792c0_context;
struct s_component_contact_pair;
bool function_2792c0(s_2792c0_context *);
void function_1cfd10(s_component_contact_pair const *, s_havok_component *, real);

/* an entity's property value read as a real (0 when it has no such property) */
static inline real havok_entity_property_real_get(hkEntity const *entity, dword key)
{
    for (long i = 0; i < entity->m_property_count; i++)
    {
        if (entity->m_properties[i].m_key == key)
            return *(real const *)&entity->m_properties[i].m_value.m_data;
    }
    return 0.0f;
}

// Retail 0x279410, disabled with its blocker above.
bool __cdecl function_279410(s_component_contact_pair const *pair, hkEntity *moving,
    hkEntity *opposing, real limit, real reduction, real approach)
{
    long slot = havok_entity_property_get(moving, 0x2001);
    real absolute_speed = havok_entity_property_real_get(moving, 0x2002);
    long opposite_slot = havok_entity_property_get(opposing, 0x2001);
    s_havok_component *part = havok_component_get(slot);
    byte *entity = (byte *)havok_object_get(part->object_index);
    byte *second_entity = NULL;
    if (opposite_slot != NONE && part)
        second_entity = (byte *)havok_object_get(part->object_index);

    __m128 squared = ((hkRigidBody *)moving)->m_motion->m_linear_velocity.m_quad;
    squared = _mm_mul_ps(squared, squared);
    __m128 accumulated = _mm_add_ss(_mm_shuffle_ps(squared, squared, 0x55), squared);
    accumulated = _mm_add_ss(_mm_shuffle_ps(squared, squared, 0xaa), accumulated);
    accumulated = _mm_sqrt_ss(accumulated);
    _mm_store_ss(&absolute_speed, accumulated);
    real speed = absolute_speed;
    if (((hkRigidBody *)opposing)->m_motion->kind_code() != 7)
    {
        __m128 difference = _mm_sub_ps(((hkRigidBody *)moving)->m_motion->m_linear_velocity.m_quad,
            ((hkRigidBody *)opposing)->m_motion->m_linear_velocity.m_quad);
        difference = _mm_mul_ps(difference, difference);
        __m128 sum = _mm_add_ss(_mm_shuffle_ps(difference, difference, 0x55), difference);
        sum = _mm_add_ss(_mm_shuffle_ps(difference, difference, 0xaa), sum);
        sum = _mm_sqrt_ss(sum);
        real relative_speed;
        _mm_store_ss(&relative_speed, sum);
        if (absolute_speed > relative_speed)
            speed = relative_speed;
    }
    bool result;
    if (entity[0xaa] == 0)
    {
        byte *current = (byte *)havok_object_get(part->object_index);
        if (current[0x3dd] && second_entity && second_entity[0xaa] == 0)
            return false;
        if (!current[0x34b])
        {
            real projected = approach;
            if (projected > speed)
                projected = speed;
            result = projected > limit || speed > limit && !function_2792c0((s_2792c0_context *)pair);
        }
        else
            result = speed > limit;
    }
    else
        result = speed > limit;
    if (result)
        function_1cfd10(pair, part, speed - reduction);
    return result;
}

#endif

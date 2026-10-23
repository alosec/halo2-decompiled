#include "unknown_1cec30.h"

#ifdef H2_NATIVE_PORT
static hkVector4 rotate_vector(hkRotation const &rotation,
    hkVector4 const &vector)
{
    hkVector4 result;
    for (long component = 0; component < 4; ++component)
    {
        result.m_quad.m128_f32[component] =
            rotation.m_col0(component) * vector(0) +
            rotation.m_col1(component) * vector(1) +
            rotation.m_col2(component) * vector(2);
    }
    return result;
}

// Scalar native reconstruction of retail 0x2da5d0. Havok transforms are
// column-major: this = this * b, including rotated child translation.
void hkTransform::transform_compose_in_place(hkTransform const &b)
{
    hkTransform left = *this;
    m_rotation.m_col0 = rotate_vector(left.m_rotation, b.m_rotation.m_col0);
    m_rotation.m_col1 = rotate_vector(left.m_rotation, b.m_rotation.m_col1);
    m_rotation.m_col2 = rotate_vector(left.m_rotation, b.m_rotation.m_col2);
    m_translation = rotate_vector(left.m_rotation, b.m_translation);
    for (long component = 0; component < 4; ++component)
        m_translation.m_quad.m128_f32[component] +=
            left.m_translation(component);
}
#endif

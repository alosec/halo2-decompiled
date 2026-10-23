// @flags /O2 /GL-
// Havok (third-party physics library): a rotation held as four floats (an
// axis part and a scalar part), set from a 3x3 rotation matrix held as three
// columns of four floats.
#include "unknown_1cec30.h"
#include <math.h>

class c_component_rotation
{
public:
	hkVector4 value;
	void set(hkRotation const &rotation);
};

// @retail 0x2da2d0
void c_component_rotation::set(hkRotation const &rotation)
{
	// element (row, column) is m[column * 4 + row]
	float const *m = (float const *)&rotation;
	float *q = (float *)&value;
	float const trace = rotation.m_col0(0) + rotation.m_col1(1) + rotation.m_col2(2);
	if (trace > 0.0f)
	{
		float const root = sqrtf(trace + 1.0f);
		float const scale = 0.5f / root;
		q[0] = (m[6] - m[9]) * scale;
		q[1] = (m[8] - m[2]) * scale;
		q[2] = (m[1] - m[4]) * scale;
		q[3] = root * 0.5f;
	}
	else
	{
		long const next[3] = {1, 2, 0};
		long i = 0;
		if (m[5] > m[0])
		{
			i = 1;
		}
		if (m[10] > m[i * 5])
		{
			i = 2;
		}
		long const j = next[i];
		long const k = next[j];
		float const root = sqrtf(m[i * 5] - (m[j * 5] + m[k * 5]) + 1.0f);
		float const scale = 0.5f / root;
		q[i] = root * 0.5f;
		q[3] = (m[j * 4 + k] - m[k * 4 + j]) * scale;
		q[j] = (m[j * 4 + i] + m[i * 4 + j]) * scale;
		q[k] = (m[k * 4 + i] + m[i * 4 + k]) * scale;
	}
}

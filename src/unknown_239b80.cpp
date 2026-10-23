// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_239B80.CPP: two-dimensional polygon helpers (convex hulls,
   clipping against lines and point-in-polygon tests over point2f
   arrays) */

#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include <float.h>
#include <string.h>

#define k_real_epsilon 0.0001f
#define MAXIMUM_POLYGON_POINTS 48

struct vector2f { real i, j; };

/* a line in the plane: points p with dot(n, p) == d */
struct plane2f
{
	vector2f n;
	real d;
};

PRIVATE __forceinline bool polygon_clip_intersection_23a430(plane2f const *plane,
	point2f const *first, point2f const *second, long count, short capacity, point2f *output)
{
	real dy = first->y - second->y;
	real dx = first->x - second->x;
	real denominator = plane->n.i * dx;
	denominator += plane->n.j * dy;
	if ((denominator < -0.0001f || denominator > 0.0001f) && count < capacity)
	{
		real fraction = (plane->n.j * first->y + plane->n.i * first->x - plane->d) / denominator;
		dx *= fraction;
		output->x = first->x - dx;
		dy *= fraction;
		output->y = first->y - dy;
		return true;
	}
	return false;
}

// @retail 0x23a430
short function_23a430(short count, point2f const *points, plane2f const *plane,
	real epsilon, short capacity, point2f *output)
{
	point2f copy[128];
	long sides[128];
	if (points == output)
	{
		memcpy(copy, points, count * sizeof(point2f));
		points = copy;
	}
	long original_count = count;
	long positive = 0;
	long negative = 0;
	for (long i = 0; i < original_count; i++)
	{
		real distance = points[i].y * plane->n.j + points[i].x * plane->n.i - plane->d;
		if (distance > epsilon)
		{
			sides[i] = 1;
			positive++;
		}
		else if (0.f - epsilon > distance)
		{
			sides[i] = 0;
			negative++;
		}
		else
			sides[i] = NONE;
	}
	long result = 0;
	if (!negative)
	{
		memcpy(output, points, original_count * sizeof(point2f));
		return (short)original_count;
	}
	if (!positive)
		return 0;
	long previous = original_count - 1;
	for (long current = 0; current < original_count; previous = current++)
	{
		if (sides[current] == 0)
		{
			if (sides[previous] == 1 && polygon_clip_intersection_23a430(plane,
				&points[current], &points[previous], result, capacity, &output[result]))
				result++;
		}
		else
		{
			if (sides[current] == 1 && sides[previous] == 0 && polygon_clip_intersection_23a430(plane,
				&points[previous], &points[current], result, capacity, &output[result]))
				result++;
			if (result < capacity)
				output[result++] = points[current];
		}
	}
	return (short)result;
}

static inline real dot_product2d(vector2f const *a, point2f const *b)
{
	return a->i * b->x + a->j * b->y;
}

static inline real normalize2d(vector2f *v)
{
	real m = (real)sqrt(v->i * v->i + v->j * v->j);
	if (fabs(m) < k_real_epsilon)
		return 0.f;
	real inv = 1.f / m;
	v->i = inv * v->i;
	v->j = v->j * inv;
	return m;
}

static inline bool points2d_equal(point2f const *a, point2f const *b)
{
	return fabs(a->x - b->x) < k_real_epsilon && fabs(a->y - b->y) < k_real_epsilon;
}

/* two pi, set up at run time */
real g_55e470;

long function_239d50(long count, point2f const *points, point2f *hull);
short function_239e50(short count, point2f const *points, short *indices);

/* 0 when every point is the same, 1 when they are on one line, 2 otherwise */
// @retail 0x239b80
short function_239b80(short count, point2f const *points)
{
	point2f first;
	plane2f line;
	short state = NONE;

	for (short i = 0; state < 2 && i < count; i++)
	{
		switch (state)
		{
		case NONE:
			first = points[i];
			state = 0;
			break;
		case 0:
			line.n.i = first.y - points[i].y;
			line.n.j = points[i].x - first.x;
			if (normalize2d(&line.n) != 0.f)
			{
				line.d = dot_product2d(&line.n, &points[i]);
				state = 1;
			}
			else
			{
				line.d = 0.f;
			}
			break;
		case 1:
			if (!(fabs(dot_product2d(&line.n, &points[i]) - line.d) < k_real_epsilon))
				state = 2;
			break;
		}
	}

	return state;
}

// @retail 0x239cb0
long function_239cb0(long count, point3f const *points, point2f *hull)
{
	point2f points2d[MAXIMUM_POLYGON_POINTS];

	if (count > MAXIMUM_POLYGON_POINTS)
		count = MAXIMUM_POLYGON_POINTS;
	for (long i = 0; i < count; i++)
	{
		points2d[i].x = points[i].x;
		points2d[i].y = points[i].y;
	}

	return function_239d50(count, points2d, hull);
}

// @retail 0x239d50
long function_239d50(long count, point2f const *points, point2f *hull)
{
	point2f unique_points[MAXIMUM_POLYGON_POINTS];
	short indices[MAXIMUM_POLYGON_POINTS];

	short unique_count = (short)count;
	memcpy(unique_points, points, sizeof(point2f) * (count > MAXIMUM_POLYGON_POINTS ? MAXIMUM_POLYGON_POINTS : count));
	for (long i = 0; i < unique_count - 1; i++)
	{
		for (long j = i + 1; j < unique_count; j++)
		{
			if (fabs(unique_points[i].x - unique_points[j].x) < k_real_epsilon &&
				fabs(unique_points[i].y - unique_points[j].y) < k_real_epsilon)
			{
				unique_points[j] = unique_points[unique_count - 1];
				unique_count--;
			}
		}
	}

	short hull_count = function_239e50(unique_count, unique_points, indices);
	if (hull_count >= 3)
	{
		for (long i = 0; i < hull_count; i++)
			hull[i] = unique_points[indices[i]];
	}
	else
	{
		hull_count = 0;
	}
	return hull_count;
}

/* gift wrapping: the indices of the convex hull of the points, starting from
   the lowest */
// @retail 0x239e50
short function_239e50(short count, point2f const *points, short *indices)
{
	long hull_count = 0;
	short **indices_reference = &indices;

	if (function_239b80(count, points) == 2)
	{
		real previous_angle = 0.f;
		bool left_start = false;
		real lowest_x = FLT_MAX;
		real lowest_y = FLT_MAX;
		short current = count;

		{
			short i = 0;
			if (i < count)
			{
				do
				{
					if (lowest_y - k_real_epsilon > points[i].y ||
						lowest_y > points[i].y && lowest_x + k_real_epsilon > points[i].x ||
						lowest_y + k_real_epsilon > points[i].y && lowest_x - k_real_epsilon > points[i].x)
					{
						lowest_x = points[i].x;
						lowest_y = points[i].y;
						current = i;
					}
					i++;
				} while (i < count);
			}
		}

		while ((short)hull_count < count)
		{
			real best_distance = -FLT_MAX;
			(*indices_reference)[(short)hull_count++] = current;
			short next = NONE;
			real best_angle = FLT_MAX;
			real best_delta = FLT_MAX;

			for (short j = 0; j < count; j++)
			{
				real dx = points[current].x - points[j].x;
				real dy = points[current].y - points[j].y;
				real distance = (real)sqrt(dx * dx + dy * dy);
				if (distance > 0.000244140625f)
				{
					real angle = (real)atan2(points[j].y - points[current].y, points[j].x - points[current].x);
					real delta = angle - previous_angle;
					while (delta < -k_real_epsilon)
						delta += g_55e470;
					if (delta < best_delta - k_real_epsilon ||
						!(delta > best_delta + k_real_epsilon) &&
						(j == (*indices_reference)[0] || next != (*indices_reference)[0] && distance > best_distance))
					{
						best_distance = distance;
						best_angle = angle;
						best_delta = delta;
						next = j;
					}
				}
			}

			current = next;
			previous_angle = best_angle;
			if (next == NONE)
				goto finished;
			if (!left_start)
				left_start = !points2d_equal(&points[next], &points[(*indices_reference)[0]]);
			if (next == (*indices_reference)[0])
				goto finished;
			if (left_start && points2d_equal(&points[next], &points[(*indices_reference)[0]]))
				goto finished;
		}

		for (short i = 0; i < (short)hull_count; i++)
		{
			short j;
			for (j = 0; j < i; j++)
			{
				if ((*indices_reference)[j] == (*indices_reference)[i])
				{
					hull_count = i - j;
					memcpy(*indices_reference, &(*indices_reference)[j], sizeof(short) * (short)hull_count);
					break;
				}
			}
			if (j < i)
				break;
		}
	}

finished:
	return hull_count;
}

PRIVATE __forceinline void polygon_difference_23a220(point2f const *a, point2f const *b, vector2f *result)
{
	result->i = a->x - b->x;
	result->j = a->y - b->y;
}

// @retail 0x23a160
bool function_23a160(point2f const *point, real radius, short volatile count, point2f const *points)
{
	real radius_squared = radius * radius;
	bool result = true;
	for (short i = 0; i < count; i++)
	{
		vector2f offset;
		polygon_difference_23a220(point, &points[i], &offset);
		long next = (i + 1 >= count) ? 0 : i + 1;
		vector2f edge;
		polygon_difference_23a220(&points[next], &points[i], &edge);
		real length_squared = (real)(edge.i * edge.i) + (real)(edge.j * edge.j);
		if (length_squared != 0.f)
		{
			real cross = edge.j * offset.i - edge.i * offset.j;
			if (cross > 0.f && cross * cross > length_squared * radius_squared)
			{
				result = false;
				break;
			}
		}
	}
	return result;
}

// @retail 0x23a220
bool function_23a220(point2f const *point, short volatile count, point2f const *points, real epsilon)
{
	(void)&count;
	bool result = true;
	for (short i = 0; i < count; i++)
	{
		vector2f offset;
		polygon_difference_23a220(point, &points[i], &offset);
		long next = (i + 1 >= count) ? 0 : i + 1;
		vector2f edge;
		polygon_difference_23a220(&points[next], &points[i], &edge);
		if (edge.i * offset.j - edge.j * offset.i < -epsilon)
		{
			result = false;
			break;
		}
	}
	return result;
}

PRIVATE __forceinline bool clip_points_equal_23a6f0(point2f const *a, point2f const *b, real epsilon)
{
	return fabs((double)a->x - b->x) < epsilon && fabs((double)a->y - b->y) < epsilon;
}

// @retail 0x23a6f0
short function_23a6f0(point2f *output, short count, point2f const *points,
	plane2f const *plane, short capacity, dword *point_mask, bool *clipped, real epsilon)
{
	point2f copy[512];
	short result = 0;
	bool positive = false;
	bool negative = false;
	dword mask = 0;
	if (clipped)
		*clipped = false;
	if (points == output)
	{
		memcpy(copy, points, count * sizeof(point2f));
		points = copy;
	}
	point2f const *previous = &points[count - 1];
	volatile bool previous_inside = previous->y * plane->n.j + previous->x * plane->n.i - plane->d >= 0.f;
	for (short i = 0; i < count; i++)
	{
		point2f const *current = &points[i];
		real distance = current->y * plane->n.j + plane->n.i * current->x - plane->d;
		bool inside = distance >= 0.f;
		if (distance > epsilon)
			positive = true;
		else if (0.f - epsilon > distance)
			negative = true;
		if (inside != previous_inside)
		{
			if (result == capacity)
				goto overflow;
			if (clipped)
				*clipped = true;
			real dy = previous->y - current->y;
			real dx = previous->x - current->x;
			real denominator = plane->n.i * dx + dy * plane->n.j;
			real fraction = 0.f;
			if (denominator != 0.f)
				fraction = 0.f - (current->y * plane->n.j + plane->n.i * current->x - plane->d) / denominator;
			fraction = fraction < 0.f ? 0.f : fraction > 1.f ? 1.f : fraction;
			output[result].x = dx * fraction + current->x;
			output[result].y = dy * fraction + current->y;
			mask |= 1 << result;
			result++;
			if (result != 1 && (clip_points_equal_23a6f0(&output[result - 1], output, epsilon) ||
				clip_points_equal_23a6f0(&output[result - 1], &output[result - 2], epsilon)))
				result--;
		}
		if (inside)
		{
			if (result == capacity)
				goto overflow;
			output[result] = *current;
			if (point_mask && (*point_mask & (1 << i)))
				mask |= 1 << result;
			else
				mask &= ~(1 << result);
			result++;
			if (result != 1 && (clip_points_equal_23a6f0(&output[result - 1], output, epsilon) ||
				clip_points_equal_23a6f0(&output[result - 1], &output[result - 2], epsilon)))
				result--;
		}
		previous = current;
		previous_inside = inside;
	}
	if (result < 3)
		result = 0;
	if (!positive)
		result = 0;
	else if (!negative)
	{
		result = count;
		memcpy(output, points, count * sizeof(point2f));
	}
	if (point_mask)
		*point_mask = mask;
	return result;
overflow:
	memcpy(output, points, count * sizeof(point2f));
	if (point_mask)
		*point_mask = mask;
	return NONE;
}

// @retail 0x23a2b0
short function_23a2b0(short count, point2f const *points, short clip_count,
    point2f const *clip, short capacity, point2f *output, real epsilon)
{
    point2f buffers[2][512];
    for (short i = 0; i < clip_count && count > 0; i++)
    {
        short previous = i ? i - 1 : clip_count - 1;
        point2f *destination = i == clip_count - 1 ? output : buffers[i & 1];
        plane2f plane;
        plane.n.i = clip[previous].y - clip[i].y;
        plane.n.j = clip[i].x - clip[previous].x;
        real length = (real)sqrt(plane.n.i * plane.n.i + plane.n.j * plane.n.j);
        if (!(fabs(length) < 0.0001f))
        {
            real inverse = 1.f / length;
            plane.n.i *= inverse;
            plane.n.j *= inverse;
        }
        else
            length = 0.f;
        if (length != 0.f)
        {
            plane.d = plane.n.i * clip[i].x + plane.n.j * clip[i].y;
            count = function_23a430(count, points, &plane, epsilon, capacity, destination);
            if (count == NONE)
                return NONE;
        }
        else
        {
            plane.d = 0.f;
            memcpy(destination, points, count * sizeof(point2f));
        }
        points = destination;
    }
    return count;
}

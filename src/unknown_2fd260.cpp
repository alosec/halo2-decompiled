#include "unknown_11c920.h"

// @flags /O2 /Gr

struct s_physics_vertex_block
{
    float x[4];
    float y[4];
    float z[4];
};
static_assert(sizeof(s_physics_vertex_block) == 0x30,
    "physics vertex block layout");

union u_physics_vertex_key
{
    dword bits;
    float real;
};

class c_physics_vertex_cloud_shape
{
    byte field_4[8];
    float radius;
    byte field_10[0x20];
    s_physics_vertex_block *blocks;
    long block_count;
    byte field_38[0x98];
    long vertex_count;
public:
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void v6();
    virtual void v7();
    virtual void v8();
    virtual void v9();
    virtual void v10();
    virtual float *function_2fd730(float *vertices);
    virtual void function_2fd260(const float *direction, float *result);
    virtual void function_2fd430(const word *keys, float *vertices, long count);
};
static_assert(sizeof(c_physics_vertex_cloud_shape) == 0xd4,
    "vertex cloud shape layout");

static void physics_vertex_cloud_get(const s_physics_vertex_block *blocks,
    long index, float *result)
{
    const s_physics_vertex_block &block = blocks[index >> 2];
    long lane = index & 3;
    result[0] = block.x[lane];
    result[1] = block.y[lane];
    result[2] = block.z[lane];
}

// @retail 0x2fd260
void c_physics_vertex_cloud_shape::function_2fd260(
    const float *direction, float *result)
{
    float maximum = -3.402823466e+38F;
    long maximum_index = 0;
    for (long block_index = 0; block_index < block_count; block_index++)
    {
        const s_physics_vertex_block &block = blocks[block_index];
        float block_maximum = -3.402823466e+38F;
        long block_lane = 0;
        for (long lane = 0; lane < 4; lane++)
        {
            float projection = direction[0] * block.x[lane] +
                direction[1] * block.y[lane] +
                direction[2] * block.z[lane];
            if (projection >= block_maximum)
            {
                block_maximum = projection;
                block_lane = lane;
            }
        }
        if (block_maximum > maximum)
        {
            maximum = block_maximum;
            maximum_index = block_index * 4 + block_lane;
        }
    }

    physics_vertex_cloud_get(blocks, maximum_index, result);
    u_physics_vertex_key key;
    key.bits = 0x3f000000 | (dword)maximum_index;
    result[3] = key.real;
}

// @retail 0x2fd430
void c_physics_vertex_cloud_shape::function_2fd430(
    const word *keys, float *vertices, long count)
{
    for (long output = 0; output < count; output++)
    {
        long index = keys[output];
        physics_vertex_cloud_get(blocks, index, vertices + output * 4);
        u_physics_vertex_key key;
        key.bits = 0x3f000000 | (dword)index;
        vertices[output * 4 + 3] = key.real;
    }
}

// @retail 0x2fd730
float *c_physics_vertex_cloud_shape::function_2fd730(float *vertices)
{
    for (long index = 0; index < vertex_count; index++)
    {
        physics_vertex_cloud_get(blocks, index, vertices + index * 4);
        vertices[index * 4 + 3] = radius;
    }
    return vertices;
}

#ifndef H2_PORT_RENDER_PROTOCOL_H
#define H2_PORT_RENDER_PROTOCOL_H

#define H2_RENDER_REQUEST_IMMEDIATE 1u
#define H2_RENDER_REQUEST_PREVIEW_QUAD 2u
#define H2_RENDER_PROTOCOL_VERSION 1u
#define H2_RENDER_IMMEDIATE_MAX_VERTICES 4u

struct h2_render_immediate_request {
    unsigned int kind;
    unsigned int version;
    unsigned int primitive_type;
    unsigned int vertex_count;
    unsigned int color;
    int texture_tag;
    int texture_index;
    int layer;
};

struct h2_render_immediate_vertex {
    float texture_x;
    float texture_y;
    float x;
    float y;
    float z;
    float w;
};

#ifdef __cplusplus
static_assert(sizeof(h2_render_immediate_request) == 32,
              "immediate render request layout");
static_assert(sizeof(h2_render_immediate_vertex) == 24,
              "immediate render vertex layout");
#endif

#endif

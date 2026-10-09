#ifndef H2_PORT_RENDER_PROTOCOL_H
#define H2_PORT_RENDER_PROTOCOL_H

#define H2_RENDER_REQUEST_IMMEDIATE 1u
#define H2_RENDER_REQUEST_PREVIEW_QUAD 2u
#define H2_RENDER_REQUEST_INTERFACE_QUAD 3u
#define H2_RENDER_PROTOCOL_VERSION 1u
#define H2_RENDER_IMMEDIATE_MAX_VERTICES 4u
#define H2_TEXT_PROTOCOL_VERSION 1u

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

struct h2_text_request_metadata {
    unsigned int version;
    int font;
    int style;
    int justification;
    unsigned int flags;
    unsigned int shadow_color;
};

#ifdef __cplusplus
static_assert(sizeof(h2_render_immediate_request) == 32,
              "immediate render request layout");
static_assert(sizeof(h2_render_immediate_vertex) == 24,
              "immediate render vertex layout");
static_assert(sizeof(h2_text_request_metadata) == 24,
              "text request metadata layout");
#endif

#endif

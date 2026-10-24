/* The Metal renderer behind the guest D3D runtime's draw packets
 * (h2/gpu_protocol.h).
 *
 * It renders the game's back buffer offscreen, at a whole multiple of the
 * game's resolution, and presents it into a CAMetalLayer when there is a
 * window. Every draw reads its vertices, indices, constants and texels
 * straight from guest memory. */
#pragma once

#include "h2/gpu_protocol.h"

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace h2::port {
class GuestArena;
}

namespace h2::gpu {

struct RendererStats {
    std::uint64_t draws = 0;
    std::uint64_t draws_skipped = 0;      /* to targets other than the back buffer */
    std::uint64_t draws_failed = 0;       /* shader translation or pipeline errors */
    std::uint64_t clears = 0;
    std::uint64_t frames = 0;
    std::uint64_t pipelines = 0;
    std::uint64_t textures = 0;
};

class Renderer {
public:
    /* scale: the offscreen target is (640 x 480) * scale */
    static std::unique_ptr<Renderer> create(const h2::port::GuestArena& arena, int scale,
                                            std::string& error);
    virtual ~Renderer() = default;

    /* a CAMetalLayer (as a void *) to present into, or nullptr */
    virtual void attach_layer(void* layer) = 0;
    virtual void draw(const h2_gpu_draw& packet) = 0;
    virtual void clear(const h2_gpu_clear& packet) = 0;
    /* ends the frame; keep_pixels makes the next read_pixels see it */
    virtual void present(bool keep_pixels) = 0;
    /* the last kept frame as BGRA rows, top first */
    virtual bool read_pixels(std::vector<std::uint8_t>& bgra, int& width, int& height) = 0;
    virtual RendererStats stats() const = 0;
};

} // namespace h2::gpu

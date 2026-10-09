#include "unknown_11c920.h"

#ifdef H2_NATIVE_PORT
class c_library_30c470
{
public:
    void detach(void *callback);
private:
    byte unknown00[0x6c];
    void **callbacks;
    long callback_count;
};
static_assert(sizeof(c_library_30c470) == 0x74,
    "Havok listener cache layout");

// Native reconstruction of retail 0x30c470. Retail assumes the callback is
// attached and clears the matching slot; its absent case writes through index
// -1. Native preserves the attached case exactly and fails closed if lifecycle
// ownership is inconsistent instead of corrupting adjacent cache state.
void c_library_30c470::detach(void *callback)
{
    if (!callbacks || callback_count <= 0)
        return;
    for (long i = 0; i < callback_count; ++i)
    {
        if (callbacks[i] == callback)
        {
            callbacks[i] = NULL;
            return;
        }
    }
}
#endif

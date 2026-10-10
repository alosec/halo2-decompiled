#include "effects.h"
#include "globals.h"

extern "C" byte h2_native_effect_particle_provider_prt3_object[];
extern "C" byte h2_native_effect_particle_provider_prtm_object[];

typedef void (__thiscall *particle_provider_initialize)(void *, long);

static c_type_4e7709 *particle_provider_initialize_for_tag(byte *object, long tag_index)
{
    particle_provider_initialize *table =
        *(particle_provider_initialize **)object;
    table[20](object, tag_index);
    return (c_type_4e7709 *)object;
}

// @retail 0x137bd0
c_type_4e7709 *function_137bd0(long tag_index)
{
    dword group = *(dword *)&g_4e3b44[(short)tag_index];
    if (group == 0x70727433) /* prt3 */
        return particle_provider_initialize_for_tag(
            h2_native_effect_particle_provider_prt3_object, tag_index);
    if (group == 0x5052544d) /* PRTM */
        return particle_provider_initialize_for_tag(
            h2_native_effect_particle_provider_prtm_object, tag_index);
    return 0;
}

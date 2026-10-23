/* PROFILE_MODE_SCREEN_NATIVE.CPP: the native port's profile chooser screen.
   It constructs the port's self-contained profile list view
   (h2_profile_list_construct, unknown_24a2e7.cpp), which reads durable native
   profiles, in place of retail's c_unknown_249fa3 (profile_mode_screen.cpp). */
#if !defined(H2_NATIVE_PORT)
#error "native port only"
#endif
// @flags /O1 /Oi /Gr
#include "screen_widgets.h"
#include <new>

extern "C" void h2_profile_list_construct(void *storage, word user_flags);

/* The retail screen at 0x18f4bb is a c_screen_with_menu followed by the
   profile list at +0x614. The list occupies the remainder through +0x11dc. */
class c_profile_mode_screen : public c_screen_with_menu
{
public:
    c_profile_mode_screen(long arg_0, long arg_1, word arg_2);
    virtual ~c_profile_mode_screen() {}
    virtual void v17();
    virtual screen_load_proc get_load_proc();
    byte profile_list[0x11dc - 0x614];
    bool mode;
    byte field_11dd[3];
};

#if defined(H2_NATIVE_PORT)
static_assert(sizeof(c_screen_with_menu) == 0x614, "profile screen base layout");
static_assert(sizeof(c_profile_mode_screen) == 0x11e0, "profile screen retail layout");
#endif

c_class_1473c9 *__stdcall profile_mode_screen_load(s_screen_parameters *arg_0);
c_class_1473c9 *__stdcall profile_mode_screen_load_online(s_screen_parameters *arg_0);

// native view of retail 0x18f4bb (profile_mode_screen.cpp)
c_profile_mode_screen::c_profile_mode_screen(long arg_0, long arg_1, word arg_2) :
    c_screen_with_menu(0xa, arg_0, arg_1, arg_2, profile_list),
    mode(false)
{
    h2_profile_list_construct(profile_list, arg_2);
}

// native view of retail 0x18f4f6 (profile_mode_screen.cpp)
void c_profile_mode_screen::v17()
{
    profile_list[0x88] = mode;
}

// native view of retail 0x18f503 (profile_mode_screen.cpp)
screen_load_proc c_profile_mode_screen::get_load_proc()
{
    return mode ? profile_mode_screen_load_online : profile_mode_screen_load;
}

// native view of retail 0x18f42d (profile_mode_screen.cpp)
c_class_1473c9 *__stdcall profile_mode_screen_load(s_screen_parameters *arg_0)
{
    c_profile_mode_screen *local_0;
    void *local_1 = function_1a47fd(sizeof(c_profile_mode_screen));
    if (local_1)
        local_0 = ::new (local_1) c_profile_mode_screen(arg_0->a, arg_0->b, arg_0->user_flags);
    else
        local_0 = 0;
    local_0->m6c = true;
    local_0->mode = false;
    ((c_class_1473c9 *)local_0)->function_147f6d(arg_0);
    return (c_class_1473c9 *)local_0;
}

// native view of retail 0x18f474 (profile_mode_screen.cpp)
c_class_1473c9 *__stdcall profile_mode_screen_load_online(s_screen_parameters *arg_0)
{
    c_profile_mode_screen *local_0;
    void *local_1 = function_1a47fd(sizeof(c_profile_mode_screen));
    if (local_1)
        local_0 = ::new (local_1) c_profile_mode_screen(arg_0->a, arg_0->b, arg_0->user_flags);
    else
        local_0 = 0;
    local_0->m6c = true;
    local_0->mode = true;
    ((c_class_1473c9 *)local_0)->function_147f6d(arg_0);
    return (c_class_1473c9 *)local_0;
}

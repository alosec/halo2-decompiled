// @flags /O1 /Oi /Gr
#include "screen_widgets.h"
#include <new>

extern "C" void h2_profile_list_construct(void *storage, word user_flags);

/* The retail screen at 0x18f4bb is a c_screen_with_menu followed by the
   profile list at +0x614. The list occupies the remainder through +0x11dc. */
class c_18f4bb : public c_screen_with_menu
{
public:
    c_18f4bb(long arg_0, long arg_1, word arg_2);
    virtual ~c_18f4bb() {}
    virtual void v17();
    virtual screen_load_proc get_load_proc();
    byte profile_list[0x11dc - 0x614];
    bool mode;
    byte field_11dd[3];
};

#if defined(H2_NATIVE_PORT)
static_assert(sizeof(c_screen_with_menu) == 0x614, "profile screen base layout");
static_assert(sizeof(c_18f4bb) == 0x11e0, "profile screen retail layout");
#endif

c_class_1473c9 *__stdcall function_18f42d(s_screen_parameters *arg_0);
c_class_1473c9 *__stdcall function_18f474(s_screen_parameters *arg_0);

// @retail 0x18f4bb
c_18f4bb::c_18f4bb(long arg_0, long arg_1, word arg_2) :
    c_screen_with_menu(0xa, arg_0, arg_1, arg_2, profile_list),
    mode(false)
{
    h2_profile_list_construct(profile_list, arg_2);
}

// @retail 0x18f4f6
void c_18f4bb::v17()
{
    profile_list[0x88] = mode;
}

// @retail 0x18f503
screen_load_proc c_18f4bb::get_load_proc()
{
    return mode ? function_18f474 : function_18f42d;
}

// @retail 0x18f42d
c_class_1473c9 *__stdcall function_18f42d(s_screen_parameters *arg_0)
{
    c_18f4bb *local_0;
    void *local_1 = function_1a47fd(sizeof(c_18f4bb));
    if (local_1)
        local_0 = ::new (local_1) c_18f4bb(arg_0->a, arg_0->b, arg_0->user_flags);
    else
        local_0 = 0;
    local_0->m6c = true;
    local_0->mode = false;
    ((c_class_1473c9 *)local_0)->function_147f6d(arg_0);
    return (c_class_1473c9 *)local_0;
}

// @retail 0x18f474
c_class_1473c9 *__stdcall function_18f474(s_screen_parameters *arg_0)
{
    c_18f4bb *local_0;
    void *local_1 = function_1a47fd(sizeof(c_18f4bb));
    if (local_1)
        local_0 = ::new (local_1) c_18f4bb(arg_0->a, arg_0->b, arg_0->user_flags);
    else
        local_0 = 0;
    local_0->m6c = true;
    local_0->mode = true;
    ((c_class_1473c9 *)local_0)->function_147f6d(arg_0);
    return (c_class_1473c9 *)local_0;
}

#ifndef FRAME_STORAGE_H
#define FRAME_STORAGE_H

#include "unknown_11c920.h"
#include "unknown_0259d0.h"

struct s_245c0_state;
struct s_frame_camera
{
    point3f position;
    vector3f forward;
    vector3f up;
    byte unknown24[0xc];
    short field_30;
    short field_32;
    short field_34;
    short field_36;
    byte unknown38[0x74 - 0x38];
};

typedef char assert_frame_camera_size[(sizeof(s_frame_camera) == 0x74) ? 1 : -1];

struct s_frame_storage
{
    union
    {
        byte data[0x298];
        struct { short value; } field_0;
        struct { byte unknown00[0x2]; short value; } field_2;
        struct { byte unknown00[0x7]; byte value; } field_7;
        struct { byte unknown00[0x18]; s_frame_camera value; } field_18;
        struct { byte unknown00[0x40]; real value; } field_40;
        struct { byte unknown00[0x48]; word value; } field_48;
        struct { byte unknown00[0x4a]; word value; } field_4a;
        struct { byte unknown00[0x4c]; word value; } field_4c;
        struct { byte unknown00[0x4e]; word value; } field_4e;
        struct { byte unknown00[0x58]; real value; } field_58;
        struct { byte unknown00[0x5c]; real value; } field_5c;
        struct { byte unknown00[0x8c]; real value[48]; } field_8c;
        struct { byte unknown00[0xc4]; real value[9]; } field_c4;
        struct { byte unknown00[0x14c]; long value; } field_14c;
        struct { byte unknown00[0x15d]; byte value; } field_15d;
        struct { byte unknown00[0x168]; vector3f value; } field_168;
        struct { byte unknown00[0x174]; real value; } field_174;
        struct { byte unknown00[0x178]; real value; } field_178;
        struct { byte unknown00[0x17c]; real value; } field_17c;
        struct { byte unknown00[0x180]; vector3f value; } field_180;
        struct { byte unknown00[0x18c]; real value; } field_18c;
        struct { byte unknown00[0x190]; real value; } field_190;
        struct { byte unknown00[0x194]; real value; } field_194;
        struct { byte unknown00[0x198]; vector3f value; } field_198;
        struct { byte unknown00[0x1a4]; real value; } field_1a4;
        struct { byte unknown00[0x1a8]; color3f value; } field_1a8;
        struct { byte unknown00[0x1b4]; real value; } field_1b4;
        struct { byte unknown00[0x1b8]; long value; } field_1b8;
        struct { byte unknown00[0x1bc]; color3f value; } field_1bc;
        struct { byte unknown00[0x1d4]; real value; } field_1d4;
        struct { byte unknown00[0x1d8]; real value; } field_1d8;
        struct { byte unknown00[0x1dc]; real value; } field_1dc;
        struct { byte unknown00[0x1e0]; real value; } field_1e0;
        struct { byte unknown00[0x1ec]; vector3f value; } field_1ec;
        struct { byte unknown00[0x1f8]; real value; } field_1f8;
        struct { byte unknown00[0x258]; real value; } field_258;
        struct { byte unknown00[0x260]; byte value; } field_260;
        struct { byte unknown00[0x264]; real value; } field_264;
        struct { byte unknown00[0x268]; real value; } field_268;
        struct { byte unknown00[0x270]; long value; } field_270;
        struct { byte unknown00[0x274]; real value; } field_274;
        struct { byte unknown00[0x278]; real value; } field_278;
        struct { byte unknown00[0x27c]; real value; } field_27c;
        struct { byte unknown00[0x280]; real value; } field_280;
        struct { byte unknown00[0x284]; real value; } field_284;
        struct { byte unknown00[0x290]; s_245c0_state * value; } field_290;
    };
};

extern s_frame_storage g_frame_storage;
#define g_485600 (g_frame_storage.field_0.value)
#define g_485602 (g_frame_storage.field_2.value)
#define g_485607 (g_frame_storage.field_7.value)
#define g_485618 (g_frame_storage.field_18.value)
#define g_485640 (g_frame_storage.field_40.value)
#define g_485648 (g_frame_storage.field_48.value)
#define g_48564a (g_frame_storage.field_4a.value)
#define g_48564c (g_frame_storage.field_4c.value)
#define g_48564e (g_frame_storage.field_4e.value)
#define g_485658 (g_frame_storage.field_58.value)
#define g_48565c (g_frame_storage.field_5c.value)
#define g_48568c (g_frame_storage.field_8c.value)
#define g_4856c4 (g_frame_storage.field_c4.value)
#define g_48574c (g_frame_storage.field_14c.value)
#define g_48575d (g_frame_storage.field_15d.value)
#define g_485768 (g_frame_storage.field_168.value)
#define g_485774 (g_frame_storage.field_174.value)
#define g_485778 (g_frame_storage.field_178.value)
#define g_48577c (g_frame_storage.field_17c.value)
#define g_485780 (g_frame_storage.field_180.value)
#define g_48578c (g_frame_storage.field_18c.value)
#define g_485790 (g_frame_storage.field_190.value)
#define g_485794 (g_frame_storage.field_194.value)
#define g_485798 (g_frame_storage.field_198.value)
#define g_4857a4 (g_frame_storage.field_1a4.value)
#define g_4857a8 (g_frame_storage.field_1a8.value)
#define g_4857b4 (g_frame_storage.field_1b4.value)
#define g_4857b8 (g_frame_storage.field_1b8.value)
#define g_4857bc (g_frame_storage.field_1bc.value)
#define g_4857d4 (g_frame_storage.field_1d4.value)
#define g_4857d8 (g_frame_storage.field_1d8.value)
#define g_4857dc (g_frame_storage.field_1dc.value)
#define g_4857e0 (g_frame_storage.field_1e0.value)
#define g_4857ec (g_frame_storage.field_1ec.value)
#define g_4857f8 (g_frame_storage.field_1f8.value)
#define g_485858 (g_frame_storage.field_258.value)
#define g_485860 (g_frame_storage.field_260.value)
#define g_485864 (g_frame_storage.field_264.value)
#define g_485868 (g_frame_storage.field_268.value)
#define g_485870 (g_frame_storage.field_270.value)
#define g_485874 (g_frame_storage.field_274.value)
#define g_485878 (g_frame_storage.field_278.value)
#define g_48587c (g_frame_storage.field_27c.value)
#define g_485880 (g_frame_storage.field_280.value)
#define g_485884 (g_frame_storage.field_284.value)
#define g_485890 (g_frame_storage.field_290.value)

typedef char assert_frame_size[(sizeof(s_frame_storage) == 0x298) ? 1 : -1];

#endif

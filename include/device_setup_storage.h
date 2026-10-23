#ifndef DEVICE_SETUP_STORAGE_H
#define DEVICE_SETUP_STORAGE_H

#include <stddef.h>
#include "unknown_11c920.h"


struct s_128c0_settings
{
    byte depth_format, interval_flag;
    short interval;
    long quality, extra;
};

struct s_device_setup_storage
{
    union
    {
        byte data[0xc];
        struct { s_128c0_settings value; } field_0;
    };
};

extern s_device_setup_storage g_device_setup_storage;
#define g_43ed80 (g_device_setup_storage.field_0.value)
typedef char assert_device_setup_offset_0[(offsetof(s_device_setup_storage, field_0.value) == 0x0) ? 1 : -1];

typedef char assert_device_setup_size[(sizeof(s_device_setup_storage) == 0xc) ? 1 : -1];

struct s_runtime_device_configuration
{
    union
    {
        dword packed;
        struct { word flags; short interval; };
    };
    long quality, extra;
};
extern s_runtime_device_configuration g_runtime_device_configuration;
#define g_485ac8 (g_runtime_device_configuration.flags)
#define g_485aca (g_runtime_device_configuration.interval)
#define g_485acc (g_runtime_device_configuration.quality)
#define g_485ad0 (g_runtime_device_configuration.extra)
typedef char assert_runtime_device_configuration_size[(sizeof(s_runtime_device_configuration) == 12) ? 1 : -1];

#endif

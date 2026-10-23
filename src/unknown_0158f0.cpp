#include "surface_resource_pairs.h"
// @flags /O2 /Gr
#include "palette_cache_storage.h"
#include "palette_storage.h"
/* UNKNOWN_0158F0.CPP: D3D texture and palette creation (the D3D header
   allocation is inlined from the SDK's D3DDevice_CreateTexture2) */

#include "unknown_11c920.h"
#include <xtl.h>
#include "globals.h"

namespace D3D
{
	struct CDevice
	{
		void LazySetStateVB(dword value);
		static dword m_PushSegmentSize;
	};

	dword volatile *WINAPI MakeRequestedSpace(dword fallback_size, dword requested_size);
}

// Retail has two 24-entry format rows; unrelated data starts at 0x450828.
// Values map bitmap formats to hardware formats, with -1 for unsupported ones.
long g_450768[2][24] = {
    {0x19, 0, 1, 0x1a, -1, -1, 5, -1, 2, 4, 7, 6,
     -1, -1, 0xc, 0xe, 0xf, 0xb, 0xb, -1, -1, -1, 0x28, 0x28},
    {0x1f, 0x13, 0x1b, 0x20, -1, -1, 0x11, -1, 0x10, 0x1d, 0x1e, 0x12,
     -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 0x17, 0x17}
};
dword g_43e8a8[12];
long g_43e8d8[6];

s_surface_resource_pairs g_surface_resource_pairs;
D3DResource *g_485ae4;
D3DResource *g_485ae8;
D3DResource *g_485aec;

// @retail 0x14a60
void function_014a60(void)
{
	if (g_509378)
	{
		D3DResource_Release(g_509378);
		g_509378 = NULL;
	}
	if (g_509380)
	{
		D3DResource_Release(g_509380);
		g_509380 = NULL;
	}
	if (g_509374)
	{
		D3DResource_Release(g_509374);
		g_509374 = NULL;
	}
	if (g_50937c)
	{
		D3DResource_Release(g_50937c);
		g_50937c = NULL;
	}
}

// @retail 0x1d2a0
void function_01d2a0(void)
{
	if (g_485ae4)
	{
		D3DResource_Release(g_485ae4);
		g_485ae4 = NULL;
	}
	if (g_485ae8)
	{
		D3DResource_Release(g_485ae8);
		g_485ae8 = NULL;
	}
	if (g_485aec)
	{
		D3DResource_Release(g_485aec);
		g_485aec = NULL;
	}
}

/* the palette tag data: a list of palette groups, each pointing at 256-color
   palettes (0x400 bytes apart) that are loaded into D3D palette headers */
struct s_palette_group
{
	byte unknown00[8];
	long palette_count;
	dword palettes;
	byte unknown10[0x58];
};

struct s_palette_tag_data
{
	byte unknown00[0x80];
	long group_count;
	s_palette_group *groups;
};

/* one entry of the global array at +0x214; the palette tag index is at +0x1c */
struct s_palette_source
{
	byte unknown00[0x1c];
	long tag_index;
	byte unknown20[0x24];
};


long g_484dc0[4];
long g_467000;
long g_5093ac;
dword **g_407488;


// @retail 0x158f0
bool function_0158f0(
	short b,
	byte a,
	long width,
	long height,
	long levels,
	long usage_index,
	D3DTexture **out)
{
	D3DTexture *texture = D3DDevice_CreateTexture2(width, height, 1, levels + 1, g_43e8a8[usage_index], (D3DFORMAT)g_450768[a][b], D3DRTYPE_TEXTURE);
	if (texture == NULL)
	{
		*out = NULL;
		return false;
	}
	*out = texture;
	return true;
}

// @retail 0x159b0
bool function_0159b0(
	long edge,
	short b,
	long levels,
	long usage_index,
	D3DCubeTexture **out)
{
	D3DCubeTexture *texture = (D3DCubeTexture *)D3DDevice_CreateTexture2(edge, edge, 1, levels + 1, g_43e8a8[usage_index], (D3DFORMAT)g_450768[0][b], D3DRTYPE_CUBETEXTURE);
	if (texture == NULL)
	{
		*out = NULL;
		return false;
	}
	*out = texture;
	return true;
}

// @retail 0x15a60
bool function_015a60(
	short a,
	long width,
	long height,
	long depth,
	long levels,
	long usage_index,
	D3DVolumeTexture **out)
{
	D3DVolumeTexture *texture = (D3DVolumeTexture *)D3DDevice_CreateTexture2(width, height, depth, levels + 1, g_43e8a8[usage_index], (D3DFORMAT)g_450768[0][a], D3DRTYPE_VOLUMETEXTURE);
	if (texture == NULL)
	{
		*out = NULL;
		return false;
	}
	*out = texture;
	return true;
}

__declspec(noinline) bool function_015b10(long index, D3DPalette **out);

// @retail 0x15b10
bool function_015b10(
	long index,
	D3DPalette **out)
{
	D3DPalette *palette = D3DDevice_CreatePalette2((D3DPALETTESIZE)g_43e8d8[index]);
	if (palette == NULL)
	{
		*out = NULL;
		return false;
	}
	*out = palette;
	return true;
}

// @retail 0x15b70
void function_015b70(void)
{
	long stage;
	long group_index;
	long palette_total = 0;
	s_palette_tag_data *field_4_7;

	for (stage = 0; stage < 4; stage++)
	{
		D3DDevice_SetPalette(stage, g_484dbc);
		g_484dc0[stage] = NONE;
	}

	long tag_index = g_4e0350->sources[g_4686c4].tag_index;
	g_467000 = NONE;
	g_5093ac = 0;
	if (tag_index != NONE)
	{
		field_4_7 = g_4e3b44[tag_index & 0xffff].palette;
		for (group_index = 0; group_index < field_4_7->group_count; group_index++)
		{
			s_palette_group *group = &field_4_7->groups[group_index];
			for (long i = 0; i < group->palette_count; i++)
			{
				if (palette_total >= 0 && palette_total < 32)
				{
					D3DPalette *palette = &g_484dd0[palette_total];
					palette->Data = (group->palettes + i * 0x400) & 0x7fffffff;
					palette->Common = 0x30001;
					((D3DResource *)palette)->Lock = 0;
					palette_total++;
				}
			}
		}
		g_5093ac = palette_total;
	}
}

// @retail 0x15c90
void function_015c90(long stage, long index)
{
	if (index != g_467000 && index >= 0 && index < g_5093ac && index != g_484dc0[stage])
	{
		D3DDevice_SetPalette(stage, &g_484dd0[index]);
		g_484dc0[stage] = index;
		g_467000 = index;
	}
}

// @retail 0x15cd0
dword function_015cd0(long index, dword entry)
{
	dword result = 0xff4060;

	if (index >= 0 && index < g_5093ac)
	{
		dword *palette = (dword *)(g_484dd0[index].Data | 0x80000000);
		if (palette)
		{
			result = palette[entry & 0xff];
		}
	}
	return result;
}

s_palette_storage g_palette_storage;

s_palette_cache_storage g_palette_cache_storage;

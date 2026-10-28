/*
CRC.CPP: the LTCG feasibility spike's first match.

Halo CE's crc.c from the punpckhdq/halo decompilation (a CC0 revision from
before its 2026-10-17 change to AGPL-3.0), compiled as
C++. With XDK 5849's compiler under LTCG (/O2 /GL /Gr), function_163c00 and
function_163ba0 match the retail Halo 2 XBE at 0x163c00 and 0x163ba0:

    python tools/match.py "/O2 /Gr" spike/crc.cpp spike/crc_test.cpp -- \
        "?function_163c00@@YIXPAK@Z=163c00" \
        "?function_163ba0@@YIXPAKPBXJ@Z=163ba0"

The loop counters must be short: with long ones the compiler unrolls
function_163c00's inner loop.
*/

typedef unsigned char byte;

enum
{
	CRC_NEW = -1,
	CRC_TABLE_SIZE = 256,
	CRC32_POLYNOMIAL = 0xEDB88320
};

static void function_163c00(unsigned long *crc_table);

void function_x86aaf2(
	unsigned long *crc_reference)
{
	*crc_reference = CRC_NEW;
}

void function_163ba0(
	unsigned long *crc_reference,
	void const *buffer,
	long buffer_size)
{
	static unsigned long crc_table[CRC_TABLE_SIZE];
	static bool crc_table_built;

	unsigned long crc;
	unsigned long temp1;
	unsigned long temp2;

	if (!crc_table_built)
	{
		function_163c00(crc_table);
		crc_table_built = true;
	}

	crc = *crc_reference;

	while (buffer_size-- > 0)
	{
		temp1 = (crc >> 8) & 0x00FFFFFF;
		temp2 = crc_table[(crc ^ *((byte *)buffer)) & 0xFF];
		buffer = (byte *)buffer + 1;
		crc = temp1 ^ temp2;
	}

	*crc_reference = crc;
}

static void function_163c00(
	unsigned long *crc_table)
{
	short table_index;

	for (table_index = 0; table_index < CRC_TABLE_SIZE; ++table_index)
	{
		unsigned long crc = table_index;
		short round;

		for (round = 0; round < 8; ++round)
		{
			if (crc & 1)
			{
				crc = (crc >> 1) ^ CRC32_POLYNOMIAL;
			}
			else
			{
				crc = crc >> 1;
			}
		}

		crc_table[table_index] = crc;
	}
}

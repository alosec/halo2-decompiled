#ifndef CLUSTER_MARKS_STORAGE_H
#define CLUSTER_MARKS_STORAGE_H

#include <stddef.h>
#include "unknown_11c920.h"

/* the marks of the cluster flood (0x14a5b0): a flag set while a flood runs,
   the current stamp, and each cluster's last stamp. Retail keeps them in one
   block, so LTCG sees the flag's stores as reaching the flood's reads */
struct s_cluster_marks_storage
{
	byte unknown_0;
	bool field_1;
	byte unknown_2[0x2];
	long field_4;
	long field_8[0x200];
};

extern s_cluster_marks_storage g_cluster_marks_storage;
#define g_4e7411 (g_cluster_marks_storage.field_1)
#define g_4e7414 (g_cluster_marks_storage.field_4)
#define g_4e7418 (g_cluster_marks_storage.field_8)
typedef char assert_cluster_marks_offset_1[(offsetof(s_cluster_marks_storage, field_1) == 0x1) ? 1 : -1];
typedef char assert_cluster_marks_offset_4[(offsetof(s_cluster_marks_storage, field_4) == 0x4) ? 1 : -1];
typedef char assert_cluster_marks_offset_8[(offsetof(s_cluster_marks_storage, field_8) == 0x8) ? 1 : -1];

#endif

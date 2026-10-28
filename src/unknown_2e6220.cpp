// @flags /O2 /GL-
// Havok (third-party physics library): the name of a shape type.

// @retail 0x2e6220
char const *__cdecl function_2e6220(long type)
{
	switch (type)
	{
	case -1: return "HK_SHAPE_ALL";
	case 1: return "HK_SHAPE_CONVEX";
	case 2: return "HK_SHAPE_COLLECTION";
	case 3: return "HK_SHAPE_BV_TREE";
	case 4: return "HK_SHAPE_SPHERE";
	case 5: return "HK_SHAPE_TRIANGLE";
	case 6: return "HK_SHAPE_BOX";
	case 7: return "HK_SHAPE_CAPSULE";
	case 8: return "HK_SHAPE_CONVEX_VERTICES";
	case 9: return "HK_SHAPE_MULTI_SPHERE";
	case 10: return "HK_SHAPE_LIST";
	case 11: return "HK_SHAPE_TRIANGLE_COLLECTION";
	case 12: return "HK_SHAPE_MULTI_RAY";
	case 13: return "HK_SHAPE_HEIGHT_FIELD";
	case 14: return "HK_SHAPE_SAMPLED_HEIGHT_FIELD";
	case 16: return "HK_SHAPE_SPHERE_REP";
	case 15: return "HK_SHAPE_TRI_PATCH";
	case 17: return "HK_SHAPE_BV";
	case 18: return "HK_SHAPE_PLANE";
	case 19: return "HK_SHAPE_MOPP";
	case 21: return "HK_SHAPE_TRANSFORM";
	case 22: return "HK_SHAPE_PHANTOM_CALLBACK";
	case 23: return "HK_SHAPE_UTILITY_CONVEX_SWEEP";
	case 24: return "HK_SHAPE_USER0";
	case 25: return "HK_SHAPE_USER1";
	case 26: return "HK_SHAPE_USER2";
	case 27: return "HK_SHAPE_USER3";
	case 28: return "HK_SHAPE_USER4";
	case 29: return "HK_SHAPE_USER5";
	case 30: return "HK_SHAPE_USER6";
	case 31: return "HK_SHAPE_USER7";
	}
	return "unknown";
}

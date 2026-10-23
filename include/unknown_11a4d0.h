/* UNKNOWN_11A4D0.H: unit queries and flag setters of the script functions */

#ifndef UNKNOWN_11A4D0_H
#define UNKNOWN_11A4D0_H

#include "unknown_11c920.h"

void function_11a220(long list_index, real maximum_body_vitality, real maximum_shield_vitality);
bool function_11a4d0(long unit_index, long definition_index);
void function_11a570(long list_index, bool flag);
void function_11a680(long list_index);
void function_11a770(long unit_index, bool flag);
void function_11a7f0(long unit_index, bool flag);
void function_11a830(long unit_index, bool flag, real seconds);
void function_11a8c0(long unit_index);
void function_11a910(long unit_index);
bool function_11a960(long unit_index);
void function_11aac0(long unit_index, short ticks);
void function_11ab10(long unit_index, short ticks);
bool function_11ab60(long vehicle_index, long label, long unit_index);
bool function_11ac30(long vehicle_index, long label, long list_index);
void function_11ade0(long unit_index, long vehicle_index, long label);
void function_11af10(long unit_index, long label);
void function_11af90(long unit_index, short mode);
short function_11b0c0(long vehicle_index, long filter_range, long volatile list_index);
short function_11b2b0(long vehicle_index, long filter_range);
void function_11b350(void);
void function_11b3e0(long unit_index, bool flag);
void function_11b420(long unit_index, bool flag);
void function_11b460(long unit_index, bool flag);
bool function_11b520(long unit_index, long animation_graph_index, long animation_name, bool interpolate,
	long attached_object_index, bool flag);
void function_11b710(long unit_index, long field_7c);
real function_11b6b0(long unit_index);
bool function_11b930(long unit_index);

#endif

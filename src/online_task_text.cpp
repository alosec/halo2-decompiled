#include "unknown_11c920.h"
#include "online_tasks.h"

#ifdef H2_NATIVE_PORT
/* Native-port split of the two recovered mappings in online_tasks.cpp.  The
 * split avoids requiring unrelated Xbox Live task APIs to compile these pure
 * task-record queries. */
// @retail 0x6ba80
long online_task_get_title(long task_index)
{
	s_type_9df9da *task = online_task_try_get(task_index);
	long result = 0;

	if (task)
	{
		switch (task->type)
		{
		case 0:
			result = 0x700061f;
			break;
		case 1:
			result = 0x14000620;
			break;
		case 2:
			result = 0x11000621;
			break;
		case 3:
			result = 0x15000622;
			break;
		case 4:
			result = 0x10000623;
			break;
		case 5:
			result = 0x1c000624;
			break;
		case 6:
			result = 0xe000625;
			break;
		case 7:
			result = 0x10000626;
			break;
		case 8:
			result = 0x10000627;
			break;
		case 9:
			result = 0x16000628;
			break;
		case 10:
			result = 0x10000629;
			break;
		case 11:
			result = 0x1200062a;
			break;
		case 12:
			result = 0x1600062b;
			break;
		case 13:
			result = 0x1900062c;
			break;
		case 14:
			result = 0x1400062d;
			break;
		case 15:
			result = 0x1600062e;
			break;
		case 16:
			result = 0x1200062f;
			break;
		case 17:
			result = 0x11000630;
			break;
		case 18:
			result = 0xb000631;
			break;
		case 19:
			result = 0xb000632;
			break;
		case 20:
			result = 0xd000633;
			break;
		case 21:
			result = 0x13000634;
			break;
		case 22:
			result = 0x15000635;
			break;
		case 23:
			result = 0x11000636;
			break;
		case 24:
			result = 0xd000637;
			break;
		case 25:
			result = 0xd000638;
			break;
		case 26:
			result = 0xd000639;
			break;
		case 27:
			result = 0x1400063a;
			break;
		case 28:
			result = 0x2100063b;
			break;
		case 29:
			result = 0x1500063c;
			break;
		case 30:
			result = 0x1f00063d;
			break;
		case 31:
			result = 0x1400063e;
			break;
		case 32:
			result = 0x1800063f;
			break;
		case 33:
			result = 0xa000640;
			break;
		case 34:
			result = 0x10000641;
			break;
		case 35:
			result = 0xe000642;
			break;
		case 36:
			result = 0x1a000643;
			break;
		case 37:
			result = 0x1c000644;
			break;
		case 38:
			result = 0x1a000645;
			break;
		case 39:
			result = 0x1d000646;
			break;
		case 41:
			result = 0x1f000647;
			break;
		case 42:
			result = 0x21000648;
			break;
		case 43:
			result = 0x20000649;
			break;
		case 44:
			result = 0x2200064a;
			break;
		case 45:
			result = 0xf00064b;
			break;
		default:
			result = NONE;
			break;
		}
	}
	return result;
}

// @retail 0x6bd10
long online_task_get_description(long task_index)
{
	s_type_9df9da *task = online_task_try_get(task_index);
	long result = 0;

	if (task)
	{
		switch (task->type)
		{
		case 0:
			result = 0x700064c;
			break;
		case 1:
			result = 0x1400064d;
			break;
		case 2:
			result = 0x1100064e;
			break;
		case 3:
			result = 0x1500064f;
			break;
		case 4:
			result = 0x10000650;
			break;
		case 5:
			result = 0x1c000651;
			break;
		case 6:
			result = 0xe000652;
			break;
		case 7:
			result = 0x10000653;
			break;
		case 8:
			result = 0x10000654;
			break;
		case 9:
			result = 0x16000655;
			break;
		case 10:
			result = 0x10000656;
			break;
		case 11:
			result = 0x12000657;
			break;
		case 12:
			result = 0x16000658;
			break;
		case 13:
			result = 0x19000659;
			break;
		case 14:
			result = 0x1400065a;
			break;
		case 15:
			result = 0x1600065b;
			break;
		case 16:
			result = 0x1200065c;
			break;
		case 17:
			result = 0x1100065d;
			break;
		case 18:
			result = 0xb00065e;
			break;
		case 19:
			result = 0xb00065f;
			break;
		case 20:
			result = 0xd000660;
			break;
		case 21:
			result = 0x13000661;
			break;
		case 22:
			result = 0x15000662;
			break;
		case 23:
			result = 0x11000663;
			break;
		case 24:
			result = 0xd000664;
			break;
		case 25:
			result = 0xd000665;
			break;
		case 26:
			result = 0xd000666;
			break;
		case 27:
			result = 0x14000667;
			break;
		case 28:
			result = 0x21000668;
			break;
		case 29:
			result = 0x15000669;
			break;
		case 30:
			result = 0x1f00066a;
			break;
		case 31:
			result = 0x1400066b;
			break;
		case 32:
			result = 0x1800066c;
			break;
		case 33:
			result = 0xa00066d;
			break;
		case 34:
			result = 0x1000066e;
			break;
		case 35:
			result = 0xe00066f;
			break;
		case 36:
			result = 0x1a000670;
			break;
		case 37:
			result = 0x1c000671;
			break;
		case 38:
			result = 0x1a000672;
			break;
		case 39:
			result = 0x1d000673;
			break;
		case 41:
			result = 0x1f000674;
			break;
		case 42:
			result = 0x21000675;
			break;
		case 43:
			result = 0x20000676;
			break;
		case 44:
			result = 0x22000677;
			break;
		case 45:
			result = 0xf000678;
			break;
		default:
			result = NONE;
			break;
		}
	}
	return result;
}

#endif

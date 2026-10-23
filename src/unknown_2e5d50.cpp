// @flags /O2 /GL-
// Havok (third-party physics library): registering the four handlers for a
// pair of shape types in the handler table.
#include "agent_table_2e5d50.h"

// drops any earlier registration of the pair, records this one and installs it
// @retail 0x2e5d50
void s_1c3a40::function_2e5d50(handlers_2e5d50 const *handlers, long first_type, long second_type)
{
	for (int i = 0; i < registrations.size; i++)
	{
		if (registrations.data[i].first_type == first_type && registrations.data[i].second_type == second_type)
		{
			registrations.size--;
			for (int j = i; j < registrations.size; j++)
			{
				registrations.data[j] = registrations.data[j + 1];
			}
			i--;
		}
	}
	registration_2e5d50 &registration = registrations.next_slot();
	registration.first_type = first_type;
	registration.second_type = second_type;
	registration.handlers = *handlers;
	registration.flag = field_4d8d;
	function_2e5a90(handlers, first_type, second_type, first_type, second_type, false);
}

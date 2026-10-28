// @flags /O2 /GL-
#include "lane_a_contact_manager.h"
// @retail 0x318980
void lane_a_simple_contact_manager::remove(unsigned short id)
{
	lane_a_contact_entity *first = a;
	lane_a_contact_entity *second = b;
	event_315d20 event;
	lane_a_contact_removed_payload payload;
	payload.id = id;
	payload.manager = this;
	payload.a = first;
	payload.b = second;
	memcpy(event.unknown0,&payload.id,sizeof(payload.id));
	memcpy(event.unknown0+4,&payload.manager,12);
	function_3183b0(world,&event);
	if (first->listeners_60.size)
		function_315d90((listener_world_315d20 *)first,&event);
	if (second->listeners_60.size)
		function_315d90((listener_world_315d20 *)second,&event);
}
// @retail 0x3189f0
void lane_a_simple_contact_manager::processed(lane_a_contact_body const *first,
	lane_a_contact_body const *second, void const *, void const *input)
{
	if (--remaining == 0)
	{
		lane_a_contact_entity *entity_a = first->entity;
		lane_a_contact_entity *entity_b = second->entity;
		unsigned short delay = lane_a_contact_delay(entity_b);
		if (lane_a_contact_delay(entity_a) < delay)
			delay = lane_a_contact_delay(entity_a);
		remaining = delay + 1;
		event_315d20 event;
		lane_a_contact_processed_payload payload = {this, first, second, input};
		memcpy(event.unknown0,&payload.manager,4);
		memcpy(event.unknown0+4,&payload.a,8);
		memcpy(event.unknown0+12,&payload.input,4);
		function_318440(world,&event);
		if (entity_a->listeners_60.size)
			function_315e00((listener_world_315d20 *)entity_a,&event);
		if (entity_b->listeners_60.size)
			function_315e00((listener_world_315d20 *)entity_b,&event);
	}
}
// @retail 0x318a80
void lane_a_simple_contact_manager::cleanup()
{
	delete this;
}

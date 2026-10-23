// @flags /O2 /GL-
// Havok (third-party physics library): adding an object to the world by its
// class: an entity, a phantom, an action, a constraint, or the last kind,
// passed on with a yes.
#include "unknown_1cec30.h"

// whether a class (one of the static class objects) is in an object's chain
// of classes (src/unknown_2d9710.cpp)
struct bool_2d9710
{
	bool value;

	bool_2d9710(bool value_)
	{
		value = value_;
	}
};

class class_2d9710
{
public:
	long field_0;
	long field_4;
	long field_8;
	class_2d9710 *next;
};

class class_2d9710_owner;

class class_2d9710_link : public class_2d9710
{
public:
	bool_2d9710 is_in(class_2d9710_owner *owner) const;
};

// the static class objects
struct object_480608
{
	long unknown0;
};

struct object_480af0
{
	long unknown0;
};

struct object_480b90
{
	long unknown0;
};

struct object_480b68
{
	long unknown0;
};

struct object_48057c
{
	long unknown0;
};

extern object_480608 g_480608;
object_480af0 g_480af0;
object_480b90 g_480b90;
object_480b68 g_480b68;
object_48057c g_48057c;

// the world as the functions this one calls see it
class c_contact_query_world
{
public:
	void *add(void *volume);
};

struct s_1d1541
{
	void *function_30d2b0(void *arg_0);
};

class joint_30e3b0;

class world_30e2c0
{
public:
	joint_30e3b0 *function_30e3b0(joint_30e3b0 *joint);
};

class c_2dfbf0;

struct s_30cb70
{
	byte field_0;

	s_30cb70(byte value) : field_0(value) {}
	s_30cb70(s_30cb70 const &other) : field_0(other.field_0) {}
};

class c_3101c0
{
public:
	void function_310900(c_2dfbf0 *arg_0, s_30cb70 arg_1);
};

class world_3109b0
{
public:
	void function_3109b0(void *object);
};

// @retail 0x3109b0
void world_3109b0::function_3109b0(void *object)
{
	class_2d9710_owner *owner = (class_2d9710_owner *)object;
	if (((class_2d9710_link const *)&g_480608)->is_in(owner).value)
	{
		((hkWorld *)this)->entity_world_insert((hkEntity *)object);
	}
	else if (((class_2d9710_link const *)&g_480af0)->is_in(owner).value)
	{
		((c_contact_query_world *)this)->add(object);
	}
	else if (((class_2d9710_link const *)&g_480b90)->is_in(owner).value)
	{
		((s_1d1541 *)this)->function_30d2b0(object);
	}
	else if (((class_2d9710_link const *)&g_480b68)->is_in(owner).value)
	{
		((world_30e2c0 *)this)->function_30e3b0((joint_30e3b0 *)object);
	}
	else if (((class_2d9710_link const *)&g_48057c)->is_in(owner).value)
	{
		((c_3101c0 *)this)->function_310900((c_2dfbf0 *)object, s_30cb70(1));
	}
}

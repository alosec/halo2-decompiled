// @flags /O1 /Gr
/* USER_INTERFACE_LIST_METHODS.CPP: two small virtual methods that several
   list widgets' vtables share (retail folded the identical bodies: 0x230612
   sits in six vtables, 0x230f31 in five). One placeholder class per vtable
   until the classes are written. */

#include "unknown_11c920.h"
#include "main_globals.h"
#include "unknown_19b516.h"
#include "unknown_19b510.h"
#include "screen_widgets.h"
#include "user_interface_lists.h"

/* the list widgets' 22 slot vtables (0x458a74, 0x458b48 ...) */
class c_list_vtable
{
public:
	virtual ~c_list_vtable() {}
	virtual void v1() {}
	virtual void v2() {}
	virtual void v3() {}
	virtual void v4() {}
	virtual void v5() {}
	virtual void v6() {}
	virtual void v7() {}
	virtual void v8() {}
	virtual void v9() {}
	virtual bool v10(s_event *event) { return false; }
	virtual void v11() {}
	virtual void v12() {}
	virtual void v13() {}
	virtual void v14() {}
	virtual void v15() {}
	virtual void v16() {}
	virtual void v17() {}
	virtual void v18() {}
	virtual long v19() { return 0; }
	virtual void v20() {}
	virtual void v21() {}
};

/* the list of the screen at 0x458a00 (vtable 0x458a74) */
class c_list_458a74 : public c_list_vtable
{
public:
	virtual long v19();
};

/* the lists whose slot 10 is 0x230f31 (vtable 0x458b48 and others) */
class c_list_458b48 : public c_list_vtable
{
public:
	virtual bool v10(s_event *event);
};

/* a constant 5 */
// @retail 0x230612
long c_list_458a74::v19()
{
	return 5;
}

/* passes the event to the widget's own handler */
// @retail 0x230f31
bool c_list_458b48::v10(s_event *event)
{
	return ((c_widget *)this)->c_widget::v18(event);
}

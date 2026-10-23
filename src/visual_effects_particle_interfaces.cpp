// @flags /O2 /arch:SSE /Gr
/* VISUAL_EFFECTS_PARTICLE_INTERFACES.CPP: the interfaces the particle code
   reads particle ('prt3') and particle model ('PRTM') tags through;
   function_137bd0 picks one by a tag's group and points it at the tag
   (g_479868 for particles, g_479874 for particle models). Named to link
   after unknown_*.cpp, which keeps 0x73360 matching. */

#include "unknown_11c920.h"
#include "globals.h"
#include "effects.h"
#include "unknown_224240.h"
#include "particle_tag_views.h"

class c_particle_definition_interface
{
public:
	virtual long v0() { return 0; }
	virtual long v1() { return 0; }
	virtual long v2() { return 0; }
	virtual long v3() { return 0; }
	virtual long v4() { return NONE; }
	virtual long v5() { return NONE; }
	virtual long particle_system_count() { return 0; }
	virtual s_effect_particle_system_definition *particle_system_at(long index) { return 0; }
	virtual long name_count() { return 0; }
	virtual long const *names() { return 0; }
	virtual long const *shader_reference() { return 0; }
	virtual void render(long particle_system, long particle_location, long c, long d) { }
	virtual bool v12() { return false; }
	virtual bool v13() { return false; }
	virtual bool v14() { return false; }
	virtual bool v15() { return false; }
	virtual bool v16() { return false; }
	virtual bool multiplied() { return false; }
	virtual bool tinted() { return false; }
	virtual real function_maximum() { return 0.0f; }
	virtual void initialize(long tag_index) { }

	long tag_index;
	s_interface_definition *definition;
};

/* a particle model ('PRTM'): its vtable is at 0x453598 */
class c_particle_model_interface : public c_particle_definition_interface
{
public:
	virtual long v0();
	virtual long v1();
	virtual long v2();
	virtual long v3();
	virtual long v4();
	virtual long v5();
	virtual long particle_system_count();
	virtual s_effect_particle_system_definition *particle_system_at(long index);
	virtual long name_count();
	virtual long const *names();
	virtual long const *shader_reference();
	virtual void render(long particle_system, long particle_location, long c, long d);
	virtual bool v12();
	virtual bool v13();
	virtual bool v14();
	virtual bool v15();
	virtual bool v16();
	virtual bool multiplied();
	virtual bool tinted();
	virtual real function_maximum();
	virtual void initialize(long tag_index);

	s_particle_model_definition *model()
	{
		return (s_particle_model_definition *)definition;
	}
};

/* a particle ('prt3'): its vtable is at 0x4535f0 */
class c_particle_interface : public c_particle_definition_interface
{
public:
	virtual long v0();
	virtual long v1();
	virtual long v2();
	virtual long v3();
	virtual long v4();
	virtual long v5();
	virtual long particle_system_count();
	virtual s_effect_particle_system_definition *particle_system_at(long index);
	virtual long name_count();
	virtual long const *names();
	virtual long const *shader_reference();
	virtual void render(long particle_system, long particle_location, long c, long d);
	virtual bool v12();
	virtual bool v13();
	virtual bool v14();
	virtual bool v15();
	virtual bool v16();
	virtual bool multiplied();
	virtual bool tinted();
	virtual real function_maximum();
	virtual void initialize(long tag_index);

	s_particle_tag_view *particle()
	{
		return (s_particle_tag_view *)definition;
	}
};
// @retail 0x224cf0
long c_particle_model_interface::v0()
{
	return model()->value60;
}

// @retail 0x224d00
long c_particle_model_interface::v1()
{
	return model()->value64;
}

// @retail 0x224d10
long c_particle_model_interface::v2()
{
	return model()->value68;
}

// @retail 0x224d20
long c_particle_model_interface::v3()
{
	return model()->value6c;
}

// @retail 0x224240
long c_particle_model_interface::v4()
{
	return NONE;
}

long c_particle_model_interface::v5()
{
	return NONE;
}

/* how many particle systems the particle model attaches */
// @retail 0x224d30
long c_particle_model_interface::particle_system_count()
{
	return model()->particle_system_count;
}

/* how many names the particle model's names block holds */
// @retail 0x224d50
long c_particle_model_interface::name_count()
{
	return model()->name_count;
}

/* the particle model's names (string ids) */
// @retail 0x224d60
long const *c_particle_model_interface::names()
{
	return model()->names;
}

/* the shader reference of the tag the particle model renders */
// @retail 0x224d70
long const *c_particle_model_interface::shader_reference()
{
	s_particle_model_render_tag *render_tag =
		(s_particle_model_render_tag *)g_4e3b44[model()->render_tag_index & 0xffff].data;
	return render_tag->shader_reference;
}

/* draws a particle system of the particle model */
// @retail 0x224d90
void c_particle_model_interface::render(long particle_system, long particle_location, long c, long d)
{
	function_224db0(definition, particle_system, particle_location, c, d);
}

// @retail 0x2242e0
bool c_particle_model_interface::v12()
{
	return TEST_FIELD_BIT(definition->flag14);
}

// @retail 0x2242f0
bool c_particle_model_interface::v13()
{
	return TEST_FIELD_BIT(definition->flag9);
}

// @retail 0x224300
bool c_particle_model_interface::v14()
{
	return TEST_FIELD_BIT(definition->flag10);
}

// @retail 0x224310
bool c_particle_model_interface::v15()
{
	return TEST_FIELD_BIT(definition->flag11);
}

// @retail 0x224320
bool c_particle_model_interface::v16()
{
	return TEST_FIELD_BIT(definition->flag12);
}

/* whether the particle model's color is multiplied */
// @retail 0x224330
bool c_particle_model_interface::multiplied()
{
	return TEST_FIELD_BIT(definition->multiplied);
}

/* whether the particle model is tinted */
// @retail 0x224340
bool c_particle_model_interface::tinted()
{
	return TEST_FIELD_BIT(definition->tinted);
}

/* a particle model has no function range: always 1 */
// @retail 0x224da0
real c_particle_model_interface::function_maximum()
{
	return 1.0f;
}

/* points the interface at a tag */
// @retail 0x2243a0
void c_particle_model_interface::initialize(long index)
{
	tag_index = index;
	definition = (s_interface_definition *)g_4e3b44[index & 0xffff].data;
}

long c_particle_interface::v0()
{
	return particle()->value6c;
}

long c_particle_interface::v1()
{
	return particle()->value70;
}

long c_particle_interface::v2()
{
	return particle()->value74;
}

long c_particle_interface::v3()
{
	return particle()->value78;
}

long c_particle_interface::v4()
{
	return NONE;
}

long c_particle_interface::v5()
{
	return NONE;
}

/* how many particle systems the particle attaches */
// @retail 0x224250
long c_particle_interface::particle_system_count()
{
	return particle()->particle_system_count;
}

/* how many names the particle's names block holds */
// @retail 0x224280
long c_particle_interface::name_count()
{
	return particle()->name_count;
}

/* the particle's names (string ids) */
// @retail 0x224290
long const *c_particle_interface::names()
{
	return particle()->names;
}

/* the particle's shader reference */
// @retail 0x2242a0
long const *c_particle_interface::shader_reference()
{
	return particle()->shaders;
}

/* draws a particle system of the particle */
// @retail 0x2242b0
void c_particle_interface::render(long particle_system, long particle_location, long c, long d)
{
	function_2243c0(definition, particle_system, particle_location, tag_index, c, d);
}

bool c_particle_interface::v12()
{
	return TEST_FIELD_BIT(definition->flag14);
}

bool c_particle_interface::v13()
{
	return TEST_FIELD_BIT(definition->flag9);
}

bool c_particle_interface::v14()
{
	return TEST_FIELD_BIT(definition->flag10);
}

bool c_particle_interface::v15()
{
	return TEST_FIELD_BIT(definition->flag11);
}

bool c_particle_interface::v16()
{
	return TEST_FIELD_BIT(definition->flag12);
}

bool c_particle_interface::multiplied()
{
	return TEST_FIELD_BIT(definition->multiplied);
}

bool c_particle_interface::tinted()
{
	return TEST_FIELD_BIT(definition->tinted);
}

/* the larger end of the particle's function range */
// @retail 0x224350
real c_particle_interface::function_maximum()
{
	return particle()->function44.get_range_maximum();
}

void c_particle_interface::initialize(long index)
{
	tag_index = index;
	definition = (s_interface_definition *)g_4e3b44[index & 0xffff].data;
}

/* a particle system the particle model attaches */
// @retail 0x224d40
s_effect_particle_system_definition *c_particle_model_interface::particle_system_at(long index)
{
	return &model()->system_entries[index];
}

/* a particle system the particle attaches */
// @retail 0x224260
s_effect_particle_system_definition *c_particle_interface::particle_system_at(long index)
{
	return &particle()->system_entries[index];
}

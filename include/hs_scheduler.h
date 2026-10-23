/* HS_SCHEDULER.H: the script runtime's data: the threads that evaluate
   scripts ("hs thread", g_4f9384), their stack frames, the syntax nodes
   they evaluate ("script node", g_4f9394), the global values ("hs
   globals", g_4f9380) and the scenario's script and global blocks that
   name them. src/hs_scheduler.cpp and src/hs_evaluation.cpp use these
   layouts; src/unknown_209520.cpp has its own views of the same data. */

#ifndef HS_SCHEDULER_H
#define HS_SCHEDULER_H

#include "unknown_11c920.h"
#include "data_array.h"
#include "hs.h"
#include <stddef.h>

enum
{
	k_maximum_hs_threads = 0x140,
	k_maximum_hs_expression_nodes = 0x9000,
	k_maximum_hs_globals = 0xc00,

	/* the first slots of the global values hold the engine's external
	   globals; the scenario's own globals follow them */
	k_hs_engine_global_count = 0x41d,

	/* the number of script value types (e_hs_type) */
	k_hs_type_count = 0x3e,

	/* the function table index of the evaluator that sleeps until a
	   condition holds ("<boolean> [<short>]", 0x2089b0) */
	k_hs_function_sleep_until_condition = 21
};

typedef char assert_hs_type_count[(k_hs_type_count == _hs_type_scenery_name + 1) ? 1 : -1];

/* a thread's sleep_until: a game time, 0 to run at once, or one of these */
enum
{
	k_hs_thread_finished = NONE,
	k_hs_thread_sleep_dormant = -2,			/* until another script wakes it */
	k_hs_thread_sleep_command_script = -3	/* until its command script resumes it */
};

/* what started a thread (s_hs_thread::type) */
enum e_hs_thread_kind
{
	_hs_thread_kind_script = 0,				/* one per scenario script, at map start */
	_hs_thread_kind_global_initialize,		/* evaluates the globals' initial values */
	_hs_thread_kind_runtime_evaluate,		/* an expression run from outside; may sleep */
	_hs_thread_kind_expression_evaluate,	/* an expression run to completion at once */
	_hs_thread_kind_static_script			/* a static script called on demand */
};

/* s_hs_thread::flags */
enum e_hs_thread_flags
{
	_hs_thread_flag_initialize = 1,			/* the next evaluation starts its frame */
	_hs_thread_flag_sleep_overridden = 2,	/* another script set sleep_until; the
											   thread's own is in saved_sleep_until */
	_hs_thread_flag_verbose = 4				/* formats each evaluated value */
};

/* s_hs_expression_node::flags */
enum e_hs_expression_node_flags
{
	_hs_expression_node_flag_primitive = 1,			/* a constant or a global reference */
	_hs_expression_node_flag_script_call = 2,		/* calls a script, not a function */
	_hs_expression_node_flag_global_reference = 4,
	_hs_expression_node_flag_permanent = 8			/* kept when the map is disposed */
};

/* a frame of a thread's stack: the syntax node it evaluates and the slot,
   in the frame below, that receives its value; the frame's own values follow
   it, size bytes from data */
struct s_hs_frame
{
	s_hs_frame *next;
	long expression_node_index;
	long *result;
	short size;
	byte data[2];
};
typedef char assert_hs_frame_size[(sizeof(s_hs_frame) == 0x10) ? 1 : -1];

/* a script thread (g_4f9384, k_maximum_hs_threads of them) */
struct s_hs_thread
{
	short salt;
	byte type;					/* e_hs_thread_kind */
	byte flags;					/* e_hs_thread_flags */
	long script_index;			/* NONE for an expression thread */
	long sleep_until;
	long saved_sleep_until;
	s_hs_frame *frame;			/* the top of the stack */
	long result;
	s_hs_frame stack;			/* the bottom frame; the rest grow above it */
	byte stack_bytes[0x418 - 0x28];
};
typedef char assert_hs_thread_size[(sizeof(s_hs_thread) == 0x418) ? 1 : -1];
typedef char assert_hs_thread_frame[(offsetof(s_hs_thread, frame) == 0x10) ? 1 : -1];

/* a syntax node of a compiled script (g_4f9394, k_maximum_hs_expression_nodes
   of them, 20 bytes each) */
struct s_hs_expression_node
{
	short salt;
	union
	{
		short function_index;	/* a function call: the index in g_4744e0 */
		short script_index;		/* a script call (_hs_expression_node_flag_script_call) */
		short constant_type;	/* a primitive: the type its value has */
	};
	short type;					/* the type the parent wants */
	byte flags;					/* e_hs_expression_node_flags */
	byte unknown07;
	long next_node_index;		/* the next argument */
	long source_offset;
	long value;
};
typedef char assert_hs_expression_node_size[(sizeof(s_hs_expression_node) == 0x14) ? 1 : -1];

/* a script of the scenario (0x28 bytes) */
struct s_hs_script
{
	char name[32];
	short type;
	short return_type;
	long root_node_index;
};
typedef char assert_hs_script_size[(sizeof(s_hs_script) == 0x28) ? 1 : -1];

/* a global of the scenario (0x28 bytes) */
struct s_hs_global
{
	char name[32];
	short type;
	byte unknown22[2];
	long initial_value_node_index;
};
typedef char assert_hs_global_size[(sizeof(s_hs_global) == 0x28) ? 1 : -1];

/* the script blocks of the scenario (g_4e0350) */
struct s_hs_scenario
{
	byte unknown000[0x1b8];
	long script_count;
	s_hs_script *scripts;
	long global_count;
	s_hs_global *globals;
	byte unknown1c8[8];
	long block_1d0_count;		/* a block the code tests only for being empty */
	byte unknown1d4[0x238 - 0x1d4];
	long expression_node_count;
	s_hs_expression_node *expression_nodes;
};
typedef char assert_hs_scenario_scripts[(offsetof(s_hs_scenario, scripts) == 0x1bc) ? 1 : -1];
typedef char assert_hs_scenario_block_1d0[(offsetof(s_hs_scenario, block_1d0_count) == 0x1d0) ? 1 : -1];
typedef char assert_hs_scenario_expression_nodes[(offsetof(s_hs_scenario, expression_nodes) == 0x23c) ? 1 : -1];

/* the runtime's data arrays (see above), and whether the threads run */
extern s_record_pool *g_4f9384;	/* threads */
extern s_record_pool *g_4f9380;	/* global values */
extern s_record_pool *g_4f9394;	/* syntax nodes */
extern bool g_4f9388;			/* the threads are set up for the map */

#endif

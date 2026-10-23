# Finishing an Xbox Live logon (unknown_06bff0)

Retail range covered: `0x6bff0`–`0x6c16f`. This is one inventory entry, 382
retail bytes, `todo`, and `config/functions.csv` gives it no source. The repo
has a declaration (`src/online_tasks.cpp:807`) and a stub that returns
`false` (`src/stubs/lane_d.cpp:42`), and nothing else. Its neighbours
`0x6bfa0` (`src/online_tasks.cpp:526`) and `0x6c170`
(`src/unknown_06b590.cpp:71`) are `matched`, in lane D's online code.

`function_6bff0` runs once a logon task reports that the connection is up. Both
callers are the logon tasks' update functions in `src/online_tasks.cpp`
(see [The callers](#the-callers)). It:

1. Gives up with result 5 unless the logon task `g_467214` exists and
   `online_task_get_logon_status` reports it established (1).
2. Gives up with result 10 if `XOnlineGetLogonUsers` returns no users.
3. Reads the users' logon status: from the current users for a logon task
   (`type` 0), from the task's results for a change of users (`type` 1).
   Status 1 (a user has a message) sets `g_50944f`. Status 2 (an account
   needs management) marks the task failed and ends with result 2.
4. Asks `XOnlineGetServiceInfo` about each of the 13 services in `g_467178`
   and stores the answer in the service's `connect`. A service that fails
   and has `unknown04` set ends the function with result 10.
5. Refreshes the users' mute lists, clears their presence, starts the voice
   engine, finds the first signed-in non-guest slot and requests the users'
   settings (`function_8c660`).
6. For a logon task only, queues downloads of three online files for that
   slot (`network_configuration.dat`, `multiplayer_hopper.dat`,
   `message_of_the_day.dat`), starts the friends list and checks for new
   content.
7. Stores the result (1 on success) in `task->result` and returns
   `result == 1`.

**Analysis only:** this document adds no source, and nothing in it has been
built or checked against retail with the original compiler. Names are
provisional. The `function_<va>` form is primary; the descriptions are offered
for whoever decompiles it.

## Boundary

- **The range** is one function, `0x6bff0`–`0x6c16d`, then two `int3` bytes
  (`0xcc`) to `0x6c16f`. The last instruction is `ret 4` (`0x6c16b`, three
  bytes). The next entry is `0x6c170` (`matched`,
  `src/unknown_06b590.cpp:71`).
- **The callers** are `0x6c2a0` (`matched`, `src/online_tasks.cpp:817`), at
  `0x6c323`, and `0x6c360` (`todo`, source at `src/online_tasks.cpp:852`), at
  `0x6c402`.
- **The callees** are eleven game functions, ten of them `matched` (see
  [Existing declarations](#existing-declarations)), and two Xbox Live library
  routines with no inventory entry: `0x3a0671` and `0x3a067c` (see
  [Callees without source](#callees-without-source)).

## Conventions

| Retail | Bytes | Arguments | Returns | Callers | What it does |
| --- | --- | --- | --- | --- | --- |
| `0x6bff0` | 382 | stack: `task` (`s_type_9df9da *`, `include/online_tasks.h:18`); `ret 4` | `bool` in `al` | 2 + 0 | Finishes a logon task once the connection is up |

**The frame.** No locals. `ebx`, `ebp`, `esi` and `edi` are pushed
(`0x6bff8`–`0x6bfff`); `task` is read once, at `0x6bffa`, into `ebp`, after
two pushes. The registers through the body:

| Register | Holds |
| --- | --- |
| `ebp` | `task` |
| `ebx` | `result`: 1 from `0x6c036`, 2 at `0x6c06b`, 10 at `0x6c140`, 5 at `0x6c157` |
| `edi` | the users from `XOnlineGetLogonUsers` (`0x6c017`), then the slot from `player_slot_find_online_user` (`0x6c0b8`) |
| `esi` | the service table's byte offset in the loop (`0x6c045`–`0x6c0a0`), then each file's header for `0x80f70` |

`eax` carries `g_467214` from `0x6bff0` into `0x6cd50`, which takes its index
in `eax` (see [Existing declarations](#existing-declarations)).

### The callers

Both callers push the task they are updating (`push esi`, `0x6c322` and
`0x6c3fa`) and test `al`:

- **`0x6c2a0`** (`src/online_tasks.cpp:817`–`:850`) calls it when the logon
  status of its `task_index` is 1 and the task is not yet `running`
  (`:830`–`:841`). On `true` it sets `running`; on `false` it sets `finished`
  and `failed` and turns a result of 0 or 1 into 10.
- **`0x6c360`** (`src/online_tasks.cpp:852`–`:918`) calls it for the
  continue result `0x1513f1` (`:899`), after setting `finished` and
  `result = 1` (`0x6c3f6`, `0x6c3fb`). On `false` it sets `failed`, with the
  same 0-or-1 rule.

The result codes `function_6bff0` writes (1, 2, 5, 10) are the ones
`function_6cd53` uses for the logon task (`src/online_tasks.cpp:551`–`:582`):
1 is `XONLINE_S_LOGON_CONNECTION_ESTABLISHED`, 2
`XONLINE_E_LOGON_USER_ACCOUNT_REQUIRES_MANAGEMENT`, 5
`XONLINE_E_LOGON_CONNECTION_LOST` (and a failed `function_8d7c0`, `:559`–`:564`),
and 10 the `default`: a null or `NONE` handle (`:554`–`:557`) and any result
without a case of its own, `XONLINE_S_LOGON_USER_HAS_MESSAGE` among them
(`:575`–`:576`). 0 and 3, 4, 6, 7 and 8 are its other codes, which this
function does not produce.

## Data

**The task** (`s_type_9df9da`, `include/online_tasks.h:18`–`37`). The function
reads `type` (`+4`, `0x6c021` and `0x6c0bf`), sets `flag_bits.failed` (bit 5
of `+2`, `or byte ptr [ebp + 2], 0x20`, `0x6c067`) and writes `result`
(`+0x10`). `0x6c8b0` creates a logon task with `type` 0
(`src/unknown_06b590.cpp:294`) and a change-of-users task with `type` 1
(`:324`).

**The logon** is `g_467214` (`src/online_tasks.cpp:547`), the index of the
logon task, `NONE` when there is none. The function tests this task's status,
whichever task it was passed. `g_50944f` (`src/online_tasks.cpp:549`) means a
user has a message: `function_6cd53` sets it (`:575`), `0x6c8b0` clears it
when a logon starts (`src/unknown_06b590.cpp:289`, `:319`).

**The services** are `g_467178` (`src/unknown_06b590.cpp:26`), 13
`s_online_service` records of `0xc` bytes (`:17`): `service_id` at `+0`
(read at `0x6c047`), `unknown04` at `+4` (`0x6c082`, as `0x46717c` plus the
offset), `connect` at `+8` (`0x6c059`, `0x6c08a`, as `0x467180` plus the
offset). The loop ends at offset `0x9c`, 13 records (`0x6c09a`). `unknown04` is
1 for the twelve Live services and 0 for the last, `0x4d530064`, which the
repo's comment calls "not a Live service" (`:16`). `0x6d220` later reads
`connect` for service `0x14` (`src/unknown_06b590.cpp:252`–`:267`).

**The three files.** `0x6c0de`, `0x6c0fc` and `0x6c11a` load `esi` with
`0x477058`, `0x47d8f4` and `0x479780` for `pending_message_request_receive`
(`0x80f70`), which reads them as `s_pending_message_header`s
(`include/pending_messages.h:18`): `state` at `+0xa` (`0x80f73`), `kind` at
`+8` (`0x80f7a`), `unknown10` at `+0x10` (`0x80f86`), `values` at `+0x14`
(`0x80f83`–`0x80f9b`). Retail's initial data for the three, and for
`g_479750` (`src/unknown_08c5b0.cpp:115`), the header the repo does declare as
one, has one shape (dwords from `+0`):

| Global | `+0` definition (its file) | `+4` | `+8` (`kind`, `state`) | `+0xc` | `+0x10` | `+0x14`, `+0x18`, `+0x1c` | `+0x20` | `+0x24` | `+0x28` | `+0x2c` |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| `0x477058` | `0x467140` (`network_configuration.dat`) | 0 | 1, 0 | 0 | -1 | -1, -1, 0 | -1 | `0x4ce040` | `0x1730` | 0 |
| `0x47d8f4` | `0x46dd64` (`multiplayer_hopper.dat`) | 0 | 4, 0 | 0 | -1 | -1, -1, 0 | -1 | 0 | 0 | `0x20000` |
| `0x479780` | `0x4672bc` (`message_of_the_day.dat`) | 0 | 4, 0 | 0 | -1 | -1, -1, 0 | -1 | 0 | 0 | `0x8000` |
| `0x479750` | `0x46729c` (`bungienet_user.dat`) | 0 | 1, 0 | 0 | -1 | -1, -1, 0 | -1 | `0x4d8800` | `0x314` | 0 |

Each definition's second dword points at the file name, a wide string. The
repo defines three of the four definitions as wholes: `g_467140`
(`src/unknown_0662e0.cpp:854`), `g_46729c` (`src/unknown_08c5b0.cpp:110`) and
`g_4672bc` (`src/unknown_08ca00.cpp:202`). Of the hopper's, `0x46dd64`, it
names only the two handler slots at `+0x10` and `+0x18` (`g_46dd74`,
`g_46dd7c`, `src/unknown_1932c0.cpp:944`–`:945`). How the repo names the headers
themselves is under [Findings](#findings-about-existing-names).

## The function

### The logon (`0x6bff0`–`0x6c01b`)

`g_467214 == NONE` jumps to `0x6c157` (result 5). Otherwise `0x6c006` calls
`online_task_get_logon_status` with `g_467214` still in `eax`, and any result
but 1 (`dec eax; jne`, `0x6c00b`) also goes to `0x6c157`. `0x6c012` calls
`XOnlineGetLogonUsers` (`0x3a0671`); a null pointer goes to `0x6c140`
(result 10). The pointer is the library's own array, kept in `edi`; it is not
copied, as `online_get_logon_users` (`0x6ced0`) does.

### The users' status (`0x6c021`–`0x6c07f`)

`task->type == 0` calls `online_get_logon_users_status` (`0x6c028`); any other
type calls `online_task_get_change_logon_status` with `task` in `ecx`
(`0x6c02f`). Both return 0, 1 (a user has a message) or 2 (an account needs
management) (`src/unknown_06b590.cpp:92`–`:133`). `0x6c036` sets `ebx` to 1
and compares the status against it:

- **1:** `g_50944f = true` (`mov byte ptr [0x50944f], bl`, `0x6c03f`), then
  the services.
- **2** (`0x6c062`): sets `task->flag_bits.failed`, sets `result` to 2, stores
  it in `task->result` twice (`0x6c06d` and `0x6c075`) and returns `false`
  (`ret 4`, `0x6c07f`).
- **anything else:** the services.

### The services (`0x6c045`–`0x6c0a0`)

For each service, `0x6c050` calls `XOnlineGetServiceInfo(service_id, NULL)`
(`push 0; push eax`, `0x6c04d`–`0x6c04f`). Success (`jl` not taken, so
`hr >= 0`) sets `connect` to 1 (`0x6c059`). Failure sets `connect` to 0
(`0x6c08a`), and when `unknown04` is non-zero (`0x6c082`, tested before the
store and branched on after it) jumps to `0x6c140` (result 10) without looking
at the rest. A failed service with `unknown04` 0, in retail data only
`0x4d530064`, does not stop the loop.

### The users' services (`0x6c0a2`–`0x6c0c4`)

In order, with `edi` = the users:

1. `online_mutelists_refresh` (`0x6c0a2`), users in `edi`;
2. `online_presence_clear(users)` (`0x6c0a8`), on the stack;
3. `voice_start_engine()` (`0x6c0ad`);
4. `player_slot_find_online_user(users)` (`0x6c0b3`), on the stack; its
   result, the first slot with a signed-in non-guest user or `NONE`
   (`src/player_slots.cpp:141`–`:142`), replaces the users in `edi` (`0x6c0b8`);
5. `function_8c660()` (`0x6c0ba`), which requests `bungienet_user.dat` for
   each signed-in non-guest user whose voice setting is 0, and sets a
   guest's voice setting to 1 (`src/unknown_08c5b0.cpp:155`, `:170`–`:180`).

Then `task->type != 0` goes to `0x6c15c` with `result` still 1 (`0x6c0c4`).

### The files, friends and content (`0x6c0ca`–`0x6c13d`)

A logon task queues three receives, each the same eleven instructions: zero
`eax`, push it (`value06`), reserve twelve bytes and zero them through `edx`
(the `values` passed by value), set `eax` to the slot and `esi` to the header,
and call `0x80f70`, which pops `0x10`. The headers are `0x477058` (call at
`0x6c0e3`), `0x47d8f4` (`0x6c101`) and `0x479780` (`0x6c11f`). Then
`online_friends_startup()` (`0x6c124`) and `online_offering_check_new_content()`
(`0x6c129`); both return values are ignored. The function ends with result 1.

### The exits

Every exit stores `ebx` in `task->result` and returns `ebx == 1`
(`cmp ebx, 1; sete al`), so the source most likely ends with one shared
`task->result = result; return result == 1;` that the compiler copied into
four exits: `0x6c070`–`0x6c07f` (result 2), `0x6c12e`–`0x6c13d` (1, logon),
`0x6c145`–`0x6c154` (10) and `0x6c15c`–`0x6c16b` (5, or 1 for a change of
users). Only the status-2 exit sets
`failed`; for 5 and 10 the callers set it.

## A possible source form

```cpp
bool __stdcall function_6bff0(s_type_9df9da *task)
{
	long result;
	XONLINE_USER *users;
	long status;
	long controller;

	if (g_467214 == NONE || online_task_get_logon_status(g_467214) != 1)
	{
		result = 5;
		goto done;
	}
	users = XOnlineGetLogonUsers();
	if (!users)
	{
		result = 10;
		goto done;
	}
	status = task->type == 0 ? online_get_logon_users_status()
		: online_task_get_change_logon_status(task);
	result = 1;
	if (status == 1)
		g_50944f = true;
	else if (status == 2)
	{
		task->flag_bits.failed = true;
		result = status;
		task->result = status;
		goto done;
	}
	for (long i = 0; i < k_online_service_count; i++)
	{
		if (SUCCEEDED(XOnlineGetServiceInfo(g_467178[i].service_id, NULL)))
			g_467178[i].connect = true;
		else
		{
			g_467178[i].connect = false;
			if (g_467178[i].unknown04)
			{
				result = 10;
				goto done;
			}
		}
	}
	online_mutelists_refresh(users);
	online_presence_clear(users);
	voice_start_engine();
	controller = player_slot_find_online_user(users);
	function_8c660();
	if (task->type == 0)
	{
		s_pending_message_values values = { 0, 0, 0 };
		pending_message_request_receive(network_configuration_header, controller, values, 0);
		pending_message_request_receive(multiplayer_hopper_header, controller, values, 0);
		pending_message_request_receive(message_of_the_day_header, controller, values, 0);
		online_friends_startup();
		online_offering_check_new_content();
	}
done:
	task->result = result;
	return result == 1;
}
```

The three `*_header` names stand for `0x477058`, `0x47d8f4` and `0x479780`
seen as `s_pending_message_header *`; the repo has no such names yet (see
[Findings](#findings-about-existing-names)).

### Code-shape notes

- **`result = 1` before the status tests.** `ebx` is set to 1 at `0x6c036`
  and the status is compared with `cmp eax, ebx`; `g_50944f` is stored from
  `bl`. The constant may be the compiler's choice, but it fits a `result = 1`
  ahead of the tests.
- **The double store** at `0x6c06d` (`eax`, the status) and `0x6c075` (`ebx`)
  suggests an assignment of `task->result` in the status-2 branch, as
  `function_6cd53` writes `task->result = result` before its own exit
  (`src/online_tasks.cpp:581`), followed by the shared tail's.
- **`!= 1`** compiles to `dec eax; jne` (`0x6c00b`).
- **The loop** is strength-reduced to a byte offset in `esi` against three
  absolute bases (`0x467178`, `0x46717c`, `0x467180`), as `g_467178[i].<field>`
  gives. The test of `unknown04` (`0x6c088`) is scheduled before the
  `connect = 0` store, with the branch after it (`0x6c091`); either an
  `if`/`else` as above or a `connect = false` ahead of a `FAILED` test fits.
- **The zeroed `values`** are rebuilt before each call from `eax` = 0, with
  `ecx` as a second copy for `+4` (`0x6c0d4`, `0x6c0f2`, `0x6c110`). Whether
  the source has one zeroed local, as above, or a temporary per call, is open.

## Callees without source

- **`0x3a0671`**, called with no arguments, returning a pointer in `eax`. It
  is `XOnlineGetLogonUsers`: matched `online_get_logon_users` (`0x6ced0`,
  `src/unknown_06b590.cpp:54`) and `function_8c660`
  (`src/unknown_08c5b0.cpp:160`) call it and have no other library callee.
- **`0x3a067c`**, stdcall, two arguments, `HRESULT`. It is
  `XOnlineGetServiceInfo`: matched `0x6d220` calls it as
  `XOnlineGetServiceInfo(0x14, &info)` (`src/unknown_06b590.cpp:265`;
  `push eax; push edx`, `0x6d248`–`0x6d249`, with `edx` = `0x14`).
  `function_6bff0` passes a null second argument.
- **`0x6cd50`** (`online_task_get_logon_status`) has source at
  `src/online_tasks.cpp:586` but is `todo`.

## Existing declarations

| Where | Declaration | Retail |
| --- | --- | --- |
| `src/online_tasks.cpp:807` | `bool __stdcall function_6bff0(s_type_9df9da *task)` | The same; `ret 4` |
| `src/stubs/lane_d.cpp:42` | the same, `return false` | — |
| `src/online_tasks.cpp:587` | `long online_task_get_logon_status(long task_index)` | `task_index` in `eax` (`0x6cd50` begins `cmp eax, -1`; matched `0x6c7c0` calls it the same way) |
| `src/unknown_06b590.cpp:93` | `long online_get_logon_users_status(void)` | The same |
| `src/unknown_06b590.cpp:115` | `long online_task_get_change_logon_status(s_type_9df9da *task)` | `task` in `ecx` (`0x6c750` reads `[ecx + 0xc]`) |
| `src/unknown_06b590.cpp:72` | `void online_mutelists_refresh(XONLINE_USER *users)` | `users` in `edi` (`0x6c178` reads `[eax + edi]`) |
| `src/unknown_06c1d0.cpp:9` | `void online_presence_clear(XONLINE_USER *users)` | `users` on the stack; `ret 4` |
| `src/network_voice.cpp:792` | `void voice_start_engine(void)` | The same |
| `src/player_slots.cpp:144` | `long player_slot_find_online_user(XONLINE_USER *users)` | `users` on the stack (`0x18fa9a` reads `[esp + 8]` after one push); `ret 4` |
| `src/unknown_08c5b0.cpp:156` | `void function_8c660(void)` | The same |
| `src/unknown_080f30.cpp:102` | `bool pending_message_request_receive(s_pending_message_header *header, long value10, s_pending_message_values values, long value06)` | `header` in `esi`, `value10` in `eax`, `values` and `value06` on the stack; `ret 0x10` |
| `src/online_friends.cpp:17` | `long online_friends_startup(void)` | The same; the result is ignored here |
| `src/online_feedback.cpp:52` | `long online_offering_check_new_content(void)` | The same; the result is ignored here |

`function_8c660`'s copy of the `0x80f70` declaration
(`src/unknown_08c5b0.cpp:121`) calls `value10` `controller`; here it is the
slot from `player_slot_find_online_user`.

## Findings about existing names

- **One header type, four names.** Retail passes `0x477058`, `0x47d8f4` and
  `0x479780` to `0x80f70` as `s_pending_message_header`s, their initial data
  matches `g_479750`'s field for field ([Data](#data)), and `0x6cb60` clears
  bit 1 of the `+4` flags of all three (`and word ptr [0x479784], ax`,
  `[0x47d8f8]`, `[0x47705c]`; `0x6cbf4`–`0x6cc02`). The repo describes them
  four ways:
  - `g_479750` as `s_pending_message_header` (`include/pending_messages.h:18`),
    `0x2c` bytes;
  - `g_477058` as `s_online_file` (`include/unknown_0662e0.h:373`), which
    lines up with the header (`unknown8` the `kind` and `state`, `unknown20`
    the `pending_index`, `buffer` and `buffer_size` the `data` and `size`) and
    adds `unknown2c`;
  - `g_47d8f4` as `s_game_variant_globals` (`src/unknown_1932c0.cpp:41`, and
    copies in `src/unknown_06b590.cpp:343` and `src/unknown_054fe0.cpp:1462`):
    its `flags` is the header's `+4`, `count` its `kind` and `state` its
    `state`. The file is `multiplayer_hopper.dat`. The repo half knows this
    already: `src/unknown_1932c0.cpp:370` says "g_47d8f4 starts with a
    pending message request" and `:381` casts it to
    `s_pending_message_header *`; and the hopper definition's `+0x10` and
    `+0x18` slots, `g_46dd74` and `g_46dd7c` (`:944`–`:945`), hold
    `game_variants_retry` and `game_variants_received`, both taking an
    `s_pending_message_header *` (`:930`, `:905`);
  - `0x479780` not at all: `g_479784` (`s_message_of_the_day_globals`,
    `src/unknown_08ca00.cpp:11`–`17`) starts at its `+4`, so its `flags` is
    the header's flags and its `length` (`+4`, address `0x479788`) is the
    header's `kind`. `message_of_the_day_available` (`0x8ca50`) reads
    `word ptr [0x479788]` and `byte ptr [0x479784]`: the `kind != 0 &&
    (flags & 2)` test that `0x80f70` makes on a header before its own
    `!(flags & 8)` (`0x80fa2`–`0x80fae`), and that the message of the day's
    handler `function_8c790` makes on its `request` argument
    (`src/unknown_08ca00.cpp:214`, `+8` and `+4`). Nothing in `src/` or
    `include/` names `0x479780`–`0x479783`.

  The repo's `g_47d8f4` (`src/unknown_1932c0.cpp:49`, `0xc` bytes) and
  `g_479784` (`src/unknown_08ca00.cpp:17`, 8 bytes) also have no initializer,
  where retail's two `0x30`-byte headers start with the non-zero data in the
  [Data](#data) table.
- **The header is `0x30` bytes.** `g_479750` and the header at `0x479780` are
  `0x30` apart, `g_479750`'s `+0x2c` is 0 in retail, and `s_online_file`
  has the `+0x2c` field (`unknown2c`) that `s_pending_message_header` lacks.
  For the two `kind` 4 files, which have no buffer, `+0x2c` is `0x20000` and
  `0x8000`.
- **`s_online_service::unknown04`** (`src/unknown_06b590.cpp:20`) decides
  whether a service that `XOnlineGetServiceInfo` rejects fails the logon (10).
  It is 1 for the Live services and 0 for `0x4d530064`, so `required` would
  fit.

## Evidence

- `tools/fncfg.py 0x6bff0`: 20 blocks, four `ret`s. Its six problem lines
  come from its assumption that the two library calls take no stack
  arguments. `0x3a067c` takes two and pops them: in `0x6d220`, after
  `push eax; push edx; call 0x3a067c` (`0x6d248`–`0x6d24a`), `0x6d253` reads
  the local at `[esp + 4]` and `0x6d25d` frees only the `0xc`-byte local; with that, every join is at depth `0x10` and every `ret` at
  depth 0.
- `tools/xrefs.py 6bff0`: two calls, at `0x6c323` (in `0x6c2a0`) and
  `0x6c402` (in `0x6c360`).
- `0x80f70` ends with `ret 0x10` (`0x80fb3`, `0x80fe1`, `0x80fe7`); `0x6c1d0`
  and `0x18fa94` with `ret 4` (`0x6c28f`, `0x18fad7`); `0x6c170`, `0x6c6c0`,
  `0x6c750`, `0x8c660`, `0x8cc40` and `0x8d4d0` with `ret`.
- The [Data](#data) table is read from the retail image at the four headers
  and at each definition's file-name pointer.
- Line numbers are at `9f844175`.

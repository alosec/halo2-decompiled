#include "unknown_11c920.h"
#include "slot_handler.h"
#include "globals.h"
#include "squads.h"
// @flags /O2 /Gr

struct s_1c7bb0
{
 byte field_0[2];
 char field_2;
 byte field_3[0x28 - 3];
 short field_28;
 byte field_2a[0x7e - 0x2a];
 short field_7e;
 byte field_80[0x98 - 0x80];
};
bool function_204390(long squad_index);
bool actor_update_bsp_locations(long actor_index);
void function_203ed0(long actor_index, bool keep_count);
void function_1e22d0(long actor_index, bool conditional);
void function_203cc0(long squad_index);
bool function_1e13f0(long actor_index);
void function_203fb0(long actor_index);
void function_203d70(long actor_index, short squad_index, bool keep_team);
void function_295e60(short arg_0);
void function_210280();
void function_203f60(long actor_index);
void function_292f10();

struct s_squad_iterator_1c7bb0
{
 byte *squad;
 s_record_pool_iterator records;
 long squad_index;
};

PRIVATE __forceinline bool function_1c7bb1(s_squad_iterator_1c7bb0 *arg_0)
{
 arg_0->squad = NULL;
 if (g_4f55d0->active)
 {
  arg_0->squad = data_iterator_next_calling(&arg_0->records);
  arg_0->squad_index = arg_0->records.datum_index & 0xffff;
 }
 return arg_0->squad != NULL;
}
// score 164 (was 164): Frame 0x28 agrees; separate pool/iterator homes, early null and successor EBP remain.
// Round 43: iterator-pool carrier 164; with field-pool cache 198; with state owner 193;
// typed iterator/carrier 164; const carrier 164; carrier declared first 164; local result
// with late/early member publication 164/164.
// Earlier 164 (was 164): frame 0x28 agrees; separate pool/iterator residency, early null and successor EBP remain.
// Frame 0x28 agrees; separate pool/iterator homes, early null publication and saved successor EBP remain.
// frame 0x28 agrees; separate pool/iterator residency, early null and successor EBP remain.
// Frame 0x28 agrees; separate pool/iterator homes, early null publication and successor EBP remain.
// Earlier: Frame 0x28 agrees; separate pool/iterator homes, null timing and successor residency remain.
// frame 0x28 agrees; separate pool/iterator homes,
// frame 0x28 agrees; pool/iterator separation, null timing and successor EBP remain.
// Frame 0x28 agrees; separate pool/iterator storage, early null publication and successor EBP remain.
// Earlier 164 (was 164): frame 0x28 agrees; pool/iterator separation, null timing and successor EBP remain.
// null-result timing and actor-successor residency remain. Inactive early exit
// 164; native flag mask 164; successful inner-update continue 164; pointer-result
// for loop 275; early-exit/mask/continue combination 164.
// Initialized iterator 164; direct iterator index 164; unsigned squad index
// 164; wider inner actor-pointer scope 164; timestamp before active clear 165.
// Frame 0x28 agrees; separate pool homes and iterator/actor lifetimes differ.
// Native record iterator 222; long partition index 163; full actor view 164; outer actor workspace
// 164; ineligible-first branch 171.
// Cached pool 197; role names 164; cached active state 193; nested eligibility 164; inactive
// continue 164; direct datum 164; unsigned successor 164.
// traversal phase lifetimes do not recover separate pool
// Earlier: traversal phase lifetimes do not recover separate pool
// Earlier 164 (was 164): traversal phase lifetimes do not recover separate pool
// storage or the actor successor register. Squad/actor/both phase scopes 164/164/164;
// squad/both scopes with actor for-loop 164/164; squad/both scopes with read-only
// first acquisition 164/164; both scopes with read-only acquisition and for-loop 164.
// Actor-next call alone 234; unsigned-word squad index 230; iterator index initialized first 164;
// active count before timestamp 166; nested source-squad validation 164; per-squad pool observation
// 231.
// Ordinary private next 281; private next out of line 281; separate reacquired actor 164;
// closed initial-actor scope 164; ordinary private next/separate reacquired actor 281.
// Private step byte/signed-byte/short/long results 164 each; inactive index initialization 164;
// inactive index with byte/short/long results 164 each.
// begin expansion 164; actor next expansion 164; actor accessor expansion 164; pool next expansion
// 164; actor iterator expansion 164; actor next accessor expansion 164; begin pool expansion 164;
// all shared expansions 164. Separate pool residency, early null publication and successor EBP
// remain.
// Iterator before entry guard 164; unsigned partition word 164; native actor iterator
// initializer 206; active count before both actor stores 166; Boolean completion-first step 165.
// Round 112: Typed squad result 164; native record-step call 252; shared datum step expanded 267;
// active-aware shared step 240; native datum initialization 252.
// Existing iterator member orders datum/index/records, records/datum/index, records/index/datum,
// index/datum/records and index/records/datum each 164; original layout restored.
// Private absolute-index ordinal 217; native ordinal member 218; exhaustion result 164;
// inactive step early return 165; native ordinal/exhaustion result 218.
// Separate inline/noinline declarations for eligibility, location update, detach, actor test, attach, transfer, partition update and final update each 164. All eight original declarations and both traversals restored.
// Native step reference 164; unsigned-word completion 164; shared absolute ordinal 250;
// native record-array view 180; cleanup inline declaration 164; reference/word contract 164.
// Round 211: Masked squad slot 250; opaque datum 164; unsigned ordinal member 164; slot/full-width
// partition 252; slot/opaque datum 250. Separate pool/iterator and successor homes remain.
// Round 116: Successor advanced at tail 190; forced existing inactive setter 166; live-pool aggregate 164; both workspaces at entry 164; source-squad field reads 164. Opening code restored.
// Round 356: Explicit active input 164; private datum result 164; both contracts 164; iterator-owned table 195; table with active input 195.
// Round 123: Native force-inline member 164; member pointer result 164; ordinary inline member 281; current-handle for 190; force-inline member with current-handle for 190.
// Admission-seeded completion/exhaustion 165/202; single datum publication 164; read-only concrete datum 164; each completion/publication with concrete datum 165/202/164; read-only byte publication 164. Frame 0x28, separate pool/iterator homes, early null and successor EBP remain.
// Round 358: Complete native iterator initializer 164; second-pass scalar scope 164; current-frame row
// receiver 207; word ordinal with row 233; complete initializer with row 207. Pool/iterator separation, early
// null and successor EBP remain; opening body and private iterator restored. Repeated standalone word form
// 218 excluded.
// Native byte/signed-byte/Boolean high-bit fields with Boolean tests 164/164/164; direct tests 164/164/164; native bit values 164/187. Record size stayed 0x98; original flag byte restored.
// Round 332: Immutable partition/squad ordinal/current actor scalars, each and
// every combination, 164; all with immutable source-squad handle 164. All restored.
// Scalarized iterator datum/record/ordinal member names, their three pairs and all three 164 each;
// complete member names with outer iterator name 164. Separate pool/iterator and successor homes remain.
// Round 141: Coordinated provider/caller inline-noinline declarations for eligibility, location update,
// detach, actor test, attach, transfer, partition update and final update 164 each. Original providers
// and declarations restored; pool/iterator and successor homes remain.
// Round 141: Canonical inline-only provider/caller declarations at the same eight boundaries also give
// 164 each; every provider and original declaration restored.
// Round 222: split iterator plus datum/index outputs 164; split pointer result 164; returned datum with ordinal output 164; outputs-first contract 164; typed split datum 164; split completion result first 165; split byte completion 164; internal split-step linkage 164.
// Round 187 twice-used absolute actor slot long/dword/word/outer long with original view
// 188/188/168/188; same four with full native actor accessor 188/188/168/188 (fast).
// All restored; separate pool/iterator, early null and successor EBP remain.
// Current actor-handle ownership: inner long/const/unsigned 164 each; iterator lifetime 164;
// shared long/unsigned 164 each; both read-only 164; inner with unsigned current 164.
// Round 215: Actual ordinal-return step, long with retained/returned-only ordinal and
// conditional/result-first exits 164/198/204/199; unsigned 198/222/205/220.
// Every returned result tested; original private helper and body restored.
// Persistent full-function/first-traversal payload tables 190/197; with full-width partition 191/198;
// the same four table/partition forms with pointer step results 190/197/191/198. Original helper and
// traversals restored; separate pool/iterator, early null and successor homes remain.
// Round 147: Coherent inline lookup/activation/squad cleanup/actor removal 234/164/164/164; lookup
// plus each other boundary 234/234/234; all four 234; separately outlined inline lookup 164 with
// checked losses.
// Round 149: Complete iterator result state with value input and long/int/dword/word ordinals
// 316/316/316/335; const-reference input with genuine state copy gives the same four scores. All
// copied members initialized; larger frame and extent regress. Original private step, layout and both
// traversals restored.
// Round 174: isolated partition/ordinal/both/successor/partition-successor/ordinal-successor/all
// scalar home names, then all with the second-pass actor receiver, 164 each. Original names restored.
// Eligibility result bool/const-bool/long/short with conditional or branch publication:
// 194/200, 194/200, 195/178, 193/193; original body restored.
// Native iterator constructors with implicit/explicit pool input, partial/full initialization and
// free/member next contracts in all eight combinations 164 each. Original construction restored.
// Second-pass combined bool/byte/short/long completion 164 each;
// second-pass result-first bool/byte/short/long completion 164/164/188/186.
// Round 248: One pool observation plus inline/noinline/both/force-inline finder
// contracts with native partition 214/197/197/222; full-width partition
// 216/165/165/223. Original pool observations, helper and contracts restored.
// Round 204: Actual state-owner input to the existing step, owner last/first with
// bool/byte/short/long completion, 164 each. Original helper and both traversals restored;
// separate pool/iterator homes, early null and successor EBP remain.
// Round 206: Actual second-pass source-squad/pending-flag field receivers as pointer/reference,
// individually and all four pairs, 164 each. Both acquisitions and pre-call successor retained.
// Round 197: Coherent defining-unit/caller final-phase inline and inline/noinline
// declarations, each with bool/byte/short/long private completion, 164 each. Original
// provider declarations, private step and both traversals restored.
// @retail 0x1c7bb0
void function_1c7bb0()
{
 if (g_4f55d0->active)
 {
  short local_0 = g_4686c4;
  s_squad_iterator_1c7bb0 local_1;
  byte *datum;
  local_1.records.data = g_51e9d8;
  local_1.records.index = NONE;
  while (function_1c7bb1(&local_1))
  {
   datum = local_1.squad;
   long local_2 = local_1.squad_index;
   if (function_204390(local_2))
   {
    ((s_1c7bb0 *)g_51e9d8->data)[local_2 & 0xffff].field_7e = (short)local_0;
    s_squad_actor_iterator local_4;
    squad_actors_begin(&local_4, local_2);
    s_actor_datum *local_6;
    while ((local_6 = squad_actors_next(&local_4)) != NULL)
    {
     if (!actor_update_bsp_locations(local_4.actor_index))
     {
      *(long *)((byte *)local_6 + 0x34) = local_6->squad_index;
      function_203ed0(local_4.actor_index, false);
      function_203f60(local_4.actor_index);
     }
    }
   }
   else
    ((s_1c7bb0 *)g_51e9d8->data)[local_2 & 0xffff].field_7e = NONE;
   if (((s_1c7bb0 *)datum)->field_2 < 0)
   {
    ((s_1c7bb0 *)g_51e9d8->data)[local_2 & 0xffff].field_28 = 0;
    function_203cc0(local_2);
   }
  }
  long local_4 = g_4f55d0->unknown14;
  while (local_4 != NONE)
  {
   long local_5 = local_4;
   s_actor_view *local_6 = actor_get(local_5);
   local_4 = local_6->unknown020;
   if (function_1e13f0(local_5) && actor_update_bsp_locations(local_5))
   {
    long local_7 = *(long *)((byte *)local_6 + 0x34);
    if (local_7 != NONE && ((s_1c7bb0 *)g_51e9d8->data)[local_7 & 0xffff].field_7e == (short)local_0)
    {
     function_203fb0(local_5);
     function_203d70(local_5, (short)*(long *)((byte *)local_6 + 0x34), true);
    }
   }
   local_6 = actor_get(local_5);
   if (local_6->unknown009)
   {
    local_6->unknown009 = false;
    *(long *)((byte *)local_6 + 0x10) = g_510c54->game_time;
    --*(short *)((byte *)g_4f55d0 + 0x36a);
   }
  }
  function_295e60(local_0);
  function_210280();
  function_292f10();
 }
}

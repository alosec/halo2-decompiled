// @flags /O2 /arch:SSE /Gr
#include "pool_storage.h"
#include "unknown_11c920.h"
#include "sound_driver.h"
#include "globals.h"
#include "crc.h"
#include <float.h>
#include <stddef.h>
#include <string.h>

struct s_sound_effect_buffers;
struct s_sound_i3dl2_parameters;
struct s_sound_send_parameters;
struct s_sound_mixbin_source;
struct s_mixbin_settings;

struct s_sound_effect_settings
{
	real level00;
	real level04;
	real level08;
	real level0c;
	real level10;
	real gain;
	real unknown18;
	real level1c;
	real level20;
	real level24;
	real level28;
	real level2c;
	byte unknown30[0x10];
};

class c_sound_effects
{
public:
	virtual void initialize(s_sound_effect_settings *settings, s_sound_effect_buffers *buffers, bool rear);
	virtual void v1() {}
	virtual void set_i3dl2(s_sound_i3dl2_parameters *parameters, long unused);
	virtual void add_effect_sends(s_sound_send_parameters *parameters, bool rear, long mode, s_mixbin_settings *mixbins);
	virtual void add_send(long mixbin, long mode, real gain, s_mixbin_settings *mixbins);
	virtual void add_source_mixbins(s_sound_mixbin_source *source, bool rear, long mode, s_mixbin_settings *mixbins);
	virtual void add_sends(s_sound_send_parameters *parameters, long unused, long mode, s_mixbin_settings *mixbins);
	void add_effect_sends_direct(s_sound_send_parameters *parameters, bool rear, long mode, s_mixbin_settings *mixbins);
	void add_effect_sends_split(s_sound_send_parameters *parameters, bool rear, long mode, s_mixbin_settings *mixbins);
	s_sound_effect_settings m_settings;
	s_sound_effect_buffers *m_buffers;
};

c_sound_effects g_47f088;

struct s_47f0d0
{
	void *vtable;
	long field04;
	long field08;
	long field0c;
	long field10;
	void *field14;
};

extern s_47f0d0 g_47f0d0;

struct s_sound_listener_environment;
struct s_sound_listener
{
	point3f position;
	vector3f forward;
	vector3f up;
	byte unknown24[0xc];
	long environment_count;
	s_sound_listener_environment const *environments;
};

struct s_sound_driver_counts
{
	short channels[3];
	byte unknown06[6];
	short voices;
};

typedef char check_effects_instance_size[sizeof(c_sound_effects) == 0x48 ? 1 : -1];
typedef char check_codec_view_size[sizeof(s_47f0d0) == 0x18 ? 1 : -1];
typedef char check_driver_voice_count_offset[offsetof(s_sound_driver_counts, voices) == 0xc ? 1 : -1];

bool function_191300(LPDIRECTSOUND direct_sound);
void function_191420(LPDIRECTSOUND direct_sound);
void function_12c040(void);
void function_21eae0(void);
void function_21e3b0(c_sound_driver_effects *effects);
bool function_21f4e0(long channel_index, long type);
void function_220790(s_sound_listener const *listener);
bool sound_driver_voice_create_buffer(long voice_index);
void function_221490(LPDIRECTSOUNDBUFFER *buffer_reference);

// score 165 (was 169): success/type registers and first-channel spill;
// byte allocation result 165; early byte result 280; listener aggregate 328;
// aggregate with early result 357; early result with count cursor 300.
// failure arm first 168; direct result exits 364; byte category index 311;
// headroom do 167; flat eight-entry submix scan 258. Frame 0x80 still agrees.
// long channel-count snapshot 303; reused voice count 165; both 322;
// complete count record 454; pointer-primary count scan 302;
// unsigned category long/word 165/311; separate voice result 323.
// signed allocation byte 165; word allocation state 301; unsigned category 165;
// counts record reference 166; count field reference 165; published first 165;
// field plus first 165; signed byte plus field 165. Success/type spills still differ.
// returned byte 313; returned byte/count cursor 341; serial HRESULT 169; descriptor prefixes 323;
// complete listener value 468; frame pointer 443; global optimization disabled 1166; size preference 645;
// separate final-call result 302; result transfer before reset 294.
// Frame 0x80 agrees; allocation byte, category and first-channel spill still differ.
// published first channel 165; count from first channel 283; count before range publication
// 298; published first count 294; native mixbin identifiers 165; input table row traversal
// 232; output table traversal 268; both native table traversals 274.
// score 165 (was 165): success/type carriers and first-channel spill remain;
// native startup/create/buffer calls 209/201/165; two-row input table 165;
// absolute channel index 283; state before codec stores 165; failure state 163; early failure state 286.
// result from startup tests 424; tested-state transfer 424; result at first test 424;
// final result after reset completion 303; before effects 303; before submix 262;
// before impulse cleanup 174; before volume stores 165.
// score 165 (was 165): allocation/type carriers and first-channel spill remain;
// cross-reset channel / voice / both bitwise accumulation 307/306/328;
// both Boolean accumulations late / across reset 332/331; listener / table scopes
// with cross-reset accumulations 328/328; native submix record initializers 328.
// success/type carriers and first-channel spill remain;
// voice/both compound states 312/329; channel/voice/both call-first 266/286/274;
// channel/voice/both conditional states 282/282/301. Original guarded updates retained.
// score 165 (was 165): saved success/category/first-channel homes still differ;
// final byte 167; final byte with wide state 301; final byte cross-reset 288;
// final byte with bool state 167, cross-reset 288; scoped wide state 302;
// wide state across reset/listener 293/294. Original byte state retained.
// saved success/type/first-channel allocation remains;
// listener consumer scope 165; with reset/earlier byte state 280/283; submix scope 165;
// both scopes 165; both with reset state 280; listener/reset with pointer-first bins 280;
// all consumer boundaries 280. Original short-circuit loops retained.
// @retail 0x21e4b0
bool function_21e4b0(s_sound_driver_counts const *counts)
{
	byte *top = (byte *)(g_510804_pool_size + g_510800_pool_base);
	byte *memory = (byte *)(((dword)top + 3) & ~3);
	long allocated_size = (memory - top) + sizeof(s_sound_driver_globals);
	bool result = false;
	g_510804_pool_size += allocated_size;
	function_163ba0(&g_510808_pool_checksum, &allocated_size, sizeof(allocated_size));
	memset(memory, 0, sizeof(s_sound_driver_globals));
	g_51ebe4 = (s_bink_sound_settings *)memory;
	SOUND_DRIVER_GLOBALS->unknown0000 = false;
	if (SUCCEEDED(DirectSoundCreate(NULL, &SOUND_DRIVER_GLOBALS->direct_sound, NULL)) &&
		SUCCEEDED(SOUND_DRIVER_GLOBALS->direct_sound->GetCaps(&SOUND_DRIVER_GLOBALS->caps)) &&
		SUCCEEDED(SOUND_DRIVER_GLOBALS->direct_sound->SetDistanceFactor(3.048f, DS3D_IMMEDIATE)) &&
		SUCCEEDED(SOUND_DRIVER_GLOBALS->direct_sound->SetDopplerFactor(0.0f, DS3D_IMMEDIATE)))
	{
		DWORD speaker_flags;
		if (SUCCEEDED(SOUND_DRIVER_GLOBALS->direct_sound->GetSpeakerConfig(&speaker_flags)))
		{
			SOUND_DRIVER_GLOBALS->surround = (speaker_flags & 0xffff) == 2 && (speaker_flags & 0x10000);
			for (long mixbin = 0; mixbin <= 31; mixbin++)
				SOUND_DRIVER_GLOBALS->direct_sound->SetMixBinHeadroom(mixbin, 0);
			SOUND_DRIVER_GLOBALS->direct_sound->SetRolloffFactor(0.0f, DS3D_DEFERRED);
			DirectSoundDoWork();
			if (function_191300(SOUND_DRIVER_GLOBALS->direct_sound))
			{
				s_sound_listener listener;
				memset(&listener, 0, sizeof(listener));
				listener.forward = *g_4687a8;
				listener.up = *g_4687b0;
				function_220790(&listener);
				function_12c040();
				g_47f0d0.field04 = 0;
				g_47f0d0.field14 = NULL;
				g_47f0d0.field10 = 0x10000;
				byte buffers_created = true;
				for (long type = 0; type < 3; type++)
				{
					long first = SOUND_DRIVER_GLOBALS->channel_count;
					SOUND_DRIVER_GLOBALS->unknown1a0c[type] = SOUND_DRIVER_GLOBALS->channel_count;
					SOUND_DRIVER_GLOBALS->channel_count += counts->channels[type];
					for (long i = 0; buffers_created && i < counts->channels[type]; i++)
					{
						buffers_created = buffers_created && function_21f4e0(first + i, type);
					}
					SOUND_DRIVER_GLOBALS->unknown1a12[type] = SOUND_DRIVER_GLOBALS->channel_count - 1;
				}
				short voices = counts->voices;
				SOUND_DRIVER_GLOBALS->voice_count = voices;
				SOUND_DRIVER_GLOBALS->unknown0008 = voices;
				for (long voice = 0; buffers_created && voice < counts->voices; voice++)
					buffers_created = buffers_created && sound_driver_voice_create_buffer(voice);
				function_191420(SOUND_DRIVER_GLOBALS->direct_sound);
				SOUND_DRIVER_GLOBALS->effect_levels_a[0] = 0;
				SOUND_DRIVER_GLOBALS->effect_levels_a[1] = 0xc0c0a8c1;
				SOUND_DRIVER_GLOBALS->effect_levels_b[0] = 0;
				SOUND_DRIVER_GLOBALS->effect_levels_b[1] = 0xc0c0a8c1;
				((dword *)SOUND_DRIVER_GLOBALS->effect_settings)[0] = 0xc0c0a8c1;
				((dword *)SOUND_DRIVER_GLOBALS->effect_settings)[1] = 0xc0400000;
				((dword *)SOUND_DRIVER_GLOBALS->effect_settings)[2] = 0xc0c0a8c1;
				((dword *)SOUND_DRIVER_GLOBALS->effect_settings)[7] = 0xc0c0a8c1;
				SOUND_DRIVER_GLOBALS->effect_mixbins[0] = 23;
				SOUND_DRIVER_GLOBALS->effect_mixbins[1] = 24;
				SOUND_DRIVER_GLOBALS->effect_mixbins[2] = 25;
				for (long send = 0; send < 3; send++)
				{
					long mixbin = SOUND_DRIVER_GLOBALS->effect_mixbins[send];
					LPDIRECTSOUNDBUFFER *spatial_buffer = &SOUND_DRIVER_GLOBALS->effect_buffers_b[send];
					LPDIRECTSOUNDBUFFER *buffer = &SOUND_DRIVER_GLOBALS->effect_buffers_a[send];
					{
						DSBUFFERDESC description;
						memset(&description, 0, sizeof(DSBUFFERDESC));
						description.dwSize = sizeof(description);
						description.dwFlags = DSBCAPS_FXIN2;
						description.dwInputMixBin = mixbin;
						if (SUCCEEDED(SOUND_DRIVER_GLOBALS->direct_sound->CreateSoundBuffer(&description, buffer, NULL)))
							(*buffer)->Play(0, 0, 0);
					}
					DSBUFFERDESC spatial_description;
					memset(&spatial_description, 0, sizeof(DSBUFFERDESC));
					spatial_description.dwSize = sizeof(spatial_description);
					spatial_description.dwFlags = DSBCAPS_FXIN2 | DSBCAPS_CTRL3D;
					spatial_description.dwInputMixBin = mixbin;
					if (SUCCEEDED(SOUND_DRIVER_GLOBALS->direct_sound->CreateSoundBuffer(&spatial_description, spatial_buffer, NULL)))
					{
						(*spatial_buffer)->SetMaxDistance(FLT_MAX, DS3D_DEFERRED);
						(*spatial_buffer)->SetMinDistance(FLT_MAX, DS3D_DEFERRED);
						(*spatial_buffer)->Play(0, 0, 0);
					}
				}
				function_21e3b0((c_sound_driver_effects *)&g_47f088);
				long input_mixbins[8] = {15, 16, 17, 18, 19, 20, 21, 22};
				long output_mixbins[4] = {6, 7, 8, 9};
				for (long group = 0; group < 2; group++)
				{
					for (long i = 0; i < 4; i++)
					{
						LPDIRECTSOUNDBUFFER *buffer = &SOUND_DRIVER_GLOBALS->submix_buffers[group * 4 + i];
						DSBUFFERDESC description = {0};
						description.dwSize = sizeof(description);
						description.dwFlags = DSBCAPS_FXIN2;
						description.dwInputMixBin = input_mixbins[group * 4 + i];
						if (SUCCEEDED(SOUND_DRIVER_GLOBALS->direct_sound->CreateSoundBuffer(&description, buffer, NULL)))
						{
							(*buffer)->SetVolume(0);
							DSMIXBINVOLUMEPAIR pair;
							DSMIXBINS bins;
							pair.dwMixBin = output_mixbins[i];
							pair.lVolume = 0;
							bins.dwMixBinCount = 1;
							bins.lpMixBinVolumePairs = &pair;
							(*buffer)->SetMixBins(&bins);
							(*buffer)->Play(0, 0, 0);
						}
					}
				}
				for (long impulse = 0; impulse < 2; impulse++)
				{
					function_221490(&SOUND_DRIVER_GLOBALS->impulse_buffers[impulse]);
					SOUND_DRIVER_GLOBALS->impulse_ids[impulse] = NONE;
				}
				SOUND_DRIVER_GLOBALS->volume_a = 1.0f;
				SOUND_DRIVER_GLOBALS->volume_b = 1.0f;
				SOUND_DRIVER_GLOBALS->volume_c = 1.0f;
				SOUND_DRIVER_GLOBALS->volume_d = 1.0f;
				SOUND_DRIVER_GLOBALS->unknown2acc = 8000;
				SOUND_DRIVER_GLOBALS->unknown2ad0 = 0.0f;
				result = buffers_created;
			}
		}
	}
	if (result)
		SOUND_DRIVER_GLOBALS->unknown0000 = true;
	else
		function_21eae0();
	return result;
}

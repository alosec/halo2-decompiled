#!/usr/bin/env python3
import argparse
import json
import subprocess
import tempfile
from pathlib import Path


def symbols(llvm_nm: Path, object_path: Path, option: str) -> set[str]:
    output = subprocess.check_output(
        [llvm_nm, option, "--extern-only", "--format=posix", object_path],
        text=True)
    return {
        line.split()[0].removeprefix("_")
        for line in output.splitlines() if line
    }


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--root", type=Path, required=True)
    parser.add_argument("--llvm-nm", type=Path, required=True)
    parser.add_argument("--lld", type=Path, required=True)
    parser.add_argument("--cluster-object", type=Path, required=True)
    parser.add_argument("--production-object", type=Path, required=True)
    args = parser.parse_args()

    manifest_path = args.root / "port/runtime/sound_idle_direct_cluster.json"
    manifest = json.loads(manifest_path.read_text())
    if manifest.get("version") != 1:
        raise RuntimeError("unsupported sound-idle direct cluster manifest")
    providers = manifest.get("providers")
    expected_providers = [
        ("src/sound_idle_service_native.cpp", "?function_125d60@@YAXXZ"),
        ("src/sound_system_availability_native.cpp", "?sound_system_available@@YAJXZ"),
        ("src/sound_system_update_time_native.cpp", "?sound_system_update_time@@YAXXZ"),
        ("src/sound_voice_chunks_native.cpp", "?sound_voices_update_chunks@@YAXXZ"),
        ("src/sound_playback_update_native.cpp", "?function_1293f0@@YAXXZ"),
        ("src/sound_cache_frame_native.cpp", "?sound_cache_new_frame@@YAXXZ"),
        ("src/sound_driver_pre_update_native.cpp", "?function_21ec00@@YAXXZ"),
        ("src/sound_driver_post_update_native.cpp", "?function_21ee80@@YAXXZ"),
        ("src/sound_driver_effect_refresh_native.cpp", "?function_191550@@YAXXZ"),
        ("src/sound_driver_effect_refresh_native.cpp", "?function_1915f0@@YAXXZ"),
        ("src/unknown_13d170.cpp", "?physical_memory_reset_time@@YAXPAUs_physical_object@@@Z"),
        ("src/sound_driver_channel_usage_native.cpp", "?function_21f360@@YAXPAUs_sound_driver_channel_usage@@@Z"),
        ("src/sound_master_gain_native.cpp", "?function_2197f0@@YAJM@Z"),
        ("src/sound_fade_helpers_native.cpp", "?function_12aff0@@YAMMMM_N@Z"),
        ("src/sound_playback_stop_native.cpp", "?function_127320@@YAXJJ@Z"),
        ("src/sound_playback_reference_native.cpp", "?function_126960@@YAXPAUs_sound_playback@@@Z"),
        ("src/sound_playback_delete_native.cpp", "?sound_playback_delete@@YAXJ@Z"),
        ("src/sound_voice_free_native.cpp", "?sound_voice_free@@YAXF@Z"),
        ("src/sound_voice_release_native.cpp", "?sound_voice_release@@YAXJ@Z"),
        ("src/sound_looping_controller_release_native.cpp", "?looping_sound_controller_release@@YAXJ@Z"),
        ("src/sound_playback_fade_native.cpp", "?function_12a810@@YAJJ@Z"),
        ("src/unknown_2197f0.cpp", "?function_2195f0@@YAMM@Z"),
        ("src/sound_stream_chunk_release_native.cpp", "?sound_stream_release_chunks@@YAXPAUs_sound_stream@@@Z"),
        ("src/sound_stream_update_native.cpp", "?sound_stream_update@@YAXPAUs_sound_stream@@@Z"),
        ("src/sound_voice_chunk_transition_native.cpp", "?sound_voice_update_chunks@@YAXF@Z"),
        ("src/sound_cache_request_native.cpp", "?function_218850@@YAKJPAUs_sound_chunk@@K@Z"),
        ("src/sound_voice_chunk_helpers_native.cpp", "?function_219290@@YAJFPAUs_permutation_group@@@Z"),
        ("src/sound_driver_playback_rate_native.cpp", "?function_21f650@@YAMJJ@Z"),
        ("src/sound_voice_chunk_helpers_native.cpp", "?sound_reference_release@@YAXPAUs_sound_reference_holder@@@Z"),
        ("src/sound_cache_allocation_native.cpp", "?function_218a10@@YAXPAUs_sound_chunk@@J@Z"),
        ("src/sound_cache_file_route_native.cpp", "?function_213760@@YAJKJPAXPAKPA_NJJ@Z"),
        ("src/sound_cache_async_read_native.cpp", "?function_1a0f10@@YAJUs_file_handle@@PAXKKJJPAKPC_N@Z"),
        ("src/sound_cache_async_read_native.cpp", "?function_1a0e70@@YAJPATs_async_task@@@Z"),
        ("src/particle_texture_cache_request_native.cpp", "?function_120ba0@@YAJJPATs_async_task@@JP6AJ0@ZPC_N@Z"),
        ("src/sound_cache_read_limit_native.cpp", "?g_46e488@@3KA"),
        ("src/sound_gain_defaults_native.cpp", "?g_440c48@@3JA"),
        ("src/sound_gain_defaults_native.cpp", "?g_440c4c@@3JA"),
        ("src/sound_gain_defaults_native.cpp", "?g_468834@@3MA"),
        ("src/sound_busy_state_native.cpp", "?function_12be90@@YA_NXZ"),
        ("src/sound_spatialization_native.cpp", "?function_129aa0@@YAXFPAJPAUs_looping_channel_spatialization@@@Z"),
        ("src/sound_spatial_gain_native.cpp", "?function_12a6d0@@YAJFMM@Z"),
        ("src/sound_attenuation_native.cpp", "?function_125060@@YAJPBUs_sound@@J@Z"),
        ("src/sound_cone_attenuation_native.cpp", "?function_12ad30@@YAMPBTvector3f@@PBUs_sound@@J0@Z"),
        ("src/sound_attenuation_native.cpp", "?sound_get_distance_gain@@YAMJPBUs_sound@@M@Z"),
        ("src/sound_attenuation_native.cpp", "?sound_get_inner_cone_angle@@YAMPBUs_sound@@J@Z"),
        ("src/sound_attenuation_native.cpp", "?sound_get_outer_cone_angle@@YAMPBUs_sound@@J@Z"),
        ("src/sound_attenuation_native.cpp", "?sound_get_outer_cone_gain@@YAJPBUs_sound@@J@Z"),
        ("src/unknown_11cc90.cpp", "?function_11cf50@@YAMPBTvector3f@@0@Z"),
        ("src/unknown_030290.cpp", "?function_30bf0@@YAMPATvector3f@@@Z"),
        ("src/unknown_141e10_native.cpp", "?function_142700@@YAPATpoint3f@@PBUtransform4x3f@@PBT1@PAT1@@Z"),
        ("src/sound_source_position_native.cpp", "?function_1427f0@@YAPATvector3f@@PBUtransform4x3f@@PBT1@PAT1@@Z"),
        ("src/sound_source_position_native.cpp", "?sound_source_get_position@@YAXPBUs_sound_location_source@@JPATpoint3f@@@Z"),
        ("src/unknown_188dd0.cpp", "?vector3d_decompress@@YAPATvector3f@@KPAT1@@Z"),
        ("src/sound_class_route_native.cpp", "?function_221810@@YAPAUs_unknown_5c@@F@Z"),
        ("src/sound_impulse_channel_native.cpp", "?function_128ca0@@YAXFJPBUs_looping_channel_spatialization@@@Z"),
        ("src/unknown_12a1b0.cpp", "?function_12a1b0@@YAXFPAUs_looping_track_sound@@PBUs_looping_channel_spatialization@@PAUs_looping_channel_properties@@@Z"),
        ("src/sound_doppler_native.cpp", "?function_12a9d0@@YAMJPBUs_sound_position@@@Z"),
        ("src/sound_effect_inputs_native.cpp", "?function_222770@@YAXJJPBUs_effect_inputs@@PAUs_sound_playback_effects@@@Z"),
        ("src/unknown_222930.cpp", "?function_222930@@YAXJJPAUs_looping_impulse_parameters@@@Z"),
        ("src/sound_effect_inputs_native.cpp", "?function_1914f0@@YAJJ@Z"),
        ("src/unknown_192b90.cpp", "?function_192b90@@YAMPBTpoint3f@@J_N@Z"),
        ("src/sound_get_class.cpp", "?sound_get_class@@YAPAUs_sound_class_definition@@J@Z"),
        ("src/unknown_18d090.cpp", "?function_18d090@@YAPAXJJ@Z"),
        ("src/unknown_219110.cpp", "?function_2194a0@@YAPAEPAUs_object_ref@@@Z"),
        ("src/sound_playback_reference_native.cpp", "?function_125e60@@YAJPAUs_looping_track_sound@@@Z"),
        ("src/sound_playback_gain_native.cpp", "?function_1251e0@@YAJPBXJM@Z"),
        ("src/tag_function_general_native.cpp", "?function_13b390@@YAMPBXMM@Z"),
        ("src/tag_function_periodic_native.cpp", "?function_17c900@@YAMFM@Z"),
        ("src/light_curve_lookup_native.cpp", "?function_17ca10@@YAMMF@Z"),
        ("src/tag_function_general_native.cpp", "g_44178c"),
        ("src/unknown_218ac0.cpp", "?function_218f50@@YAFPAUs_looping_playback_definition@@FM@Z"),
        ("src/unknown_219110.cpp", "?function_219110@@YAFPAUs_set_ref@@FPAKF1PA_N_N@Z"),
        ("src/unknown_219110.cpp", "?function_2192b0@@YAXPAUs_animation_state@@PAUs_animation_ref@@E@Z"),
        ("src/unknown_219110.cpp", "?function_219370@@YAJPAUs_animation_state@@PAUs_animation_ref@@@Z"),
        ("src/unknown_219110.cpp", "?function_2194c0@@YAKPAUs_packed_value@@PAUs_animation_ref@@@Z"),
        ("src/unknown_219110.cpp", "?function_219560@@YAXPAUs_packed_value@@PAUs_animation_ref@@K@Z"),
        ("src/sound_looping_voice_policy_native.cpp", "?function_12a420@@YAXF@Z"),
        ("src/sound_looping_voice_policy_native.cpp", "?function_128a60@@YAFJFPBF@Z"),
        ("src/sound_looping_voice_policy_native.cpp", "?function_128500@@YAXJPAUs_looping_voice_counts@@@Z"),
        ("src/sound_looping_chunk_policy_native.cpp", "?sound_playback_set_chunk@@YAXJJDDF@Z"),
        ("src/sound_looping_chunk_policy_native.cpp", "?sound_voice_queue_chunk@@YAXFPBUs_sound_permutation@@F@Z"),
        ("src/sound_stream_chunk_release_native.cpp", "?sound_stream_add_chunk@@YAXPAUs_sound_stream@@PAUs_sound_chunk@@@Z"),
        ("src/tag_function_range_native.cpp", "?function_13bb90@@YAMPBUs_tag_data@@MM@Z"),
        ("src/sound_looping_impulse_submit_native.cpp", "?function_21f720@@YAXPBUs_looping_impulse_parameters@@@Z"),
        ("src/sound_stream_envelope_native.cpp", "?sound_stream_set_envelope@@YAXPAUs_sound_stream@@MM@Z"),
        ("src/sound_looping_channel_start_native.cpp", "?function_21f8a0@@YAXJPBUs_looping_channel_properties@@PBUs_looping_effect_playback@@@Z"),
        ("src/sound_driver_filter_helpers_native.cpp", "?function_21e330@@YAJM@Z"),
        ("src/sound_driver_filter_helpers_native.cpp", "?function_219880@@YAMPBUs_sound_random_parameter@@PAKM@Z"),
        ("src/sound_driver_filter_coefficients_native.cpp", "?sound_filter_frequency_coefficient@@YAJM@Z"),
        ("src/sound_driver_filter_coefficients_native.cpp", "?sound_filter_gain_coefficient@@YAKM@Z"),
        ("src/sound_channel_parameters_native.cpp", "?function_2201f0@@YAXJPBUs_sound_channel_parameters@@_N@Z"),
        ("src/sound_effect_data_submit_native.cpp", "?function_21f960@@YAXKPBK@Z"),
        ("src/sound_mixbin_submit_native.cpp", "?sound_mixbins_add@@YAXPAUs_mixbin_settings@@JM@Z"),
        ("src/sound_mixbin_submit_native.cpp", "?function_2215d0@@YAXPAUs_mixbin_list@@PAUs_mixbin_settings@@PAUIDirectSoundStream@@@Z"),
        ("src/sound_mixbin_submit_native.cpp", "?function_2216f0@@YAXPAUs_mixbin_list@@PAUs_mixbin_settings@@PAUIDirectSoundBuffer@@@Z"),
        ("src/sound_looping_channel_update_native.cpp", "?function_21fa80@@YAXJPBUs_looping_channel_properties@@PBUs_looping_effect_playback@@_N@Z"),
        ("src/unknown_12a1b0.cpp", "?function_21c400@@YAXFMPBUs_looping_channel_spatialization@@@Z"),
        ("src/unknown_12a1b0.cpp", "g_44a150"),
        ("src/unknown_12a1b0.cpp", "g_44a16c"),
        ("src/unknown_12a1b0.cpp", "g_44a188"),
        ("src/sound_stream_flush_native.cpp", "?sound_stream_flush@@YAXPAUs_sound_stream@@@Z"),
        ("src/voice_xhv_dispose_native.cpp", "?voice_xhv_dispose@@YAXPAVc_voice_xhv@@@Z"),
        ("src/unknown_192b90.cpp", "g_444c84"),
        ("src/sound_system_startup_native.cpp", "?function_1253d0@@YAXXZ"),
        ("src/sound_system_startup_native.cpp", "?function_125600@@YAXXZ"),
        ("src/unknown_12a1b0.cpp", "?looping_sound_controllers_initialize@@YA_NXZ"),
        ("src/sound_controller_storage_native.cpp", "?g_51ebd8@@3PAXA"),
        ("src/sound_controller_storage_native.cpp", "?g_51ebdc@@3PAXA"),
        ("src/unknown_11cc10.cpp", "?function_11cc20@@YAPAUs_record_pool@@JPBDJ@Z"),
        ("src/sound_cache_dispose.cpp", "?function_2186b0@@YAXXZ"),
        ("src/unknown_12a1b0.cpp", "?function_21a0c0@@YA_NXZ"),
        ("src/unknown_218850.cpp", "?sound_cache_initialize@@YAXXZ"),
        ("src/unknown_21d110.cpp", "?sound_effects_initialize@@YA_NXZ"),
        ("src/unknown_21dda0.cpp", "?sound_voices_initialize@@YA_NPBUs_sound_voice_settings@@@Z"),
        ("src/sound_driver_startup_native.cpp", "?function_21e4b0@@YA_NPBUs_sound_driver_counts@@@Z"),
        ("src/sound_driver_teardown_native.cpp", "?function_21eae0@@YAXXZ"),
        ("src/sound_crc_update_native.cpp", "?function_163ba0@@YAXPAKPBXJ@Z"),
        ("src/sound_crc_update_native.cpp", "?g_sound_crc_table@@3PAKA"),
        ("src/sound_crc_update_native.cpp", "?g_sound_crc_table_built@@3_NA"),
        ("src/sound_cache_callbacks_native.cpp", "?sound_cache_entry_busy@@YA_NJ@Z"),
        ("src/sound_cache_callbacks_native.cpp", "?sound_cache_entry_delete@@YAXJ@Z"),
        ("src/sound_wma_decoder_release_native.cpp", "?release_decoder@c_wma_codec@@QAAXXZ"),
        ("src/sound_stream_chunk_release_native.cpp", "?sound_stream_release_chunk@@YAXPAUs_sound_stream@@PAUs_sound_chunk@@@Z"),
        ("src/sound_driver_channel_shutdown_native.cpp", "?function_21f290@@YAXXZ"),
        ("src/sound_stream_stop_native.cpp", "?sound_stream_stop@@YAXPAUs_sound_stream@@@Z"),
        ("src/unknown_13d170.cpp", "?function_13d170@@YAXPAUs_physical_object@@PBDJJJP6AXJ@ZP6A_NJ@ZP6AEJ@ZPAVc_data_allocator@@@Z"),
        ("src/sound_effect_image_startup_native.cpp", "?function_191300@@YA_NPAUIDirectSound@@@Z"),
        ("src/sound_driver_listener_native.cpp", "?function_220790@@YAXPBUs_sound_listener@@@Z"),
        ("src/sound_driver_reverb_default_native.cpp", "?g_44a2a0@@3Us_sound_driver_reverb@@A"),
        ("src/sound_driver_device_startup_native.cpp", "?sound_driver_device_startup@@YA_NXZ"),
        ("src/sound_driver_effect_buffers_startup_native.cpp", "?sound_driver_effect_buffers_startup@@YAXXZ"),
        ("src/sound_driver_effects_install_native.cpp", "?function_21e3b0@@YAXPAVc_sound_driver_effects@@@Z"),
        ("src/sound_silence_buffer_native.cpp", "?function_221490@@YAXPAPAUIDirectSoundBuffer@@@Z"),
        ("src/sound_driver_channels_startup_native.cpp", "?sound_driver_channels_startup@@YA_NPBUs_sound_driver_counts@@@Z"),
        ("src/sound_static_allocator_native.cpp", "?function_12c040@@YAXXZ"),
        ("src/sound_effect_input_voices_native.cpp", "?function_191420@@YAXPAUIDirectSound@@@Z"),
        ("src/sound_driver_stream_format_native.cpp", "?function_21f4e0@@YA_NJJ@Z"),
        ("src/sound_driver_stream_format_native.cpp", "?function_21e410@@YAXJPAUXBOXADPCMWAVEFORMAT@@PAPAVc_sound_stream_codec@@@Z"),
        ("src/sound_driver_voice_buffer_native.cpp", "?sound_driver_voice_create_buffer@@YA_NJ@Z"),
        ("src/sound_stream_create_native.cpp", "?sound_stream_create@@YA_NPAUs_sound_stream@@PAVc_sound_stream_codec@@PAUDSSTREAMDESC@@F@Z"),
        ("src/sound_driver_stream_callback_native.cpp", "?sound_driver_stream_callback@@YAXPAX0K@Z"),
        ("src/sound_driver_voice_new_native.cpp", "?sound_driver_voice_new@@YAFKK@Z"),
        ("src/unknown_218850.cpp", "??_C@_0BB@KEGBILOA@xbox?5sound?5cache?$AA@"),
        ("src/unknown_12a1b0.cpp", "??_C@_0BL@LDINANCG@sound?5playback?5controllers?$AA@"),
        ("src/unknown_218850.cpp", "??_C@_0L@LPLEFDPA@xbox?5sound?$AA@"),
        ("src/unknown_21dda0.cpp", "??_C@_0N@JEHEBEHD@sound?5voices?$AA@"),
        ("src/sound_system_startup_native.cpp", "??_C@_0O@JBDOIMAP@sound?5sources?$AA@"),
        ("src/unknown_12a1b0.cpp", "??_C@_0P@FIIBGGGF@looping?5sounds?$AA@"),
        ("src/unknown_21d110.cpp", "??_C@_0P@HNBFCPDP@sounds?5effects?$AA@"),
        ("src/sound_effect_parameters_state_native.cpp", "?g_47005c@@3Us_sound_effect_parameters@@A"),
        ("src/sound_runtime_storage_native.cpp", "?g_4e6374@@3JA"),
        ("src/sound_runtime_storage_native.cpp", "?g_4e6378@@3PAUs_sound_voice@@A"),
        ("src/sound_runtime_storage_native.cpp", "?g_502104@@3PAUs_record_pool@@A"),
        ("src/sound_runtime_storage_native.cpp", "?g_502108@@3KA"),
        ("src/sound_runtime_storage_native.cpp", "?g_50210c@@3PAUs_sound_cache_allocator@@A"),
        ("src/sound_runtime_storage_native.cpp", "?g_510c90@@3PAUs_voice_effects@@A"),
        ("src/sound_runtime_storage_native.cpp", "?g_51ebe0@@3PAXA"),
        ("src/sound_runtime_storage_native.cpp", "?g_51ebe4@@3PAUs_bink_sound_settings@@A"),
        ("src/game_state_static_allocator_native.cpp", "?function_123d40@@YAPAXPBD0J@Z"),
        ("src/sound_class_fade_lifetime_native.cpp", "?function_221850@@YAXXZ"),
        ("src/sound_class_fade_lifetime_native.cpp", "?function_2218a0@@YAXXZ"),
        ("src/sound_class_fade_lifetime_native.cpp", "?function_2218e0@@YAXXZ"),
        ("src/sound_class_fade_lifetime_native.cpp", "?function_2218f0@@YAXXZ"),
        ("src/sound_class_fade_lifetime_native.cpp", "?g_502118@@3PAUs_sound_class_fade@@A"),
        ("src/sound_class_fade_lifetime_native.cpp", "?g_50211c@@3_NA"),
        ("src/sound_codec_allocator_state_native.cpp", "?g_510c3c@@3PAXA"),
        ("src/sound_codec_allocator_state_native.cpp", "?g_510c40@@3_NA"),
        ("src/unknown_12b400.cpp", "?function_12b400@@YAXXZ"),
        ("src/globals.cpp", "?g_global_f9ae07@@3Us_physical_memory_globals@@A"),
        ("src/sound_map_lifetime_native.cpp", "?function_125690@@YAXXZ"),
        ("src/sound_map_lifetime_native.cpp", "?function_1257b0@@YAXXZ"),
        ("src/sound_map_lifetime_native.cpp", "?g_51ebd4@@3PAUs_sound_globals@@A"),
        ("src/sound_map_reset_helpers_native.cpp", "?function_191270@@YAXXZ"),
        ("src/sound_map_reset_helpers_native.cpp", "?function_21f6d0@@YAXPBK0PBX@Z"),
        ("src/sound_map_reset_helpers_native.cpp", "?sound_mix_apply@@YAXXZ"),
        ("src/sound_old_map_disposal_native.cpp", "?function_1264c0@@YAXK@Z"),
        ("src/sound_old_map_disposal_native.cpp", "?function_21d4d0@@YAXXZ"),
        ("src/sound_old_map_disposal_native.cpp", "?function_21b070@@YAXJ@Z"),
        ("src/sound_old_map_disposal_native.cpp", "?function_21a1e0@@YAXXZ"),
        ("src/sound_pcm_codec_object_native.cpp", "?g_47f0f0@@3Us_47f0f0@@A"),
        ("src/sound_pcm_codec_object_native.cpp", "h2_pcm_codec_vtable"),
        ("src/sound_pcm_codec_object_native.cpp", "?attach@c_sound_stream_codec@@UAAXPAUs_sound_stream@@@Z"),
        ("src/sound_pcm_codec_object_native.cpp", "?start@c_sound_stream_codec@@UAAXPAUs_sound_stream@@@Z"),
        ("src/sound_pcm_codec_object_native.cpp", "?stop@c_sound_stream_codec@@UAAXXZ"),
        ("src/sound_pcm_codec_control_native.cpp", "?mark_cache_usage@c_pcm_codec@@UAAXPAUs_sound_stream@@PAUs_sound_cache_usage@@@Z"),
        ("src/sound_pcm_codec_control_native.cpp", "?get_packet@c_pcm_codec@@UAA_NPAUs_sound_stream@@PAUXMEDIAPACKET@@@Z"),
        ("src/sound_pcm_codec_control_native.cpp", "?packet_submitted@c_pcm_codec@@UAAXPAUs_sound_stream@@PAUXMEDIAPACKET@@@Z"),
        ("src/sound_pcm_codec_control_native.cpp", "?packet_failed@c_pcm_codec@@UAAXPAUs_sound_stream@@PAUXMEDIAPACKET@@@Z"),
        ("src/sound_pcm_codec_control_native.cpp", "?chunk_finished@c_pcm_codec@@UAAXPAUs_sound_stream@@PAUs_sound_chunk@@J@Z"),
        ("src/sound_pcm_codec_control_native.cpp", "?can_submit@c_pcm_codec@@UAA_NPAUs_sound_stream@@_N@Z"),
        ("src/unknown_218850.cpp", "?sound_cache_chunk_get_data@@YAPAEPAUs_sound_chunk@@@Z"),
        ("src/sound_wma_codec_object_native.cpp", "?g_47f0d0@@3Us_47f0d0@@A"),
        ("src/sound_wma_codec_object_native.cpp", "h2_wma_codec_vtable"),
        ("src/sound_wma_codec_object_native.cpp", "?mark_cache_usage@c_sound_stream_codec@@UAAXPAUs_sound_stream@@PAUs_sound_cache_usage@@@Z"),
        ("src/sound_wma_codec_object_native.cpp", "?packet_failed@c_sound_stream_codec@@UAAXPAUs_sound_stream@@PAUXMEDIAPACKET@@@Z"),
        ("src/sound_wma_codec_object_native.cpp", "?chunk_finished@c_sound_stream_codec@@UAAXPAUs_sound_stream@@PAUs_sound_chunk@@J@Z"),
        ("src/sound_wma_codec_control_native.cpp", "?attach@c_wma_codec@@UAAXPAUs_sound_stream@@@Z"),
        ("src/sound_wma_codec_start_native.cpp", "?start@c_wma_codec@@UAAXPAUs_sound_stream@@@Z"),
        ("src/sound_wma_codec_start_native.cpp", "?get_packet@c_wma_codec@@UAA_NPAUs_sound_stream@@PAUXMEDIAPACKET@@@Z"),
        ("src/sound_wma_codec_control_native.cpp", "?packet_submitted@c_wma_codec@@UAAXPAUs_sound_stream@@PAUXMEDIAPACKET@@@Z"),
        ("src/sound_wma_codec_control_native.cpp", "?stop@c_wma_codec@@UAAXXZ"),
        ("src/sound_wma_codec_control_native.cpp", "?can_submit@c_wma_codec@@UAA_NPAUs_sound_stream@@_N@Z"),
        ("src/sound_wma_codec_control_native.cpp", "?read@c_wma_codec@@QAAKKKPAPAX@Z"),
        ("src/sound_wma_codec_start_native.cpp", "?wma_codec_read_callback@@YAKPAXKKPAPAX@Z"),
        ("src/sound_wma_codec_initialize_native.cpp", "?function_2ae1d0@@YAXPAUs_47f0d0@@@Z"),
        ("src/sound_wma_codec_object_native.cpp", "?function_21ebc0@@YAXJ@Z"),
        ("src/sound_wma_codec_object_native.cpp", "?function_21ebe0@@YAXXZ"),
        ("src/sound_effects_object_native.cpp", "?g_44f70c@@3MB"),
        ("src/sound_effects_object_native.cpp", "?g_44f710@@3MB"),
        ("src/sound_effects_object_native.cpp", "?g_47f088@@3Vc_sound_effects@@A"),
        ("src/sound_effects_object_native.cpp", "h2_sound_effects_vtable"),
        ("src/sound_effects_object_native.cpp", "?initialize@c_sound_effects@@UAAXPAUs_sound_effect_settings@@PAUs_sound_effect_buffers@@_N@Z"),
        ("src/sound_effects_object_native.cpp", "?v1@c_sound_effects@@UAAXXZ"),
        ("src/sound_effects_object_native.cpp", "?set_i3dl2@c_sound_effects@@UAAXPAUs_sound_i3dl2_parameters@@J@Z"),
        ("src/sound_effects_object_native.cpp", "?add_effect_sends@c_sound_effects@@UAAXPAUs_sound_send_parameters@@_NJPAUs_mixbin_settings@@@Z"),
        ("src/sound_effects_object_native.cpp", "?add_send@c_sound_effects@@UAAXJJMPAUs_mixbin_settings@@@Z"),
        ("src/sound_effects_object_native.cpp", "?add_source_mixbins@c_sound_effects@@UAAXPAUs_sound_mixbin_source@@_NJPAUs_mixbin_settings@@@Z"),
        ("src/sound_effects_object_native.cpp", "?add_sends@c_sound_effects@@UAAXPAUs_sound_send_parameters@@JJPAUs_mixbin_settings@@@Z"),
        ("src/sound_data_allocator_object_native.cpp", "?g_46875c@@3PAVc_data_allocator@@A"),
        ("src/sound_data_allocator_object_native.cpp", "h2_sound_data_allocator_object"),
        ("src/sound_data_allocator_object_native.cpp", "h2_sound_data_allocator_vtable"),
        ("src/sound_data_allocator_object_native.cpp", "h2_sound_data_allocator_allocate"),
        ("src/sound_data_allocator_object_native.cpp", "h2_sound_data_allocator_deallocate"),
        ("src/sound_static_arena_lifetime_native.cpp", "?g_510800_pool_base@@3KA"),
        ("src/sound_static_arena_lifetime_native.cpp", "?g_510804_pool_size@@3JA"),
        ("src/sound_static_arena_lifetime_native.cpp", "?g_510808_pool_checksum@@3KA"),
        ("src/sound_static_arena_lifetime_native.cpp", "?sound_static_arena_initialize@@YA_NXZ"),
        ("src/sound_static_arena_lifetime_native.cpp", "?sound_static_arena_dispose@@YAXXZ"),
        ("src/voice_xhv_lifetime_native.cpp", "?LocalChatDataReady@c_voice_xhv@@UAGJKKPAX@Z"),
        ("src/voice_xhv_lifetime_native.cpp", "?CommunicatorStatusUpdate@c_voice_xhv@@UAGJKW4XHV_VOICE_COMMUNICATOR_STATUS@@@Z"),
        ("src/voice_xhv_lifetime_native.cpp", "?SpeechRecognized@c_voice_xhv@@UAGJKPAUXHV_SR_ITEM@@K@Z"),
        ("src/voice_xhv_lifetime_native.cpp", "?MicrophoneRawDataReady@c_voice_xhv@@UAGJKKPAXPAH@Z"),
        ("src/voice_xhv_lifetime_native.cpp", "?VoiceMailDataReady@c_voice_xhv@@UAGJKKK@Z"),
        ("src/voice_xhv_lifetime_native.cpp", "?VoiceMailStopped@c_voice_xhv@@UAGJK@Z"),
        ("src/voice_xhv_lifetime_native.cpp", "?voice_xhv_reset_masks@@YAXPAVc_voice_xhv@@@Z"),
        ("src/voice_xhv_lifetime_native.cpp", "?voice_xhv_reset_port_modes@@YAXPAVc_voice_xhv@@@Z"),
        ("src/voice_xhv_lifetime_native.cpp", "?voice_start_engine@@YAXXZ"),
        ("src/voice_xhv_lifetime_native.cpp", "?voice_stop_engine@@YAXXZ"),
        ("src/voice_xhv_lifetime_native.cpp", "?h2_voice_xhv_callback_vtable@@3QBQ6AXXZB"),
        ("src/voice_xhv_lifetime_native.cpp", "?g_476fc8@@3Vc_voice_xhv@@A"),
        ("src/voice_xhv_lifetime_native.cpp", "?g_52731c@@3PAPAXA"),
        ("src/voice_runtime_storage_native.cpp", "?g_46dd54@@3PAVc_memory_source@@A"),
        ("src/voice_runtime_storage_native.cpp", "?g_4c9878@@3Us_voice_globals@@A"),
        ("src/voice_runtime_storage_native.cpp", "?g_5259b8@@3Us_voice_player_values@@A"),
        ("src/voice_runtime_storage_native.cpp", "?g_525a00@@3Us_voice_channels@@A"),
        ("src/voice_runtime_storage_native.cpp", "?g_527104@@3Us_voice_routing@@A"),
        ("src/voice_runtime_storage_native.cpp", "?function_53210@@YAXXZ"),
        ("src/voice_runtime_storage_native.cpp", "?voice_channels_initialize@@YAXPAUs_voice_channels@@@Z"),
        ("src/voice_runtime_storage_native.cpp", "?voice_player_settings_initialize@@YAXPAUs_voice_player_settings@@@Z"),
        ("src/voice_stage_lifetime_native.cpp", "?allocate@c_physical_memory_source@@UAEPAXJ@Z"),
        ("src/voice_stage_lifetime_native.cpp", "?release@c_physical_memory_source@@UAEXPAX@Z"),
        ("src/voice_stage_lifetime_native.cpp", "?function_53310@@YGXJ@Z"),
        ("src/voice_stage_lifetime_native.cpp", "?function_533e0@@YAXXZ"),
        ("src/voice_stage_lifetime_native.cpp", "?voice_initialize_menu_pool@@YAXXZ"),
        ("src/voice_stage_lifetime_native.cpp", "?voice_dispose_menu_pool@@YAXXZ"),
        ("src/voice_stage_lifetime_native.cpp", "?h2_physical_memory_source_vtable@@3Us_table@c_memory_source@@B"),
        ("src/voice_stage_lifetime_native.cpp", "?g_476fbc@@3Vc_physical_memory_source@@A"),
        ("src/voice_stage_lifetime_native.cpp", "?g_voice_pool_name@@3QBDB"),
        ("src/voice_stage_lifetime_native.cpp", "?g_voice_menu_pool_name@@3QBDB"),
    ]
    if [(entry.get("source"), entry.get("symbol", entry.get("global")))
            for entry in providers] != expected_providers:
        raise RuntimeError("sound-idle provider inventory changed")
    provider_symbols = [entry.get("symbol", entry.get("global"))
                        for entry in providers]
    if len(set(provider_symbols)) != 259:
        raise RuntimeError("sound-idle direct provider symbols are not unique")
    for entry in providers:
        if ("symbol" in entry) == ("global" in entry):
            raise RuntimeError("sound-idle provider kind is ambiguous")
        source = args.root / entry.get("source", "")
        if not source.is_file():
            raise RuntimeError(f"sound-idle provider source is absent: {source}")

    if manifest.get("compact_helpers") != [{
            "symbol": "?physical_memory_reset_time@@YAXPAUs_physical_object@@@Z",
            "semantics":
                "rebase every live physical-cache block timestamp after the "
                "clock reaches its maximum"}, {
            "symbol":
                "?function_21f360@@YAXPAUs_sound_driver_channel_usage@@@Z",
            "semantics":
                "snapshot per-state channel occupancy, free voices, and the "
                "three paired driver counts"}, {
            "symbol": "?function_2197f0@@YAJM@Z",
            "semantics":
                "clamp linear gain and encode the recovered -64 through 0 "
                "decibel range"}, {
            "symbol": "?function_12aff0@@YAMMMM_N@Z",
            "semantics":
                "bounded linear interpolation with optional reversed "
                "direction"}]:
        raise RuntimeError("sound-idle compact helper inventory changed")
    if manifest.get("playback_stop_helpers") != [{
            "symbol": "?function_127320@@YAXJJ@Z",
            "semantics":
                "stop a live playback in recovered voice, reference, callback, "
                "looping-controller, and record-release order"}, {
            "symbol": "?function_126960@@YAXPAUs_sound_playback@@@Z",
            "semantics":
                "release the playback-held sound cache reference selected "
                "through authentic tag permutation data"}, {
            "symbol": "?sound_playback_delete@@YAXJ@Z",
            "semantics":
                "adjust and release the optional looping controller before "
                "releasing the playback record"}, {
            "symbol": "?sound_voice_free@@YAXF@Z",
            "semantics":
                "unlock queued and current chunks, flush the live stream, "
                "clear ownership, and release the shared voice"}, {
            "symbol": "?sound_voice_release@@YAXJ@Z",
            "semantics":
                "clear sound-to-voice ownership and release the shared voice "
                "record at zero references"}, {
            "symbol": "?looping_sound_controller_release@@YAXJ@Z",
            "semantics":
                "preserve seven-bit looping-controller reference semantics "
                "and release at zero"}]:
        raise RuntimeError("sound-idle playback-stop inventory changed")
    if manifest.get("playback_fade_helpers") != [{
            "symbol": "?function_12a810@@YAJJ@Z",
            "semantics":
                "anchor NONE-relative fade times, evaluate linear or power "
                "gain curves, and return pinned decibel bits"}, {
            "symbol": "?function_2195f0@@YAMM@Z",
            "semantics":
                "pin decibels to the recovered -64 through 0 range and "
                "convert to linear gain"}]:
        raise RuntimeError("sound-idle playback-fade inventory changed")
    voice_transition_helpers = manifest.get("voice_transition_helpers", [])
    if [entry.get("symbol") for entry in voice_transition_helpers] != [
            "?sound_voice_update_chunks@@YAXF@Z",
            "?sound_stream_update@@YAXPAUs_sound_stream@@@Z",
            "?sound_stream_release_chunks@@YAXPAUs_sound_stream@@@Z",
            "?function_218850@@YAKJPAUs_sound_chunk@@K@Z",
            "?function_219290@@YAJFPAUs_permutation_group@@@Z",
            "?function_21f650@@YAMJJ@Z",
            "?sound_reference_release@@YAXPAUs_sound_reference_holder@@@Z"]:
        raise RuntimeError("sound-idle voice-transition inventory changed")
    if any(not entry.get("semantics") for entry in voice_transition_helpers):
        raise RuntimeError("sound-idle voice-transition semantics are missing")
    cache_helpers = manifest.get("sound_cache_allocation_helpers", [])
    if [entry.get("symbol") for entry in cache_helpers] != [
            "?function_218a10@@YAXPAUs_sound_chunk@@J@Z",
            "?function_213760@@YAJKJPAXPAKPA_NJJ@Z"]:
        raise RuntimeError("sound-idle cache-allocation inventory changed")
    if any(not entry.get("semantics") for entry in cache_helpers):
        raise RuntimeError("sound-idle cache-allocation semantics are missing")
    async_read_helpers = manifest.get("sound_cache_async_read_helpers", [])
    if [entry.get("symbol") for entry in async_read_helpers] != [
            "?function_1a0f10@@YAJUs_file_handle@@PAXKKJJPAKPC_N@Z",
            "?function_1a0e70@@YAJPATs_async_task@@@Z"]:
        raise RuntimeError("sound-idle async-read inventory changed")
    if any(not entry.get("semantics") for entry in async_read_helpers):
        raise RuntimeError("sound-idle async-read semantics are missing")
    queue_helpers = manifest.get("sound_cache_async_queue_helpers", [])
    if [entry.get("symbol") for entry in queue_helpers] != [
            "?function_120ba0@@YAJJPATs_async_task@@JP6AJ0@ZPC_N@Z",
            "?async_task_queue@@YAJPAUs_job_node@@@Z",
            "?function_120900@@YAXPAUs_job_node@@@Z"]:
        raise RuntimeError("sound-idle async-queue inventory changed")
    if any(not entry.get("semantics") for entry in queue_helpers):
        raise RuntimeError("sound-idle async-queue semantics are missing")
    read_limit = manifest.get("sound_cache_read_limit", {})
    if read_limit.get("symbol") != "?g_46e488@@3KA" or \
            read_limit.get("value") != "0x40000" or \
            not read_limit.get("semantics"):
        raise RuntimeError("sound-idle cache-read limit changed")
    cache_state_bindings = manifest.get("production_cache_state_bindings")
    expected_cache_state_bindings = {
        "?g_55aca8@@3JA": "0x0055aca8",
        "?g_557c90@@3PAUs_texture_cache_file@@A": "0x00557c90",
    }
    if not isinstance(cache_state_bindings, list) or \
            len(cache_state_bindings) != 2 or \
            any(not entry.get("semantics") for entry in cache_state_bindings) or \
            {entry.get("symbol"): entry.get("address")
             for entry in cache_state_bindings} != expected_cache_state_bindings:
        raise RuntimeError("sound-idle production cache-state bindings changed")
    main_menu_bindings = json.loads((
        args.root / "port/runtime/main_menu_bindings.json").read_text())
    binding_addresses = {
        entry.get("symbol"): entry.get("address")
        for entry in main_menu_bindings.get("symbols", [])}
    for symbol, address in expected_cache_state_bindings.items():
        if binding_addresses.get(symbol) != address:
            raise RuntimeError(
                f"sound-idle production cache-state binding is absent: {symbol}")
    gain_defaults = manifest.get("sound_gain_defaults")
    expected_gain_defaults = {
        "?g_440c48@@3JA": "0x00000000",
        "?g_440c4c@@3JA": "0xc2800000",
        "?g_468834@@3MA": "1.0",
    }
    if not isinstance(gain_defaults, list) or len(gain_defaults) != 3 or \
            any(not entry.get("semantics") for entry in gain_defaults) or \
            {entry.get("symbol"): entry.get("value")
             for entry in gain_defaults} != expected_gain_defaults:
        raise RuntimeError("sound-idle gain defaults changed")
    busy_state = manifest.get("busy_state_provider", {})
    if busy_state.get("symbol") != "?function_12be90@@YA_NXZ" or \
            not busy_state.get("semantics") or \
            busy_state.get("mutable_state") != [
                "?main_globals@@3Us_main_globals@@A",
                "?g_4ed39d@@3_NA",
                "?g_4e6470@@3JA",
                "?g_4e64a0@@3JA"]:
        raise RuntimeError("sound-idle busy-state ownership changed")
    spatialization = manifest.get("spatialization_provider", {})
    if spatialization.get("symbol") != \
            "?function_129aa0@@YAXFPAJPAUs_looping_channel_spatialization@@@Z" or \
            not spatialization.get("semantics") or \
            not spatialization.get("execution_gate"):
        raise RuntimeError("sound-idle spatialization ownership changed")
    spatial_gain = manifest.get("spatial_gain_provider", {})
    if spatial_gain.get("symbol") != "?function_12a6d0@@YAJFMM@Z" or \
            not spatial_gain.get("semantics") or \
            not spatial_gain.get("execution_gate"):
        raise RuntimeError("sound-idle spatial-gain ownership changed")
    directional_cone = manifest.get("directional_cone_provider", {})
    if directional_cone.get("symbol") != "?function_125060@@YAJPBUs_sound@@J@Z" or \
            not directional_cone.get("semantics") or \
            not directional_cone.get("execution_gate"):
        raise RuntimeError("sound-idle directional-cone ownership changed")
    cone_closure = manifest.get("cone_attenuation_closure", {})
    if cone_closure.get("root") != \
            "?function_12ad30@@YAMPBTvector3f@@PBUs_sound@@J0@Z" or \
            len(cone_closure.get("providers", [])) != 6 or \
            not cone_closure.get("semantics") or \
            not cone_closure.get("execution_gate"):
        raise RuntimeError("sound-idle cone-attenuation closure changed")
    source_transforms = manifest.get("source_transform_closure", {})
    if len(source_transforms.get("providers", [])) != 4 or \
            not source_transforms.get("semantics") or \
            not source_transforms.get("execution_gate"):
        raise RuntimeError("sound-idle source-transform closure changed")
    class_route = manifest.get("sound_class_route_provider", {})
    if class_route.get("symbol") != "?function_221810@@YAPAUs_unknown_5c@@F@Z" or \
            not class_route.get("semantics") or \
            not class_route.get("execution_gate"):
        raise RuntimeError("sound-idle sound-class routing changed")
    impulse_closure = manifest.get("impulse_channel_closure", {})
    if impulse_closure.get("root") != \
            "?function_128ca0@@YAXFJPBUs_looping_channel_spatialization@@@Z" or \
            len(impulse_closure.get("providers", [])) != 3 or \
            not impulse_closure.get("semantics") or \
            not impulse_closure.get("execution_gate"):
        raise RuntimeError("sound-idle impulse-channel closure changed")
    effect_routes = manifest.get("effect_routing_closure", {})
    if len(effect_routes.get("roots", [])) != 2 or \
            effect_routes.get("recursive_definitions") != [
                "?function_2222c0@@YAXPAUs_effect_data_block@@PBUs_effect_component@@PBUs_effect_component_overrides@@PBUs_effect_inputs@@J@Z",
                "?function_191660@@YAKJJ@Z"] or \
            not effect_routes.get("semantics") or \
            not effect_routes.get("execution_gate"):
        raise RuntimeError("sound-idle effect-routing closure changed")
    effect_lookups = manifest.get("effect_lookup_closure", {})
    if len(effect_lookups.get("providers", [])) != 5 or \
            effect_lookups.get("immutable_speaker_directions") != [
                [0.7071067690849304, 0.7071067690849304, 0.0],
                [0.7071067690849304, -0.7071067690849304, 0.0],
                [-0.7071067690849304, 0.7071067690849304, 0.0],
                [-0.7071067690849304, -0.7071067690849304, 0.0],
                [0.7071067690849304, 0.0, 0.0]
            ] or not effect_lookups.get("semantics") or \
            not effect_lookups.get("execution_gate"):
        raise RuntimeError("sound-idle effect-lookup closure changed")
    speaker_source = (args.root / "src/unknown_192b90.cpp").read_text()
    for fragment in ["static point3f const g_444c84[5]", "front left",
                     "front right", "back left", "back", "center"]:
        if fragment not in speaker_source:
            raise RuntimeError(
                f"sound-idle speaker table lost {fragment}")
    channel_scalars = manifest.get("channel_parameter_scalar_closure", {})
    if channel_scalars.get("providers") != [
            "?function_21e330@@YAJM@Z",
            "?function_219880@@YAMPBUs_sound_random_parameter@@PAKM@Z",
            "?sound_filter_frequency_coefficient@@YAJM@Z",
            "?sound_filter_gain_coefficient@@YAKM@Z"
            ] or not channel_scalars.get("semantics") or \
            not channel_scalars.get("execution_gate"):
        raise RuntimeError("sound-idle channel parameter scalar closure changed")
    channel_submission = manifest.get("channel_parameter_submission_closure", {})
    if channel_submission.get("provider") != \
            "?function_2201f0@@YAXJPBUs_sound_channel_parameters@@_N@Z" or \
            channel_submission.get("filter_modes") != {
                "bypass": 0,
                "dls2": 1,
                "parametric_equalizer": 2,
                "multiband": 3
            } or not channel_submission.get("semantics") or \
            len(channel_submission.get("execution_gates", [])) != 4:
        raise RuntimeError("sound-idle channel parameter submission closure changed")
    effect_data = manifest.get("effect_data_submission_closure", {})
    if effect_data.get("provider") != "?function_21f960@@YAXKPBK@Z" or \
            effect_data.get("packed_header_size") != 16 or \
            effect_data.get("effect_map_stride") != 32 or \
            not effect_data.get("semantics") or \
            len(effect_data.get("execution_gates", [])) != 4:
        raise RuntimeError("sound-idle effect data submission closure changed")
    mixbins = manifest.get("mixbin_submission_closure", {})
    if mixbins.get("providers") != [
            "?sound_mixbins_add@@YAXPAUs_mixbin_settings@@JM@Z",
            "?function_2215d0@@YAXPAUs_mixbin_list@@PAUs_mixbin_settings@@PAUIDirectSoundStream@@@Z",
            "?function_2216f0@@YAXPAUs_mixbin_list@@PAUs_mixbin_settings@@PAUIDirectSoundBuffer@@@Z"
            ] or mixbins.get("pair_capacity") != 8 or \
            not mixbins.get("semantics") or \
            len(mixbins.get("execution_gates", [])) != 3:
        raise RuntimeError("sound-idle mixbin submission closure changed")
    xhv_dispose = manifest.get("xhv_dispose_closure", {})
    if xhv_dispose.get("source") != "src/voice_xhv_dispose_native.cpp" or \
            xhv_dispose.get("provider") != \
            "?voice_xhv_dispose@@YAXPAVc_voice_xhv@@@Z" or \
            xhv_dispose.get("checked_layout") != {
                "xuid_size": 12,
                "initialized_offset": 4,
                "flags_offset": 12,
                "engine_offset": 16
            } or xhv_dispose.get("retail_verified_policy") != {
                "local_ports": 4,
                "remote_capacity": 30,
                "remote_enabled_mask": 4,
                "user_flags": 0,
                "local_unregister":
                    "?UnregisterLocalTalker@CXHVEngine@XHVNamespace@@QAGJK@Z",
                "remote_enumeration":
                    "?GetRemoteTalkers@CXHVEngine@XHVNamespace@@QAGJPAKPAU_XUID@@@Z",
                "remote_unregister":
                    "?UnregisterRemoteTalker@CXHVEngine@XHVNamespace@@QAGJU_XUID@@@Z",
                "engine_release":
                    "?Release@CXHVEngine@XHVNamespace@@QAGKXZ"
            } or not xhv_dispose.get("semantics") or \
            len(xhv_dispose.get("execution_gates", [])) != 3:
        raise RuntimeError("sound-idle XHV disposal closure changed")
    xhv_source = (args.root / xhv_dispose["source"]).read_text()
    for fragment in [
            "for (dword port = 0; port < 4; port++)",
            "_XUID talkers[30]",
            "xhv_get_remote_talkers",
            "xhv_unregister_remote_talker",
            "xhv_release(xhv->engine)",
            "xhv->engine = 0",
            "xhv->initialized = false"]:
        if fragment not in xhv_source:
            raise RuntimeError(f"sound-idle XHV disposal lost {fragment}")
    stream_flush = manifest.get("stream_flush_closure", {})
    if stream_flush.get("source") != "src/sound_stream_flush_native.cpp" or \
            stream_flush.get("provider") != \
            "?sound_stream_flush@@YAXPAUs_sound_stream@@@Z" or \
            stream_flush.get("checked_layout") != {
                "stream_size": 52,
                "state_offset": 1,
                "flags_offset": 3,
                "interface_offset": 36,
                "codec_offset": 48
            } or stream_flush.get("retail_verified_constants") != {
                "paused_status_bit": 131072,
                "resume_argument": 0,
                "async_envelope_flags": 3,
                "codec_stop_vtable_offset": 28
            } or not stream_flush.get("semantics") or \
            len(stream_flush.get("execution_gates", [])) != 3:
        raise RuntimeError("sound-idle stream flush closure changed")
    stream_flush_source = (args.root / stream_flush["source"]).read_text()
    for fragment in [
            "k_stream_status_paused = 0x00020000",
            "k_stream_pause_resume = 0",
            "k_stream_flush_async_envelope = 3",
            "IDirectSoundStream_FlushEx",
            "stream->codec->stop()",
            "stream->flags |= 4"]:
        if fragment not in stream_flush_source:
            raise RuntimeError(
                f"sound-idle stream flush lost {fragment}")
    looping_root = manifest.get("looping_channel_state_machine_closure", {})
    if looping_root.get("source") != "src/unknown_12a1b0.cpp" or \
            looping_root.get("root") != \
            "?function_21c400@@YAXFMPBUs_looping_channel_spatialization@@@Z" or \
            looping_root.get("recursive_definitions") != [
                "?function_21c190@@YA_NPAUs_type_5ef569@@PAUs_type_06b94f@@JPAK@Z",
                "?looping_sound_get_permutations@@YAKPAUs_type_5ef569@@PAUlooping_sound_definition@@PAUs_looping_playback_definition@@JF_N@Z",
                "?looping_sound_set_permutations@@YAXPAUs_type_5ef569@@PAUlooping_sound_definition@@PAUs_looping_playback_definition@@JFKF@Z"
            ] or not looping_root.get("semantics") or \
            looping_root.get("source_owned_transition_tables") != [
                "g_44a150", "g_44a16c", "g_44a188"
            ] or len(looping_root.get("new_mutable_imports", [])) != 3 or \
            len(looping_root.get("execution_gates", [])) != 3:
        raise RuntimeError("sound-idle looping channel closure changed")
    looping_source = (args.root / looping_root["source"]).read_text()
    for fragment in [
            "function_21f8a0(voice->channel_index",
            "function_21c190(sound, track",
            "sound_playback_set_chunk(voice->sound_index",
            "sound_voice_queue_chunk(voice_index",
            "function_21fa80(",
            "g_44a150[7] = { 1, 1, 2, 5, 5, 1, 6 }",
            "g_44a16c[7] = { 3, 3, 2, 4, 4, 3, 6 }",
            "g_44a188[7] = { 20, 28, 36, 64, 44, 72, 52 }"]:
        if fragment not in looping_source:
            raise RuntimeError(
                f"sound-idle looping root lost {fragment}")
    shared_update = manifest.get("shared_channel_update_closure", {})
    expected_update_symbol = \
        "?function_21fa80@@YAXJPBUs_looping_channel_properties@@PBUs_looping_effect_playback@@_N@Z"
    if shared_update.get("source") != \
            "src/sound_looping_channel_update_native.cpp" or \
            shared_update.get("symbol") != expected_update_symbol or \
            shared_update.get("checked_layouts") != {
                "channel_update_input_size": 72,
                "channel_levels_offset": 56,
                "sound_stream_size": 52,
                "driver_channels_offset": 12,
                "driver_voices_offset": 6680,
                "driver_effects_offset": 10932
            } or shared_update.get("source_owned_providers") != [
                "?function_2201f0@@YAXJPBUs_sound_channel_parameters@@_N@Z",
                "?sound_mixbins_add@@YAXPAUs_mixbin_settings@@JM@Z",
                "?function_2215d0@@YAXPAUs_mixbin_list@@PAUs_mixbin_settings@@PAUIDirectSoundStream@@@Z",
                "?function_2216f0@@YAXPAUs_mixbin_list@@PAUs_mixbin_settings@@PAUIDirectSoundBuffer@@@Z"
            ] or len(shared_update.get("directsound_operations", [])) != 9 or \
            len(shared_update.get("effects_virtual_methods", [])) != 4 or \
            not shared_update.get("semantics") or \
            len(shared_update.get("execution_gates", [])) != 4 or \
            not shared_update.get("prohibited_substitution"):
        raise RuntimeError("sound-idle shared channel update frontier changed")
    shared_source = (args.root / shared_update["source"]).read_text()
    for fragment in [
            "IDirectSoundStream_GetVoiceProperties",
            "function_2215d0",
            "IDirectSoundBuffer_GetVoiceProperties",
            "function_2216f0",
            "channel_effects(globals)->add_effect_sends",
            "IDirectSoundStream_SetMixBins",
            "IDirectSoundStream_SetPitch",
            "function_2201f0"]:
        if fragment not in shared_source:
            raise RuntimeError(
                f"sound-idle shared channel source lost {fragment}")
    looping_channel_start = manifest.get("looping_channel_start_closure", {})
    if looping_channel_start.get("providers") != [
            "?sound_stream_set_envelope@@YAXPAUs_sound_stream@@MM@Z",
            "?function_21f8a0@@YAXJPBUs_looping_channel_properties@@PBUs_looping_effect_playback@@@Z"
            ] or not looping_channel_start.get("semantics") or \
            not looping_channel_start.get("execution_gate"):
        raise RuntimeError("sound-idle looping channel start closure changed")
    looping_impulse = manifest.get("looping_impulse_submission_closure", {})
    if looping_impulse.get("providers") != [
            "?function_13bb90@@YAMPBUs_tag_data@@MM@Z",
            "?function_21f720@@YAXPBUs_looping_impulse_parameters@@@Z"
            ] or not looping_impulse.get("semantics") or \
            not looping_impulse.get("execution_gate"):
        raise RuntimeError("sound-idle looping impulse closure changed")
    looping_chunk_policy = manifest.get("looping_chunk_policy_closure", {})
    if looping_chunk_policy.get("providers") != [
            "?sound_playback_set_chunk@@YAXJJDDF@Z",
            "?sound_voice_queue_chunk@@YAXFPBUs_sound_permutation@@F@Z",
            "?sound_stream_add_chunk@@YAXPAUs_sound_stream@@PAUs_sound_chunk@@@Z"
            ] or not looping_chunk_policy.get("semantics") or \
            not looping_chunk_policy.get("execution_gate"):
        raise RuntimeError("sound-idle looping chunk policy closure changed")
    looping_voice_policy = manifest.get("looping_voice_policy_closure", {})
    if looping_voice_policy.get("providers") != [
            "?function_128500@@YAXJPAUs_looping_voice_counts@@@Z",
            "?function_128a60@@YAFJFPBF@Z",
            "?function_12a420@@YAXF@Z"
            ] or not looping_voice_policy.get("semantics") or \
            not looping_voice_policy.get("execution_gate"):
        raise RuntimeError("sound-idle looping voice policy closure changed")
    looping_selection = manifest.get("looping_selection_closure", {})
    if looping_selection.get("providers") != [
            "?function_218f50@@YAFPAUs_looping_playback_definition@@FM@Z",
            "?function_219110@@YAFPAUs_set_ref@@FPAKF1PA_N_N@Z",
            "?function_2192b0@@YAXPAUs_animation_state@@PAUs_animation_ref@@E@Z",
            "?function_219370@@YAJPAUs_animation_state@@PAUs_animation_ref@@@Z",
            "?function_2194c0@@YAKPAUs_packed_value@@PAUs_animation_ref@@@Z",
            "?function_219560@@YAXPAUs_packed_value@@PAUs_animation_ref@@K@Z"
            ] or not looping_selection.get("semantics") or \
            not looping_selection.get("execution_gate"):
        raise RuntimeError("sound-idle looping selection closure changed")
    general_function = manifest.get("general_function_closure", {})
    if general_function.get("root") != "?function_13b390@@YAMPBXMM@Z" or \
            general_function.get("providers") != [
                "?function_13b390@@YAMPBXMM@Z",
                "?function_17c900@@YAMFM@Z",
                "?function_17ca10@@YAMMF@Z",
                "g_44178c"
            ] or general_function.get("numeric_boundary") != \
            "?h2_xbox_rsqrtss@@YAMM@Z" or \
            not general_function.get("semantics") or \
            not general_function.get("execution_gate"):
        raise RuntimeError("sound-idle general-function closure changed")
    playback_gain = manifest.get("playback_gain_closure", {})
    if playback_gain.get("root") != "?function_1251e0@@YAJPBXJM@Z" or \
            playback_gain.get("recursive_definitions") != [
                "?function_127010@@YAJF@Z",
                "?sound_definition_gain_lower@@YAJPBUs_sound_definition@@@Z",
                "?sound_definition_gain_upper@@YAJPBUs_sound_definition@@@Z"
            ] or not playback_gain.get("semantics") or \
            not playback_gain.get("execution_gate"):
        raise RuntimeError("sound-idle playback gain closure changed")
    playback_chunk_abi = manifest.get("playback_chunk_abi", {})
    if playback_chunk_abi != {
            "symbol": "?sound_playback_set_chunk@@YAXJJDDF@Z",
            "parameters": ["long", "long", "char", "char", "short"],
            "rejected_widened_symbol":
                "?sound_playback_set_chunk@@YAXJJFFF@Z",
            "semantics":
                "preserve the recovered signed byte pitch-range and "
                "permutation indices plus signed short chunk index"}:
        raise RuntimeError("sound-idle playback chunk ABI changed")
    playback_reference = manifest.get("playback_reference_closure", {})
    if playback_reference.get("root") != \
            "?function_125e60@@YAJPAUs_looping_track_sound@@@Z" or \
            playback_reference.get("recursive_definitions") != [
                "?sound_playback_acquire_reference@@YAXPAUs_sound_playback@@@Z"
            ] or not playback_reference.get("semantics") or \
            not playback_reference.get("execution_gate"):
        raise RuntimeError("sound-idle playback-reference closure changed")

    device_startup = manifest.get("device_startup_closure", {})
    if device_startup.get("symbol") != \
            "?sound_driver_device_startup@@YA_NXZ" or \
            device_startup.get("backend_operations") != [
                "DirectSoundCreate", "DirectSoundDoWork",
                "IDirectSound_GetCaps", "IDirectSound_GetSpeakerConfig",
                "IDirectSound_SetDistanceFactor",
                "IDirectSound_SetDopplerFactor",
                "IDirectSound_SetMixBinHeadroom",
                "IDirectSound_SetRolloffFactor"] or \
            not device_startup.get("semantics") or \
            not device_startup.get("failure_policy") or \
            not device_startup.get("execution_gate"):
        raise RuntimeError("sound-idle device-startup closure changed")

    effect_startup = manifest.get("effect_buffer_startup_closure", {})
    if effect_startup.get("root") != \
            "?sound_driver_effect_buffers_startup@@YAXXZ" or \
            effect_startup.get("providers") != [
                "?sound_driver_effect_buffers_startup@@YAXXZ",
                "?function_21e3b0@@YAXPAVc_sound_driver_effects@@@Z",
                "?function_221490@@YAXPAPAUIDirectSoundBuffer@@@Z"] or \
            effect_startup.get("backend_operations") != [
                "DirectSoundCreateBuffer",
                "IDirectSoundBuffer_SetBufferData",
                "IDirectSoundBuffer_SetHeadroom",
                "IDirectSoundBuffer_SetMaxDistance",
                "IDirectSoundBuffer_SetMinDistance",
                "IDirectSound_CreateSoundBuffer"] or \
            effect_startup.get("mutable_effects_lifetime") != \
            "?g_47f088@@3Vc_sound_effects@@A" or \
            not effect_startup.get("semantics") or \
            not effect_startup.get("failure_policy") or \
            not effect_startup.get("execution_gate"):
        raise RuntimeError("sound-idle effect-buffer closure changed")

    channel_startup = manifest.get("channel_startup_closure", {})
    if channel_startup.get("root") != \
            "?sound_driver_channels_startup@@YA_NPBUs_sound_driver_counts@@@Z" or \
            channel_startup.get("providers") != [
                "?sound_driver_channels_startup@@YA_NPBUs_sound_driver_counts@@@Z",
                "?function_12c040@@YAXXZ",
                "?function_191420@@YAXPAUIDirectSound@@@Z",
                "?function_21f4e0@@YA_NJJ@Z",
                "?function_21e410@@YAXJPAUXBOXADPCMWAVEFORMAT@@PAPAVc_sound_stream_codec@@@Z",
                "?sound_driver_voice_create_buffer@@YA_NJ@Z",
                "?sound_stream_create@@YA_NPAUs_sound_stream@@PAVc_sound_stream_codec@@PAUDSSTREAMDESC@@F@Z",
                "?sound_driver_stream_callback@@YAXPAX0K@Z",
                "?sound_driver_voice_new@@YAFKK@Z"] or \
            channel_startup.get("mutable_lifetimes") != [
                "?g_47f0f0@@3Us_47f0f0@@A",
                "?g_510c40@@3_NA"] or \
            channel_startup.get("backend_operations") != [
                "IDirectSoundBuffer_SetConeOutsideVolume",
                "IDirectSoundBuffer_SetDopplerFactor",
                "IDirectSoundBuffer_SetRolloffFactor",
                "IDirectSoundStream_SetHeadroom",
                "IDirectSound_CreateSoundStream"] or \
            not channel_startup.get("semantics") or \
            not channel_startup.get("callback_semantics") or \
            not channel_startup.get("voice_semantics") or \
            not channel_startup.get("failure_policy") or \
            not channel_startup.get("execution_gate"):
        raise RuntimeError("sound-idle channel-startup closure changed")

    diagnostics = manifest.get("diagnostic_string_closure", {})
    if diagnostics.get("providers") != [
            {"symbol": "??_C@_0BB@KEGBILOA@xbox?5sound?5cache?$AA@",
             "value": "xbox sound cache"},
            {"symbol": "??_C@_0BL@LDINANCG@sound?5playback?5controllers?$AA@",
             "value": "sound playback controllers"},
            {"symbol": "??_C@_0L@LPLEFDPA@xbox?5sound?$AA@",
             "value": "xbox sound"},
            {"symbol": "??_C@_0N@JEHEBEHD@sound?5voices?$AA@",
             "value": "sound voices"},
            {"symbol": "??_C@_0O@JBDOIMAP@sound?5sources?$AA@",
             "value": "sound sources"},
            {"symbol": "??_C@_0P@FIIBGGGF@looping?5sounds?$AA@",
             "value": "looping sounds"},
            {"symbol": "??_C@_0P@HNBFCPDP@sounds?5effects?$AA@",
             "value": "sounds effects"}] or \
            not diagnostics.get("semantics") or \
            not diagnostics.get("execution_gate"):
        raise RuntimeError("sound-idle diagnostic string closure changed")

    effect_state = manifest.get("effect_parameter_state_closure", {})
    if effect_state.get("provider") != \
            "?g_47005c@@3Us_sound_effect_parameters@@A" or \
            effect_state.get("size") != 0x30 or \
            effect_state.get("initial_values") != {
                "dirty": True,
                "room": 0,
                "room_hf": 0,
                "direct": 0,
                "direct_hf": 0,
                "unknown70": 0.0,
                "unknown74": 0,
                "unknown78": 0.0,
                "unknown7c": 0,
                "unknown80": 0.25,
                "unknown84": 8000,
                "unknown88": 0.0} or \
            not effect_state.get("semantics") or \
            not effect_state.get("execution_gate"):
        raise RuntimeError("sound-idle effect parameter state changed")

    class_fades = manifest.get("sound_class_fade_lifetime_closure", {})
    if class_fades.get("providers") != [
            "?function_123d40@@YAPAXPBD0J@Z",
            "?function_221850@@YAXXZ",
            "?function_2218a0@@YAXXZ",
            "?function_2218e0@@YAXXZ",
            "?function_2218f0@@YAXXZ",
            "?g_502118@@3PAUs_sound_class_fade@@A",
            "?g_50211c@@3_NA"] or \
            class_fades.get("record_count") != 54 or \
            class_fades.get("record_size") != 16 or \
            class_fades.get("allocation_bytes") != 864 or \
            not class_fades.get("initial_state") or \
            not class_fades.get("semantics") or \
            not class_fades.get("execution_gate"):
        raise RuntimeError("sound-idle class-fade lifetime changed")

    codec_storage = manifest.get("sound_codec_allocator_storage_closure", {})
    if codec_storage.get("providers") != [
            {"symbol": "?g_510c3c@@3PAXA",
             "publication": "0xcc-byte static codec block publication"},
            {"symbol": "?g_510c40@@3_NA",
             "publication": "codec block in-use publication"}] or \
            codec_storage.get("storage_bytes") != 5 or \
            not codec_storage.get("initial_state") or \
            not codec_storage.get("semantics") or \
            not codec_storage.get("execution_gate"):
        raise RuntimeError("sound-idle codec allocator storage changed")

    physical_memory = manifest.get("physical_memory_lifetime", {})
    if physical_memory.get("storage") != \
            "?g_global_f9ae07@@3Us_physical_memory_globals@@A" or \
            physical_memory.get("initializer") != \
            "?function_12b400@@YAXXZ" or \
            "0x61000" not in physical_memory.get("layout", "") or \
            "0x3145000" not in physical_memory.get("layout", "") or \
            "XPhysicalAlloc" not in physical_memory.get("semantics", "") or \
            not physical_memory.get("execution_gate"):
        raise RuntimeError("sound physical-memory lifetime changed")
    physical_bridge = manifest.get("native_physical_memory_bridge", {})
    if physical_bridge.get("host_abi_version") != 15 or \
            physical_bridge.get("guest_wrappers") != [
                {"guest_symbol": "XPhysicalAlloc",
                 "host_import": "host_h2_physical_alloc"},
                {"guest_symbol": "XPhysicalProtect",
                 "host_import": "host_h2_physical_protect"}] or \
            physical_bridge.get("physical_bounds") != {
                "base": "0x61000", "end": "0x3145000",
                "virtual_alias_base": "0x80061000",
                "virtual_alias_end": "0x83145000"} or \
            physical_bridge.get("production_score_before") != 76 or \
            physical_bridge.get("production_score_after") != 74 or \
            not physical_bridge.get("semantics") or \
            not physical_bridge.get("execution_gate"):
        raise RuntimeError("native physical-memory bridge changed")

    map_lifetime = manifest.get("map_sound_globals_lifetime", {})
    if map_lifetime.get("storage") != \
            "?g_51ebd4@@3PAUs_sound_globals@@A" or \
            map_lifetime.get("buffer_size") != \
            "?g_47f0d0@@3Us_47f0d0@@A+0x10" or \
            "interior field" not in \
            map_lifetime.get("buffer_size_semantics", "") or \
            map_lifetime.get("startup_owner") != \
            "?function_125690@@YAXXZ" or \
            map_lifetime.get("teardown_owner") != \
            "?function_1257b0@@YAXXZ" or \
            map_lifetime.get("reset_helpers") != [
                "?sound_mix_apply@@YAXXZ",
                "?function_21f6d0@@YAXPBK0PBX@Z",
                "?function_191270@@YAXXZ"] or \
            len(map_lifetime.get("required_order", [])) != 7 or \
            map_lifetime.get("production_score") != {
                "before": 77,
                "after": 77,
                "removed": ["?g_51ebd4@@3PAUs_sound_globals@@A"],
                "added": ["?function_1264c0@@YAXK@Z"],
                "interpretation":
                    "The coherent map publication/clear lifetime is "
                    "score-neutral: one opaque mutable pointer becomes the "
                    "exact recovered old-map disposal boundary after compact "
                    "mix and DSP reset helpers are owned."} or \
            "function_1264c0" not in map_lifetime.get("execution_gate", "") or \
            not map_lifetime.get("semantics") or \
            not map_lifetime.get("prohibited_shortcut"):
        raise RuntimeError("sound map-globals lifetime changed")
    map_source = (args.root / "src/sound_map_lifetime_native.cpp").read_text()
    for fragment in [
            "g_51ebd4 = reinterpret_cast<s_sound_globals *>",
            "g_4e6380->master_fade = 1.0f",
            "g_4e6380->listeners[listener_index].active = false",
            "g_47f0d0.packet_size = g_4e6948->state == 3",
            "playback->fade_start_time = 300",
            "function_125d60();",
            "function_1264c0(0);",
            "record_pool_release_all",
            "g_51ebd4 = NULL"]:
        if fragment not in map_source:
            raise RuntimeError(
                f"sound map-globals lifetime lost {fragment}")
    reset_source = (
        args.root / "src/sound_map_reset_helpers_native.cpp").read_text()
    for fragment in [
            "IDirectSound_SetEffectData",
            "g_510c90->previous_changed = 0",
            "memcpy(driver->effect_settings",
            "function_21e3b0(driver->effects)",
            "sound_system->master_fade_times[2]"]:
        if fragment not in reset_source:
            raise RuntimeError(f"sound map reset helper lost {fragment}")

    old_map = manifest.get("old_map_sound_disposal_closure", {})
    if old_map.get("root") != "?function_1264c0@@YAXK@Z" or \
            old_map.get("effect_disposal") != \
            "?function_21d4d0@@YAXXZ" or \
            old_map.get("looping_sound_disposal") != [
                "?function_21a1e0@@YAXXZ",
                "?function_21b070@@YAXJ@Z"] or \
            old_map.get("production_score") != {
                "before": 77,
                "after": 76,
                "removed": ["?function_1264c0@@YAXK@Z"],
                "added": [],
                "interpretation":
                    "Owning exact old-map disposal and its effect/looping "
                    "record cleanup removes the sole recovered lifecycle "
                    "boundary without exposing a new production import."} or \
            len(old_map.get("required_order", [])) != 6 or \
            not old_map.get("semantics") or \
            not old_map.get("execution_gate") or \
            not old_map.get("prohibited_shortcut"):
        raise RuntimeError("old-map sound disposal closure changed")
    old_map_source = (
        args.root / "src/sound_old_map_disposal_native.cpp").read_text()
    for fragment in [
            "void __stdcall function_1264c0(dword flags)",
            "function_21d4d0();",
            "function_127320(sound_index, 10);",
            "sound->retained_marker = NONE",
            "function_125d60();",
            "function_21f290();",
            "g_4ed288->value20 = 0",
            "function_21a1e0();",
            "looping_sound_controller_release(effect->controller_index)",
            "record_pool_release(looping_sounds, looping_sound_index)"]:
        if fragment not in old_map_source:
            raise RuntimeError(f"old-map sound disposal lost {fragment}")

    pcm = manifest.get("pcm_codec_object_lifetime", {})
    pcm_slots = [
        "?attach@c_sound_stream_codec@@UAAXPAUs_sound_stream@@@Z",
        "?start@c_sound_stream_codec@@UAAXPAUs_sound_stream@@@Z",
        "?mark_cache_usage@c_pcm_codec@@UAAXPAUs_sound_stream@@PAUs_sound_cache_usage@@@Z",
        "?get_packet@c_pcm_codec@@UAA_NPAUs_sound_stream@@PAUXMEDIAPACKET@@@Z",
        "?packet_submitted@c_pcm_codec@@UAAXPAUs_sound_stream@@PAUXMEDIAPACKET@@@Z",
        "?packet_failed@c_pcm_codec@@UAAXPAUs_sound_stream@@PAUXMEDIAPACKET@@@Z",
        "?chunk_finished@c_pcm_codec@@UAAXPAUs_sound_stream@@PAUs_sound_chunk@@J@Z",
        "?stop@c_sound_stream_codec@@UAAXXZ",
        "?can_submit@c_pcm_codec@@UAA_NPAUs_sound_stream@@_N@Z"]
    if pcm.get("object") != "?g_47f0f0@@3Us_47f0f0@@A" or \
            pcm.get("vtable") != "h2_pcm_codec_vtable" or \
            pcm.get("retail_vtable") != "0x45715c" or \
            pcm.get("slots") != pcm_slots or \
            pcm.get("cache_data_provider") != \
            "?sound_cache_chunk_get_data@@YAPAEPAUs_sound_chunk@@@Z" or \
            pcm.get("production_score_before") != 76 or \
            pcm.get("production_score_after") != 75 or \
            not pcm.get("semantics") or not pcm.get("execution_gate"):
        raise RuntimeError("PCM codec object lifetime changed")
    pcm_object_source = (
        args.root / "src/sound_pcm_codec_object_native.cpp").read_text()
    for fragment in [
            "s_pcm_codec_method const h2_pcm_codec_vtable[9]",
            "s_47f0f0 g_47f0f0 = { h2_pcm_codec_vtable }",
            "static_assert(sizeof(s_47f0f0) == 0x04",
            "static_assert(sizeof(h2_pcm_codec_vtable) == 0x24",
            "?attach@c_sound_stream_codec@@UAAXPAUs_sound_stream@@@Z",
            "?start@c_sound_stream_codec@@UAAXPAUs_sound_stream@@@Z",
            "?stop@c_sound_stream_codec@@UAAXXZ"]:
        if fragment not in pcm_object_source:
            raise RuntimeError(f"PCM codec object lost {fragment}")
    pcm_method_source = (
        args.root / "src/sound_pcm_codec_control_native.cpp").read_text()
    for fragment in [
            "usage->counts[cache_index]++",
            "byte *data = sound_cache_chunk_get_data(chunk)",
            "stream->started_count++",
            "entry->reference_count--",
            "sound_stream_release_chunk(stream, chunk)",
            "stream->chunk_count > stream->started_count"]:
        if fragment not in pcm_method_source:
            raise RuntimeError(f"PCM codec method lost {fragment}")

    wma = manifest.get("wma_codec_object_lifetime", {})
    wma_slots = [
        "?attach@c_wma_codec@@UAAXPAUs_sound_stream@@@Z",
        "?start@c_wma_codec@@UAAXPAUs_sound_stream@@@Z",
        "?mark_cache_usage@c_sound_stream_codec@@UAAXPAUs_sound_stream@@PAUs_sound_cache_usage@@@Z",
        "?get_packet@c_wma_codec@@UAA_NPAUs_sound_stream@@PAUXMEDIAPACKET@@@Z",
        "?packet_submitted@c_wma_codec@@UAAXPAUs_sound_stream@@PAUXMEDIAPACKET@@@Z",
        "?packet_failed@c_sound_stream_codec@@UAAXPAUs_sound_stream@@PAUXMEDIAPACKET@@@Z",
        "?chunk_finished@c_sound_stream_codec@@UAAXPAUs_sound_stream@@PAUs_sound_chunk@@J@Z",
        "?stop@c_wma_codec@@UAAXXZ",
        "?can_submit@c_wma_codec@@UAA_NPAUs_sound_stream@@_N@Z"]
    if wma.get("object") != "?g_47f0d0@@3Us_47f0d0@@A" or \
            wma.get("size") != 0x20 or \
            wma.get("vtable") != "h2_wma_codec_vtable" or \
            wma.get("retail_vtable") != "0x45711c" or \
            wma.get("packet_size_offset") != "0x10" or \
            wma.get("slots") != wma_slots or \
            wma.get("stage_initializer") != "?function_21ebc0@@YAXJ@Z" or \
            wma.get("stage_disposer") != "?function_21ebe0@@YAXXZ" or \
            wma.get("storage_initializer") != \
            "?function_2ae1d0@@YAXPAUs_47f0d0@@@Z" or \
            wma.get("decoder_backend") != "WmaCreateInMemoryDecoder" or \
            wma.get("production_score_before") != 75 or \
            wma.get("production_score_after") != 75 or \
            not wma.get("semantics") or not wma.get("execution_gate"):
        raise RuntimeError("WMA codec object lifetime changed")
    wma_object_source = (
        args.root / "src/sound_wma_codec_object_native.cpp").read_text()
    for fragment in [
            "s_wma_codec_method const h2_wma_codec_vtable[9]",
            "static_assert(sizeof(s_47f0d0) == 0x20",
            "offsetof(s_47f0d0, packet_size) == 0x10",
            "s_47f0d0 g_47f0d0 =",
            "if (stage == 1)",
            "function_2ae1d0(&g_47f0d0)",
            "g_47f0d0.buffer = NULL"]:
        if fragment not in wma_object_source:
            raise RuntimeError(f"WMA codec object lost {fragment}")
    for source_name, fragments in {
            "src/sound_wma_codec_start_native.cpp": [
                "WmaCreateInMemoryDecoder",
                "decoder->vtable[4]",
                "m_packet_status[index] != k_packet_status_pending"],
            "src/sound_wma_codec_control_native.cpp": [
                "m_stream = stream",
                "stream->offset = 0",
                "return m_decoder && !submitted",
                "sound_cache_chunk_get_data(chunk)"],
            "src/sound_wma_codec_initialize_native.cpp": [
                "physical_memory_malloc_fixed(0x20000, PAGE_READWRITE)",
                "memset(codec->packet_status, 0"]}.items():
        source = (args.root / source_name).read_text()
        for fragment in fragments:
            if fragment not in source:
                raise RuntimeError(f"WMA codec method lost {fragment}")

    effects = manifest.get("sound_effects_object_lifetime", {})
    effects_slots = [
        "?initialize@c_sound_effects@@UAAXPAUs_sound_effect_settings@@PAUs_sound_effect_buffers@@_N@Z",
        "?v1@c_sound_effects@@UAAXXZ",
        "?set_i3dl2@c_sound_effects@@UAAXPAUs_sound_i3dl2_parameters@@J@Z",
        "?add_effect_sends@c_sound_effects@@UAAXPAUs_sound_send_parameters@@_NJPAUs_mixbin_settings@@@Z",
        "?add_send@c_sound_effects@@UAAXJJMPAUs_mixbin_settings@@@Z",
        "?add_source_mixbins@c_sound_effects@@UAAXPAUs_sound_mixbin_source@@_NJPAUs_mixbin_settings@@@Z",
        "?add_sends@c_sound_effects@@UAAXPAUs_sound_send_parameters@@JJPAUs_mixbin_settings@@@Z"]
    if effects.get("object") != "?g_47f088@@3Vc_sound_effects@@A" or \
            effects.get("size") != 0x48 or \
            effects.get("vtable") != "h2_sound_effects_vtable" or \
            effects.get("retail_vtable") != "0x457140" or \
            effects.get("slots") != effects_slots or \
            effects.get("immutable_routing_constants") != {
                "?g_44f70c@@3MB": "0.0", "?g_44f710@@3MB": "-64.0"} or \
            effects.get("source") != \
            "src/sound_effects_object_native.cpp" or \
            effects.get("production_score_before") != 75 or \
            effects.get("production_score_after") != 76 or \
            not effects.get("semantics") or not effects.get("execution_gate"):
        raise RuntimeError("sound effects object lifetime changed")
    effects_source = (
        args.root / "src/sound_effects_object_native.cpp").read_text()
    for fragment in [
            "static_assert(sizeof(c_sound_effects) == 0x48",
            "offsetof(c_sound_effects, settings) == 0x04",
            "offsetof(c_sound_effects, buffers) == 0x44",
            "extern real const g_44f70c = 0.0f",
            "extern real const g_44f710 = -64.0f",
            "s_sound_effects_method const h2_sound_effects_vtable[7]",
            "c_sound_effects g_47f088 =",
            "IDirectSoundBuffer_SetI3DL2Source",
            "IDirectSoundBuffer_SetFilter",
            "IDirectSoundBuffer_SetMixBinVolumes",
            "IDirectSoundBuffer_SetVolume",
            "sound_filter_frequency_coefficient",
            "sound_filter_gain_coefficient",
            "first.value = self->settings.gain + first.value",
            "second.value = self->settings.gain + second.value"]:
        if fragment not in effects_source:
            raise RuntimeError(f"sound effects object lost {fragment}")

    allocator = manifest.get("sound_data_allocator_object_lifetime", {})
    if allocator.get("source") != \
            "src/sound_data_allocator_object_native.cpp" or \
            allocator.get("retail_object_address") != "0x479864" or \
            allocator.get("retail_table_address") != "0x453300" or \
            allocator.get("object_size") != "0x4" or \
            allocator.get("table_slots") != [
                "allocate_returns_null", "deallocate_noop"] or \
            allocator.get("constructor_free") is not True or \
            allocator.get("owns_static_sound_arena") is not False or \
            not allocator.get("semantics"):
        raise RuntimeError("sound data allocator object lifetime changed")
    allocator_source = (
        args.root / "src/sound_data_allocator_object_native.cpp").read_text()
    for fragment in [
            "h2_sound_data_allocator_method const h2_sound_data_allocator_vtable[2]",
            "static_assert(sizeof(s_sound_data_allocator_object) == 0x04",
            "s_sound_data_allocator_object h2_sound_data_allocator_object =",
            "c_data_allocator *g_46875c =",
            "return 0;",
            "(void)block;"]:
        if fragment not in allocator_source:
            raise RuntimeError(f"sound data allocator object lost {fragment}")

    arena = manifest.get("sound_static_arena_lifetime", {})
    audit = arena.get("retail_reference_audit", {})
    if arena.get("source") != \
            "src/sound_static_arena_lifetime_native.cpp" or \
            arena.get("providers") != [
                "?g_510800_pool_base@@3KA",
                "?g_510804_pool_size@@3JA",
                "?g_510808_pool_checksum@@3KA",
                "?sound_static_arena_initialize@@YA_NXZ",
                "?sound_static_arena_dispose@@YAXXZ"] or \
            arena.get("retail_startup_function") != "0x00012000" or \
            arena.get("retail_teardown_function") != "0x00012080" or \
            arena.get("capacity") != "0x23000" or \
            arena.get("allocation_type") != "0x1000" or \
            arena.get("protection") != "0x04" or \
            arena.get("release_type") != "0x8000" or \
            audit.get("base_encoded_text_references") != 11 or \
            audit.get("size_encoded_text_references") != 18 or \
            audit.get("checksum_encoded_text_references") != 9 or \
            audit.get("final_size_validation") is not False or \
            audit.get("final_checksum_validation") is not False or \
            not audit.get("publication") or \
            not audit.get("allocation_accounting") or \
            not audit.get("teardown") or \
            not arena.get("semantics") or \
            not arena.get("execution_gate"):
        raise RuntimeError("sound static arena lifetime changed")
    arena_source = (args.root / arena["source"]).read_text()
    for fragment in [
            "dword g_510800_pool_base;",
            "long g_510804_pool_size;",
            "dword g_510808_pool_checksum;",
            "k_sound_static_arena_size = 0x23000",
            "VirtualAlloc(NULL, k_sound_static_arena_size",
            "MEM_COMMIT, PAGE_READWRITE",
            "g_510804_pool_size = 0;",
            "g_510808_pool_checksum = 0;",
            "return arena != NULL;",
            "if (g_510800_pool_base)",
            "VirtualFree((void *)(uintptr_t)g_510800_pool_base, 0, MEM_RELEASE)",
            "static_assert(sizeof(void *) == 4"]:
        if fragment not in arena_source:
            raise RuntimeError(f"sound static arena lifetime lost {fragment}")
    xdk_header = (args.root / "port/include/xdk/xtl.h").read_text()
    for fragment in [
            "#define MEM_COMMIT 0x1000",
            "#define MEM_RELEASE 0x8000",
            "void* WINAPI VirtualAlloc(void* address, DWORD size, DWORD allocation_type,",
            "BOOL WINAPI VirtualFree(void* address, DWORD size, DWORD free_type);"]:
        if fragment not in xdk_header:
            raise RuntimeError(f"sound static arena platform ABI lost {fragment}")
    virtual_bridge = manifest.get("native_virtual_memory_bridge", {})
    if virtual_bridge.get("host_abi_version") != 15 or \
            virtual_bridge.get("guest_wrappers") != [
                {"guest_symbol": "VirtualAlloc",
                 "host_import": "host_h2_virtual_alloc"},
                {"guest_symbol": "VirtualFree",
                 "host_import": "host_h2_virtual_free"}] or \
            virtual_bridge.get("guest_region") != {
                "base": "0x30000000", "maximum_size": "0x01000000"} or \
            virtual_bridge.get("sound_arena_request") != {
                "size": "0x23000", "allocation_type": "MEM_COMMIT",
                "protection": "PAGE_READWRITE", "free_size": 0,
                "free_type": "MEM_RELEASE"} or \
            virtual_bridge.get("production_score_before") != 72 or \
            virtual_bridge.get("production_score_after") != 70 or \
            not virtual_bridge.get("semantics") or \
            not virtual_bridge.get("execution_gate"):
        raise RuntimeError("native virtual-memory bridge changed")

    voice = manifest.get("voice_runtime_lifetime_frontier", {})
    voice_objects = voice.get("process_objects", [])
    if voice.get("source_files") != [
            "src/network_voice.cpp", "src/unknown_053310.cpp",
            "src/unknown_453c00.cpp", "include/network_voice.h",
            "include/globals.h"] or len(voice_objects) != 7 or \
            [(item.get("retail_address"), item.get("size"))
             for item in voice_objects] != [
                ("0x00476fbc", "0x08"), ("0x00476fc8", "0x90"),
                ("0x004c9878", "0x144"), ("0x005259b8", "0x44"),
                ("0x00525a00", "0x1704"), ("0x00527104", "0x218"),
                ("0x0052731c", "0x10")]:
        raise RuntimeError("voice runtime object map changed")
    physical_source, xhv_object = voice_objects[:2]
    if physical_source.get("table_address") != "0x004508fc" or \
            physical_source.get("initial_field04") != 2 or \
            physical_source.get("table_slots") != [
                "0x000531b0 allocate from physical-memory top",
                "0x00072c70 inert release"] or \
            xhv_object.get("table_address") != "0x0045095c" or \
            xhv_object.get("table_slots") != [
                "0x000555f0 LocalChatDataReady",
                "0x000555b0 CommunicatorStatusUpdate",
                "0x00055630 SpeechRecognized",
                "0x00055620 MicrophoneRawDataReady",
                "0x00055640 VoiceMailDataReady",
                "0x00055680 VoiceMailStopped"]:
        raise RuntimeError("voice runtime object table map changed")
    voice_global_init = voice.get("global_initializer", {})
    voice_stage = voice.get("registered_stage_lifetime", {})
    voice_menu = voice.get("menu_pool_lifetime", {})
    voice_engine = voice.get("engine_lifetime", {})
    voice_callback = voice.get("callback_object", {})
    if voice.get("assembled") is not True or \
            len(voice.get("providers", [])) != 31 or \
            voice.get("production_score_before") != 70 or \
            voice.get("production_score_after") != 76 or \
            voice.get("remaining_code_gate") != \
            "?function_53a20@@YGXKPAXK@Z" or \
            not voice.get("execution_gate") or \
            voice_global_init.get("function") != "?function_53210@@YAXXZ" or \
            voice_global_init.get("retail_address") != "0x00053210" or \
            voice_global_init.get("size") != 120 or \
            voice_global_init.get("owner_function") != "0x0008d9f0" or \
            voice_global_init.get("owner_size") != 892 or \
            voice_global_init.get("owner_callsite") != "0x0008dd36" or \
            len(voice_global_init.get("dependencies", [])) != 3 or \
            voice_stage.get("table") != "?g_453c00@@3PAUs_callback_pair@@A" or \
            voice_stage.get("table_slot") != 3 or \
            voice_stage.get("initialize") != "?function_53310@@YGXJ@Z" or \
            voice_stage.get("initialize_size") != 206 or \
            voice_stage.get("dispose") != "?function_533e0@@YAXXZ" or \
            voice_stage.get("dispose_size") != 62 or \
            voice_stage.get("stage_policy") != {
                "0_or_1": "type 2, mode 2, 0x78000-byte primary pool",
                "2_or_3": "type 1, mode 1, deferred menu pool",
                "above_3": "type 0, mode 0, no pool"} or \
            voice_menu.get("initialize_address") != "0x00053420" or \
            voice_menu.get("initialize_size") != 118 or \
            voice_menu.get("dispose_address") != "0x000534a0" or \
            voice_menu.get("dispose_size") != 106 or \
            voice_menu.get("bytes") != "0x61800" or \
            voice_engine.get("start_address") != "0x00053510" or \
            voice_engine.get("start_size") != 58 or \
            voice_engine.get("stop_address") != "0x00053550" or \
            voice_engine.get("stop_size") != 38 or \
            voice_engine.get("initialize_address") != "0x00055080" or \
            voice_engine.get("initialize_size") != 48 or \
            voice_engine.get("create_address") != "0x00055810" or \
            voice_engine.get("create_size") != 270 or \
            voice_engine.get("dispose_address") != "0x000550b0" or \
            voice_engine.get("dispose_size") != 128 or \
            voice_engine.get("startup_helpers") != [
                {"symbol": "?voice_xhv_reset_masks@@YAXPAVc_voice_xhv@@@Z",
                 "address": "0x000556a0", "size": 116},
                {"symbol": "?voice_xhv_reset_port_modes@@YAXPAVc_voice_xhv@@@Z",
                 "address": "0x00055720", "size": 88},
                {"symbol": "?voice_xhv_get_runtime_parameters@@YA_NPAUXHV_RUNTIME_PARAMS@@PAVc_voice_xhv@@@Z",
                 "address": "0x00055780", "size": 132}] or \
            voice_engine.get("backend_operations") != [
                "XHVEngineCreate", "EnableProcessingMode",
                "SetCallbackInterface", "SetMaxPlaybackStreamsCount",
                "RegisterLocalTalker", "SetProcessingMode",
                "GetLocalTalkerStatus", "GetRemoteTalkers",
                "UnregisterLocalTalker", "UnregisterRemoteTalker", "Release"] or \
            voice_callback.get("slots") != 6 or \
            voice_callback.get("slot_sizes") != [41, 49, 8, 8, 51, 21] or \
            not voice_global_init.get("semantics") or \
            not voice_stage.get("semantics") or \
            not voice_menu.get("semantics") or \
            not voice_engine.get("semantics") or \
            not voice_callback.get("semantics") or \
            not voice.get("production_inadequacy") or \
            not voice.get("prohibited_shortcut"):
        raise RuntimeError("voice runtime lifetime frontier changed")
    network_voice_source = (args.root / "src/network_voice.cpp").read_text()
    voice_stage_source = (args.root / "src/unknown_053310.cpp").read_text()
    voice_table_source = (args.root / "src/unknown_453c00.cpp").read_text()
    for fragment in [
            "c_voice_xhv g_476fc8;", "s_voice_globals g_4c9878;",
            "XHV_PROCESSING_MODE g_52731c[4];",
            "s_voice_player_values g_5259b8;", "s_voice_channels g_525a00;",
            "s_voice_routing g_527104;", "void function_53210(void)",
            "voice_channels_initialize(&g_525a00)",
            "voice_player_settings_initialize(&g_527104.settings)",
            "g_4c9878.initialized = true;", "bool voice_xhv_create(",
            "XHVEngineCreate(&parameters, engine)",
            "(*engine)->SetCallbackInterface(xhv)",
            "(*engine)->RegisterLocalTalker(port)",
            "void voice_start_engine(void)", "void voice_stop_engine(void)"]:
        if fragment not in network_voice_source:
            raise RuntimeError(f"voice runtime recovered source lost {fragment}")
    initializer = network_voice_source[
        network_voice_source.index("// @retail 0x53210"):
        network_voice_source.index("/* the header of a voice packet")]
    if initializer.index("memset(&g_4c9878") > \
            initializer.index("g_4c9878.initialized = true"):
        raise RuntimeError("voice globals initialized publication moved")
    for fragment in [
            "c_physical_memory_source g_476fbc;",
            "void __stdcall function_53310(long stage)",
            "size = 0x78000;", "mode = 2;",
            "source->allocate(size + 0x50)",
            "function_18e250(loop, size, \"voice pool\", source)",
            "g_476fc8.initialized = voice_xhv_create(&g_476fc8)",
            "void function_533e0(void)",
            "voice_xhv_dispose(&g_476fc8)",
            "function_18e230(g_4c9878.pool)",
            "g_4c9878.pool = 0;"]:
        if fragment not in voice_stage_source:
            raise RuntimeError(f"voice stage lifetime lost {fragment}")
    if "{ function_533e0, function_53310 }" not in voice_table_source:
        raise RuntimeError("voice registered stage pair changed")
    xhv_native = (args.root / "src/voice_xhv_lifetime_native.cpp").read_text()
    storage_native = (args.root /
        "src/voice_runtime_storage_native.cpp").read_text()
    stage_native = (args.root /
        "src/voice_stage_lifetime_native.cpp").read_text()
    for fragment in [
            "sizeof(c_voice_xhv) == 0x90",
            "h2_voice_xhv_callback_vtable[6]",
            "function_53a20(port, data, size)",
            "return static_cast<long>(0x80004001u)",
            "XHVEngineCreate(&parameters, &xhv->engine)",
            "xhv_set_callback_interface(xhv->engine, xhv)",
            "xhv_register_local_talker(xhv->engine, port)",
            "xhv->initialized = voice_xhv_create(xhv)",
            "void voice_start_engine(void)",
            "voice_xhv_dispose(&g_476fc8)"]:
        if fragment not in xhv_native:
            raise RuntimeError(f"native voice XHV lifetime lost {fragment}")
    for fragment in [
            "sizeof(s_voice_globals) == 0x144",
            "sizeof(s_voice_channels) == 0x1704",
            "sizeof(s_voice_routing) == 0x218",
            "c_memory_source *g_46dd54",
            "s_voice_globals g_4c9878",
            "voice_channels_initialize(&g_525a00)",
            "voice_player_settings_initialize(&g_527104.settings)",
            "g_4c9878.initialized = true"]:
        if fragment not in storage_native:
            raise RuntimeError(f"native voice storage lifetime lost {fragment}")
    for fragment in [
            "sizeof(c_physical_memory_source) == 0x08",
            "h2_physical_memory_source_vtable",
            "long const aligned_size = (size + 0xfff) & 0xfffff000",
            "XPhysicalProtect(result, aligned_size, 0x04)",
            "void function_53310(long stage)",
            "size = 0x78000",
            "function_18e250(loop, size, g_voice_pool_name, source)",
            "g_4c9878.pool->field3e = 1",
            "void function_533e0(void)",
            "voice_initialize_menu_pool",
            "voice_dispose_menu_pool",
            "g_4c9878.use_pool2 = false"]:
        if fragment not in stage_native:
            raise RuntimeError(f"native voice stage lifetime lost {fragment}")

    math_bridge = manifest.get("native_sound_math_bridge", {})
    if math_bridge.get("host_abi_version") != 15 or \
            math_bridge.get("routes") != [
                {"guest_symbol": "acos", "host_import": "host_h2_math_acos"},
                {"guest_symbol": "exp", "host_import": "host_h2_math_exp"},
                {"guest_symbol": "expf", "host_import": "host_h2_math_expf"},
                {"guest_symbol": "fmod", "host_import": "host_h2_math_fmod"},
                {"guest_symbol": "log", "host_import": "host_h2_math_log"},
                {"guest_symbol": "log10", "host_import": "host_h2_math_log10"},
                {"guest_symbol": "logf", "host_import": "host_h2_math_logf"},
                {"guest_symbol": "pow", "host_import": "host_h2_math_pow"},
                {"guest_symbol": "sinf", "host_import": "host_h2_math_sinf"}] or \
            not math_bridge.get("semantics") or \
            "h2_xbox_rsqrtss" not in math_bridge.get("execution_gate", ""):
        raise RuntimeError("sound-idle native math bridge changed")

    publications = manifest.get("runtime_publication_storage_closure", {})
    if publications.get("providers") != [
            {"symbol": "?g_4e6374@@3JA",
             "publication": "sound system reference count",
             "owners": "function_1253d0 / function_125600"},
            {"symbol": "?g_4e6378@@3PAUs_sound_voice@@A",
             "publication": "73-entry static runtime voice publication",
             "owners": "function_1253d0 / function_125600"},
            {"symbol": "?g_502104@@3PAUs_record_pool@@A",
             "publication": "512-entry sound cache record-pool publication",
             "owners": "sound_cache_initialize / function_2186b0"},
            {"symbol": "?g_502108@@3KA",
             "publication": "3 MiB sound cache upload-base publication",
             "owners": "sound_cache_initialize / function_2186b0"},
            {"symbol": "?g_50210c@@3PAUs_sound_cache_allocator@@A",
             "publication": "sound physical-cache allocator publication",
             "owners": "sound_cache_initialize / function_2186b0"},
            {"symbol": "?g_510c90@@3PAUs_voice_effects@@A",
             "publication": "15-effect DSP image state publication",
             "owners": "function_191300 / driver teardown ownership"},
            {"symbol": "?g_51ebe0@@3PAXA",
             "publication": "16-entry sound-effects record-pool publication",
             "owners": "sound_effects_initialize / function_125600"},
            {"symbol": "?g_51ebe4@@3PAUs_bink_sound_settings@@A",
             "publication": "0x2ad8 driver-state publication",
             "owners": "sound_driver_device_startup / function_21eae0"}] or \
            publications.get("storage_bytes") != 32 or \
            not publications.get("initial_state") or \
            not publications.get("semantics") or \
            not publications.get("execution_gate"):
        raise RuntimeError("sound-idle runtime publication storage changed")

    expected_definitions = set(manifest.get("definitions", []))
    expected_imports = set(manifest.get("imports", []))
    if len(expected_definitions) != 272 or len(expected_imports) != 139:
        raise RuntimeError("sound-idle provider cluster coverage ratchet changed")
    if playback_chunk_abi["rejected_widened_symbol"] in expected_imports:
        raise RuntimeError("widened playback chunk ABI returned to the cluster")
    if "?function_163ba0@@YAXPAKPAXJ@Z" in expected_imports:
        raise RuntimeError("mutable checksum ABI returned to the cluster")
    if not set(provider_symbols).issubset(expected_definitions):
        raise RuntimeError("sound-idle direct provider definition is missing")
    if manifest.get("production_linked") is not False:
        raise RuntimeError("sound-idle direct cluster unexpectedly marked linked")
    if manifest.get("execution_enabled") is not False:
        raise RuntimeError("sound-idle direct cluster unexpectedly marked executable")
    if not manifest.get("blocker"):
        raise RuntimeError("sound-idle direct cluster blocker is missing")

    actual_definitions = symbols(
        args.llvm_nm, args.cluster_object, "--defined-only")
    if actual_definitions != expected_definitions:
        raise RuntimeError(
            "sound-idle direct definitions changed: " +
            repr(sorted(actual_definitions)))
    actual_imports = symbols(
        args.llvm_nm, args.cluster_object, "--undefined-only")
    if actual_imports != expected_imports:
        raise RuntimeError(
            "sound-idle direct imports changed: " +
            repr(sorted(actual_imports)))
    typed_symbols = {
        line.split()[0].removeprefix("_"): line.split()[1]
        for line in subprocess.check_output([
            args.llvm_nm, "--defined-only", "--extern-only", "--format=posix",
            args.cluster_object], text=True).splitlines()
        if line
    }
    for symbol in arena["providers"][:3]:
        if typed_symbols.get(symbol) != "B":
            raise RuntimeError(
                f"sound static arena publication is not BSS: {symbol}")

    relocations = subprocess.check_output(
        [args.llvm_nm.with_name("llvm-readelf"), "-r", args.cluster_object],
        text=True)
    pcm_relocations = [
        line.split()[-3]
        for line in relocations.splitlines()
        if "R_AARCH64_ABS32" in line and
        any(symbol in line for symbol in
            ["h2_pcm_codec_vtable", *pcm_slots])]
    if pcm_relocations != ["h2_pcm_codec_vtable", *pcm_slots]:
        raise RuntimeError(
            "PCM codec object/vtable relocation order changed: " +
            repr(pcm_relocations))
    wma_relocations = [
        line.split()[-3]
        for line in relocations.splitlines()
        if "R_AARCH64_ABS32" in line and
        any(symbol in line for symbol in
            ["h2_wma_codec_vtable", *wma_slots])]
    if wma_relocations != ["h2_wma_codec_vtable", *wma_slots]:
        raise RuntimeError(
            "WMA codec object/vtable relocation order changed: " +
            repr(wma_relocations))
    effects_relocations = [
        line.split()[-3]
        for line in relocations.splitlines()
        if "R_AARCH64_ABS32" in line and
        any(symbol in line for symbol in
            ["h2_sound_effects_vtable", *effects_slots])]
    if effects_relocations != ["h2_sound_effects_vtable", *effects_slots]:
        raise RuntimeError(
            "sound effects object/vtable relocation order changed: " +
            repr(effects_relocations))
    allocator_relocations = [
        line.split()[-3]
        for line in relocations.splitlines()
        if "R_AARCH64_ABS32" in line and
        any(symbol in line for symbol in [
            "h2_sound_data_allocator_vtable",
            "h2_sound_data_allocator_allocate",
            "h2_sound_data_allocator_deallocate",
            "h2_sound_data_allocator_object"])]
    if allocator_relocations != [
            "h2_sound_data_allocator_object",
            "h2_sound_data_allocator_vtable",
            "h2_sound_data_allocator_allocate",
            "h2_sound_data_allocator_deallocate"]:
        raise RuntimeError(
            "sound data allocator object/vtable relocation order changed: " +
            repr(allocator_relocations))
    voice_relocations = [
        line.split()[-3]
        for line in relocations.splitlines()
        if "R_AARCH64_ABS32" in line and
        any(symbol in line for symbol in [
            "LocalChatDataReady@c_voice_xhv",
            "CommunicatorStatusUpdate@c_voice_xhv",
            "SpeechRecognized@c_voice_xhv",
            "MicrophoneRawDataReady@c_voice_xhv",
            "VoiceMailDataReady@c_voice_xhv",
            "VoiceMailStopped@c_voice_xhv",
            "h2_voice_xhv_callback_vtable"])]
    if voice_relocations != [
            "?LocalChatDataReady@c_voice_xhv@@UAGJKKPAX@Z",
            "?CommunicatorStatusUpdate@c_voice_xhv@@UAGJKW4XHV_VOICE_COMMUNICATOR_STATUS@@@Z",
            "?SpeechRecognized@c_voice_xhv@@UAGJKPAUXHV_SR_ITEM@@K@Z",
            "?MicrophoneRawDataReady@c_voice_xhv@@UAGJKKPAXPAH@Z",
            "?VoiceMailDataReady@c_voice_xhv@@UAGJKKK@Z",
            "?VoiceMailStopped@c_voice_xhv@@UAGJK@Z",
            "?h2_voice_xhv_callback_vtable@@3QBQ6AXXZB"]:
        raise RuntimeError(
            "voice callback object/vtable relocation order changed: " +
            repr(voice_relocations))
    physical_source_relocations = [
        line.split()[-3]
        for line in relocations.splitlines()
        if "R_AARCH64_ABS32" in line and
        any(symbol in line for symbol in [
            "allocate@c_physical_memory_source",
            "release@c_physical_memory_source",
            "h2_physical_memory_source_vtable"])]
    if physical_source_relocations != [
            "?allocate@c_physical_memory_source@@UAEPAXJ@Z",
            "?release@c_physical_memory_source@@UAEXPAX@Z",
            "?h2_physical_memory_source_vtable@@3Us_table@c_memory_source@@B"]:
        raise RuntimeError(
            "voice physical source object/vtable relocation order changed: " +
            repr(physical_source_relocations))
    if any("GLOBAL__sub_I" in symbol or
           "cxx_global_var_init" in symbol
           for symbol in actual_definitions):
        raise RuntimeError("voice lifetime gained a host constructor")

    production_definitions = symbols(
        args.llvm_nm, args.production_object, "--defined-only")
    linked = sorted(expected_definitions & production_definitions)
    if linked != ["?voice_start_engine@@YAXXZ"]:
        raise RuntimeError(
            "dormant sound-idle direct providers entered production: " +
            repr(linked))

    expected_production_imports = set(
        manifest.get("production_score_imports", []))
    if len(expected_production_imports) != 76:
        raise RuntimeError("sound-idle production score ratchet changed")
    runtime_frontier = manifest.get("remaining_runtime_frontier", {})
    categories = [
        runtime_frontier.get("mutable_lifetimes", []),
        runtime_frontier.get("backend_operations", []),
        runtime_frontier.get("recovered_lifecycle_operations", []),
        runtime_frontier.get("immutable_diagnostics", []),
        runtime_frontier.get("numeric_boundaries", [])]
    if [len(category) for category in categories] != [4, 70, 1, 0, 1]:
        raise RuntimeError("sound-idle runtime frontier category changed")
    flattened = [symbol for category in categories for symbol in category]
    if len(flattened) != len(set(flattened)) or \
            set(flattened) != expected_production_imports:
        raise RuntimeError(
            "sound-idle runtime frontier no longer partitions production")
    crc = runtime_frontier.get("crc_storage_lifetime", {})
    if crc.get("updater") != "?function_163ba0@@YAXPAKPBXJ@Z" or \
            crc.get("table") != "?g_sound_crc_table@@3PAKA" or \
            crc.get("built_flag") != \
            "?g_sound_crc_table_built@@3_NA" or \
            crc.get("table_entries") != 256 or \
            crc.get("polynomial") != "0xedb88320" or \
            ".crctable" not in crc.get("semantics", "") or \
            ".crctable" not in crc.get("prohibited_shortcut", ""):
        raise RuntimeError("sound CRC storage lifetime changed")
    controller = runtime_frontier.get("controller_storage_lifetime", {})
    if controller.get("storage") != "?g_51ebdc@@3PAXA" or \
            controller.get("record_pool") != "?g_51ebd8@@3PAXA" or \
            controller.get("initializer") != \
            "?looping_sound_controllers_initialize@@YA_NXZ" or \
            controller.get("startup_owner") != \
            "?function_1253d0@@YAXXZ" or \
            controller.get("teardown_owner") != \
            "?function_125600@@YAXXZ" or \
            controller.get("dependent_providers") != [
                "?function_11cc20@@YAPAUs_record_pool@@JPBDJ@Z",
                "?function_2186b0@@YAXXZ",
                "?function_21a0c0@@YA_NXZ",
                "?function_21e4b0@@YA_NPBUs_sound_driver_counts@@@Z",
                "?function_21eae0@@YAXXZ",
                "?sound_cache_initialize@@YAXXZ",
                "?sound_effects_initialize@@YA_NXZ",
                "?sound_voices_initialize@@YA_NPBUs_sound_voice_settings@@@Z"
            ] or controller.get("capacity") != 64 or \
            controller.get("alignment") != 4 or \
            len(controller.get("required_order", [])) != 7 or \
            not controller.get("semantics") or \
            not controller.get("prohibited_shortcut"):
        raise RuntimeError("sound-idle controller storage lifetime changed")
    crc_source = (args.root / crc["source"]).read_text()
    for fragment in [
            "dword g_sound_crc_table[k_crc_table_size]",
            "bool g_sound_crc_table_built",
            "__attribute__((optnone))",
            "k_crc32_polynomial = 0xedb88320",
            "if (!g_sound_crc_table_built)",
            "for (short table_index = 0;",
            "for (short round = 0; round < 8;",
            "g_sound_crc_table[table_index] = crc",
            "g_sound_crc_table_built = true"]:
        if fragment not in crc_source:
            raise RuntimeError(f"sound CRC implementation lost {fragment}")
    reverb_source = (args.root /
        "src/sound_driver_reverb_default_native.cpp").read_text()
    for fragment in [
            "s_sound_driver_reverb g_44a2a0",
            "{ 0, 0, 0, 0, 0, 0x80, 0, 0 }",
            "-100.0f, -100.0f, 0.0f, 1.0f, 1.0f, -100.0f",
            "0.0f, -100.0f, 0.0f, 1.0f, 1.0f, 5000.0f",
            "sizeof(s_sound_driver_reverb) == 0x48"]:
        if fragment not in reverb_source:
            raise RuntimeError(f"default listener reverb lost {fragment}")
    device_source = (args.root /
        "src/sound_driver_device_startup_native.cpp").read_text()
    for fragment in [
            "sizeof(s_driver_device_view) == 0x2ad8",
            "offsetof(s_driver_device_view, caps) == 0x2aa0",
            "function_163ba0(&checksum, &allocated_size",
            "g_51ebe4 = reinterpret_cast<s_bink_sound_settings *>(memory)",
            "DirectSoundCreate(NULL, &globals->direct_sound, NULL)",
            "IDirectSound_GetCaps(globals->direct_sound, globals->caps)",
            "IDirectSound_SetDistanceFactor(globals->direct_sound, 3.048f",
            "IDirectSound_SetDopplerFactor(globals->direct_sound, 0.0f",
            "(speaker_flags & 0xffff) == 2",
            "for (long mixbin = 0; mixbin <= 31; mixbin++)",
            "IDirectSound_SetRolloffFactor(globals->direct_sound, 0.0f",
            "DirectSoundDoWork();"]:
        if fragment not in device_source:
            raise RuntimeError(f"sound device startup lost {fragment}")
    effect_buffer_source = (args.root /
        "src/sound_driver_effect_buffers_startup_native.cpp").read_text()
    for fragment in [
            "sizeof(s_driver_effect_buffers_view) == 0x2ad8",
            "globals->effect_levels_a[1] = 0xc0c0a8c1",
            "description.flags = k_effect_input",
            "spatial_description.flags = k_effect_input | k_control_3d",
            "IDirectSoundBuffer_SetMaxDistance(*spatial_buffer, max_distance",
            "function_21e3b0(reinterpret_cast<c_sound_driver_effects *>",
            "description.input_mixbin = 15 + group * 4 + i",
            "function_221490(&globals->impulse_buffers[impulse])",
            "globals->impulse_ids[impulse] = NONE",
            "globals->unknown2acc = 8000"]:
        if fragment not in effect_buffer_source:
            raise RuntimeError(f"sound effect-buffer startup lost {fragment}")
    silence_source = (args.root /
        "src/sound_silence_buffer_native.cpp").read_text()
    for fragment in [
            "void function_163ba0(dword *checksum, void const *data",
            "static const dword k_silence_buffer_size = 1000",
            "pair.mixbin = 0xe",
            "DirectSoundCreateBuffer(&description, &buffer)",
            "function_163ba0(&g_510808_pool_checksum, &aligned_size",
            "IDirectSoundBuffer_SetBufferData(buffer, memory",
            "IDirectSoundBuffer_SetHeadroom(buffer, 0)",
            "IDirectSoundBuffer_SetVolume(buffer, -10000)",
            "IDirectSoundBuffer_Play(buffer, 0, 0, k_play_looping)"]:
        if fragment not in silence_source:
            raise RuntimeError(f"sound silence buffer lost {fragment}")
    channel_source = (args.root /
        "src/sound_driver_channels_startup_native.cpp").read_text()
    for fragment in [
            "function_12c040();",
            "g_47f0d0.field10 = 0x10000",
            "globals->channel_first[type] = globals->channel_count",
            "result = result && function_21f4e0(first + i, type)",
            "globals->channel_last[type] = globals->channel_count - 1",
            "result = result && sound_driver_voice_create_buffer(voice)",
            "function_191420(globals->direct_sound)"]:
        if fragment not in channel_source:
            raise RuntimeError(f"sound channel startup lost {fragment}")
    callback_source = (args.root /
        "src/sound_driver_stream_callback_native.cpp").read_text()
    for fragment in [
            "sizeof(s_sound_stream) == 0x34",
            "offsetof(s_driver_callback_view, channels) == 0x0c",
            "if (packet_context)",
            "channel_index >= 0 && channel_index < globals->channel_count",
            "stream->codec->chunk_finished(stream",
            "status == k_packet_status_success",
            "!(stream->flags & k_stream_flush_or_transient)",
            "sound_stream_update(stream)"]:
        if fragment not in callback_source:
            raise RuntimeError(f"sound stream callback lost {fragment}")
    voice_new_source = (args.root /
        "src/sound_driver_voice_new_native.cpp").read_text()
    for fragment in [
            "offsetof(s_driver_voice_new_view, voice_count) == 0x06",
            "offsetof(s_driver_voice_new_view, voices) == 0x1a18",
            "static const dword k_effect_input = 0x1000",
            "static const dword k_effect_input_2 = 0x2000",
            "if (voice_index >= 0x40)",
            "globals->voice_count = voice_index + 1",
            "s_mixbins mixbins = { 6, pairs }",
            "buffer_description.flags = k_effect_input | k_control_3d",
            "globals->voice_count--",
            "s_mixbins mixbins = { 5, pairs }",
            "IDirectSoundBuffer_SetMaxDistance(voice->buffer, max_distance",
            "voice->buffer = NULL"]:
        if fragment not in voice_new_source:
            raise RuntimeError(f"sound driver voice creation lost {fragment}")
    effect_state_source = (args.root /
        "src/sound_effect_parameters_state_native.cpp").read_text()
    for fragment in [
            "sizeof(s_sound_effect_parameters) == 0x30",
            "offsetof(s_sound_effect_parameters, room) == 0x04",
            "offsetof(s_sound_effect_parameters, unknown80) == 0x24",
            "offsetof(s_sound_effect_parameters, unknown88) == 0x2c",
            "s_sound_effect_parameters g_47005c =",
            "true, {0, 0, 0}",
            "0.0f, 0, 0.0f, 0, 0.25f, 8000, 0.0f"]:
        if fragment not in effect_state_source:
            raise RuntimeError(
                f"sound effect parameter state lost {fragment}")
    game_state_allocator_source = (args.root /
        "src/game_state_static_allocator_native.cpp").read_text()
    for fragment in [
            "long aligned_size = (size + 3) & ~3",
            "game_state_globals.base_address +",
            "game_state_globals.cpu_allocation_size += aligned_size",
            "&game_state_globals.allocation_size_checksum",
            "&aligned_size",
            "sizeof(aligned_size)"]:
        if fragment not in game_state_allocator_source:
            raise RuntimeError(
                f"game-state static allocator lost {fragment}")
    class_fade_source = (args.root /
        "src/sound_class_fade_lifetime_native.cpp").read_text()
    for fragment in [
            "sizeof(s_sound_class_fade) == 0x10",
            "offsetof(s_sound_class_fade, time) == 0x08",
            "offsetof(s_sound_class_fade, flags) == 0x0c",
            "s_sound_class_fade *g_502118;",
            "bool g_50211c;",
            "g_502118 = (s_sound_class_fade *)function_123d40(",
            "k_sound_class_count * sizeof(s_sound_class_fade)",
            "memset(g_502118, 0, k_sound_class_count * sizeof(s_sound_class_fade))",
            "g_50211c = true",
            "i < k_sound_class_count",
            "g_502118[i].current = 0",
            "g_502118[i].target = 0",
            "g_502118[i].time = 0.0f",
            "g_50211c = false",
            "g_502118 = NULL"]:
        if fragment not in class_fade_source:
            raise RuntimeError(
                f"sound class-fade lifetime lost {fragment}")
    lifecycle_table_source = (args.root / "src/unknown_11c1b0.cpp").read_text()
    if "0x00221850, 0x002218f0, 0x002218a0, 0x002218e0" not in \
            lifecycle_table_source:
        raise RuntimeError("sound class-fade lifecycle registration changed")
    codec_storage_source = (args.root /
        "src/sound_codec_allocator_state_native.cpp").read_text()
    for fragment in ["void *g_510c3c;", "bool g_510c40;"]:
        if fragment not in codec_storage_source:
            raise RuntimeError(
                f"sound codec allocator storage lost {fragment}")
    codec_startup_source = (args.root /
        "src/sound_static_allocator_native.cpp").read_text()
    for fragment in [
            "long aligned_size = (long)(memory - top) + 0xcc",
            "g_510c3c = memory",
            "g_510c40 = false"]:
        if fragment not in codec_startup_source:
            raise RuntimeError(
                f"sound codec allocator startup lost {fragment}")
    codec_allocate_source = (args.root / "src/unknown_12c040.cpp").read_text()
    for fragment in [
            "attributes->dwObjectType == 1",
            "void *block = g_510c3c",
            "g_510c40 = true",
            "memset(block, 0, 0xcc)",
            "return XMemAllocDefault(dwSize, dwAllocAttributes)"]:
        if fragment not in codec_allocate_source:
            raise RuntimeError(
                f"sound codec allocator path lost {fragment}")
    codec_free_source = (args.root / "src/unknown_012280.cpp").read_text()
    for fragment in [
            "g_510c3c == pAddress",
            "g_510c40 = false",
            "XMemFreeDefault(pAddress, dwAllocAttributes)"]:
        if fragment not in codec_free_source:
            raise RuntimeError(
                f"sound codec allocator free path lost {fragment}")
    physical_memory_source = (args.root / "src/unknown_12b400.cpp").read_text()
    for fragment in [
            "memset(&g_global_f9ae07, 0, sizeof(g_global_f9ae07))",
            "g_global_f9ae07.field_0 = 0",
            "g_global_f9ae07.base_address = 0x61000",
            "g_global_f9ae07.field_8_3 = 0x3145000",
            "g_global_f9ae07.field_c_6[0] = 0x61000",
            "g_global_f9ae07.field_20[0] = 0x3145000",
            "XPhysicalAlloc(0x3145000 - 0x61000, 0x61000, 0, PAGE_READWRITE)"]:
        if fragment not in physical_memory_source:
            raise RuntimeError(
                f"sound physical-memory initializer lost {fragment}")
    xdk_header = (args.root / "port/include/xdk/xtl.h").read_text()
    if "void* __stdcall XPhysicalAlloc(DWORD size, DWORD physical_address," \
            not in xdk_header or \
            "DWORD alignment, DWORD protection);" not in xdk_header:
        raise RuntimeError("sound physical allocation ABI changed")
    guest_host_runtime = (args.root /
        "port/runtime/guest_host_runtime.cpp").read_text()
    host_services_source = (args.root / "port/src/host_services.cpp").read_text()
    for fragment in [
            "XPhysicalAlloc(DWORD size, DWORD physical_address",
            "return host_h2_physical_alloc(size, physical_address, alignment, protection)",
            "XPhysicalProtect(void *address, DWORD size",
            "host_h2_physical_protect(address, size, protection)"]:
        if fragment not in guest_host_runtime:
            raise RuntimeError(f"guest physical bridge lost {fragment}")
    for fragment in [
            "kPhysicalRegionSize = 0x030e5000u",
            "kPhysicalAllocationEnd = 0x83145000u",
            "host_h2_physical_alloc(unsigned int size",
            "physical_address) | 0x80000000ull",
            "host_h2_physical_protect(void* address",
            "protection & ~0x400u",
            "GuestMemoryProtection::read_only",
            "GuestMemoryProtection::read_write"]:
        if fragment not in host_services_source:
            raise RuntimeError(f"host physical bridge lost {fragment}")
    for fragment in [
            "VirtualAlloc(void *address, DWORD size",
            "return host_h2_virtual_alloc(address, size, allocation_type, protection)",
            "VirtualFree(void *address, DWORD size",
            "return host_h2_virtual_free(address, size, free_type)"]:
        if fragment not in guest_host_runtime:
            raise RuntimeError(f"guest virtual-memory bridge lost {fragment}")
    for fragment in [
            "kVirtualAllocationAddress = 0x30000000u",
            "kVirtualAllocationLimit = 0x01000000u",
            "host_h2_virtual_alloc(void* address",
            "allocation_type != 0x1000u || protection != 0x04u",
            "state.virtual_allocation_size",
            "GuestMemoryProtection::read_write",
            "host_h2_virtual_free(void* address",
            "native_address != h2::port::kGuestBias + kVirtualAllocationAddress",
            "free_type != 0x8000u",
            "GuestMemoryProtection::none"]:
        if fragment not in host_services_source:
            raise RuntimeError(f"host virtual-memory bridge lost {fragment}")
    for guest, host, native in [
            ("acos", "host_h2_math_acos", "std::acos"),
            ("exp", "host_h2_math_exp", "std::exp"),
            ("expf", "host_h2_math_expf", "std::exp"),
            ("fmod", "host_h2_math_fmod", "std::fmod"),
            ("log", "host_h2_math_log", "std::log"),
            ("log10", "host_h2_math_log10", "std::log10"),
            ("logf", "host_h2_math_logf", "std::log"),
            ("pow", "host_h2_math_pow", "std::pow"),
            ("sinf", "host_h2_math_sinf", "std::sin")]:
        if f'extern "C"' not in guest_host_runtime or \
                f"{guest}(" not in guest_host_runtime or \
                f"return {host}(" not in guest_host_runtime:
            raise RuntimeError(f"guest math route lost {guest} -> {host}")
        if f'{{"{host}", function_address(&{host})}}' not in \
                host_services_source or \
                f"return {native}(" not in host_services_source:
            raise RuntimeError(f"host math route lost {host} -> {native}")
    runtime_storage_source = (args.root /
        "src/sound_runtime_storage_native.cpp").read_text()
    for fragment in [
            "long g_4e6374;",
            "s_sound_voice *g_4e6378;",
            "s_record_pool *g_502104;",
            "dword g_502108;",
            "s_sound_cache_allocator *g_50210c;",
            "s_voice_effects *g_510c90;",
            "void *g_51ebe0;",
            "s_bink_sound_settings *g_51ebe4;"]:
        if fragment not in runtime_storage_source:
            raise RuntimeError(
                f"sound runtime publication storage lost {fragment}")
    publication_lifetimes = {
        "src/sound_system_startup_native.cpp": [
            "if (g_4e6374 == 0)",
            "g_4e6378 = reinterpret_cast<s_sound_voice *>(voices)",
            "g_4e6374++",
            "if (--g_4e6374 == 0)",
            "g_4e6378 = NULL",
            "g_51ebe0 = NULL"],
        "src/unknown_218850.cpp": [
            'g_502104 = data_new_inlined("xbox sound"',
            "g_50210c = (s_sound_cache_allocator *)physical",
            "g_502108 = (dword)physical_memory_malloc_fixed(0x300000"],
        "src/sound_cache_dispose.cpp": [
            "data_dispose(g_502104)",
            "((s_physical_object *)g_50210c)->allocator->deallocate(g_50210c)",
            "g_502108 = 0"],
        "src/sound_effect_image_startup_native.cpp": [
            "g_510c90 = effects"],
        "src/unknown_21d110.cpp": [
            'g_51ebe0 = function_11cc20(0x10, "sounds effects"'],
        "src/sound_driver_device_startup_native.cpp": [
            "g_51ebe4 = reinterpret_cast<s_bink_sound_settings *>(memory)"],
        "src/sound_driver_teardown_native.cpp": [
            "g_510c3c = NULL",
            "globals->initialized = false"]}
    for source, fragments in publication_lifetimes.items():
        text = (args.root / source).read_text()
        for fragment in fragments:
            if fragment not in text:
                raise RuntimeError(
                    f"sound runtime publication lifetime lost {fragment}")
    diagnostic_sources = {
        "src/unknown_218850.cpp": [
            'data_new_inlined("xbox sound", 0x200',
            'physical_memory_new("xbox sound cache", 0xc0'],
        "src/unknown_12a1b0.cpp": [
            'function_11cc20(128, "looping sounds"',
            'function_11cc20(64, "sound playback controllers"'],
        "src/unknown_21dda0.cpp": [
            'function_11cc20(settings->maximum_voice_count, "sound voices"'],
        "src/sound_system_startup_native.cpp": [
            'function_11cc20(0x180, "sound sources", 0xbc)'],
        "src/unknown_21d110.cpp": [
            'function_11cc20(0x10, "sounds effects"']}
    for source, fragments in diagnostic_sources.items():
        text = (args.root / source).read_text()
        for fragment in fragments:
            if fragment not in text:
                raise RuntimeError(
                    f"sound diagnostic provider lost {fragment}")
    controller_source = (args.root / controller["source"]).read_text()
    storage_source = (args.root / controller["storage_source"]).read_text()
    teardown_source = (args.root / controller["teardown_source"]).read_text()
    for fragment in ["void *g_51ebd8", "void *g_51ebdc"]:
        if fragment not in storage_source:
            raise RuntimeError(
                f"sound-idle controller storage lost {fragment}")
    for fragment in [
            "looping_sound_controllers_initialize(void)",
            "function_11cc20(64, \"sound playback controllers\"",
            "((dword)top + 3) & ~3",
            "function_163ba0(&g_510808_pool_checksum",
            "g_51ebdc = memory",
            "record_pool_release_all(CONTROLLERS)"]:
        if fragment not in controller_source:
            raise RuntimeError(
                f"sound-idle controller initialization lost {fragment}")
    startup_body = teardown_source[
        teardown_source.index("// @retail 0x1253d0"):]
    startup_order = [
        startup_body.index("looping_sound_controllers_initialize()"),
        startup_body.index("function_21a0c0()"),
        startup_body.index("sound_effects_initialize()"),
        startup_body.index("sound_voices_initialize("),
        startup_body.index("function_21e4b0(&counts)"),
        startup_body.index("system->initialized = true")]
    if startup_order != sorted(startup_order):
        raise RuntimeError("sound-idle controller startup order changed")
    for fragment in [
            "void function_125600(void)",
            "if (g_51ebd8)",
            "g_51ebd8 = NULL",
            "if (g_51ebdc)",
            "g_51ebdc = NULL"]:
        if fragment not in teardown_source:
            raise RuntimeError(
                f"sound-idle controller teardown lost {fragment}")
    if manifest.get("production_inadequate_resolutions") != [{
            "symbol": "?voice_start_engine@@YAXXZ",
            "provider": "port/runtime/guest_online_runtime.cpp",
            "exact_dormant_provider":
                "src/voice_xhv_lifetime_native.cpp",
            "reason":
                "the production native-offline provider remains intentionally "
                "empty; the complete exact implementation is confined to the "
                "dormant cluster and cannot enter production before genuine "
                "XHV and callback routing ownership"}]:
        raise RuntimeError("sound-idle inadequate production resolution changed")
    with tempfile.TemporaryDirectory(
            prefix="h2-sound-idle-direct-score-") as temporary:
        scored = Path(temporary) / "scored.o"
        subprocess.run([
            args.lld, "-r", "--allow-multiple-definition",
            "-o", scored, args.cluster_object, args.production_object,
        ], check=True)
        production_imports = symbols(
            args.llvm_nm, scored, "--undefined-only")
    if production_imports != expected_production_imports:
        raise RuntimeError(
            "sound-idle production score changed: " +
            repr(sorted(production_imports)))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

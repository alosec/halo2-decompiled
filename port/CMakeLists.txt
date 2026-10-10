find_package(OpenSSL REQUIRED COMPONENTS Crypto)
find_package(SDL3 CONFIG REQUIRED)

set(H2_LLVM_ROOT "/opt/homebrew/opt/llvm@22" CACHE PATH "LLVM used for the ARM ILP32 guest")
list(APPEND CMAKE_PREFIX_PATH "${H2_LLVM_ROOT}/lib/cmake/llvm")
find_package(LLVM CONFIG REQUIRED)
find_package(Python3 REQUIRED COMPONENTS Interpreter)
find_program(H2_LLD NAMES ld.lld REQUIRED)
find_program(H2_LLVM_NM NAMES llvm-nm HINTS "${H2_LLVM_ROOT}/bin" REQUIRED)

add_library(h2_guest_rebase MODULE compiler/guest_rebase.cpp)
target_include_directories(h2_guest_rebase SYSTEM PRIVATE ${LLVM_INCLUDE_DIRS})
target_compile_features(h2_guest_rebase PRIVATE cxx_std_20)
target_compile_options(h2_guest_rebase PRIVATE -Wall -Wextra -Wpedantic -Werror)
target_link_libraries(h2_guest_rebase PRIVATE LLVM)
set_target_properties(h2_guest_rebase PROPERTIES
    PREFIX ""
    SUFFIX ".dylib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/port/compiler"
)

set(H2_GUEST_OBJECT_DIR "${CMAKE_BINARY_DIR}/port/guest")
set(H2_GUEST_RANDOM_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_0259a0.o")
set(H2_GUEST_GLOBALS_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_07a9a0_2.o")
set(H2_GUEST_CACHE_TAG_OBJECT "${H2_GUEST_OBJECT_DIR}/cache_tag_instances.o")
set(H2_GUEST_CRT_OBJECT "${H2_GUEST_OBJECT_DIR}/guest_crt.o")
set(H2_GUEST_MAP_LOADER_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_122870.o")
set(H2_GUEST_CACHE_RUNTIME_OBJECT "${H2_GUEST_OBJECT_DIR}/guest_cache_runtime.o")
set(H2_GUEST_MAP_LOCATION_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_213d20.o")
set(H2_GUEST_MAP_PROGRESS_OBJECT "${H2_GUEST_OBJECT_DIR}/loading.o")
set(H2_GUEST_MAP_READY_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_18f100.o")
set(H2_GUEST_RANDOM_ID_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_07ad80.o")
set(H2_GUEST_GAME_OPTIONS_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_137dd0.o")
set(H2_GUEST_PROFILE_RECORD_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_153750.o")
set(H2_GUEST_TAG_INITIALIZE_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_11c840.o")
set(H2_GUEST_PART_INITIALIZE_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_1c22c0.o")
set(H2_GUEST_VEHICLE_INITIALIZE_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_205550.o")
set(H2_GUEST_PHYSICS_RUNTIME_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_246c60.o")
set(H2_GUEST_PHYSICS_TYPE_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_2fbb30.o")
set(H2_GUEST_PHYSICS_RESOLVER_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_1eb1b0.o")
set(H2_GUEST_COLLISION_INITIALIZE_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_2122b0.o")
set(H2_GUEST_MAIN_MENU_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_14741b.o")
set(H2_GUEST_WIDGET_BASE_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_22e27b.o")
set(H2_GUEST_SCREEN_BASE_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_2300cf.o")
set(H2_GUEST_UI_HEAP_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_1a4742.o")
set(H2_GUEST_UI_MANAGER_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_147f6d.o")
set(H2_GUEST_UI_MESSAGE_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_1490ec.o")
set(H2_GUEST_LIST_WIDGET_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_24c177.o")
set(H2_GUEST_DIALOG_CALLBACK_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_23690b.o")
set(H2_GUEST_WINDOW_CHANNEL_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_234c64.o")
set(H2_GUEST_WINDOW_QUERY_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_235276.o")
set(H2_GUEST_GAME_STATE_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_138800_2.o")
set(H2_GUEST_CAMPAIGN_STATE_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_138800.o")
set(H2_GUEST_LIFECYCLE_QUERY_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_13bf00.o")
set(H2_GUEST_TIME_QUERY_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_146240.o")
set(H2_GUEST_SIMULATION_QUERY_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_067e10.o")
set(H2_GUEST_RECORD_POOL_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_16b570.o")
set(H2_GUEST_RECORD_POOL_DISPOSE_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_16b5d0.o")
set(H2_GUEST_RECORD_POOL_PREVIOUS_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_16bcc0.o")
set(H2_GUEST_RECORD_POOL_LAST_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_16bc40.o")
set(H2_GUEST_UI_HEAP_GLOBALS_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_1a474c.o")
set(H2_GUEST_UI_HEAP_FREE_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_1a4826.o")
set(H2_GUEST_LOOP_ALLOCATOR_OBJECT "${H2_GUEST_OBJECT_DIR}/loop_allocator.o")
set(H2_GUEST_NAME_BUFFER_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_08cc20.o")
set(H2_GUEST_NAME_TABLE_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_11c050.o")
set(H2_GUEST_BITMAP_LOOKUP_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_137550.o")
set(H2_GUEST_WIDGET_INDEX_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_0bb760.o")
set(H2_GUEST_APPEARANCE_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_139296.o")
set(H2_GUEST_PLAYER_COLORS_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_07f720.o")
set(H2_GUEST_PACK_COLOR_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_131e50.o")
set(H2_GUEST_COLOR_TABLE_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_1a06a0.o")
set(H2_GUEST_UNICODE_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_13fa70.o")
set(H2_GUEST_MS_RUNTIME_OBJECT "${H2_GUEST_OBJECT_DIR}/guest_ms_runtime.o")
set(H2_GUEST_HOST_RUNTIME_OBJECT "${H2_GUEST_OBJECT_DIR}/guest_host_runtime.o")
set(H2_GUEST_ONLINE_RUNTIME_OBJECT "${H2_GUEST_OBJECT_DIR}/guest_online_runtime.o")
set(H2_GUEST_AUDIO_RUNTIME_OBJECT "${H2_GUEST_OBJECT_DIR}/guest_audio_runtime.o")
set(H2_GUEST_RENDER_RUNTIME_OBJECT "${H2_GUEST_OBJECT_DIR}/guest_render_runtime.o")
set(H2_GUEST_CAMPAIGN_ENTRY_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_19c1d0.o")
set(H2_GUEST_LEVEL_TABLES_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_19bd50.o")
set(H2_GUEST_SORT_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_13dcd0.o")
set(H2_GUEST_MOVIE_QUERY_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_155f60.o")
set(H2_GUEST_TEXT_STATE_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_13eb20.o")
set(H2_GUEST_UI_MESSAGE_BUILD_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_149f49.o")
set(H2_GUEST_UI_LAYOUT_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_1482e8.o")
set(H2_GUEST_VARIANT_INDEX_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_216ab0.o")
set(H2_GUEST_VARIANT_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_19d220.o")
set(H2_GUEST_VARIANT_FIELDS_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_236ae6.o")
set(H2_GUEST_CONTROLLER_INDEX_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_1a2c81.o")
set(H2_GUEST_ONLINE_AVAILABLE_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_08d7c0.o")
set(H2_GUEST_SESSION_QUERY_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_059670.o")
set(H2_GUEST_PROFILE_UPDATE_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_18fcc4.o")
set(H2_GUEST_PLAYER_SLOT_SERVICES_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_18f576.o")
set(H2_GUEST_PLAYER_SLOT_INDEX_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_190262.o")
set(H2_GUEST_PLAYER_SLOT_GET_OBJECT "${H2_GUEST_OBJECT_DIR}/player_slot_get.o")
set(H2_GUEST_PLAYER_PROFILE_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_24b5bc.o")
set(H2_GUEST_PLAYER_SESSION_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_190001.o")
set(H2_GUEST_MENU_ACTION_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_250155.o")
set(H2_GUEST_SESSION_PRESENCE_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_19987f.o")
set(H2_GUEST_SESSION_MANAGER_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_058ee0.o")
set(H2_GUEST_TASK_SCREEN_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_1a2ca7.o")
set(H2_GUEST_TASK_TEXT_OBJECT "${H2_GUEST_OBJECT_DIR}/online_task_text.o")
set(H2_GUEST_TASK_CALLBACK_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_2521f2.o")
set(H2_GUEST_SESSION_MODE_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_0592d0.o")
set(H2_GUEST_SESSION_VALUES_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_05b040.o")
set(H2_GUEST_SAVED_GAME_INDEX_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_215b40.o")
set(H2_GUEST_SCREEN_SETTERS_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_2b116a.o")
set(H2_GUEST_CAMPAIGN_SCREEN_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_23068b.o")
set(H2_GUEST_CAMPAIGN_LEVEL_LIST_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_2c4e9c.o")
set(H2_GUEST_DATA_ITERATOR_OBJECT "${H2_GUEST_OBJECT_DIR}/data_iterator.o")
set(H2_GUEST_LOCALIZED_LEVEL_TEXT_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_2afd10.o")
set(H2_GUEST_SCREEN_WIDGETS_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_2afd97.o")
set(H2_GUEST_WIDGET18_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_2afea0.o")
set(H2_GUEST_DIALOG_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_19b510.o")
set(H2_GUEST_DIALOG_BASE_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_253c8b.o")
set(H2_GUEST_DIALOG_EVENT_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_24c7c1.o")
set(H2_GUEST_SCREEN_CLEANUP_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_22f6ca.o")
set(H2_GUEST_ACTIVE_SESSION_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_0597d0.o")
set(H2_GUEST_PREFERENCES_UPDATE_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_121040.o")
set(H2_GUEST_TEXT_PARSER_OBJECT "${H2_GUEST_OBJECT_DIR}/user_interface_text_parser.o")
set(H2_GUEST_MUTELIST_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_058cb0.o")
set(H2_GUEST_SESSION_LEAVE_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_199e2e.o")
set(H2_GUEST_UI_RECT_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_236299.o")
set(H2_GUEST_QUIT_CALLBACK_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_230612.o")
set(H2_GUEST_MUSIC_SILENCE_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_235b46.o")
set(H2_GUEST_SESSION_USER_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_054fe0.o")
set(H2_GUEST_STORAGE_HANDLE_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_2172a0.o")
set(H2_GUEST_STORAGE_SIZE_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_216a50.o")
set(H2_GUEST_PROFILE_STORAGE_UPDATE_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_2157e0.o")
set(H2_GUEST_UI_GEOMETRY_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_13c050.o")
set(H2_GUEST_PREVIEW_RENDER_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_22a664.o")
set(H2_GUEST_PRESS_START_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_22f118.o")
set(H2_GUEST_SIGNIN_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_2523e3.o")
set(H2_GUEST_PROFILE_SCREEN_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_18f42d.o")
set(H2_GUEST_PROFILE_LIST_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_24a2e7.o")
set(H2_GUEST_VIRTUAL_KEYBOARD_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_23760b.o")
set(H2_GUEST_PROFILE_NAME_CHECK_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_216b60.o")
set(H2_GUEST_WIDGET_ITEM_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_2b0149.o")
set(H2_GUEST_LOCALIZED_TEXT_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_1a0120.o")
set(H2_GUEST_UTF8_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_13fd90.o")
set(H2_GUEST_COLOR_SPACE_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_1317a0.o")
file(GLOB H2_GUEST_HEADERS CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/include/*.h")
file(GLOB_RECURSE H2_RECOVERED_SOURCES CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE H2_PORT_GUEST_HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/include/*")

set(H2_GUEST_PHYSICS_MODEL_TABLE_ASM "${H2_GUEST_OBJECT_DIR}/elf/physics_model_vtables.s")
set(H2_GUEST_PHYSICS_MODEL_TABLE_OBJECT "${H2_GUEST_OBJECT_DIR}/elf/physics_model_vtables.o")
set(H2_GUEST_PHYSICS_MODEL_LEAF_OBJECT "${H2_GUEST_OBJECT_DIR}/elf/guest_physics_model_runtime.o")
set(H2_GUEST_PHYSICS_MODEL_INITIALIZE_OBJECT "${H2_GUEST_OBJECT_DIR}/elf/guest_physics_model_initialize.o")
set(H2_GUEST_PHYSICS_MODEL_PROVIDERS_OBJECT "${H2_GUEST_OBJECT_DIR}/elf/physics_model_providers.o")
set(H2_GUEST_PHYSICS_MODEL_PROVIDER_CLUSTER_OBJECT "${H2_GUEST_OBJECT_DIR}/elf/physics_model_provider_cluster.o")
add_custom_command(
    OUTPUT "${H2_GUEST_PHYSICS_MODEL_TABLE_ASM}"
    COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/tools/generate_physics_model_vtables.py"
            --tables "${CMAKE_CURRENT_SOURCE_DIR}/runtime/physics_model_vtables.json"
            --providers "${CMAKE_CURRENT_SOURCE_DIR}/runtime/physics_model_native_providers.json"
            --output "${H2_GUEST_PHYSICS_MODEL_TABLE_ASM}"
    DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/tools/generate_physics_model_vtables.py"
        "${CMAKE_CURRENT_SOURCE_DIR}/runtime/physics_model_vtables.json"
        "${CMAKE_CURRENT_SOURCE_DIR}/runtime/physics_model_native_providers.json"
    VERBATIM
)
add_custom_command(
    OUTPUT "${H2_GUEST_PHYSICS_MODEL_TABLE_OBJECT}"
    COMMAND "${H2_LLVM_ROOT}/bin/clang" --target=aarch64-linux-gnu -c
            "${H2_GUEST_PHYSICS_MODEL_TABLE_ASM}"
            -o "${H2_GUEST_PHYSICS_MODEL_TABLE_OBJECT}"
    DEPENDS "${H2_GUEST_PHYSICS_MODEL_TABLE_ASM}"
    VERBATIM
)
add_custom_command(
    OUTPUT "${H2_GUEST_PHYSICS_MODEL_LEAF_OBJECT}"
    COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/tools/guest_elf_cc.py"
            --llvm "${H2_LLVM_ROOT}/bin"
            --plugin "$<TARGET_FILE:h2_guest_rebase>"
            --include "${CMAKE_CURRENT_SOURCE_DIR}/include"
            --include "${CMAKE_CURRENT_SOURCE_DIR}/include/crt"
            --include "${CMAKE_CURRENT_SOURCE_DIR}/include/xdk"
            --include "${PROJECT_SOURCE_DIR}/include"
            --ms-abi
            --output "${H2_GUEST_PHYSICS_MODEL_LEAF_OBJECT}"
            "${CMAKE_CURRENT_SOURCE_DIR}/runtime/guest_physics_model_runtime.cpp"
    DEPENDS
        h2_guest_rebase
        "${CMAKE_CURRENT_SOURCE_DIR}/tools/guest_elf_cc.py"
        "${CMAKE_CURRENT_SOURCE_DIR}/runtime/guest_physics_model_runtime.cpp"
        ${H2_GUEST_HEADERS}
        ${H2_PORT_GUEST_HEADERS}
    VERBATIM
)
add_custom_command(
    OUTPUT "${H2_GUEST_PHYSICS_MODEL_INITIALIZE_OBJECT}"
    COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/tools/guest_elf_cc.py"
            --llvm "${H2_LLVM_ROOT}/bin"
            --plugin "$<TARGET_FILE:h2_guest_rebase>"
            --include "${CMAKE_CURRENT_SOURCE_DIR}/include"
            --include "${CMAKE_CURRENT_SOURCE_DIR}/include/crt"
            --include "${CMAKE_CURRENT_SOURCE_DIR}/include/xdk"
            --include "${PROJECT_SOURCE_DIR}/include"
            --ms-abi
            --output "${H2_GUEST_PHYSICS_MODEL_INITIALIZE_OBJECT}"
            "${CMAKE_CURRENT_SOURCE_DIR}/runtime/guest_physics_model_initialize.cpp"
    DEPENDS
        h2_guest_rebase
        "${CMAKE_CURRENT_SOURCE_DIR}/tools/guest_elf_cc.py"
        "${CMAKE_CURRENT_SOURCE_DIR}/runtime/guest_physics_model_initialize.cpp"
        ${H2_GUEST_HEADERS}
        ${H2_PORT_GUEST_HEADERS}
    VERBATIM
)
add_custom_command(
    OUTPUT
        "${H2_GUEST_PHYSICS_MODEL_PROVIDERS_OBJECT}"
        "${H2_GUEST_PHYSICS_MODEL_PROVIDER_CLUSTER_OBJECT}"
    COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/tools/build_physics_model_provider_cluster.py"
            --root "${PROJECT_SOURCE_DIR}"
            --llvm "${H2_LLVM_ROOT}/bin"
            --lld "${H2_LLD}"
            --plugin "$<TARGET_FILE:h2_guest_rebase>"
            --tables "${H2_GUEST_PHYSICS_MODEL_TABLE_OBJECT}"
            --leaves "${H2_GUEST_PHYSICS_MODEL_LEAF_OBJECT}"
            --crt "${H2_GUEST_OBJECT_DIR}/elf/guest_crt.o"
            --bindings "${H2_GUEST_OBJECT_DIR}/elf/main_menu_bindings.o"
            --providers-output "${H2_GUEST_PHYSICS_MODEL_PROVIDERS_OBJECT}"
            --output "${H2_GUEST_PHYSICS_MODEL_PROVIDER_CLUSTER_OBJECT}"
    DEPENDS
        h2_guest_rebase
        "${CMAKE_CURRENT_SOURCE_DIR}/tools/build_physics_model_provider_cluster.py"
        "${CMAKE_CURRENT_SOURCE_DIR}/tools/guest_elf_cc.py"
        "${CMAKE_CURRENT_SOURCE_DIR}/runtime/physics_model_native_providers.json"
        "${H2_GUEST_PHYSICS_MODEL_TABLE_OBJECT}"
        "${H2_GUEST_PHYSICS_MODEL_LEAF_OBJECT}"
        "${H2_GUEST_OBJECT_DIR}/elf/guest_crt.o"
        "${H2_GUEST_OBJECT_DIR}/elf/main_menu_bindings.o"
        ${H2_RECOVERED_SOURCES}
        ${H2_GUEST_HEADERS}
        ${H2_PORT_GUEST_HEADERS}
    VERBATIM
)
add_custom_target(h2_physics_model_table_objects ALL DEPENDS
    "${H2_GUEST_PHYSICS_MODEL_TABLE_OBJECT}"
    "${H2_GUEST_PHYSICS_MODEL_LEAF_OBJECT}"
    "${H2_GUEST_PHYSICS_MODEL_INITIALIZE_OBJECT}"
    "${H2_GUEST_PHYSICS_MODEL_PROVIDERS_OBJECT}"
    "${H2_GUEST_PHYSICS_MODEL_PROVIDER_CLUSTER_OBJECT}"
)

set(H2_GUEST_HAVOK_PRIMARY_LISTENER_TABLE_ASM "${H2_GUEST_OBJECT_DIR}/elf/havok_primary_listener_vtables.s")
set(H2_GUEST_HAVOK_PRIMARY_LISTENER_TABLE_OBJECT "${H2_GUEST_OBJECT_DIR}/elf/havok_primary_listener_vtables.o")
set(H2_GUEST_HAVOK_PRIMARY_LISTENER_PROVIDER_CLUSTER_OBJECT "${H2_GUEST_OBJECT_DIR}/elf/havok_primary_listener_provider_cluster.o")
add_custom_command(
    OUTPUT "${H2_GUEST_HAVOK_PRIMARY_LISTENER_TABLE_ASM}"
    COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/tools/generate_havok_primary_listener_vtables.py"
            --tables "${CMAKE_CURRENT_SOURCE_DIR}/runtime/havok_primary_listener_vtables.json"
            --providers "${CMAKE_CURRENT_SOURCE_DIR}/runtime/havok_primary_listener_native_providers.json"
            --output "${H2_GUEST_HAVOK_PRIMARY_LISTENER_TABLE_ASM}"
    DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/tools/generate_havok_primary_listener_vtables.py"
        "${CMAKE_CURRENT_SOURCE_DIR}/runtime/havok_primary_listener_vtables.json"
        "${CMAKE_CURRENT_SOURCE_DIR}/runtime/havok_primary_listener_native_providers.json"
    VERBATIM
)
add_custom_command(
    OUTPUT "${H2_GUEST_HAVOK_PRIMARY_LISTENER_TABLE_OBJECT}"
    COMMAND "${H2_LLVM_ROOT}/bin/clang" --target=aarch64-linux-gnu -c
            "${H2_GUEST_HAVOK_PRIMARY_LISTENER_TABLE_ASM}"
            -o "${H2_GUEST_HAVOK_PRIMARY_LISTENER_TABLE_OBJECT}"
    DEPENDS "${H2_GUEST_HAVOK_PRIMARY_LISTENER_TABLE_ASM}"
    VERBATIM
)
add_custom_command(
    OUTPUT "${H2_GUEST_HAVOK_PRIMARY_LISTENER_PROVIDER_CLUSTER_OBJECT}"
    COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/tools/build_havok_primary_listener_provider_cluster.py"
            --root "${PROJECT_SOURCE_DIR}"
            --python "${Python3_EXECUTABLE}"
            --llvm "${H2_LLVM_ROOT}/bin"
            --lld "${H2_LLD}"
            --plugin "$<TARGET_FILE:h2_guest_rebase>"
            --tables "${H2_GUEST_HAVOK_PRIMARY_LISTENER_TABLE_OBJECT}"
            --output "${H2_GUEST_HAVOK_PRIMARY_LISTENER_PROVIDER_CLUSTER_OBJECT}"
    DEPENDS
        h2_guest_rebase
        "${CMAKE_CURRENT_SOURCE_DIR}/tools/build_havok_primary_listener_provider_cluster.py"
        "${CMAKE_CURRENT_SOURCE_DIR}/tools/guest_elf_cc.py"
        "${H2_GUEST_HAVOK_PRIMARY_LISTENER_TABLE_OBJECT}"
        "${PROJECT_SOURCE_DIR}/src/havok_primary_listener_construct_native.cpp"
        "${PROJECT_SOURCE_DIR}/src/unknown_0b58c0.cpp"
        "${PROJECT_SOURCE_DIR}/src/unknown_2777d0.cpp"
        "${PROJECT_SOURCE_DIR}/src/unknown_181e90_native.cpp"
        "${PROJECT_SOURCE_DIR}/src/unknown_1cec30.cpp"
        "${PROJECT_SOURCE_DIR}/src/unknown_1c55b0_native.cpp"
        "${PROJECT_SOURCE_DIR}/src/unknown_1d8dd0.cpp"
        "${PROJECT_SOURCE_DIR}/src/unknown_2789f0.cpp"
        "${PROJECT_SOURCE_DIR}/src/unknown_279740.cpp"
        "${PROJECT_SOURCE_DIR}/src/unknown_30bc40_native.cpp"
        "${PROJECT_SOURCE_DIR}/src/unknown_30c470_native.cpp"
        "${PROJECT_SOURCE_DIR}/src/unknown_3123a0_native.cpp"
        "${PROJECT_SOURCE_DIR}/src/unknown_2da5d0_native.cpp"
        "${PROJECT_SOURCE_DIR}/src/unknown_315340_native.cpp"
        ${H2_GUEST_HEADERS}
        ${H2_PORT_GUEST_HEADERS}
    VERBATIM
)
add_custom_target(h2_havok_primary_listener_table_object ALL DEPENDS
    "${H2_GUEST_HAVOK_PRIMARY_LISTENER_TABLE_OBJECT}"
    "${H2_GUEST_HAVOK_PRIMARY_LISTENER_PROVIDER_CLUSTER_OBJECT}"
)

set(H2_GUEST_HAVOK_CONTACT_PROCESSING_CANDIDATE_OBJECT "${H2_GUEST_OBJECT_DIR}/elf/havok_contact_processing_candidate.o")
set(H2_GUEST_HAVOK_IMPACT_ALLOCATION_CANDIDATE_OBJECT "${H2_GUEST_OBJECT_DIR}/elf/havok_impact_allocation_candidate.o")
set(H2_GUEST_HAVOK_EFFECT_LIFECYCLE_CANDIDATE_OBJECT "${H2_GUEST_OBJECT_DIR}/elf/havok_effect_lifecycle_candidate.o")
add_custom_command(
    OUTPUT
        "${H2_GUEST_HAVOK_CONTACT_PROCESSING_CANDIDATE_OBJECT}"
        "${H2_GUEST_HAVOK_IMPACT_ALLOCATION_CANDIDATE_OBJECT}"
        "${H2_GUEST_HAVOK_EFFECT_LIFECYCLE_CANDIDATE_OBJECT}"
    COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/tools/build_havok_contact_processing_candidate.py"
            --root "${PROJECT_SOURCE_DIR}"
            --python "${Python3_EXECUTABLE}"
            --llvm "${H2_LLVM_ROOT}/bin"
            --lld "${H2_LLD}"
            --plugin "$<TARGET_FILE:h2_guest_rebase>"
            --output "${H2_GUEST_HAVOK_CONTACT_PROCESSING_CANDIDATE_OBJECT}"
            --impact-output "${H2_GUEST_HAVOK_IMPACT_ALLOCATION_CANDIDATE_OBJECT}"
            --effect-output "${H2_GUEST_HAVOK_EFFECT_LIFECYCLE_CANDIDATE_OBJECT}"
    DEPENDS
        h2_guest_rebase
        "${CMAKE_CURRENT_SOURCE_DIR}/tools/build_havok_contact_processing_candidate.py"
        "${CMAKE_CURRENT_SOURCE_DIR}/tools/guest_elf_cc.py"
        "${PROJECT_SOURCE_DIR}/src/impacts.cpp"
        "${PROJECT_SOURCE_DIR}/src/impact_audio_boundary_native.cpp"
        "${PROJECT_SOURCE_DIR}/src/havok_motion_basis_native.cpp"
        "${PROJECT_SOURCE_DIR}/src/first_person_marker_native.cpp"
        "${PROJECT_SOURCE_DIR}/src/impact_match_tolerance_native.cpp"
        "${PROJECT_SOURCE_DIR}/src/unknown_0259a0.cpp"
        "${PROJECT_SOURCE_DIR}/src/unknown_0259d0.cpp"
        "${PROJECT_SOURCE_DIR}/src/unknown_03eb20.cpp"
        "${PROJECT_SOURCE_DIR}/src/unknown_0a58d0.cpp"
        "${PROJECT_SOURCE_DIR}/src/unknown_0b58c0.cpp"
        "${PROJECT_SOURCE_DIR}/src/unknown_0b8bd0.cpp"
        "${PROJECT_SOURCE_DIR}/src/unknown_0dc3a0.cpp"
        "${PROJECT_SOURCE_DIR}/src/unknown_11c050.cpp"
        "${PROJECT_SOURCE_DIR}/src/unknown_11cc90.cpp"
        "${PROJECT_SOURCE_DIR}/src/unknown_11fc80.cpp"
        "${PROJECT_SOURCE_DIR}/src/unknown_1317a0.cpp"
        "${PROJECT_SOURCE_DIR}/src/unknown_146240.cpp"
        "${PROJECT_SOURCE_DIR}/src/unknown_1469f0.cpp"
        "${PROJECT_SOURCE_DIR}/src/unknown_163080.cpp"
        "${PROJECT_SOURCE_DIR}/src/unknown_16f3a0.cpp"
        "${PROJECT_SOURCE_DIR}/src/unknown_170d70.cpp"
        "${PROJECT_SOURCE_DIR}/src/unknown_173b90.cpp"
        "${PROJECT_SOURCE_DIR}/src/unknown_175bd0.cpp"
        "${PROJECT_SOURCE_DIR}/src/unknown_182020.cpp"
        "${PROJECT_SOURCE_DIR}/src/unknown_187ec0.cpp"
        "${PROJECT_SOURCE_DIR}/src/unknown_188dd0.cpp"
        "${PROJECT_SOURCE_DIR}/src/unknown_18a2f0.cpp"
        "${PROJECT_SOURCE_DIR}/src/unknown_2055f0.cpp"
        "${PROJECT_SOURCE_DIR}/src/unknown_1cec30.cpp"
        "${PROJECT_SOURCE_DIR}/src/unknown_1d5460.cpp"
        "${PROJECT_SOURCE_DIR}/src/unknown_1ee410_native.cpp"
        "${PROJECT_SOURCE_DIR}/src/unknown_277ec0.cpp"
        "${PROJECT_SOURCE_DIR}/src/unknown_277b80.cpp"
        ${H2_GUEST_HEADERS}
        ${H2_PORT_GUEST_HEADERS}
    VERBATIM
)
add_custom_target(h2_havok_contact_processing_candidate ALL DEPENDS
    "${H2_GUEST_HAVOK_CONTACT_PROCESSING_CANDIDATE_OBJECT}"
    "${H2_GUEST_HAVOK_IMPACT_ALLOCATION_CANDIDATE_OBJECT}"
    "${H2_GUEST_HAVOK_EFFECT_LIFECYCLE_CANDIDATE_OBJECT}"
)

set(H2_GUEST_STRUCTURE_BSP_CALLBACK_TABLE_OBJECT "${H2_GUEST_OBJECT_DIR}/elf/structure_bsp_callbacks.o")
set(H2_GUEST_STRUCTURE_BSP_PROVIDERS_OBJECT "${H2_GUEST_OBJECT_DIR}/elf/structure_bsp_providers.o")
set(H2_GUEST_STRUCTURE_BSP_PROVIDER_CLUSTER_OBJECT "${H2_GUEST_OBJECT_DIR}/elf/structure_bsp_provider_cluster.o")
add_custom_command(
    OUTPUT "${H2_GUEST_STRUCTURE_BSP_CALLBACK_TABLE_OBJECT}"
    COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/tools/guest_elf_cc.py"
            --llvm "${H2_LLVM_ROOT}/bin"
            --plugin "$<TARGET_FILE:h2_guest_rebase>"
            --include "${CMAKE_CURRENT_SOURCE_DIR}/include"
            --include "${CMAKE_CURRENT_SOURCE_DIR}/include/crt"
            --include "${CMAKE_CURRENT_SOURCE_DIR}/include/xdk"
            --include "${PROJECT_SOURCE_DIR}/include"
            --ms-abi
            --extract-symbol h2_native_structure_bsp_idle
            --extract-global h2_native_structure_bsp_switch_callbacks
            --output "${H2_GUEST_STRUCTURE_BSP_CALLBACK_TABLE_OBJECT}"
            "${CMAKE_CURRENT_SOURCE_DIR}/runtime/guest_structure_bsp_callbacks.cpp"
    DEPENDS
        h2_guest_rebase
        "${CMAKE_CURRENT_SOURCE_DIR}/tools/guest_elf_cc.py"
        "${CMAKE_CURRENT_SOURCE_DIR}/runtime/guest_structure_bsp_callbacks.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/runtime/structure_bsp_switch_callbacks.json"
        "${CMAKE_CURRENT_SOURCE_DIR}/runtime/structure_bsp_native_providers.json"
        ${H2_GUEST_HEADERS}
        ${H2_PORT_GUEST_HEADERS}
    VERBATIM
)
add_custom_command(
    OUTPUT
        "${H2_GUEST_STRUCTURE_BSP_PROVIDERS_OBJECT}"
        "${H2_GUEST_STRUCTURE_BSP_PROVIDER_CLUSTER_OBJECT}"
    COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/tools/build_structure_bsp_provider_cluster.py"
            --root "${PROJECT_SOURCE_DIR}"
            --llvm "${H2_LLVM_ROOT}/bin"
            --lld "${H2_LLD}"
            --plugin "$<TARGET_FILE:h2_guest_rebase>"
            --table "${H2_GUEST_STRUCTURE_BSP_CALLBACK_TABLE_OBJECT}"
            --crt "${H2_GUEST_OBJECT_DIR}/elf/guest_crt.o"
            --bindings "${H2_GUEST_OBJECT_DIR}/elf/main_menu_bindings.o"
            --providers-output "${H2_GUEST_STRUCTURE_BSP_PROVIDERS_OBJECT}"
            --output "${H2_GUEST_STRUCTURE_BSP_PROVIDER_CLUSTER_OBJECT}"
    DEPENDS
        h2_guest_rebase
        "${CMAKE_CURRENT_SOURCE_DIR}/tools/build_structure_bsp_provider_cluster.py"
        "${CMAKE_CURRENT_SOURCE_DIR}/tools/guest_elf_cc.py"
        "${CMAKE_CURRENT_SOURCE_DIR}/runtime/structure_bsp_native_providers.json"
        "${H2_GUEST_STRUCTURE_BSP_CALLBACK_TABLE_OBJECT}"
        "${H2_GUEST_OBJECT_DIR}/elf/guest_crt.o"
        "${H2_GUEST_OBJECT_DIR}/elf/main_menu_bindings.o"
        ${H2_RECOVERED_SOURCES}
        ${H2_GUEST_HEADERS}
        ${H2_PORT_GUEST_HEADERS}
    VERBATIM
)
add_custom_target(h2_structure_bsp_provider_objects ALL DEPENDS
    "${H2_GUEST_STRUCTURE_BSP_CALLBACK_TABLE_OBJECT}"
    "${H2_GUEST_STRUCTURE_BSP_PROVIDERS_OBJECT}"
    "${H2_GUEST_STRUCTURE_BSP_PROVIDER_CLUSTER_OBJECT}"
)

set(H2_MS_ABI_SOURCES
    "${PROJECT_SOURCE_DIR}/src/unknown_14741b.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_22e27b.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_2300cf.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_1a4742.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_147f6d.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_1490ec.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_24c177.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_23690b.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_234c64.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_235276.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_138800_2.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_138800.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_13bf00.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_146240.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_067e10.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_16b570.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_16b5d0.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_16bcc0.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_16bc40.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_1a474c.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_1a4826.cpp"
    "${PROJECT_SOURCE_DIR}/src/loop_allocator.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_08cc20.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_11c050.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_137550.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_0bb760.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_139296.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_07f720.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_131e50.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_1a06a0.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_13fa70.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/runtime/guest_ms_runtime.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/runtime/guest_host_runtime.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_213d20.cpp"
    "${PROJECT_SOURCE_DIR}/src/loading.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_18f100.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_07ad80.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_137dd0.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_153750.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_11c840.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_1c22c0.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_205550.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_246c60.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_2fbb30.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_1eb1b0.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_2122b0.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/runtime/guest_online_runtime.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/runtime/guest_audio_runtime.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/runtime/guest_render_runtime.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_19c1d0.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_19bd50.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_13dcd0.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_155f60.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_13eb20.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_149f49.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_1482e8.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_216ab0.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_19d220.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_236ae6.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_1a2c81.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_08d7c0.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_059670.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_18fcc4.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_18f576.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_18f42d.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_24a2e7.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_23760b.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_216b60.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_190262.cpp"
    "${PROJECT_SOURCE_DIR}/src/player_slot_get.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_24b5bc.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_190001.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_250155.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_19987f.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_058ee0.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_1a2ca7.cpp"
    "${PROJECT_SOURCE_DIR}/src/online_task_text.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_2521f2.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_0592d0.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_05b040.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_215b40.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_2b116a.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_23068b.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_2c4e9c.cpp"
    "${PROJECT_SOURCE_DIR}/src/data_iterator.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_2afd10.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_2afd97.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_2afea0.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_19b510.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_253c8b.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_24c7c1.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_22f6ca.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_0597d0.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_121040.cpp"
    "${PROJECT_SOURCE_DIR}/src/user_interface_text_parser.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_058cb0.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_199e2e.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_236299.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_230612.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_235b46.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_054fe0.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_2172a0.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_216a50.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_2157e0.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_13c050.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_22a664.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_22f118.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_2523e3.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_2b0149.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_1a0120.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_13fd90.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_1317a0.cpp"
)
foreach(pair IN ITEMS
        "${H2_GUEST_RANDOM_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_0259a0.cpp"
        "${H2_GUEST_GLOBALS_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_07a9a0_2.cpp"
        "${H2_GUEST_CACHE_TAG_OBJECT}|${PROJECT_SOURCE_DIR}/src/cache_tag_instances.cpp"
        "${H2_GUEST_CRT_OBJECT}|${CMAKE_CURRENT_SOURCE_DIR}/runtime/guest_crt.c"
        "${H2_GUEST_MAP_LOADER_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_122870.cpp"
        "${H2_GUEST_CACHE_RUNTIME_OBJECT}|${CMAKE_CURRENT_SOURCE_DIR}/runtime/guest_cache_runtime.cpp"
        "${H2_GUEST_MAP_LOCATION_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_213d20.cpp"
        "${H2_GUEST_MAP_PROGRESS_OBJECT}|${PROJECT_SOURCE_DIR}/src/loading.cpp"
        "${H2_GUEST_MAP_READY_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_18f100.cpp"
        "${H2_GUEST_RANDOM_ID_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_07ad80.cpp"
        "${H2_GUEST_GAME_OPTIONS_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_137dd0.cpp"
        "${H2_GUEST_PROFILE_RECORD_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_153750.cpp"
        "${H2_GUEST_TAG_INITIALIZE_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_11c840.cpp"
        "${H2_GUEST_PART_INITIALIZE_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_1c22c0.cpp"
        "${H2_GUEST_VEHICLE_INITIALIZE_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_205550.cpp"
        "${H2_GUEST_PHYSICS_RUNTIME_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_246c60.cpp"
        "${H2_GUEST_PHYSICS_TYPE_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_2fbb30.cpp"
        "${H2_GUEST_PHYSICS_RESOLVER_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_1eb1b0.cpp"
        "${H2_GUEST_COLLISION_INITIALIZE_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_2122b0.cpp"
        "${H2_GUEST_MAIN_MENU_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_14741b.cpp"
        "${H2_GUEST_WIDGET_BASE_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_22e27b.cpp"
        "${H2_GUEST_SCREEN_BASE_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_2300cf.cpp"
        "${H2_GUEST_UI_HEAP_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_1a4742.cpp"
        "${H2_GUEST_UI_MANAGER_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_147f6d.cpp"
        "${H2_GUEST_UI_MESSAGE_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_1490ec.cpp"
        "${H2_GUEST_LIST_WIDGET_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_24c177.cpp"
        "${H2_GUEST_DIALOG_CALLBACK_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_23690b.cpp"
        "${H2_GUEST_WINDOW_CHANNEL_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_234c64.cpp"
        "${H2_GUEST_WINDOW_QUERY_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_235276.cpp"
        "${H2_GUEST_GAME_STATE_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_138800_2.cpp"
        "${H2_GUEST_CAMPAIGN_STATE_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_138800.cpp"
        "${H2_GUEST_LIFECYCLE_QUERY_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_13bf00.cpp"
        "${H2_GUEST_TIME_QUERY_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_146240.cpp"
        "${H2_GUEST_SIMULATION_QUERY_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_067e10.cpp"
        "${H2_GUEST_RECORD_POOL_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_16b570.cpp"
        "${H2_GUEST_RECORD_POOL_DISPOSE_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_16b5d0.cpp"
        "${H2_GUEST_RECORD_POOL_PREVIOUS_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_16bcc0.cpp"
        "${H2_GUEST_RECORD_POOL_LAST_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_16bc40.cpp"
        "${H2_GUEST_UI_HEAP_GLOBALS_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_1a474c.cpp"
        "${H2_GUEST_UI_HEAP_FREE_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_1a4826.cpp"
        "${H2_GUEST_LOOP_ALLOCATOR_OBJECT}|${PROJECT_SOURCE_DIR}/src/loop_allocator.cpp"
        "${H2_GUEST_NAME_BUFFER_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_08cc20.cpp"
        "${H2_GUEST_NAME_TABLE_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_11c050.cpp"
        "${H2_GUEST_BITMAP_LOOKUP_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_137550.cpp"
        "${H2_GUEST_WIDGET_INDEX_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_0bb760.cpp"
        "${H2_GUEST_APPEARANCE_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_139296.cpp"
        "${H2_GUEST_PLAYER_COLORS_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_07f720.cpp"
        "${H2_GUEST_PACK_COLOR_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_131e50.cpp"
        "${H2_GUEST_COLOR_TABLE_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_1a06a0.cpp"
        "${H2_GUEST_UNICODE_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_13fa70.cpp"
        "${H2_GUEST_MS_RUNTIME_OBJECT}|${CMAKE_CURRENT_SOURCE_DIR}/runtime/guest_ms_runtime.cpp"
        "${H2_GUEST_HOST_RUNTIME_OBJECT}|${CMAKE_CURRENT_SOURCE_DIR}/runtime/guest_host_runtime.cpp"
        "${H2_GUEST_ONLINE_RUNTIME_OBJECT}|${CMAKE_CURRENT_SOURCE_DIR}/runtime/guest_online_runtime.cpp"
        "${H2_GUEST_AUDIO_RUNTIME_OBJECT}|${CMAKE_CURRENT_SOURCE_DIR}/runtime/guest_audio_runtime.cpp"
        "${H2_GUEST_RENDER_RUNTIME_OBJECT}|${CMAKE_CURRENT_SOURCE_DIR}/runtime/guest_render_runtime.cpp"
        "${H2_GUEST_CAMPAIGN_ENTRY_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_19c1d0.cpp"
        "${H2_GUEST_LEVEL_TABLES_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_19bd50.cpp"
        "${H2_GUEST_SORT_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_13dcd0.cpp"
        "${H2_GUEST_MOVIE_QUERY_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_155f60.cpp"
        "${H2_GUEST_TEXT_STATE_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_13eb20.cpp"
        "${H2_GUEST_UI_MESSAGE_BUILD_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_149f49.cpp"
        "${H2_GUEST_UI_LAYOUT_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_1482e8.cpp"
        "${H2_GUEST_VARIANT_INDEX_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_216ab0.cpp"
        "${H2_GUEST_VARIANT_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_19d220.cpp"
        "${H2_GUEST_VARIANT_FIELDS_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_236ae6.cpp"
        "${H2_GUEST_CONTROLLER_INDEX_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_1a2c81.cpp"
        "${H2_GUEST_ONLINE_AVAILABLE_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_08d7c0.cpp"
        "${H2_GUEST_SESSION_QUERY_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_059670.cpp"
        "${H2_GUEST_PROFILE_UPDATE_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_18fcc4.cpp"
        "${H2_GUEST_PLAYER_SLOT_SERVICES_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_18f576.cpp"
        "${H2_GUEST_PROFILE_SCREEN_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_18f42d.cpp"
        "${H2_GUEST_PROFILE_LIST_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_24a2e7.cpp"
        "${H2_GUEST_VIRTUAL_KEYBOARD_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_23760b.cpp"
        "${H2_GUEST_PROFILE_NAME_CHECK_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_216b60.cpp"
        "${H2_GUEST_PLAYER_SLOT_INDEX_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_190262.cpp"
        "${H2_GUEST_PLAYER_SLOT_GET_OBJECT}|${PROJECT_SOURCE_DIR}/src/player_slot_get.cpp"
        "${H2_GUEST_PLAYER_PROFILE_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_24b5bc.cpp"
        "${H2_GUEST_PLAYER_SESSION_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_190001.cpp"
        "${H2_GUEST_MENU_ACTION_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_250155.cpp"
        "${H2_GUEST_SESSION_PRESENCE_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_19987f.cpp"
        "${H2_GUEST_SESSION_MANAGER_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_058ee0.cpp"
        "${H2_GUEST_TASK_SCREEN_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_1a2ca7.cpp"
        "${H2_GUEST_TASK_TEXT_OBJECT}|${PROJECT_SOURCE_DIR}/src/online_task_text.cpp"
        "${H2_GUEST_TASK_CALLBACK_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_2521f2.cpp"
        "${H2_GUEST_SESSION_MODE_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_0592d0.cpp"
        "${H2_GUEST_SESSION_VALUES_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_05b040.cpp"
        "${H2_GUEST_SAVED_GAME_INDEX_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_215b40.cpp"
        "${H2_GUEST_SCREEN_SETTERS_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_2b116a.cpp"
        "${H2_GUEST_CAMPAIGN_SCREEN_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_23068b.cpp"
        "${H2_GUEST_CAMPAIGN_LEVEL_LIST_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_2c4e9c.cpp"
        "${H2_GUEST_DATA_ITERATOR_OBJECT}|${PROJECT_SOURCE_DIR}/src/data_iterator.cpp"
        "${H2_GUEST_LOCALIZED_LEVEL_TEXT_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_2afd10.cpp"
        "${H2_GUEST_SCREEN_WIDGETS_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_2afd97.cpp"
        "${H2_GUEST_WIDGET18_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_2afea0.cpp"
        "${H2_GUEST_DIALOG_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_19b510.cpp"
        "${H2_GUEST_DIALOG_BASE_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_253c8b.cpp"
        "${H2_GUEST_DIALOG_EVENT_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_24c7c1.cpp"
        "${H2_GUEST_SCREEN_CLEANUP_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_22f6ca.cpp"
        "${H2_GUEST_ACTIVE_SESSION_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_0597d0.cpp"
        "${H2_GUEST_PREFERENCES_UPDATE_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_121040.cpp"
        "${H2_GUEST_TEXT_PARSER_OBJECT}|${PROJECT_SOURCE_DIR}/src/user_interface_text_parser.cpp"
        "${H2_GUEST_MUTELIST_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_058cb0.cpp"
        "${H2_GUEST_SESSION_LEAVE_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_199e2e.cpp"
        "${H2_GUEST_UI_RECT_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_236299.cpp"
        "${H2_GUEST_QUIT_CALLBACK_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_230612.cpp"
        "${H2_GUEST_MUSIC_SILENCE_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_235b46.cpp"
        "${H2_GUEST_SESSION_USER_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_054fe0.cpp"
        "${H2_GUEST_STORAGE_HANDLE_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_2172a0.cpp"
        "${H2_GUEST_STORAGE_SIZE_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_216a50.cpp"
        "${H2_GUEST_PROFILE_STORAGE_UPDATE_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_2157e0.cpp"
        "${H2_GUEST_UI_GEOMETRY_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_13c050.cpp"
        "${H2_GUEST_PREVIEW_RENDER_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_22a664.cpp"
        "${H2_GUEST_PRESS_START_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_22f118.cpp"
        "${H2_GUEST_SIGNIN_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_2523e3.cpp"
        "${H2_GUEST_WIDGET_ITEM_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_2b0149.cpp"
        "${H2_GUEST_LOCALIZED_TEXT_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_1a0120.cpp"
        "${H2_GUEST_UTF8_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_13fd90.cpp"
        "${H2_GUEST_COLOR_SPACE_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_1317a0.cpp")
    string(REPLACE "|" ";" fields "${pair}")
    list(GET fields 0 object)
    list(GET fields 1 source)
    set(guest_abi_args)
    set(guest_extract_args)
    list(FIND H2_MS_ABI_SOURCES "${source}" ms_abi_index)
    if(NOT ms_abi_index EQUAL -1)
        list(APPEND guest_abi_args --ms-abi)
    endif()
    if(object STREQUAL H2_GUEST_MAP_LOADER_OBJECT)
        list(APPEND guest_extract_args
            --extract-symbol "_Z19cache_header_verifyPK14s_cache_header"
            --extract-symbol "_Z20cache_files_load_mapPKc"
            --extract-symbol "_Z30cache_files_load_structure_bspP25s_structure_bsp_reference")
    endif()
    if(object STREQUAL H2_GUEST_MAP_LOCATION_OBJECT)
        list(APPEND guest_extract_args --extract-symbol
            "?map_location_get@@YAJPBD@Z")
    endif()
    if(object STREQUAL H2_GUEST_MAP_PROGRESS_OBJECT)
        list(APPEND guest_extract_args
            --extract-symbol "?function_163890@@YA_NPBDJ@Z"
            --extract-symbol "?map_location_progress_get@@YAJPBD@Z"
            --extract-global "switch.table.?map_location_progress_get@@YAJPBD@Z")
    endif()
    if(object STREQUAL H2_GUEST_MAP_READY_OBJECT)
        list(APPEND guest_extract_args --extract-symbol
            "?function_18f100@@YA_NPBD@Z")
    endif()
    if(object STREQUAL H2_GUEST_RANDOM_ID_OBJECT)
        list(APPEND guest_extract_args --extract-symbol
            "?function_07ad80@@YAXJPAE@Z")
    endif()
    if(object STREQUAL H2_GUEST_GAME_OPTIONS_OBJECT)
        list(APPEND guest_extract_args --extract-symbol
            "?function_138110@@YAXPAUs_game_options@@@Z")
    endif()
    if(object STREQUAL H2_GUEST_PROFILE_RECORD_OBJECT)
        list(APPEND guest_extract_args --extract-symbol
            "?function_1537f0@@YAXPAUs_profile_record@@@Z")
    endif()
    if(object STREQUAL H2_GUEST_TAG_INITIALIZE_OBJECT)
        list(APPEND guest_extract_args
            --extract-symbol "?function_11c840@@YAXXZ"
            --extract-symbol "h2_native_tag_initialization_status"
            --extract-symbol "h2_native_physics_model_census"
            --extract-symbol "h2_native_physics_model_plan_status"
            --extract-global "g_native_initialized_tags"
            --extract-global "g_native_deferred_tags"
            --extract-global "g_native_physics_model_nonzero_mask"
            --extract-global "g_native_physics_model_entry_count"
            --extract-global "g_native_physics_model_planned_objects"
            --extract-global "g_native_physics_model_planned_resolutions"
            --extract-global "g_native_physics_model_plan_valid")
    endif()
    if(object STREQUAL H2_GUEST_PART_INITIALIZE_OBJECT)
        list(APPEND guest_extract_args
            --extract-symbol "?function_1c2390@s_part_table@@QAAXXZ"
            --extract-symbol "??_Gc_part_a@@UAAPAXI@Z"
            --extract-symbol "??_Gc_part_b@@UAAPAXI@Z"
            --extract-symbol "??_Gc_part_c@@UAAPAXI@Z"
            --extract-global "??_7c_part_a@@6B@"
            --extract-global "??_7c_part_b@@6B@"
            --extract-global "??_7c_part_c@@6B@"
            --extract-alias "??_Ec_part_a@@UAAPAXI@Z"
            --extract-alias "??_Ec_part_b@@UAAPAXI@Z"
            --extract-alias "??_Ec_part_c@@UAAPAXI@Z")
    endif()
    if(object STREQUAL H2_GUEST_VEHICLE_INITIALIZE_OBJECT)
        list(APPEND guest_extract_args
            --extract-symbol "?function_205550@@YAXPAX@Z"
            --extract-symbol "?function_205551@c_205550@@UAAXXZ"
            --extract-symbol "?function_205551@c_205554@@UAAXXZ"
            --extract-symbol "?function_205552@c_205558@@UAAXXZ"
            --extract-symbol "?function_205553@c_205558@@UAAXXZ"
            --extract-global "??_7c_205550@@6B@"
            --extract-global "??_7c_205554@@6B@")
    endif()
    if(object STREQUAL H2_GUEST_PHYSICS_RUNTIME_OBJECT)
        list(APPEND guest_extract_args
            --extract-symbol "?function_246c60@@YAXPAXJ@Z"
            --extract-symbol "h2_native_physics_runtime_vtable"
            --extract-symbol "h2_native_physics_runtime_reset"
            --extract-symbol "h2_native_physics_runtime_object_count"
            --extract-symbol "?h2_native_physics_delete@@YAPAXPAXK@Z"
            --extract-symbol "?h2_native_physics_query@@YAJPAXJJ@Z"
            --extract-symbol "?h2_native_physics_enable@@YAXPAX_N@Z"
            --extract-symbol "?h2_native_physics_root_type@@YAPAXPAX@Z"
            --extract-symbol "?h2_native_physics_nested_type@@YAPAXPAX@Z"
            --extract-symbol "?h2_native_physics_wrapper_type@@YAPAXPAX@Z"
            --extract-symbol "?h2_native_physics_idle@@YAXPAX@Z"
            --extract-global "g_native_physics_runtime_vtables"
            --extract-global "g_native_physics_runtime_object_count")
    endif()
    if(object STREQUAL H2_GUEST_PHYSICS_TYPE_OBJECT)
        list(APPEND guest_extract_args
            --extract-symbol "?function_2fbb30@@YAPAXXZ"
            --extract-symbol "?function_2fdfe0@@YAPAXXZ"
            --extract-symbol "?function_2fbfe0@@YAPAXXZ")
    endif()
    if(object STREQUAL H2_GUEST_PHYSICS_RESOLVER_OBJECT)
        list(APPEND guest_extract_args --extract-symbol
            "?function_1eaf30@@YAPAXFPAUs_shape_block_table@@F@Z")
    endif()
    if(object STREQUAL H2_GUEST_COLLISION_INITIALIZE_OBJECT)
        list(APPEND guest_extract_args --extract-symbol
            "?function_2122b0@@YAXJ@Z")
    endif()
    if(object STREQUAL H2_GUEST_RECORD_POOL_LAST_OBJECT)
        list(APPEND guest_extract_args --extract-symbol
            "?data_last_index@@YAJPAUs_record_pool@@@Z")
    endif()
    if(object STREQUAL H2_GUEST_PLAYER_PROFILE_OBJECT)
        list(APPEND guest_extract_args
            --extract-symbol "?function_24bac5@@YAXPAX@Z"
            --extract-symbol "?initialize@s_player_slot_profile@@QAAXJ@Z"
            --extract-symbol "?set_profile_index@s_player_slot_profile@@QAAXJ@Z"
            --extract-symbol "?sign_in@s_player_slot_profile@@QAAXP6AXJ_N@Z@Z")
    endif()
    if(object STREQUAL H2_GUEST_PLAYER_SLOT_SERVICES_OBJECT)
        list(APPEND guest_extract_args
            --extract-symbol "?function_18f58c@@YAXXZ"
            --extract-symbol "?function_18f5e3@@YAXXZ"
            --extract-symbol "?player_slot_count_active@@YAFXZ"
            --extract-symbol "?player_slot_find_controller@@YAJJ@Z"
            --extract-symbol "?player_slot_get_profile@@YAXJPAUs_player_profile@@PAJ@Z"
            --extract-symbol "?player_slot_get_single_profile@@YAJXZ"
            --extract-symbol "?player_slot_get_single_profile_index@@YAJXZ"
            --extract-symbol "?function_18fa4d@@YAJJ@Z"
            --extract-symbol "?function_18fb34@@YAXJPAUs_player_profile_settings@@J@Z")
    endif()
    if(object STREQUAL H2_GUEST_PROFILE_SCREEN_OBJECT)
        list(APPEND guest_extract_args
            --extract-symbol "?function_18f42d@@YAPAVc_class_1473c9@@PAUs_screen_parameters@@@Z"
            --extract-symbol "?function_18f474@@YAPAVc_class_1473c9@@PAUs_screen_parameters@@@Z"
            --extract-symbol "?v17@c_18f4bb@@UAAXXZ"
            --extract-symbol "?get_load_proc@c_18f4bb@@UAAP6APAVc_class_1473c9@@PAUs_screen_parameters@@@ZXZ"
            --extract-symbol "??_Gc_18f4bb@@UAAPAXI@Z"
            --extract-global "??_7c_18f4bb@@6B@"
            --extract-alias "??_Ec_18f4bb@@UAAPAXI@Z")
    endif()
    if(object STREQUAL H2_GUEST_PROFILE_LIST_OBJECT)
        list(APPEND guest_extract_args
            --extract-symbol "h2_profile_list_construct"
            --extract-symbol "??_Gc_list_24a64d@@UAAPAXI@Z"
            --extract-symbol "?v1@c_list_24a64d@@UAAXXZ"
            --extract-symbol "?v2@c_list_24a64d@@UAAXXZ"
            --extract-symbol "?v3@c_list_24a64d@@UAAXXZ"
            --extract-symbol "?v20@c_list_24a64d@@UAAXPAVc_class_1a2c81@@J@Z"
            --extract-symbol "?v24@c_list_24a64d@@UAAXXZ"
            --extract-symbol "?v23@c_list_widget_26@@UAAXXZ"
            --extract-symbol "?v25@c_list_widget_26@@UAAXXZ"
            --extract-symbol "?function_24a30e@c_list_24a64d@@QAAXPAPAUs_controller_reference@@PAJ@Z"
            --extract-symbol "?function_24a64d@c_list_24a64d@@UAAPAXJ@Z"
            --extract-symbol "?function_24a691@c_list_24a64d@@UAAPBGJ_N@Z"
            --extract-symbol "?get_item_data@c_list_24a64d@@UAAPAXXZ"
            --extract-symbol "?get_item_count@c_list_24a64d@@UAAJXZ"
            --extract-symbol "?get_items@c_list_24a64d@@UAAPAXPAJ@Z"
            --extract-global "??_7c_list_24a64d@@6B@"
            --extract-alias "??_Ec_list_24a64d@@UAAPAXI@Z"
            --extract-global "??_C@_0BE@FKGOBLBN@player?5profile?5list?$AA@")
    endif()
    if(object STREQUAL H2_GUEST_VIRTUAL_KEYBOARD_OBJECT)
        list(APPEND guest_extract_args
            --extract-symbol "?function_238c21@@YAXJJPAGJ@Z"
            --extract-symbol "h2_native_virtual_keyboard_accept"
            --extract-symbol "?function_23784f@@YAPAVc_class_1473c9@@PAUs_screen_parameters@@@Z"
            --extract-symbol "?v10@c_keyboard_key_widget@@UAA_NPAUs_widget_event@@@Z"
            --extract-symbol "?v17@c_keyboard_key_widget@@UAAJXZ"
            --extract-symbol "?v3@c_virtual_keyboard_screen@@UAAXXZ"
            --extract-symbol "?v4@c_virtual_keyboard_screen@@UAAXJ@Z"
            --extract-symbol "?v10@c_virtual_keyboard_screen@@UAA_NPAUs_widget_event@@@Z"
            --extract-symbol "?v18@c_virtual_keyboard_screen@@UAAXPAX@Z"
            --extract-symbol "?get_load_proc@c_virtual_keyboard_screen@@UAAP6APAVc_class_1473c9@@PAUs_screen_parameters@@@ZXZ"
            --extract-symbol "?function_2380c0@@YA_NPAVc_virtual_keyboard_screen@@@Z"
            --extract-symbol "??_Gc_keyboard_key_widget@@UAAPAXI@Z"
            --extract-symbol "??_Gc_virtual_keyboard_screen@@UAAPAXI@Z"
            --extract-global "??_7c_keyboard_key_widget@@6B@"
            --extract-global "??_7c_virtual_keyboard_screen@@6B@"
            --extract-global g_44a540
            --extract-global g_44a600
            --extract-global g_44a6c0
            --extract-global g_44a780
            --extract-global g_44a840
            --extract-global g_44a900
            --extract-global g_44a9bc
            --extract-global g_native_virtual_keyboard
            --extract-alias "??_Ec_keyboard_key_widget@@UAAPAXI@Z"
            --extract-alias "??_Ec_virtual_keyboard_screen@@UAAPAXI@Z")
    endif()
    if(object STREQUAL H2_GUEST_PROFILE_NAME_CHECK_OBJECT)
        list(APPEND guest_extract_args --extract-symbol
            "?function_216120@@YA_NPAGJ@Z")
    endif()
    if(object STREQUAL H2_GUEST_PLAYER_SESSION_OBJECT)
        list(APPEND guest_extract_args
            --extract-symbol "?function_1900a5@@YA_NJ@Z"
            --extract-symbol "?function_190186@@YAXJ@Z"
            --extract-symbol "?function_1900be@@YAFXZ"
            --extract-symbol "?function_1900ff@@YAFJ@Z"
            --extract-symbol "?function_1901fc@@YAGXZ"
            --extract-symbol "?function_19022f@@YAGXZ"
            --extract-symbol "?function_19028d@@YA_NXZ"
            --extract-symbol "?function_1904cb@@YA_NJ@Z"
            --extract-symbol "?function_19052c@@YAXJPAJ0@Z"
            --extract-symbol "?function_190565@@YAJXZ"
            --extract-symbol "?function_18fd94@@YAXJPAKPAUs_profile_record@@@Z"
            --extract-symbol "?function_190001@@YAXJPBD@Z"
            --extract-symbol "?function_19060a@@YAXJJ@Z"
            --extract-symbol "?function_1906b4@@YAXXZ"
            --extract-symbol "?function_1906da@@YA_NJ@Z"
            --extract-symbol "?function_1910b8@@YAJJ@Z")
    endif()
    if(object STREQUAL H2_GUEST_MENU_ACTION_OBJECT)
        list(APPEND guest_extract_args --extract-symbol
            "?function_2523b7@@YA_NJ@Z")
    endif()
    if(object STREQUAL H2_GUEST_CAMPAIGN_STATE_OBJECT)
        list(APPEND guest_extract_args --extract-symbol
            "?function_138820@@YA_NXZ")
    endif()
    if(object STREQUAL H2_GUEST_LIFECYCLE_QUERY_OBJECT)
        list(APPEND guest_extract_args --extract-symbol
            "?function_13cb40@@YA_NXZ")
    endif()
    if(object STREQUAL H2_GUEST_NAME_TABLE_OBJECT)
        list(APPEND guest_extract_args --extract-symbol
            "?function_11c0b0@@YAFPBDPAUs_name_table_11c0b0@@@Z")
    endif()
    if(object STREQUAL H2_GUEST_BITMAP_LOOKUP_OBJECT)
        list(APPEND guest_extract_args --extract-symbol
            "?function_137550@@YAPAUs_type_7ba8e9@@JF@Z")
    endif()
    if(object STREQUAL H2_GUEST_WIDGET_INDEX_OBJECT)
        list(APPEND guest_extract_args --extract-symbol
            "?function_bb760@@YAJF@Z")
    endif()
    if(object STREQUAL H2_GUEST_APPEARANCE_OBJECT)
        list(APPEND guest_extract_args --extract-symbol
            "?function_1392f8@@YAXPBUs_player_appearance@@@Z")
    endif()
    if(object STREQUAL H2_GUEST_PLAYER_COLORS_OBJECT)
        list(APPEND guest_extract_args --extract-symbol
            "?function_7f790@@YAXF_NPBUs_player_appearance@@PAUcolor3f@@@Z"
            --extract-symbol "?function_7f720@@YAPAUcolor3f@@PAU1@F@Z")
    endif()
    if(object STREQUAL H2_GUEST_PACK_COLOR_OBJECT)
        list(APPEND guest_extract_args --extract-symbol
            "?pack_color3f@@YAKPBUcolor3f@@@Z")
    endif()
    if(object STREQUAL H2_GUEST_COLOR_TABLE_OBJECT)
        list(APPEND guest_extract_args
            --extract-symbol "?function_1a06a0@@YAPAUcolor3f@@JPAU1@@Z"
            --extract-symbol "?function_1a06f0@@YAXPAUs_key_set@@PAJ1@Z"
            --extract-symbol "?function_1a03a0@@YAJJPAG@Z"
            --extract-symbol "?function_1a0540@@YA_NPAUs_player_profile_settings@@J@Z"
            --extract-symbol "?function_1a07b0@@YAXPAUs_player_profile@@J@Z"
            --extract-global "??_C@_1M@CBFBMPEI@?$AAG?$AAu?$AAe?$AAs?$AAt?$AA?$AA@")
    endif()
    if(object STREQUAL H2_GUEST_TIME_QUERY_OBJECT)
        list(APPEND guest_extract_args
            --extract-symbol "?function_1462b0@@YAKXZ"
            --extract-symbol "?function_146840@@YAJXZ")
    endif()
    if(object STREQUAL H2_GUEST_SIMULATION_QUERY_OBJECT)
        list(APPEND guest_extract_args --extract-symbol
            "?function_68290@@YA_NXZ")
    endif()
    if(object STREQUAL H2_GUEST_SESSION_PRESENCE_OBJECT)
        list(APPEND guest_extract_args
            --extract-symbol "?function_19987f@@YAXXZ"
            --extract-symbol "?function_1999d7@@YA_NXZ"
            --extract-symbol "?function_199967@@YA_NXZ"
            --extract-symbol "?function_199971@@YA_NXZ"
            --extract-symbol "?function_1999b3@@YA_NXZ"
            --extract-symbol "?function_1999f9@@YA_NXZ"
            --extract-symbol "?function_199ebc@@YAJXZ"
            --extract-symbol "?function_199f34@@YAJXZ"
            --extract-symbol "?function_199f6d@@YAJXZ"
            --extract-symbol "?function_199e3c@@YAXJ@Z"
            --extract-symbol "?function_19a279@@YAJXZ"
            --extract-global "switch.table.?function_199b08@@YAXXZ")
    endif()
    if(object STREQUAL H2_GUEST_SESSION_MANAGER_OBJECT)
        list(APPEND guest_extract_args --extract-symbol
            "?network_session_manager_check_joining_leader@@YAXXZ"
            --extract-symbol "?function_59670@@YA_NPAPAVc_class_58d20@@@Z")
    endif()
    if(object STREQUAL H2_GUEST_TASK_SCREEN_OBJECT)
        list(APPEND guest_extract_args
            --extract-symbol "??_Gc_online_task_screen@@UAAPAXI@Z"
            --extract-symbol "?get_load_proc@c_online_task_screen@@UAAP6APAVc_class_1473c9@@PAUs_screen_parameters@@@ZXZ"
            --extract-symbol "?online_task_screen_finish@@YAXPAVc_online_task_screen@@@Z"
            --extract-symbol "?online_task_screen_load@@YAPAVc_class_1473c9@@PAUs_screen_parameters@@@Z"
            --extract-symbol "?set_description@c_online_task_screen@@QAAXJ@Z"
            --extract-symbol "?v10@c_online_task_screen@@UAA_NPAUs_widget_event@@@Z"
            --extract-symbol "?v3@c_online_task_screen@@UAAXXZ"
            --extract-global "??_7c_online_task_screen@@6B@"
            --extract-alias "??_Ec_online_task_screen@@UAAPAXI@Z")
    endif()
    if(object STREQUAL H2_GUEST_TASK_CALLBACK_OBJECT)
        list(APPEND guest_extract_args --extract-symbol
            "?function_2523bc@@YAXPAVc_online_task_screen@@@Z")
    endif()
    if(object STREQUAL H2_GUEST_SESSION_MODE_OBJECT)
        list(APPEND guest_extract_args --extract-symbol
            "?function_0592d0@@YAKXZ")
    endif()
    if(object STREQUAL H2_GUEST_SESSION_VALUES_OBJECT)
        list(APPEND guest_extract_args --extract-symbol
            "?get_values_4d08@c_class_58d20@@QAA_NPAJ0PAPAE@Z")
    endif()
    if(object STREQUAL H2_GUEST_SAVED_GAME_INDEX_OBJECT)
        list(APPEND guest_extract_args
            --extract-symbol "?function_217080@@YAXPBUs_game_variant@@@Z"
            --extract-symbol "?function_217300@@YAJJJ@Z")
    endif()
    if(object STREQUAL H2_GUEST_SCREEN_SETTERS_OBJECT)
        list(APPEND guest_extract_args
            --extract-symbol "??_Gc_screen_45bc60@@UAAPAXI@Z"
            --extract-symbol "??_Gc_screen_45bcd0@@UAAPAXI@Z"
            --extract-symbol "??_Gc_screen_45bd40@@UAAPAXI@Z"
            --extract-symbol "?function_2b0a14@@YAXPAUs_widget_view_2b0a@@F@Z"
            --extract-symbol "?function_2b0a57@@YA_NPAUs_widget_view_2b0a@@@Z"
            --extract-symbol "?function_2b72e6@@YAPAVc_class_1473c9@@PAUs_screen_parameters@@@Z"
            --extract-symbol "?function_2b7333@@YAPAVc_class_1473c9@@PAUs_screen_parameters@@@Z"
            --extract-symbol "?function_2b739a@@YAPAVc_class_1473c9@@PAUs_screen_parameters@@@Z"
            --extract-symbol "?get_load_proc@c_screen_45bc60@@UAAP6APAVc_class_1473c9@@PAUs_screen_parameters@@@ZXZ"
            --extract-symbol "?get_load_proc@c_screen_45bcd0@@UAAP6APAVc_class_1473c9@@PAUs_screen_parameters@@@ZXZ"
            --extract-symbol "?get_load_proc@c_screen_45bd40@@UAAP6APAVc_class_1473c9@@PAUs_screen_parameters@@@ZXZ"
            --extract-symbol "?invoke@c_list_item_handler@@UAAXPAPAUs_controller_reference@@PAJ@Z"
            --extract-symbol "?set_bitmap@c_screen_45bd40@@QAAXF@Z"
            --extract-symbol "?set_text@c_screen_45bd40@@QAAXPBD@Z"
            --extract-symbol "?v3@c_screen_45bd40@@UAAXXZ"
            --extract-symbol "?v18@c_screen_45bc60@@UAAXPAX@Z"
            --extract-symbol "?v18@c_screen_45bd40@@UAAXPAX@Z"
            --extract-symbol "?function_2b0a48@@YA_NPAUs_widget_view_2b0a@@@Z"
            --extract-symbol "?function_2b0a68@@YAJPAUs_widget_view_2b0a@@@Z"
            --extract-symbol "?function_2b0a7b@@YAXPAUs_widget_view_2b0a@@PAUs_type_7ba8e9@@@Z"
            --extract-symbol "?function_2b12ba@@YA_NPAUs_widget_view_2b0a@@@Z"
            --extract-symbol "?function_2b10a3@@YAXPBKPAVc_class_2b0b5e@@J@Z"
            --extract-symbol "?function_2b014f@@YAJPAUs_widget_item@@@Z"
            --extract-symbol "?function_2b015b@@YAJPAUs_widget_item@@@Z"
            --extract-symbol "?function_2b016d@@YAJPAUs_widget_item@@@Z"
            --extract-symbol "?function_2b0180@@YA_NPAUs_widget_item@@@Z"
            --extract-symbol "?function_2b0191@@YA_NPAUs_widget_item@@@Z"
            --extract-symbol "?function_2b01c7@@YAJPAUs_widget_item@@@Z"
            --extract-symbol "?function_2b01da@@YAJPAUs_widget_item@@@Z"
            --extract-symbol "?function_2b12ca@@YAXPAUs_widget_view_2b0a@@FFPBX@Z"
            --extract-symbol "?v4@c_class_2b01eb@@UAAXJ@Z"
            --extract-symbol "?v1@c_class_2b0b5e@@UAAXXZ"
            --extract-symbol "?v3@c_widget_45adf0@@UAAXXZ"
            --extract-symbol "?v4@c_widget_45adf0@@UAAXJ@Z"
            --extract-symbol "??0c_level_select_screen@@QAA@JJG_N@Z"
            --extract-symbol "??_Gc_level_select_screen@@UAAPAXI@Z"
            --extract-symbol "?function_2b130a@@YAPAVc_class_1473c9@@PAUs_screen_parameters@@@Z"
            --extract-symbol "?function_2b136d@@YAPAVc_class_1473c9@@PAUs_screen_parameters@@@Z"
            --extract-symbol "?get_load_proc@c_level_select_screen@@UAAP6APAVc_class_1473c9@@PAUs_screen_parameters@@@ZXZ"
            --extract-symbol "?v18@c_level_select_screen@@UAAXPAX@Z"
            --extract-global "??_7c_level_select_screen@@6B@"
            --extract-alias "??_Ec_level_select_screen@@UAAPAXI@Z"
            --extract-symbol "??0c_difficulty_screen@@QAA@JJG@Z"
            --extract-symbol "??_Gc_difficulty_screen@@UAAPAXI@Z"
            --extract-symbol "?function_2bb3ed@@YAPAVc_class_1473c9@@PAUs_screen_parameters@@@Z"
            --extract-symbol "?function_2bb432@@YAPAVc_class_1473c9@@PAUs_screen_parameters@@@Z"
            --extract-symbol "?get_load_proc@c_difficulty_screen@@UAAP6APAVc_class_1473c9@@PAUs_screen_parameters@@@ZXZ"
            --extract-global "??_7c_difficulty_screen@@6B@"
            --extract-alias "??_Ec_difficulty_screen@@UAAPAXI@Z"
            --extract-symbol "??0c_campaign_options_list@@QAA@G@Z"
            --extract-symbol "?handle_item@c_campaign_options_list@@QAAXPAPAUs_controller_reference@@PAJ@Z"
            --extract-symbol "?v2@c_campaign_options_list@@UAAXXZ"
            --extract-symbol "?v3@c_campaign_options_list@@UAAXXZ"
            --extract-symbol "?get_item_data@c_campaign_options_list@@UAAPAXXZ"
            --extract-symbol "?get_item_count@c_campaign_options_list@@UAAJXZ"
            --extract-symbol "?v20@c_campaign_options_list@@UAAXPAVc_class_1a2c81@@J@Z"
            --extract-symbol "?function_2b18b7@@YAFPAVc_class_1474e8@@@Z"
            --extract-symbol "?v3@c_screen_458a00@@UAAXXZ"
            --extract-symbol "??_Gc_campaign_options_list@@UAAPAXI@Z"
            --extract-global "??_7c_campaign_options_list@@6B@"
            --extract-global "??_C@_0BG@HIBDOIKC@campaign?5options?5list?$AA@"
            --extract-global "switch.table.?v3@c_screen_458a00@@UAAXXZ"
            --extract-global "??_7c_screen_45bc60@@6B@"
            --extract-global "??_7c_screen_45bcd0@@6B@"
            --extract-global "??_7c_screen_45bd40@@6B@"
            --extract-alias "??_Ec_campaign_options_list@@UAAPAXI@Z"
            --extract-alias "??_Ec_screen_45bc60@@UAAPAXI@Z"
            --extract-alias "??_Ec_screen_45bcd0@@UAAPAXI@Z"
            --extract-alias "??_Ec_screen_45bd40@@UAAPAXI@Z")
    endif()
    if(object STREQUAL H2_GUEST_CAMPAIGN_LEVEL_LIST_OBJECT)
        list(APPEND guest_extract_args
            --extract-symbol "??0c_campaign_level_handles_list@@QAA@G_N@Z"
            --extract-symbol "??_Gc_campaign_level_handles_list@@UAAPAXI@Z"
            --extract-symbol "?get_item_data@c_campaign_level_handles_list@@UAAPAXXZ"
            --extract-symbol "?get_item_count@c_campaign_level_handles_list@@UAAJXZ"
            --extract-symbol "?handle_item@c_campaign_level_handles_list@@QAAXPAPAUs_controller_reference@@PAJ@Z"
            --extract-symbol "?v1@c_campaign_level_handles_list@@UAAXXZ"
            --extract-symbol "?v20@c_campaign_level_handles_list@@UAAXPAVc_class_1a2c81@@J@Z"
            --extract-symbol "?v2@c_level_select_screen@@UAAXXZ"
            --extract-symbol "?v3@c_level_select_screen@@UAAXXZ"
            --extract-symbol "??0c_difficulty_list@@QAA@G@Z"
            --extract-symbol "??_Gc_difficulty_list@@UAAPAXI@Z"
            --extract-symbol "?get_item_data@c_difficulty_list@@UAAPAXXZ"
            --extract-symbol "?get_item_count@c_difficulty_list@@UAAJXZ"
            --extract-symbol "?handle_item@c_difficulty_list@@QAAXPAPAUs_controller_reference@@PAJ@Z"
            --extract-symbol "?v1@c_difficulty_list@@UAAXXZ"
            --extract-symbol "?v20@c_difficulty_list@@UAAXPAVc_class_1a2c81@@J@Z"
            --extract-global "??_7c_difficulty_list@@6B@"
            --extract-global "??_C@_0BA@JHMFDLGM@difficulty?5list?$AA@"
            --extract-global "switch.table.?v20@c_difficulty_list@@UAAXPAVc_class_1a2c81@@J@Z"
            --extract-alias "??_Ec_difficulty_list@@UAAPAXI@Z"
            --extract-global "??_7c_campaign_level_handles_list@@6B@"
            --extract-global "??_C@_0BH@JBNGBMEB@campaign?5level?5handles?$AA@"
            --extract-alias "??_Ec_campaign_level_handles_list@@UAAPAXI@Z")
    endif()
    if(object STREQUAL H2_GUEST_DATA_ITERATOR_OBJECT)
        list(APPEND guest_extract_args --extract-symbol
            "?data_datum_iterator_next@@YA_NPAUs_data_datum_iterator@@@Z")
    endif()
    if(object STREQUAL H2_GUEST_LOCALIZED_LEVEL_TEXT_OBJECT)
        list(APPEND guest_extract_args
            --extract-symbol "?localized_description_get@@YAPAGPAUs_localized_description@@@Z"
            --extract-symbol "?localized_name_get@@YAPAGPAUs_localized_name@@@Z")
    endif()
    if(object STREQUAL H2_GUEST_CAMPAIGN_SCREEN_OBJECT)
        list(APPEND guest_extract_args
            --extract-symbol "?function_230616@@YAPAVc_class_1473c9@@PAUs_screen_parameters@@@Z"
            --extract-symbol "??0c_screen_458a00@@QAA@JJG@Z"
            --extract-symbol "?get_load_proc@c_screen_458a00@@UAAP6APAVc_class_1473c9@@PAUs_screen_parameters@@@ZXZ"
            --extract-symbol "??_Gc_screen_458a00@@UAAPAXI@Z"
            --extract-global "??_7c_screen_458a00@@6B@"
            --extract-alias "??_Ec_screen_458a00@@UAAPAXI@Z")
    endif()
    if(object STREQUAL H2_GUEST_SCREEN_WIDGETS_OBJECT)
        list(APPEND guest_extract_args
            --extract-symbol "??0c_class_2b01eb@@QAA@PAUs_bitmap_block@@@Z"
            --extract-symbol "?function_2afeae@@YAXPAUs_widget_item@@PAVc_class_1a2c81@@@Z"
            --extract-symbol "??0c_class_2b0b5e@@QAA@PAUs_model_block@@@Z"
            --extract-symbol "??0c_widget_45ad18@@QAA@JPAUs_widget_block_18@@@Z"
            --extract-symbol "?function_2baf38@@YAPAVc_widget_45adf0@@PAVc_class_1a2c81@@PAUs_widget_block_24@@@Z"
            --extract-symbol "?place@c_widget_45ad18@@QAAXPAUs_widget_point@@@Z"
            --extract-symbol "??_Gc_class_2b01eb@@UAAPAXI@Z"
            --extract-symbol "??_Gc_class_2b0b5e@@UAAPAXI@Z"
            --extract-symbol "??_Gc_widget_45ad18@@UAAPAXI@Z"
            --extract-symbol "??_Gc_widget_45adf0@@UAAPAXI@Z"
            --extract-global "??_7c_class_2b01eb@@6B@"
            --extract-global "??_7c_class_2b0b5e@@6B@"
            --extract-global "??_7c_widget_45ad18@@6B@"
            --extract-global "??_7c_widget_45adf0@@6B@"
            --extract-global "??_C@_11LOCGONAA@?$AA?$AA@"
            --extract-alias "??_Ec_class_2b01eb@@UAAPAXI@Z"
            --extract-alias "??_Ec_class_2b0b5e@@UAAPAXI@Z"
            --extract-alias "??_Ec_widget_45ad18@@UAAPAXI@Z"
            --extract-alias "??_Ec_widget_45adf0@@UAAPAXI@Z")
    endif()
    if(object STREQUAL H2_GUEST_WIDGET18_OBJECT)
        list(APPEND guest_extract_args --extract-symbol
            "?v1@c_widget_45ad18@@UAAXXZ")
    endif()
    if(object STREQUAL H2_GUEST_DIALOG_EVENT_OBJECT)
        list(APPEND guest_extract_args
            --extract-symbol "?function_24c7c1@@YAXPAUs_item_list@@J@Z"
            --extract-symbol "?function_24c7e4@@YAXPAXPAPAUs_event@@PAJ@Z")
    endif()
    if(object STREQUAL H2_GUEST_SCREEN_CLEANUP_OBJECT)
        list(APPEND guest_extract_args
            --extract-symbol "?function_22fba9@@YAXPAVc_class_1473c9@@@Z"
            --extract-symbol "?build@c_class_1473c9@@QAAXPAUs_screen_layout@@@Z"
            --extract-symbol "?function_22fa30@@YAPAVc_class_1a2c81@@PAV1@PAUs_bitmap_block@@@Z")
    endif()
    if(object STREQUAL H2_GUEST_CAMPAIGN_ENTRY_OBJECT)
        list(APPEND guest_extract_args
            --extract-symbol "?function_19c1f0@@YAPAUs_entry_b@@J@Z"
            --extract-symbol "?function_19c270@@YAPAUs_entry_a@@JJ@Z"
            --extract-symbol "?function_19c670@@YAPAUs_record_pool@@XZ"
            --extract-symbol "?function_19c6d0@@YAXPAUs_record_pool@@J@Z"
            --extract-symbol "?function_19c7c0@@YA_NJJPBX@Z"
            --extract-symbol "?function_19c190@@YAXXZ")
    endif()
    if(object STREQUAL H2_GUEST_LEVEL_TABLES_OBJECT)
        list(APPEND guest_extract_args
            --extract-symbol "?level_handle_tables_initialize@@YAXXZ"
            --extract-global "??_C@_0O@HDBDKBPP@level?5handles?$AA@"
            --extract-global "??_C@_0BK@FEMBNBNK@multiplayer?5level?5handles?$AA@")
    endif()
    if(object STREQUAL H2_GUEST_SORT_OBJECT)
        list(APPEND guest_extract_args --extract-symbol
            "?sort_4byte@@YAXPAJKPAXP6A_NJJPBX@Z2@Z")
    endif()
    if(object STREQUAL H2_GUEST_TEXT_PARSER_OBJECT)
        list(APPEND guest_extract_args
            --extract-symbol "?function_22d2ee@@YAXPAGJ@Z"
            --extract-symbol "?parse_text@@YAXPAG@Z")
    endif()
    if(object STREQUAL H2_GUEST_MUTELIST_OBJECT)
        list(APPEND guest_extract_args
            --extract-symbol "?online_mutelist_reset@@YAXJ@Z"
            --extract-symbol "?function_058d70@@YA_NPAVc_class_58d20@@@Z")
    endif()
    if(object STREQUAL H2_GUEST_SESSION_LEAVE_OBJECT)
        list(APPEND guest_extract_args --extract-symbol
            "?function_199e2e@@YAX_N@Z")
    endif()
    if(object STREQUAL H2_GUEST_UI_RECT_OBJECT)
        list(APPEND guest_extract_args
            --extract-symbol "?function_235e5e@@YAXPAUs_sprite_element@@PAUs_float_rect@@1KJJ@Z"
            --extract-symbol "?function_235f31@@YAXPAUs_sprite_element@@PBUs_float_rect@@PBUpoint2f@@KF@Z"
            --extract-symbol "?function_2360c3@@YAXPBUshort_rectangle2d@@PATpoint3f@@@Z"
            --extract-symbol "?function_23661f@@YAXPAUs_dialog_definition@@J@Z"
            --extract-symbol "?function_23618e@@YAPAUs_float_rect@@PAU1@MPBUshort_rectangle2d@@@Z"
            --extract-symbol "?function_23625d@@YAXJ@Z"
            --extract-symbol "?function_2363d4@@YAXPBUshort_rectangle2d@@PAF1@Z"
            --extract-symbol "?function_23643f@@YAXXZ"
            --extract-symbol "?function_23654b@@YAXPAX00@Z")
    endif()
    if(object STREQUAL H2_GUEST_QUIT_CALLBACK_OBJECT)
        list(APPEND guest_extract_args --extract-symbol
            "?function_2323ab@@YA_NJ@Z")
    endif()
    if(object STREQUAL H2_GUEST_MUSIC_SILENCE_OBJECT)
        list(APPEND guest_extract_args --extract-symbol
            "?main_menu_music_silence_done@@YA_NPAUs_main_menu_music@@@Z")
    endif()
    if(object STREQUAL H2_GUEST_SESSION_USER_OBJECT)
        list(APPEND guest_extract_args
            --extract-symbol "?network_session_interface_clear_user@@YAXJ@Z"
            --extract-symbol "?network_session_interface_get_value_18@@YAJXZ"
            --extract-symbol "?network_session_interface_get_value_49a4@@YAJXZ"
            --extract-symbol "?network_session_interface_get_values_4d08@@YA_NPAJ0PAPAE@Z"
            --extract-symbol "?network_session_get_membership@@YA_NPAVc_class_58d20@@PAJ1111PAPAUs_session_member@@1PAKPAPAUs_network_session_player@@@Z"
            --extract-symbol "?network_session_interface_get_data_4db0@@YAPAEXZ"
            --extract-symbol "?network_session_interface_get_user_properties@@YA_NJPAJPAE0@Z")
    endif()
    if(object STREQUAL H2_GUEST_STORAGE_SIZE_OBJECT)
        list(APPEND guest_extract_args
            --extract-symbol "?saved_game_file_size_in_blocks@@YAJJ@Z"
            --extract-symbol "?saved_game_storage_has_free_blocks@@YA_NJ@Z"
            --extract-symbol "?saved_game_file_type_from_variant@@YAJPAUs_game_variant@@@Z"
            --extract-symbol "?saved_game_file_type_size_in_blocks@@YAJJ@Z"
            --extract-global "switch.table.?saved_game_file_type_from_variant@@YAJPAUs_game_variant@@@Z")
    endif()
    if(object STREQUAL H2_GUEST_PROFILE_STORAGE_UPDATE_OBJECT)
        list(APPEND guest_extract_args
            --extract-symbol "?function_215790@@YAXXZ")
    endif()
    if(object STREQUAL H2_GUEST_PREVIEW_RENDER_OBJECT)
        list(APPEND guest_extract_args
            --extract-symbol "?function_22a664@@YAXPBUs_widget_quad_2b11@@PBUs_float_rect@@JJJ@Z")
    endif()
    if(object STREQUAL H2_GUEST_PRESS_START_OBJECT)
        list(APPEND guest_extract_args
            --extract-symbol "?function_22f11e@@YAPAVc_class_1473c9@@PAUs_screen_parameters@@@Z"
            --extract-symbol "?function_22f36f@@YA_NPAX@Z"
            --extract-symbol "?function_22f3a1@@YA_NJ@Z"
            --extract-symbol "??_Gc_press_start_screen@@UAAPAXI@Z"
            --extract-symbol "?get_load_proc@c_press_start_screen@@UAAP6APAVc_class_1473c9@@PAUs_screen_parameters@@@ZXZ"
            --extract-symbol "?handle_start@c_press_start_screen@@QAAXPAPAUs_controller_reference@@PAJ@Z"
            --extract-symbol "?invoke@c_press_start_handler@@UAAXPAPAUs_controller_reference@@PAJ@Z"
            --extract-symbol "?v10@c_press_start_screen@@UAA_NPAUs_widget_event@@@Z"
            --extract-symbol "?v18@c_press_start_screen@@UAAXPAX@Z"
            --extract-symbol "?v3@c_press_start_screen@@UAAXXZ"
            --extract-global "??_7c_press_start_handler@@6B@"
            --extract-global "??_7c_press_start_screen@@6B@"
            --extract-alias "??_Ec_press_start_screen@@UAAPAXI@Z")
    endif()
    if(object STREQUAL H2_GUEST_SIGNIN_OBJECT)
        list(APPEND guest_extract_args
            --extract-symbol "?function_25240c@@YAPAVc_class_1473c9@@PAUs_screen_parameters@@@Z"
            --extract-symbol "?function_252433@@YAPAVc_class_1473c9@@PAUs_screen_parameters@@@Z"
            --extract-symbol "?function_25245a@@YAPAVc_class_1473c9@@PAUs_screen_parameters@@@Z"
            --extract-symbol "?function_252481@@YAPAVc_class_1473c9@@PAUs_screen_parameters@@@Z"
            --extract-symbol "?function_2524a8@@YAPAVc_class_1473c9@@PAUs_screen_parameters@@@Z"
            --extract-symbol "?function_252a32@@YA_NJ@Z"
            --extract-symbol "??_Gc_4way_signin_screen@@UAAPAXI@Z"
            --extract-symbol "?get_load_proc@c_4way_signin_screen@@UAAP6APAVc_class_1473c9@@PAUs_screen_parameters@@@ZXZ"
            --extract-symbol "?v10@c_4way_signin_screen@@UAA_NPAUs_widget_event@@@Z"
            --extract-symbol "?v18@c_4way_signin_screen@@UAAXPAX@Z"
            --extract-symbol "?v3@c_4way_signin_screen@@UAAXXZ"
            --extract-global "??_7c_4way_signin_screen@@6B@"
            --extract-global "switch.table.?get_load_proc@c_4way_signin_screen@@UAAP6APAVc_class_1473c9@@PAUs_screen_parameters@@@ZXZ"
            --extract-alias "??_Ec_4way_signin_screen@@UAAPAXI@Z")
    endif()
    if(object STREQUAL H2_GUEST_WIDGET_ITEM_OBJECT)
        list(APPEND guest_extract_args --extract-symbol "??0s_widget_item@@QAA@XZ")
    endif()
    if(object STREQUAL H2_GUEST_LOCALIZED_TEXT_OBJECT)
        list(APPEND guest_extract_args
            --extract-symbol "?function_1a0180@@YAXJJPAG@Z"
            --extract-symbol "?string_table_load@@YA_NPAUs_string_table@@@Z")
    endif()
    if(object STREQUAL H2_GUEST_UTF8_OBJECT)
        list(APPEND guest_extract_args --extract-symbol "?utf8_string_to_utf16_string@@YAXPBDPAGJ@Z")
    endif()
    if(object STREQUAL H2_GUEST_COLOR_SPACE_OBJECT)
        list(APPEND guest_extract_args
            --extract-symbol "?function_1318d0@@YAPAUhsv3f@@PBUcolor3f@@PAU1@@Z"
            --extract-symbol "?function_131a00@@YAPAUcolor3f@@PBUhsv3f@@PAU1@@Z")
    endif()
    add_custom_command(
        OUTPUT "${object}"
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/guest_cc.py
                --llvm ${H2_LLVM_ROOT}/bin
                --plugin $<TARGET_FILE:h2_guest_rebase>
                --include ${CMAKE_CURRENT_SOURCE_DIR}/include
                --include ${CMAKE_CURRENT_SOURCE_DIR}/include/crt
                --include ${CMAKE_CURRENT_SOURCE_DIR}/include/xdk
                --include ${PROJECT_SOURCE_DIR}/include
                ${guest_abi_args}
                ${guest_extract_args}
                --output "${object}"
                "${source}"
        DEPENDS "${source}" ${H2_GUEST_HEADERS} ${H2_PORT_GUEST_HEADERS}
                ${CMAKE_CURRENT_SOURCE_DIR}/tools/guest_cc.py h2_guest_rebase
        VERBATIM
    )
endforeach()
set_source_files_properties(
    "${H2_GUEST_RANDOM_OBJECT}" "${H2_GUEST_GLOBALS_OBJECT}"
    "${H2_GUEST_CACHE_TAG_OBJECT}" "${H2_GUEST_CRT_OBJECT}"
    "${H2_GUEST_MAP_LOADER_OBJECT}" "${H2_GUEST_CACHE_RUNTIME_OBJECT}"
    "${H2_GUEST_MAP_LOCATION_OBJECT}" "${H2_GUEST_MAP_PROGRESS_OBJECT}"
    "${H2_GUEST_MAP_READY_OBJECT}"
    "${H2_GUEST_RANDOM_ID_OBJECT}" "${H2_GUEST_GAME_OPTIONS_OBJECT}"
    "${H2_GUEST_PROFILE_RECORD_OBJECT}"
    "${H2_GUEST_MAIN_MENU_OBJECT}"
    "${H2_GUEST_WIDGET_BASE_OBJECT}" "${H2_GUEST_SCREEN_BASE_OBJECT}"
    "${H2_GUEST_UI_HEAP_OBJECT}" "${H2_GUEST_UI_MANAGER_OBJECT}"
    "${H2_GUEST_UI_MESSAGE_OBJECT}" "${H2_GUEST_LIST_WIDGET_OBJECT}"
    "${H2_GUEST_DIALOG_CALLBACK_OBJECT}" "${H2_GUEST_WINDOW_CHANNEL_OBJECT}"
    "${H2_GUEST_WINDOW_QUERY_OBJECT}"
    "${H2_GUEST_GAME_STATE_OBJECT}" "${H2_GUEST_CAMPAIGN_STATE_OBJECT}"
    "${H2_GUEST_LIFECYCLE_QUERY_OBJECT}" "${H2_GUEST_TIME_QUERY_OBJECT}"
    "${H2_GUEST_SIMULATION_QUERY_OBJECT}"
    "${H2_GUEST_RECORD_POOL_OBJECT}" "${H2_GUEST_RECORD_POOL_DISPOSE_OBJECT}"
    "${H2_GUEST_RECORD_POOL_PREVIOUS_OBJECT}" "${H2_GUEST_RECORD_POOL_LAST_OBJECT}"
    "${H2_GUEST_UI_HEAP_GLOBALS_OBJECT}" "${H2_GUEST_UI_HEAP_FREE_OBJECT}"
    "${H2_GUEST_LOOP_ALLOCATOR_OBJECT}" "${H2_GUEST_NAME_BUFFER_OBJECT}"
    "${H2_GUEST_NAME_TABLE_OBJECT}" "${H2_GUEST_BITMAP_LOOKUP_OBJECT}"
    "${H2_GUEST_WIDGET_INDEX_OBJECT}"
    "${H2_GUEST_APPEARANCE_OBJECT}" "${H2_GUEST_PLAYER_COLORS_OBJECT}"
    "${H2_GUEST_PACK_COLOR_OBJECT}" "${H2_GUEST_COLOR_TABLE_OBJECT}"
    "${H2_GUEST_UNICODE_OBJECT}" "${H2_GUEST_MS_RUNTIME_OBJECT}"
    "${H2_GUEST_HOST_RUNTIME_OBJECT}" "${H2_GUEST_ONLINE_RUNTIME_OBJECT}"
    "${H2_GUEST_AUDIO_RUNTIME_OBJECT}"
    "${H2_GUEST_RENDER_RUNTIME_OBJECT}"
    "${H2_GUEST_CAMPAIGN_ENTRY_OBJECT}"
    "${H2_GUEST_LEVEL_TABLES_OBJECT}"
    "${H2_GUEST_SORT_OBJECT}"
    "${H2_GUEST_MOVIE_QUERY_OBJECT}"
    "${H2_GUEST_TEXT_STATE_OBJECT}"
    "${H2_GUEST_UI_MESSAGE_BUILD_OBJECT}" "${H2_GUEST_UI_LAYOUT_OBJECT}"
    "${H2_GUEST_VARIANT_INDEX_OBJECT}" "${H2_GUEST_VARIANT_OBJECT}"
    "${H2_GUEST_VARIANT_FIELDS_OBJECT}" "${H2_GUEST_CONTROLLER_INDEX_OBJECT}"
    "${H2_GUEST_ONLINE_AVAILABLE_OBJECT}"
    "${H2_GUEST_SESSION_QUERY_OBJECT}" "${H2_GUEST_PROFILE_UPDATE_OBJECT}"
    "${H2_GUEST_PLAYER_SLOT_SERVICES_OBJECT}" "${H2_GUEST_PLAYER_SLOT_INDEX_OBJECT}"
    "${H2_GUEST_PLAYER_SLOT_GET_OBJECT}" "${H2_GUEST_PLAYER_PROFILE_OBJECT}"
    "${H2_GUEST_PLAYER_SESSION_OBJECT}"
    "${H2_GUEST_MENU_ACTION_OBJECT}" "${H2_GUEST_SESSION_PRESENCE_OBJECT}"
    "${H2_GUEST_SESSION_MANAGER_OBJECT}" "${H2_GUEST_MUTELIST_OBJECT}"
    "${H2_GUEST_TASK_SCREEN_OBJECT}" "${H2_GUEST_TASK_TEXT_OBJECT}" "${H2_GUEST_TASK_CALLBACK_OBJECT}"
    "${H2_GUEST_SESSION_MODE_OBJECT}" "${H2_GUEST_SESSION_VALUES_OBJECT}"
    "${H2_GUEST_SAVED_GAME_INDEX_OBJECT}" "${H2_GUEST_SCREEN_SETTERS_OBJECT}"
    "${H2_GUEST_CAMPAIGN_SCREEN_OBJECT}"
    "${H2_GUEST_CAMPAIGN_LEVEL_LIST_OBJECT}"
    "${H2_GUEST_DATA_ITERATOR_OBJECT}"
    "${H2_GUEST_LOCALIZED_LEVEL_TEXT_OBJECT}"
    "${H2_GUEST_SCREEN_WIDGETS_OBJECT}" "${H2_GUEST_WIDGET18_OBJECT}"
    "${H2_GUEST_DIALOG_OBJECT}" "${H2_GUEST_DIALOG_BASE_OBJECT}" "${H2_GUEST_DIALOG_EVENT_OBJECT}"
    "${H2_GUEST_SCREEN_CLEANUP_OBJECT}" "${H2_GUEST_ACTIVE_SESSION_OBJECT}"
    "${H2_GUEST_PREFERENCES_UPDATE_OBJECT}" "${H2_GUEST_TEXT_PARSER_OBJECT}"
    "${H2_GUEST_MUSIC_SILENCE_OBJECT}" "${H2_GUEST_SESSION_LEAVE_OBJECT}"
    "${H2_GUEST_UI_RECT_OBJECT}" "${H2_GUEST_QUIT_CALLBACK_OBJECT}"
    "${H2_GUEST_SESSION_USER_OBJECT}" "${H2_GUEST_STORAGE_HANDLE_OBJECT}"
    "${H2_GUEST_STORAGE_SIZE_OBJECT}" "${H2_GUEST_PROFILE_STORAGE_UPDATE_OBJECT}"
    "${H2_GUEST_UI_GEOMETRY_OBJECT}" "${H2_GUEST_PREVIEW_RENDER_OBJECT}"
    "${H2_GUEST_PRESS_START_OBJECT}" "${H2_GUEST_SIGNIN_OBJECT}"
    "${H2_GUEST_PROFILE_SCREEN_OBJECT}" "${H2_GUEST_PROFILE_LIST_OBJECT}"
    "${H2_GUEST_VIRTUAL_KEYBOARD_OBJECT}" "${H2_GUEST_PROFILE_NAME_CHECK_OBJECT}"
    "${H2_GUEST_WIDGET_ITEM_OBJECT}"
    "${H2_GUEST_LOCALIZED_TEXT_OBJECT}" "${H2_GUEST_UTF8_OBJECT}"
    "${H2_GUEST_COLOR_SPACE_OBJECT}"
    PROPERTIES GENERATED TRUE EXTERNAL_OBJECT TRUE
)
add_library(h2_recovered_guest STATIC
    "${H2_GUEST_RANDOM_OBJECT}"
    "${H2_GUEST_GLOBALS_OBJECT}"
    "${H2_GUEST_CACHE_TAG_OBJECT}"
    "${H2_GUEST_CRT_OBJECT}"
)
set_target_properties(h2_recovered_guest PROPERTIES LINKER_LANGUAGE CXX)

set(H2_MAIN_MENU_GUEST_OBJECTS
    "${H2_GUEST_CRT_OBJECT}"
    "${H2_GUEST_MAP_LOADER_OBJECT}"
    "${H2_GUEST_CACHE_RUNTIME_OBJECT}"
    "${H2_GUEST_MAP_LOCATION_OBJECT}"
    "${H2_GUEST_MAP_PROGRESS_OBJECT}"
    "${H2_GUEST_MAP_READY_OBJECT}"
    "${H2_GUEST_RANDOM_ID_OBJECT}"
    "${H2_GUEST_GAME_OPTIONS_OBJECT}"
    "${H2_GUEST_PROFILE_RECORD_OBJECT}"
    "${H2_GUEST_TAG_INITIALIZE_OBJECT}"
    "${H2_GUEST_PART_INITIALIZE_OBJECT}"
    "${H2_GUEST_VEHICLE_INITIALIZE_OBJECT}"
    "${H2_GUEST_PHYSICS_RUNTIME_OBJECT}"
    "${H2_GUEST_PHYSICS_TYPE_OBJECT}"
    "${H2_GUEST_PHYSICS_RESOLVER_OBJECT}"
    "${H2_GUEST_COLLISION_INITIALIZE_OBJECT}"
    "${H2_GUEST_MAIN_MENU_OBJECT}"
    "${H2_GUEST_WIDGET_BASE_OBJECT}"
    "${H2_GUEST_SCREEN_BASE_OBJECT}"
    "${H2_GUEST_UI_HEAP_OBJECT}"
    "${H2_GUEST_UI_MANAGER_OBJECT}"
    "${H2_GUEST_UI_MESSAGE_OBJECT}"
    "${H2_GUEST_LIST_WIDGET_OBJECT}"
    "${H2_GUEST_DIALOG_CALLBACK_OBJECT}"
    "${H2_GUEST_WINDOW_CHANNEL_OBJECT}"
    "${H2_GUEST_WINDOW_QUERY_OBJECT}"
    "${H2_GUEST_GAME_STATE_OBJECT}"
    "${H2_GUEST_CAMPAIGN_STATE_OBJECT}"
    "${H2_GUEST_LIFECYCLE_QUERY_OBJECT}" "${H2_GUEST_TIME_QUERY_OBJECT}"
    "${H2_GUEST_SIMULATION_QUERY_OBJECT}"
    "${H2_GUEST_RECORD_POOL_OBJECT}"
    "${H2_GUEST_RECORD_POOL_DISPOSE_OBJECT}"
    "${H2_GUEST_RECORD_POOL_PREVIOUS_OBJECT}"
    "${H2_GUEST_RECORD_POOL_LAST_OBJECT}"
    "${H2_GUEST_UI_HEAP_GLOBALS_OBJECT}"
    "${H2_GUEST_UI_HEAP_FREE_OBJECT}"
    "${H2_GUEST_LOOP_ALLOCATOR_OBJECT}"
    "${H2_GUEST_NAME_BUFFER_OBJECT}"
    "${H2_GUEST_NAME_TABLE_OBJECT}"
    "${H2_GUEST_BITMAP_LOOKUP_OBJECT}"
    "${H2_GUEST_WIDGET_INDEX_OBJECT}"
    "${H2_GUEST_APPEARANCE_OBJECT}"
    "${H2_GUEST_PLAYER_COLORS_OBJECT}"
    "${H2_GUEST_PACK_COLOR_OBJECT}"
    "${H2_GUEST_COLOR_TABLE_OBJECT}"
    "${H2_GUEST_UNICODE_OBJECT}"
    "${H2_GUEST_MS_RUNTIME_OBJECT}"
    "${H2_GUEST_HOST_RUNTIME_OBJECT}"
    "${H2_GUEST_ONLINE_RUNTIME_OBJECT}"
    "${H2_GUEST_AUDIO_RUNTIME_OBJECT}"
    "${H2_GUEST_RENDER_RUNTIME_OBJECT}"
    "${H2_GUEST_CAMPAIGN_ENTRY_OBJECT}"
    "${H2_GUEST_LEVEL_TABLES_OBJECT}"
    "${H2_GUEST_SORT_OBJECT}"
    "${H2_GUEST_MOVIE_QUERY_OBJECT}"
    "${H2_GUEST_TEXT_STATE_OBJECT}"
    "${H2_GUEST_UI_MESSAGE_BUILD_OBJECT}"
    "${H2_GUEST_UI_LAYOUT_OBJECT}"
    "${H2_GUEST_VARIANT_INDEX_OBJECT}"
    "${H2_GUEST_VARIANT_OBJECT}"
    "${H2_GUEST_VARIANT_FIELDS_OBJECT}"
    "${H2_GUEST_CONTROLLER_INDEX_OBJECT}"
    "${H2_GUEST_ONLINE_AVAILABLE_OBJECT}"
    "${H2_GUEST_SESSION_QUERY_OBJECT}"
    "${H2_GUEST_PROFILE_UPDATE_OBJECT}"
    "${H2_GUEST_PLAYER_SLOT_SERVICES_OBJECT}"
    "${H2_GUEST_PLAYER_SLOT_INDEX_OBJECT}"
    "${H2_GUEST_PLAYER_SLOT_GET_OBJECT}"
    "${H2_GUEST_PLAYER_PROFILE_OBJECT}"
    "${H2_GUEST_PLAYER_SESSION_OBJECT}"
    "${H2_GUEST_MENU_ACTION_OBJECT}"
    "${H2_GUEST_SESSION_PRESENCE_OBJECT}"
    "${H2_GUEST_SESSION_MANAGER_OBJECT}"
    "${H2_GUEST_MUTELIST_OBJECT}"
    "${H2_GUEST_TASK_SCREEN_OBJECT}"
    "${H2_GUEST_TASK_TEXT_OBJECT}"
    "${H2_GUEST_TASK_CALLBACK_OBJECT}"
    "${H2_GUEST_SESSION_MODE_OBJECT}"
    "${H2_GUEST_SESSION_VALUES_OBJECT}"
    "${H2_GUEST_SAVED_GAME_INDEX_OBJECT}"
    "${H2_GUEST_SCREEN_SETTERS_OBJECT}"
    "${H2_GUEST_CAMPAIGN_SCREEN_OBJECT}"
    "${H2_GUEST_CAMPAIGN_LEVEL_LIST_OBJECT}"
    "${H2_GUEST_DATA_ITERATOR_OBJECT}"
    "${H2_GUEST_LOCALIZED_LEVEL_TEXT_OBJECT}"
    "${H2_GUEST_SCREEN_WIDGETS_OBJECT}"
    "${H2_GUEST_WIDGET18_OBJECT}"
    "${H2_GUEST_DIALOG_OBJECT}"
    "${H2_GUEST_DIALOG_BASE_OBJECT}"
    "${H2_GUEST_DIALOG_EVENT_OBJECT}"
    "${H2_GUEST_SCREEN_CLEANUP_OBJECT}"
    "${H2_GUEST_ACTIVE_SESSION_OBJECT}"
    "${H2_GUEST_PREFERENCES_UPDATE_OBJECT}"
    "${H2_GUEST_TEXT_PARSER_OBJECT}"
    "${H2_GUEST_SESSION_LEAVE_OBJECT}"
    "${H2_GUEST_UI_RECT_OBJECT}"
    "${H2_GUEST_QUIT_CALLBACK_OBJECT}"
    "${H2_GUEST_MUSIC_SILENCE_OBJECT}"
    "${H2_GUEST_SESSION_USER_OBJECT}"
    "${H2_GUEST_STORAGE_HANDLE_OBJECT}"
    "${H2_GUEST_STORAGE_SIZE_OBJECT}"
    "${H2_GUEST_PROFILE_STORAGE_UPDATE_OBJECT}"
    "${H2_GUEST_UI_GEOMETRY_OBJECT}"
    "${H2_GUEST_PREVIEW_RENDER_OBJECT}"
    "${H2_GUEST_PRESS_START_OBJECT}"
    "${H2_GUEST_SIGNIN_OBJECT}"
    "${H2_GUEST_PROFILE_SCREEN_OBJECT}"
    "${H2_GUEST_PROFILE_LIST_OBJECT}"
    "${H2_GUEST_VIRTUAL_KEYBOARD_OBJECT}"
    "${H2_GUEST_PROFILE_NAME_CHECK_OBJECT}"
    "${H2_GUEST_WIDGET_ITEM_OBJECT}"
    "${H2_GUEST_LOCALIZED_TEXT_OBJECT}"
    "${H2_GUEST_UTF8_OBJECT}"
    "${H2_GUEST_COLOR_SPACE_OBJECT}"
)
add_library(h2_main_menu_guest STATIC ${H2_MAIN_MENU_GUEST_OBJECTS})
set_target_properties(h2_main_menu_guest PROPERTIES LINKER_LANGUAGE CXX)

set(H2_MAIN_MENU_BINDINGS
    "${CMAKE_CURRENT_SOURCE_DIR}/runtime/main_menu_bindings.json")
set(H2_MAIN_MENU_CLOSURE_REPORT
    "${CMAKE_BINARY_DIR}/port/main-menu-symbol-closure.json")
set(H2_MAIN_MENU_CLOSURE_OBJECTS
    ${H2_MAIN_MENU_GUEST_OBJECTS}
    "${H2_GUEST_PHYSICS_MODEL_PROVIDERS_OBJECT}"
    "${H2_GUEST_PHYSICS_MODEL_INITIALIZE_OBJECT}"
)
add_custom_command(
    OUTPUT "${H2_MAIN_MENU_CLOSURE_REPORT}"
    COMMAND ${Python3_EXECUTABLE}
            ${CMAKE_CURRENT_SOURCE_DIR}/tools/guest_symbol_closure.py
            --nm ${H2_LLVM_NM}
            --bindings ${H2_MAIN_MENU_BINDINGS}
            --format json
            --output "${H2_MAIN_MENU_CLOSURE_REPORT}"
            ${H2_MAIN_MENU_CLOSURE_OBJECTS}
    DEPENDS ${H2_MAIN_MENU_CLOSURE_OBJECTS} ${H2_MAIN_MENU_BINDINGS}
            ${CMAKE_CURRENT_SOURCE_DIR}/tools/guest_symbol_closure.py
    VERBATIM
)
add_custom_target(h2-main-menu-closure-report ALL
    DEPENDS "${H2_MAIN_MENU_CLOSURE_REPORT}")

set(H2_MAIN_MENU_IMPORT_PLAN
    "${CMAKE_BINARY_DIR}/port/main-menu-import-plan.json")
add_custom_command(
    OUTPUT "${H2_MAIN_MENU_IMPORT_PLAN}"
    COMMAND ${Python3_EXECUTABLE}
            ${CMAKE_CURRENT_SOURCE_DIR}/tools/guest_import_plan.py
            --closure "${H2_MAIN_MENU_CLOSURE_REPORT}"
            --source-root "${PROJECT_SOURCE_DIR}/src"
            --expect-unresolved 16
            --output "${H2_MAIN_MENU_IMPORT_PLAN}"
    DEPENDS "${H2_MAIN_MENU_CLOSURE_REPORT}" ${H2_RECOVERED_SOURCES}
            ${CMAKE_CURRENT_SOURCE_DIR}/tools/guest_import_plan.py
    VERBATIM
)
add_custom_target(h2-main-menu-import-plan ALL
    DEPENDS "${H2_MAIN_MENU_IMPORT_PLAN}")

set(H2_MAIN_MENU_ELF_OBJECTS)
foreach(source IN LISTS H2_MS_ABI_SOURCES)
    get_filename_component(unit "${source}" NAME_WE)
    set(object "${H2_GUEST_OBJECT_DIR}/elf/${unit}.o")
    set(guest_extract_args)
    if(source STREQUAL "${PROJECT_SOURCE_DIR}/src/unknown_11c840.cpp")
        list(APPEND guest_extract_args
            --extract-symbol "?function_11c840@@YAXXZ"
            --extract-symbol "h2_native_tag_initialization_status"
            --extract-symbol "h2_native_physics_model_census"
            --extract-symbol "h2_native_physics_model_plan_status"
            --extract-global "g_native_initialized_tags"
            --extract-global "g_native_deferred_tags"
            --extract-global "g_native_physics_model_nonzero_mask"
            --extract-global "g_native_physics_model_entry_count"
            --extract-global "g_native_physics_model_planned_objects"
            --extract-global "g_native_physics_model_planned_resolutions"
            --extract-global "g_native_physics_model_plan_valid")
    endif()
    if(source STREQUAL "${PROJECT_SOURCE_DIR}/src/unknown_1c22c0.cpp")
        list(APPEND guest_extract_args
            --extract-symbol "?function_1c2390@s_part_table@@QAAXXZ"
            --extract-symbol "??_Gc_part_a@@UAAPAXI@Z"
            --extract-symbol "??_Gc_part_b@@UAAPAXI@Z"
            --extract-symbol "??_Gc_part_c@@UAAPAXI@Z"
            --extract-global "??_7c_part_a@@6B@"
            --extract-global "??_7c_part_b@@6B@"
            --extract-global "??_7c_part_c@@6B@"
            --extract-alias "??_Ec_part_a@@UAAPAXI@Z"
            --extract-alias "??_Ec_part_b@@UAAPAXI@Z"
            --extract-alias "??_Ec_part_c@@UAAPAXI@Z")
    endif()
    if(source STREQUAL "${PROJECT_SOURCE_DIR}/src/unknown_205550.cpp")
        list(APPEND guest_extract_args
            --extract-symbol "?function_205550@@YAXPAX@Z"
            --extract-symbol "?function_205551@c_205550@@UAAXXZ"
            --extract-symbol "?function_205551@c_205554@@UAAXXZ"
            --extract-symbol "?function_205552@c_205558@@UAAXXZ"
            --extract-symbol "?function_205553@c_205558@@UAAXXZ"
            --extract-global "??_7c_205550@@6B@"
            --extract-global "??_7c_205554@@6B@")
    endif()
    if(source STREQUAL "${PROJECT_SOURCE_DIR}/src/unknown_246c60.cpp")
        list(APPEND guest_extract_args
            --extract-symbol "?function_246c60@@YAXPAXJ@Z"
            --extract-symbol "h2_native_physics_runtime_vtable"
            --extract-symbol "h2_native_physics_runtime_reset"
            --extract-symbol "h2_native_physics_runtime_object_count"
            --extract-symbol "?h2_native_physics_delete@@YAPAXPAXK@Z"
            --extract-symbol "?h2_native_physics_query@@YAJPAXJJ@Z"
            --extract-symbol "?h2_native_physics_enable@@YAXPAX_N@Z"
            --extract-symbol "?h2_native_physics_root_type@@YAPAXPAX@Z"
            --extract-symbol "?h2_native_physics_nested_type@@YAPAXPAX@Z"
            --extract-symbol "?h2_native_physics_wrapper_type@@YAPAXPAX@Z"
            --extract-symbol "?h2_native_physics_idle@@YAXPAX@Z"
            --extract-global "g_native_physics_runtime_vtables"
            --extract-global "g_native_physics_runtime_object_count")
    endif()
    if(source STREQUAL "${PROJECT_SOURCE_DIR}/src/unknown_2fbb30.cpp")
        list(APPEND guest_extract_args
            --extract-symbol "?function_2fbb30@@YAPAXXZ"
            --extract-symbol "?function_2fdfe0@@YAPAXXZ"
            --extract-symbol "?function_2fbfe0@@YAPAXXZ")
    endif()
    if(source STREQUAL "${PROJECT_SOURCE_DIR}/src/unknown_2122b0.cpp")
        list(APPEND guest_extract_args --extract-symbol
            "?function_2122b0@@YAXJ@Z")
    endif()
    if(source STREQUAL "${PROJECT_SOURCE_DIR}/src/unknown_16bc40.cpp")
        list(APPEND guest_extract_args --extract-symbol
            "?data_last_index@@YAJPAUs_record_pool@@@Z")
    endif()
    if(source STREQUAL "${PROJECT_SOURCE_DIR}/src/unknown_24b5bc.cpp")
        list(APPEND guest_extract_args
            --extract-symbol "?function_24bac5@@YAXPAX@Z"
            --extract-symbol "?initialize@s_player_slot_profile@@QAAXJ@Z"
            --extract-symbol "?set_profile_index@s_player_slot_profile@@QAAXJ@Z"
            --extract-symbol "?sign_in@s_player_slot_profile@@QAAXP6AXJ_N@Z@Z")
    endif()
    if(source STREQUAL "${PROJECT_SOURCE_DIR}/src/unknown_18f576.cpp")
        list(APPEND guest_extract_args
            --extract-symbol "?function_18f58c@@YAXXZ"
            --extract-symbol "?function_18f5e3@@YAXXZ"
            --extract-symbol "?player_slot_count_active@@YAFXZ"
            --extract-symbol "?player_slot_find_controller@@YAJJ@Z"
            --extract-symbol "?player_slot_get_profile@@YAXJPAUs_player_profile@@PAJ@Z"
            --extract-symbol "?player_slot_get_single_profile@@YAJXZ"
            --extract-symbol "?player_slot_get_single_profile_index@@YAJXZ"
            --extract-symbol "?function_18fa4d@@YAJJ@Z"
            --extract-symbol "?function_18fb34@@YAXJPAUs_player_profile_settings@@J@Z")
    endif()
    if(source STREQUAL "${PROJECT_SOURCE_DIR}/src/unknown_18f42d.cpp")
        list(APPEND guest_extract_args
            --extract-symbol "?function_18f42d@@YAPAVc_class_1473c9@@PAUs_screen_parameters@@@Z"
            --extract-symbol "?function_18f474@@YAPAVc_class_1473c9@@PAUs_screen_parameters@@@Z"
            --extract-symbol "?v17@c_18f4bb@@UAAXXZ"
            --extract-symbol "?get_load_proc@c_18f4bb@@UAAP6APAVc_class_1473c9@@PAUs_screen_parameters@@@ZXZ"
            --extract-symbol "??_Gc_18f4bb@@UAAPAXI@Z"
            --extract-global "??_7c_18f4bb@@6B@"
            --extract-alias "??_Ec_18f4bb@@UAAPAXI@Z")
    endif()
    if(source STREQUAL "${PROJECT_SOURCE_DIR}/src/unknown_24a2e7.cpp")
        list(APPEND guest_extract_args
            --extract-symbol "h2_profile_list_construct"
            --extract-symbol "??_Gc_list_24a64d@@UAAPAXI@Z"
            --extract-symbol "?v1@c_list_24a64d@@UAAXXZ"
            --extract-symbol "?v2@c_list_24a64d@@UAAXXZ"
            --extract-symbol "?v3@c_list_24a64d@@UAAXXZ"
            --extract-symbol "?v20@c_list_24a64d@@UAAXPAVc_class_1a2c81@@J@Z"
            --extract-symbol "?v24@c_list_24a64d@@UAAXXZ"
            --extract-symbol "?v23@c_list_widget_26@@UAAXXZ"
            --extract-symbol "?v25@c_list_widget_26@@UAAXXZ"
            --extract-symbol "?function_24a30e@c_list_24a64d@@QAAXPAPAUs_controller_reference@@PAJ@Z"
            --extract-symbol "?function_24a64d@c_list_24a64d@@UAAPAXJ@Z"
            --extract-symbol "?function_24a691@c_list_24a64d@@UAAPBGJ_N@Z"
            --extract-symbol "?get_item_data@c_list_24a64d@@UAAPAXXZ"
            --extract-symbol "?get_item_count@c_list_24a64d@@UAAJXZ"
            --extract-symbol "?get_items@c_list_24a64d@@UAAPAXPAJ@Z"
            --extract-global "??_7c_list_24a64d@@6B@"
            --extract-alias "??_Ec_list_24a64d@@UAAPAXI@Z"
            --extract-global "??_C@_0BE@FKGOBLBN@player?5profile?5list?$AA@")
    endif()
    if(source STREQUAL "${PROJECT_SOURCE_DIR}/src/unknown_23760b.cpp")
        list(APPEND guest_extract_args
            --extract-symbol "?function_238c21@@YAXJJPAGJ@Z"
            --extract-symbol "h2_native_virtual_keyboard_accept"
            --extract-symbol "?function_23784f@@YAPAVc_class_1473c9@@PAUs_screen_parameters@@@Z"
            --extract-symbol "?v10@c_keyboard_key_widget@@UAA_NPAUs_widget_event@@@Z"
            --extract-symbol "?v17@c_keyboard_key_widget@@UAAJXZ"
            --extract-symbol "?v3@c_virtual_keyboard_screen@@UAAXXZ"
            --extract-symbol "?v4@c_virtual_keyboard_screen@@UAAXJ@Z"
            --extract-symbol "?v10@c_virtual_keyboard_screen@@UAA_NPAUs_widget_event@@@Z"
            --extract-symbol "?v18@c_virtual_keyboard_screen@@UAAXPAX@Z"
            --extract-symbol "?get_load_proc@c_virtual_keyboard_screen@@UAAP6APAVc_class_1473c9@@PAUs_screen_parameters@@@ZXZ"
            --extract-symbol "?function_2380c0@@YA_NPAVc_virtual_keyboard_screen@@@Z"
            --extract-symbol "??_Gc_keyboard_key_widget@@UAAPAXI@Z"
            --extract-symbol "??_Gc_virtual_keyboard_screen@@UAAPAXI@Z"
            --extract-global "??_7c_keyboard_key_widget@@6B@"
            --extract-global "??_7c_virtual_keyboard_screen@@6B@"
            --extract-global g_44a540
            --extract-global g_44a600
            --extract-global g_44a6c0
            --extract-global g_44a780
            --extract-global g_44a840
            --extract-global g_44a900
            --extract-global g_44a9bc
            --extract-global g_native_virtual_keyboard
            --extract-alias "??_Ec_keyboard_key_widget@@UAAPAXI@Z"
            --extract-alias "??_Ec_virtual_keyboard_screen@@UAAPAXI@Z")
    endif()
    if(source STREQUAL "${PROJECT_SOURCE_DIR}/src/unknown_216b60.cpp")
        list(APPEND guest_extract_args --extract-symbol
            "?function_216120@@YA_NPAGJ@Z")
    endif()
    if(source STREQUAL "${PROJECT_SOURCE_DIR}/src/unknown_190001.cpp")
        list(APPEND guest_extract_args
            --extract-symbol "?function_1900a5@@YA_NJ@Z"
            --extract-symbol "?function_190186@@YAXJ@Z"
            --extract-symbol "?function_1900be@@YAFXZ"
            --extract-symbol "?function_1900ff@@YAFJ@Z"
            --extract-symbol "?function_1901fc@@YAGXZ"
            --extract-symbol "?function_19022f@@YAGXZ"
            --extract-symbol "?function_19028d@@YA_NXZ"
            --extract-symbol "?function_1904cb@@YA_NJ@Z"
            --extract-symbol "?function_19052c@@YAXJPAJ0@Z"
            --extract-symbol "?function_190565@@YAJXZ"
            --extract-symbol "?function_18fd94@@YAXJPAKPAUs_profile_record@@@Z"
            --extract-symbol "?function_190001@@YAXJPBD@Z"
            --extract-symbol "?function_19060a@@YAXJJ@Z"
            --extract-symbol "?function_1906b4@@YAXXZ"
            --extract-symbol "?function_1906da@@YA_NJ@Z"
            --extract-symbol "?function_1910b8@@YAJJ@Z")
    endif()
    if(source STREQUAL "${PROJECT_SOURCE_DIR}/src/unknown_250155.cpp")
        list(APPEND guest_extract_args --extract-symbol
            "?function_2523b7@@YA_NJ@Z")
    endif()
    if(source STREQUAL "${PROJECT_SOURCE_DIR}/src/unknown_138800.cpp")
        list(APPEND guest_extract_args --extract-symbol
            "?function_138820@@YA_NXZ")
    endif()
    if(source STREQUAL "${PROJECT_SOURCE_DIR}/src/unknown_13bf00.cpp")
        list(APPEND guest_extract_args --extract-symbol
            "?function_13cb40@@YA_NXZ")
    endif()
    if(source STREQUAL "${PROJECT_SOURCE_DIR}/src/unknown_11c050.cpp")
        list(APPEND guest_extract_args --extract-symbol
            "?function_11c0b0@@YAFPBDPAUs_name_table_11c0b0@@@Z")
    endif()
    if(source STREQUAL "${PROJECT_SOURCE_DIR}/src/unknown_137550.cpp")
        list(APPEND guest_extract_args --extract-symbol
            "?function_137550@@YAPAUs_type_7ba8e9@@JF@Z")
    endif()
    if(source STREQUAL "${PROJECT_SOURCE_DIR}/src/unknown_0bb760.cpp")
        list(APPEND guest_extract_args --extract-symbol
            "?function_bb760@@YAJF@Z")
    endif()
    if(source STREQUAL "${PROJECT_SOURCE_DIR}/src/unknown_139296.cpp")
        list(APPEND guest_extract_args --extract-symbol
            "?function_1392f8@@YAXPBUs_player_appearance@@@Z")
    endif()
    if(source STREQUAL "${PROJECT_SOURCE_DIR}/src/unknown_07f720.cpp")
        list(APPEND guest_extract_args --extract-symbol
            "?function_7f790@@YAXF_NPBUs_player_appearance@@PAUcolor3f@@@Z"
            --extract-symbol "?function_7f720@@YAPAUcolor3f@@PAU1@F@Z")
    endif()
    if(source STREQUAL "${PROJECT_SOURCE_DIR}/src/unknown_131e50.cpp")
        list(APPEND guest_extract_args --extract-symbol
            "?pack_color3f@@YAKPBUcolor3f@@@Z")
    endif()
    if(source STREQUAL "${PROJECT_SOURCE_DIR}/src/unknown_1a06a0.cpp")
        list(APPEND guest_extract_args
            --extract-symbol "?function_1a06a0@@YAPAUcolor3f@@JPAU1@@Z"
            --extract-symbol "?function_1a06f0@@YAXPAUs_key_set@@PAJ1@Z"
            --extract-symbol "?function_1a03a0@@YAJJPAG@Z"
            --extract-symbol "?function_1a0540@@YA_NPAUs_player_profile_settings@@J@Z"
            --extract-symbol "?function_1a07b0@@YAXPAUs_player_profile@@J@Z"
            --extract-global "??_C@_1M@CBFBMPEI@?$AAG?$AAu?$AAe?$AAs?$AAt?$AA?$AA@")
    endif()
    if(source STREQUAL "${PROJECT_SOURCE_DIR}/src/unknown_146240.cpp")
        list(APPEND guest_extract_args
            --extract-symbol "?function_1462b0@@YAKXZ"
            --extract-symbol "?function_146840@@YAJXZ")
    endif()
    if(source STREQUAL "${PROJECT_SOURCE_DIR}/src/unknown_067e10.cpp")
        list(APPEND guest_extract_args --extract-symbol
            "?function_68290@@YA_NXZ")
    endif()
    if(source STREQUAL "${PROJECT_SOURCE_DIR}/src/unknown_19987f.cpp")
        list(APPEND guest_extract_args
            --extract-symbol "?function_19987f@@YAXXZ"
            --extract-symbol "?function_1999d7@@YA_NXZ"
            --extract-symbol "?function_199967@@YA_NXZ"
            --extract-symbol "?function_199971@@YA_NXZ"
            --extract-symbol "?function_1999b3@@YA_NXZ"
            --extract-symbol "?function_1999f9@@YA_NXZ"
            --extract-symbol "?function_199ebc@@YAJXZ"
            --extract-symbol "?function_199f34@@YAJXZ"
            --extract-symbol "?function_199f6d@@YAJXZ"
            --extract-symbol "?function_199e3c@@YAXJ@Z"
            --extract-symbol "?function_19a279@@YAJXZ"
            --extract-global "switch.table.?function_199b08@@YAXXZ")
    endif()
    if(source STREQUAL "${PROJECT_SOURCE_DIR}/src/unknown_058ee0.cpp")
        list(APPEND guest_extract_args --extract-symbol
            "?network_session_manager_check_joining_leader@@YAXXZ"
            --extract-symbol "?function_59670@@YA_NPAPAVc_class_58d20@@@Z")
    endif()
    if(source STREQUAL "${PROJECT_SOURCE_DIR}/src/unknown_1a2ca7.cpp")
        list(APPEND guest_extract_args
            --extract-symbol "??_Gc_online_task_screen@@UAAPAXI@Z"
            --extract-symbol "?get_load_proc@c_online_task_screen@@UAAP6APAVc_class_1473c9@@PAUs_screen_parameters@@@ZXZ"
            --extract-symbol "?online_task_screen_finish@@YAXPAVc_online_task_screen@@@Z"
            --extract-symbol "?online_task_screen_load@@YAPAVc_class_1473c9@@PAUs_screen_parameters@@@Z"
            --extract-symbol "?set_description@c_online_task_screen@@QAAXJ@Z"
            --extract-symbol "?v10@c_online_task_screen@@UAA_NPAUs_widget_event@@@Z"
            --extract-symbol "?v3@c_online_task_screen@@UAAXXZ"
            --extract-global "??_7c_online_task_screen@@6B@"
            --extract-alias "??_Ec_online_task_screen@@UAAPAXI@Z")
    endif()
    if(source STREQUAL "${PROJECT_SOURCE_DIR}/src/unknown_2521f2.cpp")
        list(APPEND guest_extract_args --extract-symbol
            "?function_2523bc@@YAXPAVc_online_task_screen@@@Z")
    endif()
    if(source STREQUAL "${PROJECT_SOURCE_DIR}/src/unknown_0592d0.cpp")
        list(APPEND guest_extract_args --extract-symbol
            "?function_0592d0@@YAKXZ")
    endif()
    if(source STREQUAL "${PROJECT_SOURCE_DIR}/src/unknown_05b040.cpp")
        list(APPEND guest_extract_args --extract-symbol
            "?get_values_4d08@c_class_58d20@@QAA_NPAJ0PAPAE@Z")
    endif()
    if(source STREQUAL "${PROJECT_SOURCE_DIR}/src/unknown_215b40.cpp")
        list(APPEND guest_extract_args
            --extract-symbol "?function_217080@@YAXPBUs_game_variant@@@Z"
            --extract-symbol "?function_217300@@YAJJJ@Z")
    endif()
    if(source STREQUAL "${PROJECT_SOURCE_DIR}/src/unknown_2b116a.cpp")
        list(APPEND guest_extract_args
            --extract-symbol "??_Gc_screen_45bc60@@UAAPAXI@Z"
            --extract-symbol "??_Gc_screen_45bcd0@@UAAPAXI@Z"
            --extract-symbol "??_Gc_screen_45bd40@@UAAPAXI@Z"
            --extract-symbol "?function_2b0a14@@YAXPAUs_widget_view_2b0a@@F@Z"
            --extract-symbol "?function_2b0a57@@YA_NPAUs_widget_view_2b0a@@@Z"
            --extract-symbol "?function_2b72e6@@YAPAVc_class_1473c9@@PAUs_screen_parameters@@@Z"
            --extract-symbol "?function_2b7333@@YAPAVc_class_1473c9@@PAUs_screen_parameters@@@Z"
            --extract-symbol "?function_2b739a@@YAPAVc_class_1473c9@@PAUs_screen_parameters@@@Z"
            --extract-symbol "?get_load_proc@c_screen_45bc60@@UAAP6APAVc_class_1473c9@@PAUs_screen_parameters@@@ZXZ"
            --extract-symbol "?get_load_proc@c_screen_45bcd0@@UAAP6APAVc_class_1473c9@@PAUs_screen_parameters@@@ZXZ"
            --extract-symbol "?get_load_proc@c_screen_45bd40@@UAAP6APAVc_class_1473c9@@PAUs_screen_parameters@@@ZXZ"
            --extract-symbol "?invoke@c_list_item_handler@@UAAXPAPAUs_controller_reference@@PAJ@Z"
            --extract-symbol "?set_bitmap@c_screen_45bd40@@QAAXF@Z"
            --extract-symbol "?set_text@c_screen_45bd40@@QAAXPBD@Z"
            --extract-symbol "?v3@c_screen_45bd40@@UAAXXZ"
            --extract-symbol "?v18@c_screen_45bc60@@UAAXPAX@Z"
            --extract-symbol "?v18@c_screen_45bd40@@UAAXPAX@Z"
            --extract-symbol "?function_2b0a48@@YA_NPAUs_widget_view_2b0a@@@Z"
            --extract-symbol "?function_2b0a68@@YAJPAUs_widget_view_2b0a@@@Z"
            --extract-symbol "?function_2b0a7b@@YAXPAUs_widget_view_2b0a@@PAUs_type_7ba8e9@@@Z"
            --extract-symbol "?function_2b12ba@@YA_NPAUs_widget_view_2b0a@@@Z"
            --extract-symbol "?function_2b10a3@@YAXPBKPAVc_class_2b0b5e@@J@Z"
            --extract-symbol "?function_2b014f@@YAJPAUs_widget_item@@@Z"
            --extract-symbol "?function_2b015b@@YAJPAUs_widget_item@@@Z"
            --extract-symbol "?function_2b016d@@YAJPAUs_widget_item@@@Z"
            --extract-symbol "?function_2b0180@@YA_NPAUs_widget_item@@@Z"
            --extract-symbol "?function_2b0191@@YA_NPAUs_widget_item@@@Z"
            --extract-symbol "?function_2b01c7@@YAJPAUs_widget_item@@@Z"
            --extract-symbol "?function_2b01da@@YAJPAUs_widget_item@@@Z"
            --extract-symbol "?function_2b12ca@@YAXPAUs_widget_view_2b0a@@FFPBX@Z"
            --extract-symbol "?v4@c_class_2b01eb@@UAAXJ@Z"
            --extract-symbol "?v1@c_class_2b0b5e@@UAAXXZ"
            --extract-symbol "?v3@c_widget_45adf0@@UAAXXZ"
            --extract-symbol "?v4@c_widget_45adf0@@UAAXJ@Z"
            --extract-symbol "??0c_level_select_screen@@QAA@JJG_N@Z"
            --extract-symbol "??_Gc_level_select_screen@@UAAPAXI@Z"
            --extract-symbol "?function_2b130a@@YAPAVc_class_1473c9@@PAUs_screen_parameters@@@Z"
            --extract-symbol "?function_2b136d@@YAPAVc_class_1473c9@@PAUs_screen_parameters@@@Z"
            --extract-symbol "?get_load_proc@c_level_select_screen@@UAAP6APAVc_class_1473c9@@PAUs_screen_parameters@@@ZXZ"
            --extract-symbol "?v18@c_level_select_screen@@UAAXPAX@Z"
            --extract-global "??_7c_level_select_screen@@6B@"
            --extract-alias "??_Ec_level_select_screen@@UAAPAXI@Z"
            --extract-symbol "??0c_difficulty_screen@@QAA@JJG@Z"
            --extract-symbol "??_Gc_difficulty_screen@@UAAPAXI@Z"
            --extract-symbol "?function_2bb3ed@@YAPAVc_class_1473c9@@PAUs_screen_parameters@@@Z"
            --extract-symbol "?function_2bb432@@YAPAVc_class_1473c9@@PAUs_screen_parameters@@@Z"
            --extract-symbol "?get_load_proc@c_difficulty_screen@@UAAP6APAVc_class_1473c9@@PAUs_screen_parameters@@@ZXZ"
            --extract-global "??_7c_difficulty_screen@@6B@"
            --extract-alias "??_Ec_difficulty_screen@@UAAPAXI@Z"
            --extract-symbol "??0c_campaign_options_list@@QAA@G@Z"
            --extract-symbol "?handle_item@c_campaign_options_list@@QAAXPAPAUs_controller_reference@@PAJ@Z"
            --extract-symbol "?v2@c_campaign_options_list@@UAAXXZ"
            --extract-symbol "?v3@c_campaign_options_list@@UAAXXZ"
            --extract-symbol "?get_item_data@c_campaign_options_list@@UAAPAXXZ"
            --extract-symbol "?get_item_count@c_campaign_options_list@@UAAJXZ"
            --extract-symbol "?v20@c_campaign_options_list@@UAAXPAVc_class_1a2c81@@J@Z"
            --extract-symbol "?function_2b18b7@@YAFPAVc_class_1474e8@@@Z"
            --extract-symbol "?v3@c_screen_458a00@@UAAXXZ"
            --extract-symbol "??_Gc_campaign_options_list@@UAAPAXI@Z"
            --extract-global "??_7c_campaign_options_list@@6B@"
            --extract-global "??_C@_0BG@HIBDOIKC@campaign?5options?5list?$AA@"
            --extract-global "switch.table.?v3@c_screen_458a00@@UAAXXZ"
            --extract-global "??_7c_screen_45bc60@@6B@"
            --extract-global "??_7c_screen_45bcd0@@6B@"
            --extract-global "??_7c_screen_45bd40@@6B@"
            --extract-alias "??_Ec_campaign_options_list@@UAAPAXI@Z"
            --extract-alias "??_Ec_screen_45bc60@@UAAPAXI@Z"
            --extract-alias "??_Ec_screen_45bcd0@@UAAPAXI@Z"
            --extract-alias "??_Ec_screen_45bd40@@UAAPAXI@Z")
    endif()
    if(source STREQUAL "${PROJECT_SOURCE_DIR}/src/unknown_2c4e9c.cpp")
        list(APPEND guest_extract_args
            --extract-symbol "??0c_campaign_level_handles_list@@QAA@G_N@Z"
            --extract-symbol "??_Gc_campaign_level_handles_list@@UAAPAXI@Z"
            --extract-symbol "?get_item_data@c_campaign_level_handles_list@@UAAPAXXZ"
            --extract-symbol "?get_item_count@c_campaign_level_handles_list@@UAAJXZ"
            --extract-symbol "?handle_item@c_campaign_level_handles_list@@QAAXPAPAUs_controller_reference@@PAJ@Z"
            --extract-symbol "?v1@c_campaign_level_handles_list@@UAAXXZ"
            --extract-symbol "?v20@c_campaign_level_handles_list@@UAAXPAVc_class_1a2c81@@J@Z"
            --extract-symbol "?v2@c_level_select_screen@@UAAXXZ"
            --extract-symbol "?v3@c_level_select_screen@@UAAXXZ"
            --extract-symbol "??0c_difficulty_list@@QAA@G@Z"
            --extract-symbol "??_Gc_difficulty_list@@UAAPAXI@Z"
            --extract-symbol "?get_item_data@c_difficulty_list@@UAAPAXXZ"
            --extract-symbol "?get_item_count@c_difficulty_list@@UAAJXZ"
            --extract-symbol "?handle_item@c_difficulty_list@@QAAXPAPAUs_controller_reference@@PAJ@Z"
            --extract-symbol "?v1@c_difficulty_list@@UAAXXZ"
            --extract-symbol "?v20@c_difficulty_list@@UAAXPAVc_class_1a2c81@@J@Z"
            --extract-global "??_7c_difficulty_list@@6B@"
            --extract-global "??_C@_0BA@JHMFDLGM@difficulty?5list?$AA@"
            --extract-global "switch.table.?v20@c_difficulty_list@@UAAXPAVc_class_1a2c81@@J@Z"
            --extract-alias "??_Ec_difficulty_list@@UAAPAXI@Z"
            --extract-global "??_7c_campaign_level_handles_list@@6B@"
            --extract-global "??_C@_0BH@JBNGBMEB@campaign?5level?5handles?$AA@"
            --extract-alias "??_Ec_campaign_level_handles_list@@UAAPAXI@Z")
    endif()
    if(source STREQUAL "${PROJECT_SOURCE_DIR}/src/data_iterator.cpp")
        list(APPEND guest_extract_args --extract-symbol
            "?data_datum_iterator_next@@YA_NPAUs_data_datum_iterator@@@Z")
    endif()
    if(source STREQUAL "${PROJECT_SOURCE_DIR}/src/unknown_2afd10.cpp")
        list(APPEND guest_extract_args
            --extract-symbol "?localized_description_get@@YAPAGPAUs_localized_description@@@Z"
            --extract-symbol "?localized_name_get@@YAPAGPAUs_localized_name@@@Z")
    endif()
    if(source STREQUAL "${PROJECT_SOURCE_DIR}/src/unknown_22f118.cpp")
        list(APPEND guest_extract_args
            --extract-symbol "?function_22f11e@@YAPAVc_class_1473c9@@PAUs_screen_parameters@@@Z"
            --extract-symbol "?function_22f36f@@YA_NPAX@Z"
            --extract-symbol "?function_22f3a1@@YA_NJ@Z"
            --extract-symbol "??_Gc_press_start_screen@@UAAPAXI@Z"
            --extract-symbol "?get_load_proc@c_press_start_screen@@UAAP6APAVc_class_1473c9@@PAUs_screen_parameters@@@ZXZ"
            --extract-symbol "?handle_start@c_press_start_screen@@QAAXPAPAUs_controller_reference@@PAJ@Z"
            --extract-symbol "?invoke@c_press_start_handler@@UAAXPAPAUs_controller_reference@@PAJ@Z"
            --extract-symbol "?v10@c_press_start_screen@@UAA_NPAUs_widget_event@@@Z"
            --extract-symbol "?v18@c_press_start_screen@@UAAXPAX@Z"
            --extract-symbol "?v3@c_press_start_screen@@UAAXXZ"
            --extract-global "??_7c_press_start_handler@@6B@"
            --extract-global "??_7c_press_start_screen@@6B@"
            --extract-alias "??_Ec_press_start_screen@@UAAPAXI@Z")
    endif()
    if(source STREQUAL "${PROJECT_SOURCE_DIR}/src/unknown_2523e3.cpp")
        list(APPEND guest_extract_args
            --extract-symbol "?function_25240c@@YAPAVc_class_1473c9@@PAUs_screen_parameters@@@Z"
            --extract-symbol "?function_252433@@YAPAVc_class_1473c9@@PAUs_screen_parameters@@@Z"
            --extract-symbol "?function_25245a@@YAPAVc_class_1473c9@@PAUs_screen_parameters@@@Z"
            --extract-symbol "?function_252481@@YAPAVc_class_1473c9@@PAUs_screen_parameters@@@Z"
            --extract-symbol "?function_2524a8@@YAPAVc_class_1473c9@@PAUs_screen_parameters@@@Z"
            --extract-symbol "?function_252a32@@YA_NJ@Z"
            --extract-symbol "??_Gc_4way_signin_screen@@UAAPAXI@Z"
            --extract-symbol "?get_load_proc@c_4way_signin_screen@@UAAP6APAVc_class_1473c9@@PAUs_screen_parameters@@@ZXZ"
            --extract-symbol "?v10@c_4way_signin_screen@@UAA_NPAUs_widget_event@@@Z"
            --extract-symbol "?v18@c_4way_signin_screen@@UAAXPAX@Z"
            --extract-symbol "?v3@c_4way_signin_screen@@UAAXXZ"
            --extract-global "??_7c_4way_signin_screen@@6B@"
            --extract-global "switch.table.?get_load_proc@c_4way_signin_screen@@UAAP6APAVc_class_1473c9@@PAUs_screen_parameters@@@ZXZ"
            --extract-alias "??_Ec_4way_signin_screen@@UAAPAXI@Z")
    endif()
    if(source STREQUAL "${PROJECT_SOURCE_DIR}/src/unknown_2b0149.cpp")
        list(APPEND guest_extract_args --extract-symbol "??0s_widget_item@@QAA@XZ")
    endif()
    if(source STREQUAL "${PROJECT_SOURCE_DIR}/src/unknown_1a0120.cpp")
        list(APPEND guest_extract_args
            --extract-symbol "?function_1a0180@@YAXJJPAG@Z"
            --extract-symbol "?string_table_load@@YA_NPAUs_string_table@@@Z")
    endif()
    if(source STREQUAL "${PROJECT_SOURCE_DIR}/src/unknown_13fd90.cpp")
        list(APPEND guest_extract_args --extract-symbol "?utf8_string_to_utf16_string@@YAXPBDPAGJ@Z")
    endif()
    if(source STREQUAL "${PROJECT_SOURCE_DIR}/src/unknown_1317a0.cpp")
        list(APPEND guest_extract_args
            --extract-symbol "?function_1318d0@@YAPAUhsv3f@@PBUcolor3f@@PAU1@@Z"
            --extract-symbol "?function_131a00@@YAPAUcolor3f@@PBUhsv3f@@PAU1@@Z")
    endif()
    if(source STREQUAL "${PROJECT_SOURCE_DIR}/src/unknown_23068b.cpp")
        list(APPEND guest_extract_args
            --extract-symbol "?function_230616@@YAPAVc_class_1473c9@@PAUs_screen_parameters@@@Z"
            --extract-symbol "??0c_screen_458a00@@QAA@JJG@Z"
            --extract-symbol "?get_load_proc@c_screen_458a00@@UAAP6APAVc_class_1473c9@@PAUs_screen_parameters@@@ZXZ"
            --extract-symbol "??_Gc_screen_458a00@@UAAPAXI@Z"
            --extract-global "??_7c_screen_458a00@@6B@"
            --extract-alias "??_Ec_screen_458a00@@UAAPAXI@Z")
    endif()
    if(source STREQUAL "${PROJECT_SOURCE_DIR}/src/unknown_2afd97.cpp")
        list(APPEND guest_extract_args
            --extract-symbol "??0c_class_2b01eb@@QAA@PAUs_bitmap_block@@@Z"
            --extract-symbol "?function_2afeae@@YAXPAUs_widget_item@@PAVc_class_1a2c81@@@Z"
            --extract-symbol "??0c_class_2b0b5e@@QAA@PAUs_model_block@@@Z"
            --extract-symbol "??0c_widget_45ad18@@QAA@JPAUs_widget_block_18@@@Z"
            --extract-symbol "?function_2baf38@@YAPAVc_widget_45adf0@@PAVc_class_1a2c81@@PAUs_widget_block_24@@@Z"
            --extract-symbol "?place@c_widget_45ad18@@QAAXPAUs_widget_point@@@Z"
            --extract-symbol "??_Gc_class_2b01eb@@UAAPAXI@Z"
            --extract-symbol "??_Gc_class_2b0b5e@@UAAPAXI@Z"
            --extract-symbol "??_Gc_widget_45ad18@@UAAPAXI@Z"
            --extract-symbol "??_Gc_widget_45adf0@@UAAPAXI@Z"
            --extract-global "??_7c_class_2b01eb@@6B@"
            --extract-global "??_7c_class_2b0b5e@@6B@"
            --extract-global "??_7c_widget_45ad18@@6B@"
            --extract-global "??_7c_widget_45adf0@@6B@"
            --extract-global "??_C@_11LOCGONAA@?$AA?$AA@"
            --extract-alias "??_Ec_class_2b01eb@@UAAPAXI@Z"
            --extract-alias "??_Ec_class_2b0b5e@@UAAPAXI@Z"
            --extract-alias "??_Ec_widget_45ad18@@UAAPAXI@Z"
            --extract-alias "??_Ec_widget_45adf0@@UAAPAXI@Z")
    endif()
    if(source STREQUAL "${PROJECT_SOURCE_DIR}/src/unknown_2afea0.cpp")
        list(APPEND guest_extract_args --extract-symbol
            "?v1@c_widget_45ad18@@UAAXXZ")
    endif()
    if(source STREQUAL "${PROJECT_SOURCE_DIR}/src/unknown_24c7c1.cpp")
        list(APPEND guest_extract_args
            --extract-symbol "?function_24c7c1@@YAXPAUs_item_list@@J@Z"
            --extract-symbol "?function_24c7e4@@YAXPAXPAPAUs_event@@PAJ@Z")
    endif()
    if(source STREQUAL "${PROJECT_SOURCE_DIR}/src/unknown_22f6ca.cpp")
        list(APPEND guest_extract_args
            --extract-symbol "?function_22fba9@@YAXPAVc_class_1473c9@@@Z"
            --extract-symbol "?build@c_class_1473c9@@QAAXPAUs_screen_layout@@@Z"
            --extract-symbol "?function_22fa30@@YAPAVc_class_1a2c81@@PAV1@PAUs_bitmap_block@@@Z")
    endif()
    if(source STREQUAL "${PROJECT_SOURCE_DIR}/src/unknown_19c1d0.cpp")
        list(APPEND guest_extract_args
            --extract-symbol "?function_19c1f0@@YAPAUs_entry_b@@J@Z"
            --extract-symbol "?function_19c270@@YAPAUs_entry_a@@JJ@Z"
            --extract-symbol "?function_19c670@@YAPAUs_record_pool@@XZ"
            --extract-symbol "?function_19c6d0@@YAXPAUs_record_pool@@J@Z"
            --extract-symbol "?function_19c7c0@@YA_NJJPBX@Z"
            --extract-symbol "?function_19c190@@YAXXZ")
    endif()
    if(source STREQUAL "${PROJECT_SOURCE_DIR}/src/unknown_19bd50.cpp")
        list(APPEND guest_extract_args
            --extract-symbol "?level_handle_tables_initialize@@YAXXZ"
            --extract-global "??_C@_0O@HDBDKBPP@level?5handles?$AA@"
            --extract-global "??_C@_0BK@FEMBNBNK@multiplayer?5level?5handles?$AA@")
    endif()
    if(source STREQUAL "${PROJECT_SOURCE_DIR}/src/unknown_13dcd0.cpp")
        list(APPEND guest_extract_args --extract-symbol
            "?sort_4byte@@YAXPAJKPAXP6A_NJJPBX@Z2@Z")
    endif()
    if(source STREQUAL "${PROJECT_SOURCE_DIR}/src/user_interface_text_parser.cpp")
        list(APPEND guest_extract_args
            --extract-symbol "?function_22d2ee@@YAXPAGJ@Z"
            --extract-symbol "?parse_text@@YAXPAG@Z")
    endif()
    if(source STREQUAL "${PROJECT_SOURCE_DIR}/src/unknown_058cb0.cpp")
        list(APPEND guest_extract_args
            --extract-symbol "?online_mutelist_reset@@YAXJ@Z"
            --extract-symbol "?function_058d70@@YA_NPAVc_class_58d20@@@Z")
    endif()
    if(source STREQUAL "${PROJECT_SOURCE_DIR}/src/unknown_199e2e.cpp")
        list(APPEND guest_extract_args --extract-symbol
            "?function_199e2e@@YAX_N@Z")
    endif()
    if(source STREQUAL "${PROJECT_SOURCE_DIR}/src/unknown_236299.cpp")
        list(APPEND guest_extract_args
            --extract-symbol "?function_235e5e@@YAXPAUs_sprite_element@@PAUs_float_rect@@1KJJ@Z"
            --extract-symbol "?function_235f31@@YAXPAUs_sprite_element@@PBUs_float_rect@@PBUpoint2f@@KF@Z"
            --extract-symbol "?function_2360c3@@YAXPBUshort_rectangle2d@@PATpoint3f@@@Z"
            --extract-symbol "?function_23661f@@YAXPAUs_dialog_definition@@J@Z"
            --extract-symbol "?function_23618e@@YAPAUs_float_rect@@PAU1@MPBUshort_rectangle2d@@@Z"
            --extract-symbol "?function_23625d@@YAXJ@Z"
            --extract-symbol "?function_2363d4@@YAXPBUshort_rectangle2d@@PAF1@Z"
            --extract-symbol "?function_23643f@@YAXXZ"
            --extract-symbol "?function_23654b@@YAXPAX00@Z")
    endif()
    if(source STREQUAL "${PROJECT_SOURCE_DIR}/src/unknown_213d20.cpp")
        list(APPEND guest_extract_args --extract-symbol
            "?map_location_get@@YAJPBD@Z")
    endif()
    if(source STREQUAL "${PROJECT_SOURCE_DIR}/src/loading.cpp")
        list(APPEND guest_extract_args
            --extract-symbol "?function_163890@@YA_NPBDJ@Z"
            --extract-symbol "?map_location_progress_get@@YAJPBD@Z"
            --extract-global "switch.table.?map_location_progress_get@@YAJPBD@Z")
    endif()
    if(source STREQUAL "${PROJECT_SOURCE_DIR}/src/unknown_18f100.cpp")
        list(APPEND guest_extract_args --extract-symbol
            "?function_18f100@@YA_NPBD@Z")
    endif()
    if(source STREQUAL "${PROJECT_SOURCE_DIR}/src/unknown_07ad80.cpp")
        list(APPEND guest_extract_args --extract-symbol
            "?function_07ad80@@YAXJPAE@Z")
    endif()
    if(source STREQUAL "${PROJECT_SOURCE_DIR}/src/unknown_137dd0.cpp")
        list(APPEND guest_extract_args --extract-symbol
            "?function_138110@@YAXPAUs_game_options@@@Z")
    endif()
    if(source STREQUAL "${PROJECT_SOURCE_DIR}/src/unknown_153750.cpp")
        list(APPEND guest_extract_args --extract-symbol
            "?function_1537f0@@YAXPAUs_profile_record@@@Z")
    endif()
    if(source STREQUAL "${PROJECT_SOURCE_DIR}/src/unknown_230612.cpp")
        list(APPEND guest_extract_args --extract-symbol
            "?function_2323ab@@YA_NJ@Z")
    endif()
    if(source STREQUAL "${PROJECT_SOURCE_DIR}/src/unknown_235b46.cpp")
        list(APPEND guest_extract_args --extract-symbol
            "?main_menu_music_silence_done@@YA_NPAUs_main_menu_music@@@Z")
    endif()
    if(source STREQUAL "${PROJECT_SOURCE_DIR}/src/unknown_054fe0.cpp")
        list(APPEND guest_extract_args
            --extract-symbol "?network_session_interface_clear_user@@YAXJ@Z"
            --extract-symbol "?network_session_interface_get_value_18@@YAJXZ"
            --extract-symbol "?network_session_interface_get_value_49a4@@YAJXZ"
            --extract-symbol "?network_session_interface_get_values_4d08@@YA_NPAJ0PAPAE@Z"
            --extract-symbol "?network_session_get_membership@@YA_NPAVc_class_58d20@@PAJ1111PAPAUs_session_member@@1PAKPAPAUs_network_session_player@@@Z"
            --extract-symbol "?network_session_interface_get_data_4db0@@YAPAEXZ"
            --extract-symbol "?network_session_interface_get_user_properties@@YA_NJPAJPAE0@Z")
    endif()
    if(source STREQUAL "${PROJECT_SOURCE_DIR}/src/unknown_216a50.cpp")
        list(APPEND guest_extract_args
            --extract-symbol "?saved_game_file_size_in_blocks@@YAJJ@Z"
            --extract-symbol "?saved_game_storage_has_free_blocks@@YA_NJ@Z"
            --extract-symbol "?saved_game_file_type_from_variant@@YAJPAUs_game_variant@@@Z"
            --extract-symbol "?saved_game_file_type_size_in_blocks@@YAJJ@Z"
            --extract-global "switch.table.?saved_game_file_type_from_variant@@YAJPAUs_game_variant@@@Z")
    endif()
    if(source STREQUAL "${PROJECT_SOURCE_DIR}/src/unknown_2157e0.cpp")
        list(APPEND guest_extract_args
            --extract-symbol "?function_215790@@YAXXZ")
    endif()
    if(source STREQUAL "${PROJECT_SOURCE_DIR}/src/unknown_22a664.cpp")
        list(APPEND guest_extract_args
            --extract-symbol "?function_22a664@@YAXPBUs_widget_quad_2b11@@PBUs_float_rect@@JJJ@Z")
    endif()
    add_custom_command(
        OUTPUT "${object}"
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/guest_elf_cc.py
                --llvm ${H2_LLVM_ROOT}/bin
                --plugin $<TARGET_FILE:h2_guest_rebase>
                --include ${CMAKE_CURRENT_SOURCE_DIR}/include
                --include ${CMAKE_CURRENT_SOURCE_DIR}/include/crt
                --include ${CMAKE_CURRENT_SOURCE_DIR}/include/xdk
                --include ${PROJECT_SOURCE_DIR}/include
                --ms-abi ${guest_extract_args}
                --output "${object}" "${source}"
        DEPENDS "${source}" ${H2_GUEST_HEADERS} ${H2_PORT_GUEST_HEADERS}
                ${CMAKE_CURRENT_SOURCE_DIR}/tools/guest_cc.py
                ${CMAKE_CURRENT_SOURCE_DIR}/tools/guest_asm_elf.py
                ${CMAKE_CURRENT_SOURCE_DIR}/tools/guest_elf_cc.py h2_guest_rebase
        VERBATIM
    )
    list(APPEND H2_MAIN_MENU_ELF_OBJECTS "${object}")
endforeach()
set(H2_GUEST_MAP_LOADER_ELF_OBJECT
    "${H2_GUEST_OBJECT_DIR}/elf/unknown_122870.o")
add_custom_command(
    OUTPUT "${H2_GUEST_MAP_LOADER_ELF_OBJECT}"
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/guest_elf_cc.py
            --llvm ${H2_LLVM_ROOT}/bin
            --plugin $<TARGET_FILE:h2_guest_rebase>
            --include ${CMAKE_CURRENT_SOURCE_DIR}/include
            --include ${CMAKE_CURRENT_SOURCE_DIR}/include/crt
            --include ${CMAKE_CURRENT_SOURCE_DIR}/include/xdk
            --include ${PROJECT_SOURCE_DIR}/include
            --extract-symbol "_Z19cache_header_verifyPK14s_cache_header"
            --extract-symbol "_Z20cache_files_load_mapPKc"
            --extract-symbol "_Z30cache_files_load_structure_bspP25s_structure_bsp_reference"
            --output "${H2_GUEST_MAP_LOADER_ELF_OBJECT}"
            ${PROJECT_SOURCE_DIR}/src/unknown_122870.cpp
    DEPENDS ${PROJECT_SOURCE_DIR}/src/unknown_122870.cpp
            ${H2_GUEST_HEADERS} ${H2_PORT_GUEST_HEADERS}
            ${CMAKE_CURRENT_SOURCE_DIR}/tools/guest_cc.py
            ${CMAKE_CURRENT_SOURCE_DIR}/tools/guest_asm_elf.py
            ${CMAKE_CURRENT_SOURCE_DIR}/tools/guest_elf_cc.py h2_guest_rebase
    VERBATIM
)
list(APPEND H2_MAIN_MENU_ELF_OBJECTS "${H2_GUEST_MAP_LOADER_ELF_OBJECT}")
set(H2_GUEST_CACHE_RUNTIME_ELF_OBJECT
    "${H2_GUEST_OBJECT_DIR}/elf/guest_cache_runtime.o")
add_custom_command(
    OUTPUT "${H2_GUEST_CACHE_RUNTIME_ELF_OBJECT}"
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/guest_elf_cc.py
            --llvm ${H2_LLVM_ROOT}/bin
            --plugin $<TARGET_FILE:h2_guest_rebase>
            --include ${CMAKE_CURRENT_SOURCE_DIR}/include
            --include ${CMAKE_CURRENT_SOURCE_DIR}/include/crt
            --include ${CMAKE_CURRENT_SOURCE_DIR}/include/xdk
            --include ${PROJECT_SOURCE_DIR}/include
            --output "${H2_GUEST_CACHE_RUNTIME_ELF_OBJECT}"
            ${CMAKE_CURRENT_SOURCE_DIR}/runtime/guest_cache_runtime.cpp
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/runtime/guest_cache_runtime.cpp
            ${H2_GUEST_HEADERS} ${H2_PORT_GUEST_HEADERS}
            ${CMAKE_CURRENT_SOURCE_DIR}/tools/guest_cc.py
            ${CMAKE_CURRENT_SOURCE_DIR}/tools/guest_asm_elf.py
            ${CMAKE_CURRENT_SOURCE_DIR}/tools/guest_elf_cc.py h2_guest_rebase
    VERBATIM
)
list(APPEND H2_MAIN_MENU_ELF_OBJECTS "${H2_GUEST_CACHE_RUNTIME_ELF_OBJECT}")
set(H2_GUEST_CRT_ELF_OBJECT "${H2_GUEST_OBJECT_DIR}/elf/guest_crt.o")
add_custom_command(
    OUTPUT "${H2_GUEST_CRT_ELF_OBJECT}"
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/guest_elf_cc.py
            --llvm ${H2_LLVM_ROOT}/bin
            --plugin $<TARGET_FILE:h2_guest_rebase>
            --include ${CMAKE_CURRENT_SOURCE_DIR}/include
            --include ${CMAKE_CURRENT_SOURCE_DIR}/include/crt
            --include ${CMAKE_CURRENT_SOURCE_DIR}/include/xdk
            --include ${PROJECT_SOURCE_DIR}/include
            --output "${H2_GUEST_CRT_ELF_OBJECT}"
            ${CMAKE_CURRENT_SOURCE_DIR}/runtime/guest_crt.c
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/runtime/guest_crt.c
            ${H2_GUEST_HEADERS} ${H2_PORT_GUEST_HEADERS}
            ${CMAKE_CURRENT_SOURCE_DIR}/tools/guest_cc.py
            ${CMAKE_CURRENT_SOURCE_DIR}/tools/guest_asm_elf.py
            ${CMAKE_CURRENT_SOURCE_DIR}/tools/guest_elf_cc.py h2_guest_rebase
    VERBATIM
)
list(APPEND H2_MAIN_MENU_ELF_OBJECTS "${H2_GUEST_CRT_ELF_OBJECT}")
set(H2_MAIN_MENU_BINDINGS_ASM "${H2_GUEST_OBJECT_DIR}/elf/main_menu_bindings.s")
set(H2_MAIN_MENU_BINDINGS_OBJECT "${H2_GUEST_OBJECT_DIR}/elf/main_menu_bindings.o")
add_custom_command(
    OUTPUT "${H2_MAIN_MENU_BINDINGS_OBJECT}"
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/guest_bindings.py
            --output "${H2_MAIN_MENU_BINDINGS_ASM}" ${H2_MAIN_MENU_BINDINGS}
    COMMAND ${H2_LLVM_ROOT}/bin/llvm-mc -triple=aarch64-linux-gnu -filetype=obj
            "${H2_MAIN_MENU_BINDINGS_ASM}" -o "${H2_MAIN_MENU_BINDINGS_OBJECT}"
    DEPENDS ${H2_MAIN_MENU_BINDINGS}
            ${CMAKE_CURRENT_SOURCE_DIR}/tools/guest_bindings.py
    VERBATIM
)
list(APPEND H2_MAIN_MENU_ELF_OBJECTS "${H2_MAIN_MENU_BINDINGS_OBJECT}")
list(APPEND H2_MAIN_MENU_ELF_OBJECTS
    "${H2_GUEST_PHYSICS_MODEL_PROVIDERS_OBJECT}"
    "${H2_GUEST_PHYSICS_MODEL_INITIALIZE_OBJECT}"
)
set(H2_MAIN_MENU_HOST_IMPORTS_ASM "${H2_GUEST_OBJECT_DIR}/elf/host_imports.s")
set(H2_MAIN_MENU_HOST_IMPORTS_OBJECT "${H2_GUEST_OBJECT_DIR}/elf/host_imports.o")
add_custom_command(
    OUTPUT "${H2_MAIN_MENU_HOST_IMPORTS_OBJECT}"
    COMMAND ${Python3_EXECUTABLE}
            ${CMAKE_CURRENT_SOURCE_DIR}/tools/guest_host_imports.py
            --output "${H2_MAIN_MENU_HOST_IMPORTS_ASM}"
            "${H2_MAIN_MENU_IMPORT_PLAN}"
    COMMAND ${H2_LLVM_ROOT}/bin/llvm-mc -triple=aarch64-linux-gnu -filetype=obj
            "${H2_MAIN_MENU_HOST_IMPORTS_ASM}"
            -o "${H2_MAIN_MENU_HOST_IMPORTS_OBJECT}"
    DEPENDS "${H2_MAIN_MENU_IMPORT_PLAN}"
            ${CMAKE_CURRENT_SOURCE_DIR}/tools/guest_host_imports.py
    VERBATIM
)
list(APPEND H2_MAIN_MENU_ELF_OBJECTS "${H2_MAIN_MENU_HOST_IMPORTS_OBJECT}")
set(H2_MAIN_MENU_PARTIAL_ELF "${CMAKE_BINARY_DIR}/port/main-menu-core.partial.o")
add_custom_command(
    OUTPUT "${H2_MAIN_MENU_PARTIAL_ELF}"
    COMMAND ${H2_LLD} -r -o "${H2_MAIN_MENU_PARTIAL_ELF}"
            ${H2_MAIN_MENU_ELF_OBJECTS}
    DEPENDS ${H2_MAIN_MENU_ELF_OBJECTS}
    VERBATIM
)
add_custom_target(h2-main-menu-elf ALL DEPENDS "${H2_MAIN_MENU_PARTIAL_ELF}")

set(H2_MAIN_MENU_GUEST_START_OBJECT
    "${H2_GUEST_OBJECT_DIR}/elf/main_menu_guest_start.o")
add_custom_command(
    OUTPUT "${H2_MAIN_MENU_GUEST_START_OBJECT}"
    COMMAND ${H2_LLVM_ROOT}/bin/llvm-mc -triple=aarch64-linux-gnu -filetype=obj
            ${CMAKE_CURRENT_SOURCE_DIR}/runtime/main_menu_guest_start.s
            -o "${H2_MAIN_MENU_GUEST_START_OBJECT}"
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/runtime/main_menu_guest_start.s
    VERBATIM
)
set(H2_MAIN_MENU_GUEST_IMAGE "${CMAKE_BINARY_DIR}/port/main-menu-core.elf")
add_custom_command(
    OUTPUT "${H2_MAIN_MENU_GUEST_IMAGE}"
    COMMAND ${H2_LLD} -static --no-undefined
            -T ${CMAKE_CURRENT_SOURCE_DIR}/runtime/main_menu_guest.ld
            -z max-page-size=0x4000
            -o "${H2_MAIN_MENU_GUEST_IMAGE}"
            "${H2_MAIN_MENU_PARTIAL_ELF}"
            "${H2_MAIN_MENU_GUEST_START_OBJECT}"
    DEPENDS "${H2_MAIN_MENU_PARTIAL_ELF}"
            "${H2_MAIN_MENU_GUEST_START_OBJECT}"
            ${CMAKE_CURRENT_SOURCE_DIR}/runtime/main_menu_guest.ld
    VERBATIM
)
add_custom_target(h2-main-menu-guest-image ALL DEPENDS "${H2_MAIN_MENU_GUEST_IMAGE}")

add_library(h2_port_support STATIC
    src/guest_arena.cpp
    src/guest_elf_image.cpp
    src/host_services.cpp
    src/profile_store.cpp
    src/retail_data.cpp
    src/texture_decode.cpp
    src/font_decode.cpp
)
target_include_directories(h2_port_support PUBLIC include)
target_compile_features(h2_port_support PUBLIC cxx_std_20)
target_compile_options(h2_port_support PRIVATE -Wall -Wextra -Wpedantic -Werror)
target_link_libraries(h2_port_support PUBLIC OpenSSL::Crypto)

add_executable(halo2-port src/main.cpp runtime/guest_stack.s)
target_compile_features(halo2-port PRIVATE cxx_std_20)
target_compile_options(halo2-port PRIVATE -Wall -Wextra -Wpedantic -Werror)
target_link_libraries(halo2-port PRIVATE h2_port_support SDL3::SDL3)

if(APPLE)
    set_target_properties(halo2-port PROPERTIES OSX_ARCHITECTURES arm64)
endif()

enable_testing()
set(H2_GUEST_ELF_LOADER_FIXTURE_OBJECT
    "${CMAKE_BINARY_DIR}/port/guest-elf-loader-fixture.o")
set(H2_GUEST_ELF_LOADER_FIXTURE
    "${CMAKE_BINARY_DIR}/port/guest-elf-loader-fixture.elf")
add_custom_command(
    OUTPUT "${H2_GUEST_ELF_LOADER_FIXTURE_OBJECT}"
    COMMAND ${H2_LLVM_ROOT}/bin/llvm-mc -triple=aarch64-linux-gnu -filetype=obj
            ${CMAKE_CURRENT_SOURCE_DIR}/tests/guest_elf_loader_fixture.s
            -o "${H2_GUEST_ELF_LOADER_FIXTURE_OBJECT}"
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/tests/guest_elf_loader_fixture.s
    VERBATIM
)
add_custom_command(
    OUTPUT "${H2_GUEST_ELF_LOADER_FIXTURE}"
    COMMAND ${H2_LLD} -static
            -T ${CMAKE_CURRENT_SOURCE_DIR}/tests/guest_elf_loader_fixture.ld
            -z max-page-size=0x4000
            -o "${H2_GUEST_ELF_LOADER_FIXTURE}"
            "${H2_GUEST_ELF_LOADER_FIXTURE_OBJECT}"
    DEPENDS "${H2_GUEST_ELF_LOADER_FIXTURE_OBJECT}"
            ${CMAKE_CURRENT_SOURCE_DIR}/tests/guest_elf_loader_fixture.ld
    VERBATIM
)

add_executable(h2-retail-data-tests tests/retail_data_tests.cpp)
target_compile_features(h2-retail-data-tests PRIVATE cxx_std_20)
target_compile_options(h2-retail-data-tests PRIVATE -Wall -Wextra -Wpedantic -Werror)
target_link_libraries(h2-retail-data-tests PRIVATE h2_port_support)
add_test(NAME retail-data-errors COMMAND h2-retail-data-tests)

add_executable(h2-guest-arena-tests tests/guest_arena_tests.cpp)
target_compile_features(h2-guest-arena-tests PRIVATE cxx_std_20)
target_compile_options(h2-guest-arena-tests PRIVATE -Wall -Wextra -Wpedantic -Werror)
target_link_libraries(h2-guest-arena-tests PRIVATE h2_port_support)
add_test(NAME guest-arena-xbe-loader COMMAND h2-guest-arena-tests)

add_executable(h2-guest-elf-image-tests
    tests/guest_elf_image_tests.cpp
    "${H2_GUEST_ELF_LOADER_FIXTURE}"
)
target_compile_features(h2-guest-elf-image-tests PRIVATE cxx_std_20)
target_compile_options(h2-guest-elf-image-tests PRIVATE -Wall -Wextra -Wpedantic -Werror)
target_link_libraries(h2-guest-elf-image-tests PRIVATE h2_port_support)
add_test(NAME native-guest-elf-image
         COMMAND h2-guest-elf-image-tests "${H2_GUEST_ELF_LOADER_FIXTURE}")

add_executable(h2-host-services-tests
    tests/host_services_tests.cpp
    runtime/guest_render_runtime.cpp
)
target_compile_features(h2-host-services-tests PRIVATE cxx_std_20)
target_compile_options(h2-host-services-tests PRIVATE -Wall -Wextra -Wpedantic -Werror)
target_link_libraries(h2-host-services-tests PRIVATE h2_port_support)
add_test(NAME native-host-services COMMAND h2-host-services-tests)

add_executable(h2-profile-store-tests tests/profile_store_tests.cpp)
target_compile_features(h2-profile-store-tests PRIVATE cxx_std_20)
target_compile_options(h2-profile-store-tests PRIVATE -Wall -Wextra -Wpedantic -Werror)
target_link_libraries(h2-profile-store-tests PRIVATE h2_port_support)
add_test(NAME native-profile-store COMMAND h2-profile-store-tests)

add_executable(h2-guest-crt-tests
    tests/guest_crt_tests.c
    runtime/guest_crt.c
)
target_include_directories(h2-guest-crt-tests PRIVATE include/crt)
target_compile_options(h2-guest-crt-tests PRIVATE
    -fshort-wchar -fno-builtin -Wall -Wextra -Wpedantic -Werror)
add_test(NAME guest-crt-services COMMAND h2-guest-crt-tests)

add_executable(h2-recovered-guest-tests
    tests/recovered_guest_archive_tests.cpp
    runtime/guest_symbols.s
)
target_compile_features(h2-recovered-guest-tests PRIVATE cxx_std_20)
target_compile_options(h2-recovered-guest-tests PRIVATE -Wall -Wextra -Wpedantic -Werror)
target_link_libraries(h2-recovered-guest-tests PRIVATE h2_recovered_guest)
add_test(NAME recovered-guest-archive COMMAND h2-recovered-guest-tests)
add_test(
    NAME main-menu-ms-abi
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_main_menu_ms_abi.py
            --object ${H2_GUEST_MAIN_MENU_OBJECT}
)
add_test(
    NAME main-menu-symbol-closure
    COMMAND ${Python3_EXECUTABLE}
            ${CMAKE_CURRENT_SOURCE_DIR}/tools/guest_symbol_closure.py
            --nm ${H2_LLVM_NM}
            --bindings ${H2_MAIN_MENU_BINDINGS}
            --format text
            --expect-unresolved 16
            ${H2_MAIN_MENU_CLOSURE_OBJECTS}
)
add_test(
    NAME main-menu-import-plan
    COMMAND ${Python3_EXECUTABLE}
            ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_guest_import_plan.py
            --plan "${H2_MAIN_MENU_IMPORT_PLAN}"
            --expect-unresolved 16
)
add_test(
    NAME main-menu-elf-core
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_main_menu_elf.py
            --nm ${H2_LLVM_NM}
            --image ${H2_MAIN_MENU_PARTIAL_ELF}
            --closure ${H2_MAIN_MENU_CLOSURE_REPORT}
            --import-plan ${H2_MAIN_MENU_IMPORT_PLAN}
)
add_test(
    NAME main-menu-guest-image
    COMMAND ${Python3_EXECUTABLE}
            ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_main_menu_guest_image.py
            --nm ${H2_LLVM_NM}
            --image ${H2_MAIN_MENU_GUEST_IMAGE}
)
add_test(
    NAME guest-asm-elf
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_guest_asm_elf.py
            --converter ${CMAKE_CURRENT_SOURCE_DIR}/tools/guest_asm_elf.py
            --llvm ${H2_LLVM_ROOT}/bin
)
add_test(
    NAME guest-host-import-trampoline
    COMMAND ${Python3_EXECUTABLE}
            ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_guest_host_imports.py
            --tool ${CMAKE_CURRENT_SOURCE_DIR}/tools/guest_host_imports.py
            --llvm ${H2_LLVM_ROOT}/bin
            --lld ${H2_LLD}
)
add_test(
    NAME guest-elf-image
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_guest_elf_image.py
            --root ${PROJECT_SOURCE_DIR}
            --llvm ${H2_LLVM_ROOT}/bin
            --plugin $<TARGET_FILE:h2_guest_rebase>
            --lld ${H2_LLD}
)

add_executable(h2-recovered-cache-header-tests
    tests/recovered_cache_header_tests.cpp
    "${H2_GUEST_MAP_LOADER_OBJECT}"
)
target_compile_features(h2-recovered-cache-header-tests PRIVATE cxx_std_20)
target_compile_options(h2-recovered-cache-header-tests PRIVATE -Wall -Wextra -Wpedantic -Werror)
target_link_options(h2-recovered-cache-header-tests PRIVATE -Wl,-dead_strip -Wl,-undefined,dynamic_lookup)
target_link_libraries(h2-recovered-cache-header-tests PRIVATE h2_recovered_guest)
add_test(NAME recovered-cache-header COMMAND h2-recovered-cache-header-tests)

add_executable(h2-mainmenu-tag-probe
    tests/mainmenu_tag_probe.cpp
    runtime/guest_stack.s
    runtime/map_loader_symbols.s
    "${H2_GUEST_MAP_LOADER_OBJECT}"
)
target_include_directories(h2-mainmenu-tag-probe PRIVATE include)
target_compile_features(h2-mainmenu-tag-probe PRIVATE cxx_std_20)
target_compile_options(h2-mainmenu-tag-probe PRIVATE -Wall -Wextra -Wpedantic -Werror)
target_link_options(h2-mainmenu-tag-probe PRIVATE -Wl,-dead_strip -Wl,-undefined,dynamic_lookup)
target_link_libraries(h2-mainmenu-tag-probe PRIVATE h2_recovered_guest)

add_test(
    NAME guest-rebase-compiler
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_guest_rebase.py
            --llvm ${H2_LLVM_ROOT}/bin
            --plugin $<TARGET_FILE:h2_guest_rebase>
)
add_test(
    NAME guest-function-extraction
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_guest_extract.py
            --root ${PROJECT_SOURCE_DIR}
            --llvm ${H2_LLVM_ROOT}/bin
            --plugin $<TARGET_FILE:h2_guest_rebase>
)
add_test(
    NAME physics-model-vtables
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_physics_model_vtables.py
            --root ${PROJECT_SOURCE_DIR}
            --llvm-readelf ${H2_LLVM_ROOT}/bin/llvm-readelf
            --table-object ${H2_GUEST_PHYSICS_MODEL_TABLE_OBJECT}
            --leaf-object ${H2_GUEST_PHYSICS_MODEL_LEAF_OBJECT}
            --initialize-object ${H2_GUEST_PHYSICS_MODEL_INITIALIZE_OBJECT}
            --provider-cluster-object ${H2_GUEST_PHYSICS_MODEL_PROVIDER_CLUSTER_OBJECT}
)
add_test(
    NAME structure-bsp-switch-callbacks
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_structure_bsp_callbacks.py
            --root ${PROJECT_SOURCE_DIR}
            --llvm-nm ${H2_LLVM_NM}
            --llvm-readelf ${H2_LLVM_ROOT}/bin/llvm-readelf
            --lld ${H2_LLD}
            --provider-cluster-object ${H2_GUEST_STRUCTURE_BSP_PROVIDER_CLUSTER_OBJECT}
            --production-object ${H2_MAIN_MENU_PARTIAL_ELF}
            --primary-listener-table-object ${H2_GUEST_HAVOK_PRIMARY_LISTENER_TABLE_OBJECT}
            --primary-listener-provider-cluster-object ${H2_GUEST_HAVOK_PRIMARY_LISTENER_PROVIDER_CLUSTER_OBJECT}
            --contact-processing-candidate-object ${H2_GUEST_HAVOK_CONTACT_PROCESSING_CANDIDATE_OBJECT}
            --impact-allocation-candidate-object ${H2_GUEST_HAVOK_IMPACT_ALLOCATION_CANDIDATE_OBJECT}
            --effect-lifecycle-candidate-object ${H2_GUEST_HAVOK_EFFECT_LIFECYCLE_CANDIDATE_OBJECT}
)
add_test(
    NAME recovered-guest-unit
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_recovered_guest_unit.py
            --llvm ${H2_LLVM_ROOT}/bin
            --plugin $<TARGET_FILE:h2_guest_rebase>
            --root ${PROJECT_SOURCE_DIR}
            --host-cxx ${CMAKE_CXX_COMPILER}
)
add_test(
    NAME recovered-guest-globals
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_recovered_globals.py
            --llvm ${H2_LLVM_ROOT}/bin
            --plugin $<TARGET_FILE:h2_guest_rebase>
            --root ${PROJECT_SOURCE_DIR}
            --host-cxx ${CMAKE_CXX_COMPILER}
)
add_test(
    NAME guest-host-import
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_guest_import.py
            --llvm ${H2_LLVM_ROOT}/bin
            --plugin $<TARGET_FILE:h2_guest_rebase>
            --root ${PROJECT_SOURCE_DIR}
            --host-cxx ${CMAKE_CXX_COMPILER}
)

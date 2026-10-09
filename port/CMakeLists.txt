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
set(H2_GUEST_RECORD_POOL_PREVIOUS_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_16bcc0.o")
set(H2_GUEST_RECORD_POOL_LAST_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_16bc40.o")
set(H2_GUEST_UI_HEAP_GLOBALS_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_1a474c.o")
set(H2_GUEST_UI_HEAP_FREE_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_1a4826.o")
set(H2_GUEST_LOOP_ALLOCATOR_OBJECT "${H2_GUEST_OBJECT_DIR}/loop_allocator.o")
set(H2_GUEST_NAME_BUFFER_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_08cc20.o")
set(H2_GUEST_UNICODE_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_13fa70.o")
set(H2_GUEST_MS_RUNTIME_OBJECT "${H2_GUEST_OBJECT_DIR}/guest_ms_runtime.o")
set(H2_GUEST_HOST_RUNTIME_OBJECT "${H2_GUEST_OBJECT_DIR}/guest_host_runtime.o")
set(H2_GUEST_ONLINE_RUNTIME_OBJECT "${H2_GUEST_OBJECT_DIR}/guest_online_runtime.o")
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
set(H2_GUEST_TASK_CALLBACK_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_2521f2.o")
set(H2_GUEST_SESSION_MODE_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_0592d0.o")
set(H2_GUEST_MUTELIST_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_058cb0.o")
set(H2_GUEST_SESSION_LEAVE_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_199e2e.o")
set(H2_GUEST_UI_RECT_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_236299.o")
set(H2_GUEST_QUIT_CALLBACK_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_230612.o")
set(H2_GUEST_MUSIC_SILENCE_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_235b46.o")
set(H2_GUEST_SESSION_USER_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_054fe0.o")
set(H2_GUEST_STORAGE_HANDLE_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_2172a0.o")
set(H2_GUEST_STORAGE_SIZE_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_216a50.o")
set(H2_GUEST_UI_GEOMETRY_OBJECT "${H2_GUEST_OBJECT_DIR}/unknown_13c050.o")
file(GLOB H2_GUEST_HEADERS CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/include/*.h")
file(GLOB_RECURSE H2_RECOVERED_SOURCES CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE H2_PORT_GUEST_HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/include/*")
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
    "${PROJECT_SOURCE_DIR}/src/unknown_16bcc0.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_16bc40.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_1a474c.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_1a4826.cpp"
    "${PROJECT_SOURCE_DIR}/src/loop_allocator.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_08cc20.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_13fa70.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/runtime/guest_ms_runtime.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/runtime/guest_host_runtime.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/runtime/guest_online_runtime.cpp"
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
    "${PROJECT_SOURCE_DIR}/src/unknown_190262.cpp"
    "${PROJECT_SOURCE_DIR}/src/player_slot_get.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_24b5bc.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_190001.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_250155.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_19987f.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_058ee0.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_1a2ca7.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_2521f2.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_0592d0.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_058cb0.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_199e2e.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_236299.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_230612.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_235b46.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_054fe0.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_2172a0.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_216a50.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_13c050.cpp"
)
foreach(pair IN ITEMS
        "${H2_GUEST_RANDOM_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_0259a0.cpp"
        "${H2_GUEST_GLOBALS_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_07a9a0_2.cpp"
        "${H2_GUEST_CACHE_TAG_OBJECT}|${PROJECT_SOURCE_DIR}/src/cache_tag_instances.cpp"
        "${H2_GUEST_CRT_OBJECT}|${CMAKE_CURRENT_SOURCE_DIR}/runtime/guest_crt.c"
        "${H2_GUEST_MAP_LOADER_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_122870.cpp"
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
        "${H2_GUEST_RECORD_POOL_PREVIOUS_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_16bcc0.cpp"
        "${H2_GUEST_RECORD_POOL_LAST_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_16bc40.cpp"
        "${H2_GUEST_UI_HEAP_GLOBALS_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_1a474c.cpp"
        "${H2_GUEST_UI_HEAP_FREE_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_1a4826.cpp"
        "${H2_GUEST_LOOP_ALLOCATOR_OBJECT}|${PROJECT_SOURCE_DIR}/src/loop_allocator.cpp"
        "${H2_GUEST_NAME_BUFFER_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_08cc20.cpp"
        "${H2_GUEST_UNICODE_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_13fa70.cpp"
        "${H2_GUEST_MS_RUNTIME_OBJECT}|${CMAKE_CURRENT_SOURCE_DIR}/runtime/guest_ms_runtime.cpp"
        "${H2_GUEST_HOST_RUNTIME_OBJECT}|${CMAKE_CURRENT_SOURCE_DIR}/runtime/guest_host_runtime.cpp"
        "${H2_GUEST_ONLINE_RUNTIME_OBJECT}|${CMAKE_CURRENT_SOURCE_DIR}/runtime/guest_online_runtime.cpp"
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
        "${H2_GUEST_PLAYER_SLOT_INDEX_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_190262.cpp"
        "${H2_GUEST_PLAYER_SLOT_GET_OBJECT}|${PROJECT_SOURCE_DIR}/src/player_slot_get.cpp"
        "${H2_GUEST_PLAYER_PROFILE_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_24b5bc.cpp"
        "${H2_GUEST_PLAYER_SESSION_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_190001.cpp"
        "${H2_GUEST_MENU_ACTION_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_250155.cpp"
        "${H2_GUEST_SESSION_PRESENCE_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_19987f.cpp"
        "${H2_GUEST_SESSION_MANAGER_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_058ee0.cpp"
        "${H2_GUEST_TASK_SCREEN_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_1a2ca7.cpp"
        "${H2_GUEST_TASK_CALLBACK_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_2521f2.cpp"
        "${H2_GUEST_SESSION_MODE_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_0592d0.cpp"
        "${H2_GUEST_MUTELIST_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_058cb0.cpp"
        "${H2_GUEST_SESSION_LEAVE_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_199e2e.cpp"
        "${H2_GUEST_UI_RECT_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_236299.cpp"
        "${H2_GUEST_QUIT_CALLBACK_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_230612.cpp"
        "${H2_GUEST_MUSIC_SILENCE_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_235b46.cpp"
        "${H2_GUEST_SESSION_USER_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_054fe0.cpp"
        "${H2_GUEST_STORAGE_HANDLE_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_2172a0.cpp"
        "${H2_GUEST_STORAGE_SIZE_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_216a50.cpp"
        "${H2_GUEST_UI_GEOMETRY_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_13c050.cpp")
    string(REPLACE "|" ";" fields "${pair}")
    list(GET fields 0 object)
    list(GET fields 1 source)
    set(guest_abi_args)
    set(guest_extract_args)
    list(FIND H2_MS_ABI_SOURCES "${source}" ms_abi_index)
    if(NOT ms_abi_index EQUAL -1)
        list(APPEND guest_abi_args --ms-abi)
    endif()
    if(object STREQUAL H2_GUEST_RECORD_POOL_LAST_OBJECT)
        list(APPEND guest_extract_args --extract-symbol
            "?data_last_index@@YAJPAUs_record_pool@@@Z")
    endif()
    if(object STREQUAL H2_GUEST_PLAYER_PROFILE_OBJECT)
        list(APPEND guest_extract_args --extract-symbol
            "?initialize@s_player_slot_profile@@QAAXJ@Z")
    endif()
    if(object STREQUAL H2_GUEST_PLAYER_SLOT_SERVICES_OBJECT)
        list(APPEND guest_extract_args
            --extract-symbol "?function_18f58c@@YAXXZ"
            --extract-symbol "?player_slot_count_active@@YAFXZ"
            --extract-symbol "?player_slot_find_controller@@YAJJ@Z"
            --extract-symbol "?player_slot_get_profile@@YAXJPAUs_player_profile@@PAJ@Z"
            --extract-symbol "?function_18fa4d@@YAJJ@Z")
    endif()
    if(object STREQUAL H2_GUEST_PLAYER_SESSION_OBJECT)
        list(APPEND guest_extract_args
            --extract-symbol "?function_1900a5@@YA_NJ@Z"
            --extract-symbol "?function_1900be@@YAFXZ"
            --extract-symbol "?function_1900ff@@YAFJ@Z"
            --extract-symbol "?function_1901fc@@YAGXZ"
            --extract-symbol "?function_19022f@@YAGXZ"
            --extract-symbol "?function_19028d@@YA_NXZ"
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
    if(object STREQUAL H2_GUEST_TIME_QUERY_OBJECT)
        list(APPEND guest_extract_args --extract-symbol
            "?function_146840@@YAJXZ")
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
            --extract-symbol "?online_task_screen_finish@@YAXPAVc_online_task_screen@@@Z"
            --extract-symbol "?set_description@c_online_task_screen@@QAAXJ@Z")
    endif()
    if(object STREQUAL H2_GUEST_TASK_CALLBACK_OBJECT)
        list(APPEND guest_extract_args --extract-symbol
            "?function_2523bc@@YAXPAVc_online_task_screen@@@Z")
    endif()
    if(object STREQUAL H2_GUEST_SESSION_MODE_OBJECT)
        list(APPEND guest_extract_args --extract-symbol
            "?function_0592d0@@YAKXZ")
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
            --extract-symbol "?function_2360c3@@YAXPBUshort_rectangle2d@@PATpoint3f@@@Z"
            --extract-symbol "?function_23618e@@YAPAUs_float_rect@@PAU1@MPBUshort_rectangle2d@@@Z"
            --extract-symbol "?function_2363d4@@YAXPBUshort_rectangle2d@@PAF1@Z")
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
            --extract-symbol "?network_session_get_membership@@YA_NPAVc_class_58d20@@PAJ1111PAPAUs_session_member@@1PAKPAPAUs_network_session_player@@@Z")
    endif()
    if(object STREQUAL H2_GUEST_STORAGE_SIZE_OBJECT)
        list(APPEND guest_extract_args
            --extract-symbol "?saved_game_file_size_in_blocks@@YAJJ@Z"
            --extract-symbol "?saved_game_file_type_size_in_blocks@@YAJJ@Z")
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
    "${H2_GUEST_MAP_LOADER_OBJECT}" "${H2_GUEST_MAIN_MENU_OBJECT}"
    "${H2_GUEST_WIDGET_BASE_OBJECT}" "${H2_GUEST_SCREEN_BASE_OBJECT}"
    "${H2_GUEST_UI_HEAP_OBJECT}" "${H2_GUEST_UI_MANAGER_OBJECT}"
    "${H2_GUEST_UI_MESSAGE_OBJECT}" "${H2_GUEST_LIST_WIDGET_OBJECT}"
    "${H2_GUEST_DIALOG_CALLBACK_OBJECT}" "${H2_GUEST_WINDOW_CHANNEL_OBJECT}"
    "${H2_GUEST_WINDOW_QUERY_OBJECT}"
    "${H2_GUEST_GAME_STATE_OBJECT}" "${H2_GUEST_CAMPAIGN_STATE_OBJECT}"
    "${H2_GUEST_LIFECYCLE_QUERY_OBJECT}" "${H2_GUEST_TIME_QUERY_OBJECT}"
    "${H2_GUEST_SIMULATION_QUERY_OBJECT}"
    "${H2_GUEST_RECORD_POOL_OBJECT}"
    "${H2_GUEST_RECORD_POOL_PREVIOUS_OBJECT}" "${H2_GUEST_RECORD_POOL_LAST_OBJECT}"
    "${H2_GUEST_UI_HEAP_GLOBALS_OBJECT}" "${H2_GUEST_UI_HEAP_FREE_OBJECT}"
    "${H2_GUEST_LOOP_ALLOCATOR_OBJECT}" "${H2_GUEST_NAME_BUFFER_OBJECT}"
    "${H2_GUEST_UNICODE_OBJECT}" "${H2_GUEST_MS_RUNTIME_OBJECT}"
    "${H2_GUEST_HOST_RUNTIME_OBJECT}" "${H2_GUEST_ONLINE_RUNTIME_OBJECT}"
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
    "${H2_GUEST_TASK_SCREEN_OBJECT}" "${H2_GUEST_TASK_CALLBACK_OBJECT}"
    "${H2_GUEST_SESSION_MODE_OBJECT}"
    "${H2_GUEST_MUSIC_SILENCE_OBJECT}" "${H2_GUEST_SESSION_LEAVE_OBJECT}"
    "${H2_GUEST_UI_RECT_OBJECT}" "${H2_GUEST_QUIT_CALLBACK_OBJECT}"
    "${H2_GUEST_SESSION_USER_OBJECT}" "${H2_GUEST_STORAGE_HANDLE_OBJECT}"
    "${H2_GUEST_STORAGE_SIZE_OBJECT}" "${H2_GUEST_UI_GEOMETRY_OBJECT}"
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
    "${H2_GUEST_RECORD_POOL_PREVIOUS_OBJECT}"
    "${H2_GUEST_RECORD_POOL_LAST_OBJECT}"
    "${H2_GUEST_UI_HEAP_GLOBALS_OBJECT}"
    "${H2_GUEST_UI_HEAP_FREE_OBJECT}"
    "${H2_GUEST_LOOP_ALLOCATOR_OBJECT}"
    "${H2_GUEST_NAME_BUFFER_OBJECT}"
    "${H2_GUEST_UNICODE_OBJECT}"
    "${H2_GUEST_MS_RUNTIME_OBJECT}"
    "${H2_GUEST_HOST_RUNTIME_OBJECT}"
    "${H2_GUEST_ONLINE_RUNTIME_OBJECT}"
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
    "${H2_GUEST_TASK_CALLBACK_OBJECT}"
    "${H2_GUEST_SESSION_MODE_OBJECT}"
    "${H2_GUEST_SESSION_LEAVE_OBJECT}"
    "${H2_GUEST_UI_RECT_OBJECT}"
    "${H2_GUEST_QUIT_CALLBACK_OBJECT}"
    "${H2_GUEST_MUSIC_SILENCE_OBJECT}"
    "${H2_GUEST_SESSION_USER_OBJECT}"
    "${H2_GUEST_STORAGE_HANDLE_OBJECT}"
    "${H2_GUEST_STORAGE_SIZE_OBJECT}"
    "${H2_GUEST_UI_GEOMETRY_OBJECT}"
)
add_library(h2_main_menu_guest STATIC ${H2_MAIN_MENU_GUEST_OBJECTS})
set_target_properties(h2_main_menu_guest PROPERTIES LINKER_LANGUAGE CXX)

set(H2_MAIN_MENU_BINDINGS
    "${CMAKE_CURRENT_SOURCE_DIR}/runtime/main_menu_bindings.json")
set(H2_MAIN_MENU_CLOSURE_REPORT
    "${CMAKE_BINARY_DIR}/port/main-menu-symbol-closure.json")
add_custom_command(
    OUTPUT "${H2_MAIN_MENU_CLOSURE_REPORT}"
    COMMAND ${Python3_EXECUTABLE}
            ${CMAKE_CURRENT_SOURCE_DIR}/tools/guest_symbol_closure.py
            --nm ${H2_LLVM_NM}
            --bindings ${H2_MAIN_MENU_BINDINGS}
            --format json
            --output "${H2_MAIN_MENU_CLOSURE_REPORT}"
            ${H2_MAIN_MENU_GUEST_OBJECTS}
    DEPENDS ${H2_MAIN_MENU_GUEST_OBJECTS} ${H2_MAIN_MENU_BINDINGS}
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
            --expect-unresolved 103
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
    if(source STREQUAL "${PROJECT_SOURCE_DIR}/src/unknown_16bc40.cpp")
        list(APPEND guest_extract_args --extract-symbol
            "?data_last_index@@YAJPAUs_record_pool@@@Z")
    endif()
    if(source STREQUAL "${PROJECT_SOURCE_DIR}/src/unknown_24b5bc.cpp")
        list(APPEND guest_extract_args --extract-symbol
            "?initialize@s_player_slot_profile@@QAAXJ@Z")
    endif()
    if(source STREQUAL "${PROJECT_SOURCE_DIR}/src/unknown_18f576.cpp")
        list(APPEND guest_extract_args
            --extract-symbol "?function_18f58c@@YAXXZ"
            --extract-symbol "?player_slot_count_active@@YAFXZ"
            --extract-symbol "?player_slot_find_controller@@YAJJ@Z"
            --extract-symbol "?player_slot_get_profile@@YAXJPAUs_player_profile@@PAJ@Z"
            --extract-symbol "?function_18fa4d@@YAJJ@Z")
    endif()
    if(source STREQUAL "${PROJECT_SOURCE_DIR}/src/unknown_190001.cpp")
        list(APPEND guest_extract_args
            --extract-symbol "?function_1900a5@@YA_NJ@Z"
            --extract-symbol "?function_1900be@@YAFXZ"
            --extract-symbol "?function_1900ff@@YAFJ@Z"
            --extract-symbol "?function_1901fc@@YAGXZ"
            --extract-symbol "?function_19022f@@YAGXZ"
            --extract-symbol "?function_19028d@@YA_NXZ"
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
    if(source STREQUAL "${PROJECT_SOURCE_DIR}/src/unknown_146240.cpp")
        list(APPEND guest_extract_args --extract-symbol
            "?function_146840@@YAJXZ")
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
            --extract-symbol "?online_task_screen_finish@@YAXPAVc_online_task_screen@@@Z"
            --extract-symbol "?set_description@c_online_task_screen@@QAAXJ@Z")
    endif()
    if(source STREQUAL "${PROJECT_SOURCE_DIR}/src/unknown_2521f2.cpp")
        list(APPEND guest_extract_args --extract-symbol
            "?function_2523bc@@YAXPAVc_online_task_screen@@@Z")
    endif()
    if(source STREQUAL "${PROJECT_SOURCE_DIR}/src/unknown_0592d0.cpp")
        list(APPEND guest_extract_args --extract-symbol
            "?function_0592d0@@YAKXZ")
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
            --extract-symbol "?function_2360c3@@YAXPBUshort_rectangle2d@@PATpoint3f@@@Z"
            --extract-symbol "?function_23618e@@YAPAUs_float_rect@@PAU1@MPBUshort_rectangle2d@@@Z"
            --extract-symbol "?function_2363d4@@YAXPBUshort_rectangle2d@@PAF1@Z")
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
            --extract-symbol "?network_session_get_membership@@YA_NPAVc_class_58d20@@PAJ1111PAPAUs_session_member@@1PAKPAPAUs_network_session_player@@@Z")
    endif()
    if(source STREQUAL "${PROJECT_SOURCE_DIR}/src/unknown_216a50.cpp")
        list(APPEND guest_extract_args
            --extract-symbol "?saved_game_file_size_in_blocks@@YAJJ@Z"
            --extract-symbol "?saved_game_file_type_size_in_blocks@@YAJJ@Z")
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

add_library(h2_port_support STATIC
    src/guest_arena.cpp
    src/guest_elf_image.cpp
    src/host_services.cpp
    src/retail_data.cpp
)
target_include_directories(h2_port_support PUBLIC include)
target_compile_features(h2_port_support PUBLIC cxx_std_20)
target_compile_options(h2_port_support PRIVATE -Wall -Wextra -Wpedantic -Werror)
target_link_libraries(h2_port_support PUBLIC OpenSSL::Crypto)

add_executable(halo2-port src/main.cpp)
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

add_executable(h2-host-services-tests tests/host_services_tests.cpp)
target_compile_features(h2-host-services-tests PRIVATE cxx_std_20)
target_compile_options(h2-host-services-tests PRIVATE -Wall -Wextra -Wpedantic -Werror)
target_link_libraries(h2-host-services-tests PRIVATE h2_port_support)
add_test(NAME native-host-services COMMAND h2-host-services-tests)

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
            --expect-unresolved 103
            ${H2_MAIN_MENU_GUEST_OBJECTS}
)
add_test(
    NAME main-menu-import-plan
    COMMAND ${Python3_EXECUTABLE}
            ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_guest_import_plan.py
            --plan "${H2_MAIN_MENU_IMPORT_PLAN}"
            --expect-unresolved 103
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

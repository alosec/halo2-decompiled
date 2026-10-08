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
file(GLOB H2_GUEST_HEADERS CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/include/*.h")
file(GLOB_RECURSE H2_PORT_GUEST_HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")
set(H2_MS_ABI_SOURCES
    "${PROJECT_SOURCE_DIR}/src/unknown_14741b.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_22e27b.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_2300cf.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_1a4742.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_147f6d.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_1490ec.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_24c177.cpp"
    "${PROJECT_SOURCE_DIR}/src/unknown_23690b.cpp"
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
        "${H2_GUEST_DIALOG_CALLBACK_OBJECT}|${PROJECT_SOURCE_DIR}/src/unknown_23690b.cpp")
    string(REPLACE "|" ";" fields "${pair}")
    list(GET fields 0 object)
    list(GET fields 1 source)
    set(guest_abi_args)
    list(FIND H2_MS_ABI_SOURCES "${source}" ms_abi_index)
    if(NOT ms_abi_index EQUAL -1)
        list(APPEND guest_abi_args --ms-abi)
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
    "${H2_GUEST_DIALOG_CALLBACK_OBJECT}"
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
    "${H2_GUEST_MAIN_MENU_OBJECT}"
    "${H2_GUEST_WIDGET_BASE_OBJECT}"
    "${H2_GUEST_SCREEN_BASE_OBJECT}"
    "${H2_GUEST_UI_HEAP_OBJECT}"
    "${H2_GUEST_UI_MANAGER_OBJECT}"
    "${H2_GUEST_UI_MESSAGE_OBJECT}"
    "${H2_GUEST_LIST_WIDGET_OBJECT}"
    "${H2_GUEST_DIALOG_CALLBACK_OBJECT}"
)
add_library(h2_main_menu_guest STATIC ${H2_MAIN_MENU_GUEST_OBJECTS})
set_target_properties(h2_main_menu_guest PROPERTIES LINKER_LANGUAGE CXX)

set(H2_MAIN_MENU_CLOSURE_REPORT
    "${CMAKE_BINARY_DIR}/port/main-menu-symbol-closure.json")
add_custom_command(
    OUTPUT "${H2_MAIN_MENU_CLOSURE_REPORT}"
    COMMAND ${Python3_EXECUTABLE}
            ${CMAKE_CURRENT_SOURCE_DIR}/tools/guest_symbol_closure.py
            --nm ${H2_LLVM_NM}
            --format json
            --output "${H2_MAIN_MENU_CLOSURE_REPORT}"
            ${H2_MAIN_MENU_GUEST_OBJECTS}
    DEPENDS ${H2_MAIN_MENU_GUEST_OBJECTS}
            ${CMAKE_CURRENT_SOURCE_DIR}/tools/guest_symbol_closure.py
    VERBATIM
)
add_custom_target(h2-main-menu-closure-report ALL
    DEPENDS "${H2_MAIN_MENU_CLOSURE_REPORT}")

add_library(h2_port_support STATIC
    src/guest_arena.cpp
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
            --format text
            --expect-unresolved 211
            ${H2_MAIN_MENU_GUEST_OBJECTS}
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

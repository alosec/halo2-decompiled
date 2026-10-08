find_package(OpenSSL REQUIRED COMPONENTS Crypto)
find_package(SDL3 CONFIG REQUIRED)

add_library(h2_port_support STATIC
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

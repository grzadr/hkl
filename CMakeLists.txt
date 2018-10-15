cmake_minimum_required(VERSION 3.1)
project(pyHKL VERSION 0.1 LANGUAGES CXX)

set(CMAKE_BUILD_TYPE Release)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "/usr/lib/python3.6" CACHE PATH "..." FORCE)
endif()

if (NOT HEADER_INSTALL_PREFIX)
    set(HEADER_INSTALL_PREFIX "/usr/local" CACHE PATH "..." FORCE)
endif()

add_subdirectory(agizmo/pybind11)
pybind11_add_module(pyHKL src/pyhkl.cpp)
target_include_directories(pyHKL
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/agizmo/include
)
target_compile_options(pyHKL PRIVATE -march=x86-64 -mtune=native -O3 -g0
                       -pipe -fPIE -fPIC -fstack-protector-strong -fno-plt
                       -fvisibility=hidden -Werror -Wall -pthread)
target_compile_features(pyHKL PRIVATE cxx_std_17)
install(TARGETS pyHKL LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX})
install(DIRECTORY include/hkl DESTINATION ${HEADER_INSTALL_PREFIX}/include)

include(CTest)

add_executable(TestHKL test/src/test_hkl.cpp test/src/test_region.cpp)
target_include_directories(TestHKL
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/test/include
        ${CMAKE_CURRENT_SOURCE_DIR}/agizmo/include
)
target_compile_features(TestHKL PRIVATE cxx_std_17)
# target_compile_options(BasicTest PRIVATE -g -march=x86-64 -mtune=native -O3 -g0
# 				       -pipe -fPIE -fPIC -fstack-protector-strong -fno-plt
# 				       -fvisibility=hidden -Werror -Wall -pthread)

add_test(Test TestHKL)

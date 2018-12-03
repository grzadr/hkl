cmake_minimum_required(VERSION 3.10)
project(pyHKL VERSION 0.1 LANGUAGES CXX)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_STANDARD 17)

find_package (Python3 COMPONENTS Interpreter Development)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "/usr/local" CACHE PATH "..." FORCE)
endif()

if (NOT PYTHON_INSTALL_PREFIX)
    set(PYTHON_INSTALL_PREFIX ${Python3_SITELIB} CACHE PATH "..." FORCE)
endif()

if (NOT HEADER_INSTALL_PREFIX)
    set(HEADER_INSTALL_PREFIX "/usr/local" CACHE PATH "..." FORCE)
endif()

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/agizmo)

add_executable(GFFlatter
  ${CMAKE_CURRENT_SOURCE_DIR}/src/gfflatter.cpp
  )

target_include_directories(GFFlatter
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/test/include
    ${CMAKE_CURRENT_SOURCE_DIR}/agizmo/include
)

target_compile_options(GFFlatter PRIVATE
  -march=x86-64 -mtune=native -O3 -g0
  -pipe -fPIE -fPIC -fstack-protector-strong -fno-plt
  -fvisibility=hidden -Werror -Wall -pthread)
target_compile_features(GFFlatter PRIVATE cxx_std_17)


include(CTest)

add_executable(TestHKL
  test/src/test_hkl.cpp
  test/src/test_region.cpp
  test/src/test_regionseq.cpp
  test/src/test_gff.cpp
)
target_include_directories(TestHKL
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/test/include
        ${CMAKE_CURRENT_SOURCE_DIR}/agizmo/include
)
target_compile_features(TestHKL PRIVATE cxx_std_17)
add_dependencies(TestHKL BasicTest)

#add_test(Test TestHKL)
add_test(NAME "Test" COMMAND TestHKL)
set_tests_properties(Test PROPERTIES WORKING_DIRECTORY
  ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_command(TARGET TestHKL
                   POST_BUILD
                   COMMAND "ctest"
#                     "--verbose"
                 "--output-on-failure"
)

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

install(TARGETS pyHKL EXPORT pyHKL-export
LIBRARY DESTINATION ${PYTHON_INSTALL_PREFIX})
install(DIRECTORY include/hkl DESTINATION ${HEADER_INSTALL_PREFIX}/include)
install(TARGETS GFFlatter RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

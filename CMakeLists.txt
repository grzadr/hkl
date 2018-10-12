cmake_minimum_required(VERSION 3.1)
project(HKL)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-march=x86-64 -mtune=native -O3 -g0 -pipe -fPIE -fPIC \
-fstack-protector-strong -fno-plt -fvisibility=hidden -Werror -Wall -pthread")
set(INSTALL_TARGET "/home/adrian")
set(PYBIND11_CPP_STANDARD -std=c++1z)


set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib)

# set(CMAKE_AUTOMOC ON)
# find_package(Qt5Core REQUIRED)
# find_package(Qt5Sql REQUIRED)
# include_directories(${Qt5Core_INCLUDES})
# add_library(DOGIToys lib/dogitoys.cpp lib/dogitoys-populate.cpp)
# target_link_libraries(DOGIToys Qt5::Core Qt5::Sql)

# include_directories(${CMAKE_CURRENT_SOURCE_DIR}/test)
# add_executable(Verify main.cpp
    # test/test.cpp
    # test/test_comtol.cpp
    # test/test_region.cpp
    # test/test_dogitoys.cpp
    # benchmark/bench_comtol.cpp
# )
# target_link_libraries(Verify DOGIToys)
file(GLOB HeaderFiles ${CMAKE_CURRENT_SOURCE_DIR}/lib/*.h)
# install(TARGETS DOGIToys ARCHIVE DESTINATION ${INSTALL_TARGET}/lib)
install(FILES ${HeaderFiles} DESTINATION ${INSTALL_TARGET}/lib/hkl)

add_subdirectory(pybind11)

pybind11_add_module(pyComTol pycomtol.cpp)
pybind11_add_module(pyHKL pyhkl.cpp)
# target_link_libraries(pyHKL PRIVATE DOGIToys)



install(TARGETS pyHKL LIBRARY DESTINATION ${INSTALL_TARGET}/lib)
install(TARGETS pyComTol LIBRARY DESTINATION ${INSTALL_TARGET}/lib)

# install(TARGETS pyHKL LIBRARY DESTINATION ${INSTALL_TARGET}/lib)
# install(TARGETS pyComTol LIBRARY DESTINATION ${INSTALL_TARGET}/lib)
# install(TARGETS pyStrTol LIBRARY DESTINATION ${INSTALL_TARGET}/lib)


# include(CTest)
# add_test(Complete Verify)

cmake_minimum_required(VERSION 3.15.0)
project(NX)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 包含头文件目录
include_directories(${CMAKE_SOURCE_DIR}/backend/include)

# # 添加 Asio 的头文件目录
# # 假设 Asio 的头文件位于 external/asio/include
# set(ASIO_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/external/asio/include")
# include_directories(${ASIO_INCLUDE_DIR})

# 添加 Crow 子目录
# add_subdirectory(external/Crow)

add_executable(NX
    backend/src/main_app2.cpp
    backend/src/pugixml.cpp
)

find_package(Crow REQUIRED)
target_link_libraries(NX Crow::Crow)

# 添加可执行文件


# 链接 Crow 库、stdc++fs 和 ws2_32
# target_link_libraries(NX PRIVATE)
# target_link_libraries(NX PRIVATE Crow::Crow)

# 如果需要链接其他库，可以在这里添加
# 例如：
# target_link_libraries(NX PRIVATE SomeOtherLibrary)

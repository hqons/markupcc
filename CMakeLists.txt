cmake_minimum_required(VERSION 3.10)
project(mkcc CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找 libxml2
find_package(LibXml2 REQUIRED)

# 添加头文件搜索路径
include_directories(${LIBXML2_INCLUDE_DIR})

# 添加可执行文件
add_executable(mkcc mkcc.cpp)

# 链接 libxml2
target_link_libraries(mkcc ${LIBXML2_LIBRARIES})

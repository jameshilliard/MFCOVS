#CMake 最低版本号要求
cmake_minimum_required (VERSION 2.8)
#项目名称
project(simple_demo)

#添加头文件
INCLUDE_DIRECTORIES(
    ${PROJECT_SOURCE_DIR}/../logger
    ${PROJECT_SOURCE_DIR}/../sdk-lib)
    
#指定生成目标
add_executable(${PROJECT_NAME} main.cpp)

#引用库
target_link_libraries(${PROJECT_NAME}
    ${CMAKE_SOURCE_DIR}/../sdk-lib/libovs_sdk.so)


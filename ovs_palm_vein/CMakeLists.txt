cmake_minimum_required(VERSION 2.8.3)
project(palm-vein)


set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_CXX_FLAGS "-std=c++11")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -Wall -g")

find_package(OpenCV REQUIRED)

include_directories(
    include
    OpenCV_INCLUDE_DIRS
)

add_executable(${PROJECT_NAME}_test
    test/test.cpp
    src/palm_vein.cpp
    src/palmveinparser_impl.cpp
)
target_link_libraries(${PROJECT_NAME}_test
    ${OpenCV_LIBS}
)

# add_executable(${PROJECT_NAME}_hog
#     test/hog.cpp
# )
# target_link_libraries(${PROJECT_NAME}_hog
#     ${OpenCV_LIBS}
# )

# add_executable(${PROJECT_NAME}_train
#     test/train.cpp
# )
# target_link_libraries(${PROJECT_NAME}_train
#     ${OpenCV_LIBS}
# )
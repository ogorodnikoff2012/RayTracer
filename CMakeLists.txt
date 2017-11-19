cmake_minimum_required(VERSION 3.7)
project(RayTracer)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp -DMAGICKCORE_HDRI_ENABLE=0 -DMAGICKCORE_QUANTUM_DEPTH=16 -O3")

find_package(ImageMagick COMPONENTS Magick++)

include_directories(${ImageMagick_INCLUDE_DIRS} include)

set(SOURCE_FILES src/main.cpp src/Vector.cpp)
add_executable(RayTracer ${SOURCE_FILES})
target_link_libraries(RayTracer ${ImageMagick_LIBRARIES})
add_executable(MoonTest src/moon_test.cpp)
target_link_libraries(MoonTest ${ImageMagick_LIBRARIES})
add_executable(GridTest src/grid_test.cpp)
target_link_libraries(GridTest ${ImageMagick_LIBRARIES})

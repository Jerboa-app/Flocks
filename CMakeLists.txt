cmake_minimum_required(VERSION 3.1)
project(Jerboa)

option(BUILD_SHARED_LIBS "Build using shared libraries" ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_BUILD_TYPE Release)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -O3")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")

add_subdirectory(include/vendored/freetype)
add_subdirectory(include/vendored/glm)

find_package(OpenGL REQUIRED)

set(GLEW_SOURCE include/vendored/glew/src/glew.c)
set(GLEW_INCLUDE include/vendored/glew/include/)

include_directories(include ${OPENGL_INCLUDE_DIRS} ${GLEW_INCLUDE})

add_library(glew SHARED ${GLEW_SOURCE} ${GLEW_INCLUDE})

target_link_libraries(glew
	${OPENGL_LIBRARY}
)

if (WINDOWS)

    option(BUILD_SHARED_LIBS "Build using shared libraries" ON)
    add_compile_definitions(WINDOWS)
    set(SFML_DIR "include/SFML-2.5.1-mingw64/lib/cmake/SFML/")

else ()

    set(SFML_STATIC_LIBRARIES TRUE)
    set(SFML_DIR "include/SFML-2.5.1/build")

endif()

find_package(SFML 2.5.1 COMPONENTS system window graphics audio REQUIRED)

add_executable(Jerboa src/main.cpp)

target_link_libraries(Jerboa glew freetype glm sfml-system sfml-window sfml-graphics sfml-audio ${GLEW_LIBRARIES} ${OPENGL_LIBRARIES})

# CMakeList.txt : CMake project for ModernOpenGLWin32, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("ModernOpenGLWin32")

set(CMAKE_C_STANDARD 17)

find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)

# Add source to this project's executable.
add_executable (ModernOpenGLWin32 WIN32 "main.c")

target_link_libraries(ModernOpenGLWin32 PRIVATE GLEW::GLEW)
target_link_libraries(ModernOpenGLWin32 PRIVATE OpenGL::GL)

#add_custom_command(
#  TARGET ${PROJECT_NAME} POST_BUILD
#  COMMAND ${CMAKE_COMMAND} -E copy_directory
#  ${CMAKE_CURRENT_LIST_DIR}/resources
#  ${CMAKE_CURRENT_BINARY_DIR}/resources)

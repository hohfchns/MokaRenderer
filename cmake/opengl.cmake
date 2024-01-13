find_package(GLFW3 REQUIRED)

set(OpenGL_GL_PREFERENCE "GLVND")
set(BUILD_SHARED_LIBS OFF)

find_package(OpenGL REQUIRED)
add_definitions(${OPENGL_DEFINITIONS})

find_package(glm REQUIRED)

find_package(X11 REQUIRED)

find_package(assimp REQUIRED)

set(GL_LIBS ${GLFW3_LIBRARY} X11 Xrandr Xinerama Xi Xxf86vm Xcursor GL dl pthread freetype ${ASSIMP_LIBRARIES} glm)

add_library(GLAD "${PROJECT_SOURCE_DIR}/src/vendor/gl/glad/src/glad.c")
set(GL_LIBS ${GL_LIBS} GLAD)
include_directories(${GLFW3_INCLUDE_DIR})
include_directories(${GLM_INCLUDE_DIR})
include_directories(${ASSIMP_INCLUDE_DIR})
include_directories("${PROJECT_SOURCE_DIR}/inc/vendor/gl/glad/include")



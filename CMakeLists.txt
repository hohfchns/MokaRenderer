cmake_minimum_required(VERSION 3.10)
project(Moka)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/helpers.cmake")

set(MOKA_RENDERER_DEPS MOKA_UTILS MOKA_ECS MOKA_WORLD)
DEPENDENCIES(MOKA_RENDERER)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")

include("${PROJECT_SOURCE_DIR}/cmake/opengl.cmake")

include_directories("inc/")
include_directories(${MOKA_RENDERER_DEP_INCS})

file(
  GLOB SOURCE_FILES
  "src/moka/renderer/renderer.cpp"
  "src/moka/renderer/main_loop.cpp"
  "src/moka/renderer/wrappers/shader.cpp"
  "src/moka/renderer/wrappers/texture.cpp"
  "src/moka/renderer/components/camera.cpp"
  "src/moka/renderer/components/mesh.cpp"
  "src/moka/renderer/components/instanced_mesh.cpp"
  "src/moka/renderer/components/model.cpp"
  "src/moka/assets/raw_image.cpp"
)

file (
  GLOB TESTS_SOURCE_FILES
)

add_library(MokaRenderer STATIC ${SOURCE_FILES})
target_link_libraries(MokaRenderer ${MOKA_RENDERER_DEP_LIBS} ${GL_LIBS})
# add_link_options(-no-pie)

set_property(TARGET MokaRenderer PROPERTY CXX_STANDARD 20)
set_property(TARGET MokaRenderer PROPERTY CXX_STANDARD_REQUIRED 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-function")

if (DEFINED UNIT_TESTS)
  if (${TESTS_SOURCE_FILES})
    include(FetchContent)
    FetchContent_Declare(
      googletest
      URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
    )

    if (WIN32)
      # For Windows: Prevent overriding the parent project's compiler/linker settings
      set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
      FetchContent_MakeAvailable(googletest)
    endif()

    enable_testing()

    include(GoogleTest)
    add_executable(MokaRenderer_UnitTests ${TESTS_SOURCE_FILES})
    target_link_libraries(MokaRenderer_UnitTests gtest ${MOKA_RENDERER_DEP_LIBS} MokaRenderer ${GL_LIBS})
    set_property(TARGET MokaRenderer_UnitTests PROPERTY CXX_STANDARD 20)
    set_property(TARGET MokaRenderer_UnitTests PROPERTY CXX_STANDARD_REQUIRED 20)

    gtest_discover_tests(MokaRenderer_UnitTests)
  endif()
endif()

set_property(GLOBAL PROPERTY MOKA_RENDERER 1)
set_property(GLOBAL PROPERTY MOKA_RENDERER_INCS "${CMAKE_CURRENT_SOURCE_DIR}/inc")
set_property(GLOBAL PROPERTY MOKA_RENDERER_LIBS MokaRenderer)

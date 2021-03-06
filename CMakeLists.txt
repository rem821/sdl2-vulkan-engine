cmake_minimum_required(VERSION 3.21)

set(NAME VulkanGame)
project(${NAME} VERSION 0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)
add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC libs/sdl libs/fmt/ libs/glm/ libs/imgui/ libs/tinyobjloader/)

add_subdirectory(libs/sdl)
target_link_directories(${PROJECT_NAME} PRIVATE libs/sdl/)

add_subdirectory(libs/fmt)
target_link_directories(${PROJECT_NAME} PRIVATE libs/fmt/)

add_subdirectory(libs/glm)
target_link_directories(${PROJECT_NAME} PRIVATE libs/glm/)

add_subdirectory(libs/imgui)
target_link_directories(${PROJECT_NAME} PRIVATE libs/imgui/)

add_subdirectory(libs/tinyobjloader)
target_link_directories(${PROJECT_NAME} PRIVATE libs/tinyobjloader/)

add_subdirectory(libs/sqlite)
target_link_directories(${PROJECT_NAME} PRIVATE libs/sqlite/)


find_package(Vulkan REQUIRED)

target_link_libraries(${PROJECT_NAME} Vulkan::Vulkan SDL2 fmt glm ImGui SQLiteCpp sqlite3 pthread dl)

############## Build SHADERS #######################

# Find all vertex and fragment sources within shaders directory
# taken from VBlancos vulkan tutorial
# https://github.com/vblanco20-1/vulkan-guide/blob/all-chapters/CMakeLists.txt
find_program(GLSL_VALIDATOR glslangValidator HINTS
        ${Vulkan_GLSLANG_VALIDATOR_EXECUTABLE}
        /usr/bin
        /usr/local/bin
        ${VULKAN_SDK_PATH}/Bin
        ${VULKAN_SDK_PATH}/Bin32
        $ENV{VULKAN_SDK}/Bin/
        $ENV{VULKAN_SDK}/Bin32/
        )

# get all .vert and .frag files in shaders directory
file(GLOB_RECURSE GLSL_SOURCE_FILES
        "${PROJECT_SOURCE_DIR}/shaders/*.frag"
        "${PROJECT_SOURCE_DIR}/shaders/*.vert"
        )

foreach(GLSL ${GLSL_SOURCE_FILES})
    message(STATUS "BUILDING SHADER")
    get_filename_component(FILE_NAME ${GLSL} NAME)
    set(SPIRV "${PROJECT_SOURCE_DIR}/shaders/${FILE_NAME}.spv")
    message(STATUS ${GLSL})
    add_custom_command(
            OUTPUT ${SPIRV}
            COMMAND ${GLSL_VALIDATOR} -V ${GLSL} -o ${SPIRV}
            DEPENDS ${GLSL})
    list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)

add_custom_target(
        Shaders
        DEPENDS ${SPIRV_BINARY_FILES}
)

file(COPY shaders DESTINATION ${CMAKE_BINARY_DIR})
file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})
file(COPY models DESTINATION ${CMAKE_BINARY_DIR})
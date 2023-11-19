cmake_minimum_required(VERSION 3.18 FATAL_ERROR)

include(${CMAKE_SOURCE_DIR}/utils.cmake)

cmake_policy(SET CMP0091 NEW)
cmake_policy(SET CMP0042 NEW)

project(nodeml_gpt4all VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)

add_definitions(-DNAPI_VERSION=4)

include_directories(${CMAKE_JS_INC})

set(GPT4ALL_VERSION "2.5.2" CACHE STRING "GPT4ALL Version")
set(GPT4ALL_DIR ${CMAKE_SOURCE_DIR}/deps/gpt4all)

DownloadGpt4All(${GPT4ALL_VERSION} ${CMAKE_SOURCE_DIR}/deps)

add_subdirectory(${CMAKE_SOURCE_DIR}/deps/gpt4all llmodel)

# Build a shared library named after the project from the files in `src/`
file(GLOB_RECURSE SOURCE_FILES "src/*.cc" "src/*.h")

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES} ${CMAKE_JS_SRC})

# Gives our library file a .node extension without any "lib" prefix
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".node")

target_link_libraries(${PROJECT_NAME} ${CMAKE_JS_LIB})

target_link_libraries(${PROJECT_NAME} llmodel bert-default)

target_include_directories(${PROJECT_NAME} PRIVATE ${GPT4ALL_DIR})

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src)

GenerateNodeLIB()

set(COMPONENT_NAME_MAIN ${PROJECT_NAME})
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install)
message(STATUS "INSTALL PREFIX ${CMAKE_INSTALL_PREFIX}")

install(TARGETS llmodel DESTINATION bin)
install(TARGETS gptj-avxonly DESTINATION bin)
install(TARGETS gptj-default DESTINATION bin)
install(TARGETS llama-mainline-avxonly DESTINATION bin)
install(TARGETS llama-mainline-default DESTINATION bin)
install(TARGETS llamamodel-mainline-avxonly DESTINATION bin)
install(TARGETS llamamodel-mainline-default DESTINATION bin)
if(APPLE)
  install(TARGETS llamamodel-mainline-metal DESTINATION bin)
endif()
install(TARGETS bert-avxonly DESTINATION bin)
install(TARGETS bert-default DESTINATION bin)
# install(TARGETS llmodel DESTINATION . COMPONENT ${COMPONENT_NAME_MAIN})
# install(TARGETS gptj-avxonly DESTINATION . COMPONENT ${COMPONENT_NAME_MAIN})
# install(TARGETS gptj-default DESTINATION . COMPONENT ${COMPONENT_NAME_MAIN})
# install(TARGETS llama-mainline-avxonly DESTINATION . COMPONENT ${COMPONENT_NAME_MAIN})
# install(TARGETS llama-mainline-default DESTINATION . COMPONENT ${COMPONENT_NAME_MAIN})
# install(TARGETS llamamodel-mainline-avxonly DESTINATION . COMPONENT ${COMPONENT_NAME_MAIN})
# install(TARGETS llamamodel-mainline-default DESTINATION . COMPONENT ${COMPONENT_NAME_MAIN})
# if(APPLE)
#   install(TARGETS llamamodel-mainline-metal DESTINATION . COMPONENT ${COMPONENT_NAME_MAIN})
# endif()
# install(TARGETS bert-avxonly DESTINATION . COMPONENT ${COMPONENT_NAME_MAIN})
# install(TARGETS bert-default DESTINATION . COMPONENT ${COMPONENT_NAME_MAIN})

# Essential include files to build a node addon,
# You should add this line in every CMake.js based project
# target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_JS_INC})

# target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/src)



# Include N-API wrappers
# IncludeNapi(${PROJECT_NAME})
file(GLOB_RECURSE ADDON_DLLS ${CMAKE_BINARY_DIR}/bin/*.*)
add_custom_command(TARGET ${PROJECT_NAME}
                    POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    ${ADDON_DLLS}
                    $<TARGET_FILE_DIR:${PROJECT_NAME}>)


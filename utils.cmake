# function(DownloadPython version destination)
#   message(STATUS "DOWNLOADING PYTHON VERSION ${version}")
#   if(NOT EXISTS ${destination}/python)
#     set(DOWNLOAD_FILE ${CMAKE_CURRENT_SOURCE_DIR}/pinstall.exe)
#     if(WIN32)
#       #file(DOWNLOAD https://www.python.org/ftp/python/${version}/python-${version}-amd64.exe ${DOWNLOAD_FILE} SHOW_PROGRESS)
#       message(STATUS "Installing ${CMAKE_CURRENT_SOURCE_DIR}/pinstall.exe ${destination}/python")#/quiet,
#       execute_process(COMMAND powershell Start-Process -Wait ${CMAKE_CURRENT_SOURCE_DIR}/pinstall.exe) #-ArgumentList TargetDir=${destination}/python)
#     elseif(APPLE)
#       message(FATAL_ERROR "NO SUPPORT FOR LINUX")
#     else()
#       message(FATAL_ERROR "NO SUPPORT FOR APPLE")
#     endif()
#     # file(REMOVE ${DOWNLOAD_FILE})
#   endif()
# endfunction()

macro(IncludeNapi project_name)
    # Include N-API wrappers
    execute_process(COMMAND node -p "require('node-addon-api').include"
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            OUTPUT_VARIABLE NODE_ADDON_API_DIR
            )
    string(REPLACE "\n" "" NODE_ADDON_API_DIR ${NODE_ADDON_API_DIR})
    string(REPLACE "\"" "" NODE_ADDON_API_DIR ${NODE_ADDON_API_DIR})
    target_include_directories(${project_name} PRIVATE ${NODE_ADDON_API_DIR})
    add_definitions(-DNAPI_VERSION=3)
endmacro()

function(DownloadGpt4All VERSION DESTINATION)
  if(NOT EXISTS ${DESTINATION}/gpt4all)
    
    execute_process(
      COMMAND git clone --depth 1 --branch v${VERSION} https://github.com/nomic-ai/gpt4all ${CMAKE_SOURCE_DIR}/repo
    )

    execute_process(
      COMMAND git submodule update --init WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/repo
    )
    
    if(NOT EXISTS ${DESTINATION})
      file(MAKE_DIRECTORY ${DESTINATION})
    endif()

    file(RENAME ${CMAKE_SOURCE_DIR}/repo/gpt4all-backend ${CMAKE_SOURCE_DIR}/deps/gpt4all)

    file(REMOVE ${CMAKE_SOURCE_DIR}/repo)
  endif()
endfunction()

function(GenerateNodeLIB)
  if(MSVC AND CMAKE_JS_NODELIB_DEF AND CMAKE_JS_NODELIB_TARGET)
    # Generate node.lib
    execute_process(COMMAND ${CMAKE_AR} /def:${CMAKE_JS_NODELIB_DEF} /out:${CMAKE_JS_NODELIB_TARGET} ${CMAKE_STATIC_LINKER_FLAGS})
  endif()
endfunction(GenerateNodeLIB)

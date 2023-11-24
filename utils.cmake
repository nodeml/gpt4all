
function(DownloadGpt4All VERSION DESTINATION RESULT)
  if(NOT EXISTS ${DESTINATION}/gpt4all)
    
    set(REPO_DESTINATION_DIR ${CMAKE_BINARY_DIR}/gpt4all)

    execute_process(
      COMMAND git clone --depth 1 --branch v${VERSION} https://github.com/nomic-ai/gpt4all ${REPO_DESTINATION_DIR}
    )

    execute_process(
      COMMAND git submodule update --init WORKING_DIRECTORY ${REPO_DESTINATION_DIR}
    )
    
    if(NOT EXISTS ${DESTINATION})
      file(MAKE_DIRECTORY ${DESTINATION})
    endif()

    file(RENAME ${REPO_DESTINATION_DIR}/gpt4all-backend ${DESTINATION}/gpt4all)

    file(REMOVE ${REPO_DESTINATION_DIR})
  endif()

  set(${RESULT} ${DESTINATION}/gpt4all PARENT_SCOPE)
endfunction()

# Get the version from the package.json
function(GetVersion)
  file(READ ${CMAKE_SOURCE_DIR}/package.json PACKAGE_JSON)
  string(JSON PACKAGE_VERSION GET ${PACKAGE_JSON} version)
  set(PACKAGE_VERSION ${PACKAGE_VERSION} PARENT_SCOPE)
endfunction()

# generate node.lib
function(GenerateNodeLib)
  if(MSVC AND CMAKE_JS_NODELIB_DEF AND CMAKE_JS_NODELIB_TARGET)
    # Generate node.lib
    execute_process(COMMAND ${CMAKE_AR} /def:${CMAKE_JS_NODELIB_DEF} /out:${CMAKE_JS_NODELIB_TARGET} ${CMAKE_STATIC_LINKER_FLAGS})
  endif()
endfunction()

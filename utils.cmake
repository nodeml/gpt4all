
function(DownloadGpt4All VERSION DESTINATION)
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
endfunction()

function(GenerateNodeLIB)
  if(MSVC AND CMAKE_JS_NODELIB_DEF AND CMAKE_JS_NODELIB_TARGET)
    # Generate node.lib
    execute_process(COMMAND ${CMAKE_AR} /def:${CMAKE_JS_NODELIB_DEF} /out:${CMAKE_JS_NODELIB_TARGET} ${CMAKE_STATIC_LINKER_FLAGS})
  endif()
endfunction(GenerateNodeLIB)

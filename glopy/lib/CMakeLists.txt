set(SRC
    gloc_wrapper.cpp
    )

include_directories(${PYTHON_INCLUDE_PATH})
include_directories(BEFORE ${symengine_BINARY_DIR}/src ${teuchos_BINARY_DIR})
include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src)

cython_add_module_pyx(gloc_wrapper gloc.pxd)
add_python_library(gloc_wrapper ${SRC})
target_link_libraries(gloc_wrapper gloc)
if (CMAKE_CXX_COMPILER_ID MATCHES GNU|Clang)
  # Must suppress strict aliasing for this file
  set_source_files_properties(
    gloc_wrapper.cpp
    PROPERTIES COMPILE_FLAGS "-fno-strict-aliasing -Wno-unused-function -Wno-constant-logical-operand"
    )
endif()

add_custom_target(cython
    COMMAND cython gloc_wrapper.pyx
    )

set(PY_PATH ${PYTHON_INSTALL_PATH}/gloc/lib)
install(TARGETS gloc_wrapper
            RUNTIME DESTINATION ${PY_PATH}
            ARCHIVE DESTINATION ${PY_PATH}
            LIBRARY DESTINATION ${PY_PATH}
        )
install(FILES __init__.py
    DESTINATION ${PY_PATH}
    )

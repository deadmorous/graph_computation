function(gc_build_lib_config target)
    # message(STATUS "gc_build_lib_config ${target}")

    set(_gblct_GENERATED_LIB_CONFIG_YAML_FILE
        "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/${target}_lib_config.yaml")

    set(_gblct_GENERATED_LIB_CONFIG_CPP_FILE
        "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/${target}_lib_config.cpp")

    set(_gblct_GENERATED_LIB_CONFIG_FILE_COMMAND
        ${GRAPH_COMPUTATION_CXX_ROOT_DIR}/cmake/gc_build_lib_config.sh
        ${target}
        ${_gblct_GENERATED_LIB_CONFIG_YAML_FILE}
        ${_gblct_GENERATED_LIB_CONFIG_CPP_FILE}
        $<CONFIG>
        $<TARGET_PROPERTY:${target},INCLUDE_DIRECTORIES>
        $<TARGET_FILE:${target}>
        $<TARGET_PROPERTY:${target},COMPILE_OPTIONS>)

    add_custom_command(
        OUTPUT
            "${_gblct_GENERATED_LIB_CONFIG_CPP_FILE}"
        COMMAND ${_gblct_GENERATED_LIB_CONFIG_FILE_COMMAND}
        DEPENDS ${GRAPH_COMPUTATION_CXX_ROOT_DIR}/cmake/gc_build_lib_config.sh
        VERBATIM)

    # Add a custom target to enforce dependency resolution
    add_custom_target(${target}_lib_config DEPENDS "${_gblct_GENERATED_LIB_CONFIG_CPP_FILE}")

    message(STATUS "GENERATED_LIB_CONFIG_CPP_FILE: ${_gblct_GENERATED_LIB_CONFIG_CPP_FILE}")

    add_dependencies(lib_config ${target}_lib_config)

    target_sources(lib_config PRIVATE "${_gblct_GENERATED_LIB_CONFIG_CPP_FILE}")

endfunction()

# Author: Scott MacDonald
# CMake module for C++ ASAN/TSAN/UBSAN.
# -----------------------------------------------------------------------------
# NOTE: Only use one sanitizer per build output. Results are best when all
#       targets are built with the sanitizer. Do not mix and match.

function(target_use_cpp_sanitizer target_name sanitizer_name)
    if (${sanitizer_name} STREQUAL "none")
        # Do nothing.
    elseif (${sanitizer_name} STREQUAL "asan")
        target_use_asan(${target_name})
    elseif (${sanitizer_name} STREQUAL "ubsan")
        target_use_ubsan(${target_name})
    else()
        message(WARNING "Unknown sanitizer named '${sanitizer_name}' for target '${target_name}'")
    endif()
endfunction()

# Instrument C++ target with address sanitizer
function(target_use_asan target_name)
    message(STATUS "Enabled ASAN for ${target_name}")

    # TODO: Add support for MSVC
    # TODO: Is PUBLIC the right option here? The final executable must have it enabled but its
    #       unclear if libraries propogate this dependency.
    if (MSVC)
        message(WARNING "ASAN requested but support for MSVC missing")
    else()
        target_compile_options(${target_name} PUBLIC -fsanitize=address -fno-omit-frame-pointer)
        target_link_libraries(${target_name} PUBLIC -fsanitize=address)
    endif()
endfunction()

# Instrument C++ target with address sanitizer
function(target_use_ubsan target_name)
    message(STATUS "Enabled USAN for ${target_name}")

    # TODO: Add support for MSVC
    # TODO: Is PUBLIC the right option here? The final executable must have it enabled but its
    #       unclear if libraries propogate this dependency.
    if (MSVC)
        message(WARNING "ASAN requested but support for MSVC missing")
    else()
        target_compile_options(${target_name} PUBLIC -fsanitize=undefined)
        target_link_libraries(${target_name} PUBLIC -fsanitize=undefined)
    endif()
endfunction()
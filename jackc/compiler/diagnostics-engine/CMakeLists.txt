add_library(diagnostics_engine STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/engine.c
)

if (false)
else()
    target_sources(diagnostics_engine PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/translations/en.c
    )
endif()

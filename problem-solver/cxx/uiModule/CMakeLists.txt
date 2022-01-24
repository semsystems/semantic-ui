set(SOURCES
        "keynodes/Keynodes.cpp"
        "agents/UiJsonTranslationAgent.cpp"
        "agents/UiJsonTranslator.cpp"
        "agents/UiActionProcessingAgent.cpp"
        "utils/SearchUtils.cpp"
        "uiModule.cpp"
        )

set(HEADERS
        "keynodes/Keynodes.hpp"
        "agents/UiJsonTranslationAgent.hpp"
        "agents/UiJsonTranslator.hpp"
        "agents/UiActionProcessingAgent.hpp"
        "utils/SearchUtils.hpp"
        "uiModule.hpp")

include_directories(
        ${CMAKE_CURRENT_LIST_DIR}
        ${SC_MEMORY_SRC}
        ${SC_KPM_SRC}
        ${SC_TOOLS_SRC}
        ${SC_COMMON_MODULE_SRC})

add_library(uiModule SHARED ${SOURCES} ${HEADERS})

target_link_libraries(uiModule sc-memory sc-agents-common sc-builder-lib)

sc_codegen_ex(uiModule ${CMAKE_CURRENT_LIST_DIR} ${CMAKE_CURRENT_LIST_DIR}/generated)

if (${SC_BUILD_TESTS})
   add_subdirectory(test)
endif ()

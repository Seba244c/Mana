cmake_minimum_required (VERSION 3.31)
include(FetchContent)

# Deps
FetchContent_Declare(
    doxygen-awesome-css
    URL https://github.com/jothepro/doxygen-awesome-css/archive/refs/heads/main.zip
)
FetchContent_MakeAvailable(doxygen-awesome-css)
FetchContent_GetProperties(doxygen-awesome-css SOURCE_DIR AWESOME_CSS_DIR) # This allows us to get the path to doxygen-awesome.css

#set(DOXYGEN_USE_MDFILE_AS_MAINPAGE README.md)
set(DOXYGEN_HTML_EXTRA_STYLESHEET "${AWESOME_CSS_DIR}/doxygen-awesome.css")
set(DOXYGEN_GENERATE_HTML YES)
doxygen_add_docs(Docs ${PROJECT_SOURCE_DIR}/src/)

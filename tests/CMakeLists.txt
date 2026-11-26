find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Test)

# Builds a test program and registers it in CTest.
function(mdeditor_add_test name library)
    add_executable(${name} ${ARGN})
    target_link_libraries(${name} PRIVATE ${library} Qt${QT_VERSION_MAJOR}::Test)
    mdeditor_configure_target(${name})
    add_test(NAME ${name} COMMAND ${name})
    # The tests do not need a display, so they also run where there is none,
    # such as on a build server.
    set_tests_properties(${name} PROPERTIES
        ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
        TIMEOUT 300
    )
endfunction()

# The Markdown converter.
mdeditor_add_test(tst_markdown mdeditor_core tst_markdown.cpp)

# The files and the two exports.
mdeditor_add_test(tst_document mdeditor_core tst_document.cpp)

# The document that the preview shows and that the PDF export prints.
mdeditor_add_test(tst_rich_text mdeditor_core tst_rich_text.cpp)

# The window of the program.
mdeditor_add_test(tst_main_window mdeditor_gui tst_main_window.cpp dialog_script.h)

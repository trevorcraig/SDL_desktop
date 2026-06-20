cmake_minimum_required(VERSION 3.10)
project(sdl_desktop C)

# 1. Paths for SDL3
if(NOT SDL3_DIR)
    set(SDL3_DIR $ENV{SDL3_DIR} CACHE PATH "Path to SDL3 root directory")
endif()
if(NOT SDL3_TTF_DIR)
    set(SDL3_TTF_DIR $ENV{SDL3_TTF_DIR} CACHE PATH "Path to SDL3_ttf root directory")
endif()
if(NOT SDL3_IMAGE_DIR)
    set(SDL3_IMAGE_DIR $ENV{SDL3_IMAGE_DIR} CACHE PATH "Path to SDL3_image root directory")
endif()
# 2. Setup Include Paths
include_directories(
    "include" 
    "${SDL3_DIR}/include" 
    "${SDL3_TTF_DIR}/include"
    "${SDL3_IMAGE_DIR}/include"
)
link_directories("${SDL3_DIR}/lib" "${SDL3_TTF_DIR}/lib" "${SDL3_IMAGE_DIR}/lib")

# --- THE LIBRARY PART ---

# 3. Reference the new 'src' path
add_library(sdl_desktop_lib STATIC src/button.c src/label.c src/panel.c src/widget.c src/textbox.c src/window.c src/menu_bar.c src/file_dialog.c)
target_link_libraries(sdl_desktop_lib SDL3 SDL3_ttf SDL3_image comdlg32 m)

# --- THE EXAMPLES PART ---

function(add_desktop_example name source)
    add_executable(${name} ${source})
    target_link_libraries(${name} sdl_desktop_lib)
    
    if(WIN32)
        add_custom_command(TARGET ${name} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${SDL3_DIR}/bin/SDL3.dll"
            "${SDL3_TTF_DIR}/bin/SDL3_ttf.dll"
            "${SDL3_IMAGE_DIR}/bin/SDL3_image.dll"
            $<TARGET_FILE_DIR:${name}>)
    endif()

    # 4. Copy font from the 'assets' folder
    add_custom_command(TARGET ${name} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/assets/PTC55F.ttf"
        $<TARGET_FILE_DIR:${name}>)
endfunction()

# 5. Build your examples
add_desktop_example(demo_main examples/main.c)
add_desktop_example(demo_calculator examples/calculator.c)

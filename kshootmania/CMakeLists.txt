cmake_minimum_required(VERSION 3.16)
project(kshootmania CXX)

if (NOT CMAKE_CONFIGURATION_TYPES AND 
    NOT CMAKE_NO_BUILD_TYPE AND
    NOT CMAKE_BUILD_TYPE AND
    CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    message(STATUS "[!] Setting build type to 'Release' as none was specified.")
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/App)

file(GLOB_RECURSE KSM_SRC_FILES src/*.cpp)
list(REMOVE_ITEM KSM_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/stdafx.cpp)

add_executable(kshootmania
  ${KSM_SRC_FILES}
  )

if(APPLE)
    set_target_properties(kshootmania PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_BUNDLE_NAME "K-Shoot MANIA"
        MACOSX_BUNDLE_BUNDLE_VERSION "2.0.0"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "2.0.0-alpha2"
        MACOSX_BUNDLE_ICON_FILE icon.icns
    )
    
    set(APP_BUNDLE_PATH ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/kshootmania.app)
    
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/App/engine")
        add_custom_command(TARGET kshootmania POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/App/engine
            ${APP_BUNDLE_PATH}/Contents/Resources/engine
        )
    endif()
    
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/App/ui")
        add_custom_command(TARGET kshootmania POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/App/ui
            ${APP_BUNDLE_PATH}/Contents/Resources/ui
        )
    endif()
    
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/App/shaders")
        add_custom_command(TARGET kshootmania POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/App/shaders
            ${APP_BUNDLE_PATH}/Contents/Resources/shaders
        )
    endif()
    
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/App/assets")
        add_custom_command(TARGET kshootmania POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/App/assets
            ${APP_BUNDLE_PATH}/Contents/Resources/assets
        )
    endif()
    
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/App/imgs")
        add_custom_command(TARGET kshootmania POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/App/imgs
            ${APP_BUNDLE_PATH}/Contents/Resources/imgs
        )
    endif()
    
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/App/lang")
        add_custom_command(TARGET kshootmania POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/App/lang
            ${APP_BUNDLE_PATH}/Contents/Resources/lang
        )
    endif()
    
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/App/se")
        add_custom_command(TARGET kshootmania POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/App/se
            ${APP_BUNDLE_PATH}/Contents/Resources/se
        )
    endif()
    
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/App/icon.icns)
        set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/App/icon.icns PROPERTIES
            MACOSX_PACKAGE_LOCATION "Resources"
        )
        target_sources(kshootmania PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/App/icon.icns)
    endif()
    
    add_custom_command(TARGET kshootmania POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory
        ${APP_BUNDLE_PATH}/Contents/Frameworks
    )
    
    add_custom_command(TARGET kshootmania POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/ksmaudio/third_party_macos/bass/libbass.dylib
        ${APP_BUNDLE_PATH}/Contents/Frameworks/libbass.dylib
    )
    
    add_custom_command(TARGET kshootmania POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/ksmaudio/third_party_macos/bass_fx/libbass_fx.dylib
        ${APP_BUNDLE_PATH}/Contents/Frameworks/libbass_fx.dylib
    )
    
    set_target_properties(kshootmania PROPERTIES
        BUILD_RPATH "@executable_path/../Frameworks"
        INSTALL_RPATH "@executable_path/../Frameworks"
    )
endif()

target_include_directories(kshootmania PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/CoTaskLib/include
)

find_package(Siv3D REQUIRED)
target_link_libraries(kshootmania PRIVATE
    Siv3D::Siv3D
    kson
    ksmaudio
    NocoUI
)

if(APPLE)
	target_link_libraries(kshootmania PRIVATE ksmplatform_macos)
endif()

target_compile_features(kshootmania PRIVATE cxx_std_20)

if(APPLE)
    # Siv3DのmacOS/Linux版はプリコンパイル済みヘッダ非対応のため通常のヘッダとしてインクルード
    target_compile_options(kshootmania PRIVATE
        -Wall
        -Wextra
        -Wno-unknown-pragmas
        -include ${CMAKE_CURRENT_SOURCE_DIR}/src/stdafx.h
    )
    
    target_compile_definitions(kshootmania PRIVATE
        SIV3D_PLATFORM_MACOS
    )
elseif(UNIX)
    # Siv3DのmacOS/Linux版はプリコンパイル済みヘッダ非対応のため通常のヘッダとしてインクルード
    target_compile_options(kshootmania PRIVATE
        -Wall
        -Wextra
        -Wno-unknown-pragmas
        -include ${CMAKE_CURRENT_SOURCE_DIR}/src/stdafx.h
    )

    # BASSライブラリへのRPATHを設定
    set_target_properties(kshootmania PROPERTIES
        BUILD_RPATH "${CMAKE_SOURCE_DIR}/ksmaudio/third_party_linux/bass:${CMAKE_SOURCE_DIR}/ksmaudio/third_party_linux/bass_fx"
        INSTALL_RPATH "$ORIGIN/../lib"
    )
endif()

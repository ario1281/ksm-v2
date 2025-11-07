cmake_minimum_required(VERSION 3.16)

project(ksmplatform_macos)

# Objective-C++のソースファイル
add_library(ksmplatform_macos STATIC
	src/input_method.mm
)

target_include_directories(ksmplatform_macos
	PUBLIC
		include
)

# macOSフレームワークのリンク
target_link_libraries(ksmplatform_macos
	PUBLIC
		"-framework Carbon"
		"-framework CoreFoundation"
)

# Objective-C++のコンパイルフラグ
set_source_files_properties(src/input_method.mm PROPERTIES
	COMPILE_FLAGS "-x objective-c++"
)

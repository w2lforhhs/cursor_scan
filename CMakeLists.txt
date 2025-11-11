cmake_minimum_required(VERSION 3.10)
project(CursorInfoCollector C)

set(CMAKE_C_STANDARD 11)

# 强制使用静态库
set(CMAKE_FIND_LIBRARY_SUFFIXES ".a;.lib")
set(BUILD_SHARED_LIBS OFF)

# 设置 MSVC 运行时库为静态链接
if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    # 替换编译选项中的动态运行时为静态运行时
    foreach(flag_var
        CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE
        CMAKE_C_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELWITHDEBINFO)
        if(${flag_var} MATCHES "/MD")
            string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
        endif()
    endforeach()
endif()

# 添加可执行文件
add_executable(cursor_info_collector 
    cursor_info_collector.c
)

# 查找SQLite3库（静态版本）
find_package(unofficial-sqlite3 CONFIG QUIET)
if(unofficial-sqlite3_FOUND)
    # vcpkg 使用这个包名
    set(SQLITE3_TARGET unofficial::sqlite3::sqlite3)
else()
    find_package(SQLite3 REQUIRED)
    if(TARGET SQLite::SQLite3)
        set(SQLITE3_TARGET SQLite::SQLite3)
    else()
        set(SQLITE3_TARGET ${SQLite3_LIBRARIES})
        target_include_directories(cursor_info_collector PRIVATE ${SQLite3_INCLUDE_DIRS})
    endif()
endif()

# 链接库
target_link_libraries(cursor_info_collector 
    ${SQLITE3_TARGET}
)

# Windows特定设置
if(WIN32)
    target_link_libraries(cursor_info_collector 
        Shell32  # For SHGetFolderPath
    )
endif()

# 设置输出目录
set_target_properties(cursor_info_collector PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# 安装规则
install(TARGETS cursor_info_collector
    RUNTIME DESTINATION bin
)

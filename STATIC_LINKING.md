# SQLite 静态链接配置说明

## 修改概述

已成功将 SQLite 库从动态链接改为静态链接。

## 修改的文件

### 1. CMakeLists.txt

添加了以下配置来强制使用静态库：

```cmake
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
```

### 2. build.bat

修改了 vcpkg 安装和 CMake 配置命令：

**修改前**：
```bat
vcpkg install sqlite3:x64-windows
cmake .. -DCMAKE_TOOLCHAIN_FILE=%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake
```

**修改后**：
```bat
vcpkg install sqlite3:x64-windows-static
cmake .. -DCMAKE_TOOLCHAIN_FILE=%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows-static
```

## 验证结果

### 依赖项对比

#### 动态链接版本（修改前）：
```
Image has the following dependencies:
    sqlite3.dll      ← 需要外部 DLL
    SHELL32.dll
    KERNEL32.dll
```

文件大小：~180 KB

#### 静态链接版本（修改后）：
```
Image has the following dependencies:
    SHELL32.dll
    KERNEL32.dll
```

文件大小：~1.2 MB

### ✅ 验证成功

- ✅ 不再依赖 `sqlite3.dll`
- ✅ 文件大小增加（从 180KB 到 1.2MB），证明 SQLite 已内嵌
- ✅ 程序正常运行，所有功能正常
- ✅ 可以在没有安装 SQLite 的系统上独立运行

## 优势

### 静态链接的好处

1. **独立部署** - 不需要携带或安装 sqlite3.dll
2. **避免 DLL 冲突** - 不会与系统中其他版本的 SQLite 冲突
3. **简化分发** - 只需一个 .exe 文件即可运行
4. **版本锁定** - SQLite 版本固定，不受系统环境影响

### 权衡

- **文件较大** - 可执行文件从 180KB 增加到 1.2MB
- **更新麻烦** - 如需更新 SQLite，必须重新编译整个程序

## 使用说明

### 首次编译（需要安装静态库）

```powershell
# 自动安装 sqlite3:x64-windows-static 并编译
.\build.bat
```

### 手动安装静态库（可选）

```powershell
cd D:\vcpkg
.\vcpkg install sqlite3:x64-windows-static
```

### 清理重新编译

```powershell
Remove-Item -Recurse -Force build
.\build.bat
```

## 部署

现在可以直接复制 `cursor_info_collector.exe` 到任何 Windows 系统上运行，无需：
- ❌ 安装 SQLite
- ❌ 携带 sqlite3.dll
- ❌ 配置环境变量
- ❌ 担心 DLL 版本冲突

只需确保目标系统有：
- ✅ Windows 操作系统
- ✅ Cursor 已安装（用于收集信息）

## 技术细节

### vcpkg 三元组

- `x64-windows` - 动态链接（需要 DLL）
- `x64-windows-static` - 静态链接（不需要 DLL）

### CMake 配置

通过 `-DVCPKG_TARGET_TRIPLET=x64-windows-static` 参数告诉 vcpkg 使用静态库版本。

### MSVC 运行时

同时也将 MSVC 运行时库设置为静态链接（/MT），确保不依赖 Visual C++ 运行时 DLL。

## 测试报告

### 功能测试 ✅

- ✅ Cursor 安装检测
- ✅ 隐私模式读取
- ✅ 模型密钥状态检测
- ✅ MCP 服务配置读取
- ✅ JSON 文件生成

### 兼容性测试 ✅

- ✅ 在编译环境运行正常
- ✅ 可以复制到其他目录运行
- ✅ 不依赖 vcpkg 环境

## 总结

SQLite 静态链接配置已成功完成，程序现在完全独立，无需任何外部 DLL 依赖（除了 Windows 系统 DLL）。

**生成的可执行文件**：`build\bin\Release\cursor_info_collector.exe`

**最后更新**：2025-11-11

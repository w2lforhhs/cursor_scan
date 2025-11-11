# 快速开始指南

## 最简单的使用方法

### 1. 安装依赖（仅需一次）

#### 方式A: 使用 vcpkg（推荐）

```powershell
# 下载并安装 vcpkg
git clone https://github.com/Microsoft/vcpkg.git C:\vcpkg
cd C:\vcpkg
.\bootstrap-vcpkg.bat

# 安装 SQLite3
.\vcpkg install sqlite3:x64-windows

# 集成到系统
.\vcpkg integrate install
```

#### 方式B: 下载预编译的 SQLite

1. 访问: https://www.sqlite.org/download.html
2. 下载 `sqlite-amalgamation-xxxxxx.zip` 和 `sqlite-dll-win64-x64-xxxxxx.zip`
3. 解压到项目目录

### 2. 编译程序

双击运行 `build.bat`，或在命令行中：

```powershell
.\build.bat
```

脚本会自动：
- 检测 vcpkg 安装
- 配置 CMake
- 编译程序

### 3. 运行程序

#### 方式A: 使用快速运行脚本

双击 `run.bat` 或：

```powershell
.\run.bat
```

#### 方式B: 手动运行

```powershell
cd build\bin\Release
.\cursor_info_collector.exe
```

### 4. 查看结果

程序运行后，会在当前目录生成 JSON 文件：

```
cursor_<你的计算机名>_<时间戳>.json
```

例如：`cursor_MY-PC_20251111_143022.json`

## 输出示例

```json
{
  "pc_name": "MY-COMPUTER",
  "timestamp": "20251111_143022",
  "cursor_installed": true,
  "privacy_mode": "privacy",
  "model_keys": {
    "use_claude_key": false,
    "use_google_key": false,
    "use_openai_key": false,
    "use_azure": false,
    "use_bedrock": false
  },
  "mcp_servers": ["weather-service", "sequential-thinking"]
}
```

## 常见问题

### Q: 编译失败，提示找不到 sqlite3.h

**A:** 确保已正确安装 SQLite3：
- 使用 vcpkg：`vcpkg install sqlite3:x64-windows`
- 手动安装：下载 SQLite 开发文件并放在系统能找到的位置

### Q: 运行时提示找不到 sqlite3.dll

**A:** 
1. 如果使用 vcpkg，确保执行了 `vcpkg integrate install`
2. 或者将 sqlite3.dll 复制到程序所在目录
3. 或者将 SQLite DLL 所在目录添加到 PATH 环境变量

### Q: 提示 "Cannot open database"

**A:** 
- 确保 Cursor 已安装并至少运行过一次
- 检查是否有权限访问 `%APPDATA%\Roaming\Cursor` 目录
- 尝试以管理员身份运行程序

### Q: privacy_mode 显示为 "Unknown"

**A:** 可能的原因：
- Cursor 版本较新，数据库结构发生变化
- 数据库文件损坏
- 配置字段不存在（新安装的 Cursor）

### Q: mcp_servers 显示为空数组 []

**A:** 这是正常的，表示：
- 未配置任何 MCP 服务
- 或 `.cursor/mcp.json` 文件不存在

## 项目结构

```
cursor_scan/
├── cursor_info_collector.h    # 头文件
├── cursor_info_collector.c    # 主程序
├── CMakeLists.txt             # CMake 配置
├── build.bat                  # 编译脚本
├── run.bat                    # 快速运行脚本
├── README.md                  # 详细文档
├── QUICKSTART.md             # 本文档
├── .gitignore                # Git 忽略规则
├── ItemTable.data            # 数据样例（参考）
└── state.vscdb.txt           # 数据库结构（参考）
```

## 技术支持

如遇到问题，请检查：

1. **编译环境**
   - CMake 版本 >= 3.10
   - Visual Studio 或 MinGW-w64
   - SQLite3 开发库

2. **运行环境**
   - Windows 操作系统
   - 已安装 Cursor
   - 有权限访问 AppData 目录

3. **日志信息**
   - 查看程序输出的错误信息
   - 检查 JSON 文件中的 `error` 或 `warning` 字段

## 下一步

- 查看 [README.md](README.md) 了解更多技术细节
- 修改代码以收集其他配置信息
- 将工具集成到自动化脚本中

## 更新日志

### v1.0.0 (2025-11-11)
- 初始版本
- 支持基本信息收集
- 支持 MCP 服务检测
- 完整的错误处理

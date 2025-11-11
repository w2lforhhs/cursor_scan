# Cursor Information Collector

一个用于收集Windows系统上Cursor软件配置信息的C语言工具。

## 功能特性

该工具会自动收集以下信息：

1. **Cursor安装检测** - 检查Cursor是否已安装
2. **隐私模式设置** - 读取用户的隐私模式配置（共享/隐私模式）
3. **模型API密钥状态** - 检测以下模型密钥的启用状态：
   - Claude Key
   - Google Key
   - OpenAI Key
   - Azure
   - AWS Bedrock
4. **MCP服务配置** - 读取配置的MCP（Model Context Protocol）服务列表

## 数据源

- **SQLite数据库**: `%APPDATA%\Roaming\Cursor\User\globalStorage\state.vscdb`
  - 表: `ItemTable`
  - 字段: `key` 和 `value`
  
- **MCP配置文件**: `%USERPROFILE%\.cursor\mcp.json`

## 编译要求

### 必需组件

- **编译器**: Microsoft Visual C++ (MSVC) 或 MinGW-w64
- **CMake**: 版本 3.10 或更高
- **SQLite3**: 开发库和头文件

### Windows 上安装 SQLite3

#### 方法1: 使用 vcpkg (推荐)

```powershell
# 安装 vcpkg
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat

# 安装 SQLite3
.\vcpkg install sqlite3:x64-windows

# 集成到 CMake
.\vcpkg integrate install
```

#### 方法2: 手动下载

1. 访问 [SQLite下载页面](https://www.sqlite.org/download.html)
2. 下载以下文件：
   - `sqlite-amalgamation-xxxxxx.zip` (源代码)
   - `sqlite-dll-win64-x64-xxxxxx.zip` (DLL文件)
3. 将文件解压到项目目录

## 编译步骤

### 使用 vcpkg (推荐)

```powershell
# 创建构建目录
mkdir build
cd build

# 配置项目 (指定 vcpkg toolchain)
cmake .. -DCMAKE_TOOLCHAIN_FILE=[vcpkg安装路径]/scripts/buildsystems/vcpkg.cmake

# 编译
cmake --build . --config Release
```

### 使用 Visual Studio

```powershell
# 创建构建目录
mkdir build
cd build

# 生成 Visual Studio 解决方案
cmake .. -G "Visual Studio 17 2022" -A x64

# 打开生成的 .sln 文件，或使用命令行编译
cmake --build . --config Release
```

### 使用 MinGW

```powershell
# 创建构建目录
mkdir build
cd build

# 配置并编译
cmake .. -G "MinGW Makefiles"
mingw32-make
```

## 使用方法

编译完成后，在 `build/bin/Release` 目录下会生成 `cursor_info_collector.exe`。

```powershell
# 运行程序
.\cursor_info_collector.exe
```

程序会自动：
1. 检测Cursor安装状态
2. 读取配置信息
3. 在当前目录生成 JSON 文件：`cursor_<计算机名>_<时间戳>.json`

## 输出格式

生成的 JSON 文件格式示例：

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

### 字段说明

- `pc_name`: 计算机名称
- `timestamp`: 收集时间戳 (格式: YYYYMMDD_HHMMSS)
- `cursor_installed`: Cursor是否已安装
- `privacy_mode`: 隐私模式状态
  - `"shared"`: 共享模式
  - `"privacy"`: 隐私模式
  - `"Unknown"`: 未知或无法读取
- `model_keys`: 各个模型API密钥的启用状态
- `mcp_servers`: 配置的MCP服务名称列表

## 错误处理

如果遇到以下情况，会在JSON中记录错误信息：

- Cursor未安装：`cursor_installed` 为 `false`
- 数据库无法访问：在 `error` 字段中记录详细信息
- MCP配置文件不存在：返回空数组 `[]`
- 配置字段缺失：相应字段标记为 `Unknown` 或 `false`

## 技术细节

### 隐私模式读取

从 `ItemTable` 表中读取 `cursorai/donotchange/privacyMode` 字段：
- `true` → 隐私模式
- `false` → 共享模式

### 模型密钥检测

从 `ItemTable` 表的大JSON配置中提取：
```
key: src.vs.platform.reactivestorage.browser.reactiveStorageServiceImpl.persistentStorage.applicationUser
```

解析其中的：
- `useClaudeKey`
- `useGoogleKey`
- `useOpenAIKey`
- `azureState.useAzure`
- `bedrockState.useBedrock`

### MCP服务提取

从 `%USERPROFILE%\.cursor\mcp.json` 文件的 `mcpServers` 对象中提取所有服务名称（键名）。

## 许可证

MIT License

## 注意事项

1. **只读访问**: 程序以只读模式打开数据库，不会修改任何配置
2. **权限要求**: 需要读取用户 AppData 目录的权限
3. **路径适配**: 自动适配不同用户的路径
4. **错误容忍**: 即使部分信息无法读取，仍会生成包含可用信息的JSON文件

## 故障排除

### 编译错误

1. **找不到 sqlite3.h**
   - 确保已正确安装 SQLite3 开发库
   - 检查 CMake 是否找到了 SQLite3 (查看 CMake 输出)

2. **链接错误**
   - 确保 SQLite3 库文件在系统路径中
   - 使用 vcpkg 时，确保已执行 `vcpkg integrate install`

### 运行时错误

1. **无法打开数据库**
   - 确认 Cursor 已安装且至少运行过一次
   - 检查数据库文件路径是否正确

2. **权限错误**
   - 以管理员身份运行程序
   - 检查 AppData 目录的访问权限

## 联系方式

如有问题或建议，请提交 Issue。

# 项目文件说明

## 目录结构

```
cursor_scan/
│
├── 📄 源代码文件
│   ├── cursor_info_collector.h     # C 头文件，定义数据结构和函数声明
│   └── cursor_info_collector.c     # C 主程序，实现所有功能逻辑
│
├── 🔧 构建配置
│   ├── CMakeLists.txt              # CMake 构建配置文件
│   ├── build.bat                   # Windows 自动编译脚本
│   └── run.bat                     # Windows 快速运行脚本
│
├── 📖 文档文件
│   ├── README.md                   # 完整技术文档（编译、使用、API说明）
│   ├── QUICKSTART.md              # 快速开始指南（新手友好）
│   ├── PROJECT_SUMMARY.md         # 项目总结（功能、实现、测试）
│   └── FILE_STRUCTURE.md          # 本文件（目录结构说明）
│
├── 📋 参考资料
│   ├── ItemTable.data             # Cursor 数据库样例数据
│   ├── state.vscdb.txt            # 数据库表结构定义
│   └── example_output.json        # 程序输出示例
│
├── ⚙️ 配置文件
│   └── .gitignore                 # Git 忽略规则
│
└── 📂 生成目录（编译后）
    └── build/                     # CMake 构建目录
        ├── bin/                   # 可执行文件输出目录
        │   └── Release/
        │       └── cursor_info_collector.exe
        └── ...                    # 其他构建文件
```

## 文件详细说明

### 源代码文件

#### `cursor_info_collector.h`
**类型**: C 头文件  
**用途**: 
- 定义 `CursorInfo` 数据结构
- 声明所有公共函数接口
- 包含必要的头文件引用

**关键内容**:
```c
typedef struct {
    char pc_name[256];
    char timestamp[64];
    bool cursor_installed;
    char privacy_mode[32];
    bool use_claude_key;
    bool use_google_key;
    bool use_openai_key;
    bool use_azure;
    bool use_bedrock;
    char mcp_servers[4096];
    char error_message[1024];
} CursorInfo;
```

#### `cursor_info_collector.c`
**类型**: C 源文件  
**用途**: 实现所有功能逻辑  
**大小**: ~600 行代码

**主要函数**:
- `check_cursor_installed()` - 检测 Cursor 安装
- `get_computer_name()` - 获取计算机名
- `get_timestamp()` - 生成时间戳
- `read_database_config()` - 读取数据库配置
- `read_mcp_config()` - 读取 MCP 配置
- `collect_cursor_info()` - 主收集函数
- `save_to_json()` - 保存 JSON 结果
- `main()` - 程序入口

### 构建配置

#### `CMakeLists.txt`
**类型**: CMake 配置  
**用途**: 定义编译规则和依赖

**关键配置**:
```cmake
find_package(SQLite3 REQUIRED)
target_link_libraries(cursor_info_collector SQLite3::SQLite3)
```

#### `build.bat`
**类型**: Windows 批处理脚本  
**用途**: 自动化编译过程

**功能**:
- 检测 vcpkg 安装
- 自动安装 SQLite3（如需要）
- 配置 CMake
- 编译项目
- 显示编译结果

**使用**: 双击运行或 `.\build.bat`

#### `run.bat`
**类型**: Windows 批处理脚本  
**用途**: 快速运行程序

**功能**:
- 调用 build.bat 编译
- 查找可执行文件
- 运行程序
- 显示结果

**使用**: 双击运行或 `.\run.bat`

### 文档文件

#### `README.md`
**类型**: Markdown 技术文档  
**目标读者**: 开发者和技术用户  
**内容**: 约 400 行

**章节**:
1. 项目介绍
2. 功能特性
3. 数据源说明
4. 编译要求和步骤
5. 使用方法
6. 输出格式
7. 技术实现细节
8. 故障排除

#### `QUICKSTART.md`
**类型**: Markdown 快速指南  
**目标读者**: 新手用户  
**内容**: 约 200 行

**章节**:
1. 最简单的使用方法
2. 3 步开始（安装-编译-运行）
3. 常见问题 FAQ
4. 项目结构
5. 技术支持

#### `PROJECT_SUMMARY.md`
**类型**: Markdown 项目总结  
**目标读者**: 项目管理者和审查者  
**内容**: 约 300 行

**章节**:
1. 项目概述
2. 已实现功能（完成度 100%）
3. 关键技术实现
4. 代码质量评估
5. 测试建议
6. 已知限制
7. 未来改进方向

#### `FILE_STRUCTURE.md` (本文件)
**类型**: Markdown 结构说明  
**用途**: 说明项目文件组织

### 参考资料

#### `ItemTable.data`
**类型**: 文本数据文件  
**用途**: Cursor 数据库实际数据样例  
**大小**: ~50 行

**内容**: 包含所有需要提取的字段示例
- `cursorai/donotchange/privacyMode`
- `src.vs.platform.reactivestorage.browser...` (大JSON)
- 其他配置字段

#### `state.vscdb.txt`
**类型**: 文本文档  
**用途**: 数据库表结构定义

**内容**:
```sql
CREATE TABLE ItemTable (
    key TEXT UNIQUE ON CONFLICT REPLACE, 
    value BLOB
)
```

#### `example_output.json`
**类型**: JSON 示例文件  
**用途**: 展示程序输出格式

**示例内容**:
```json
{
  "pc_name": "EXAMPLE-PC",
  "timestamp": "20251111_143022",
  "cursor_installed": true,
  "privacy_mode": "privacy",
  "model_keys": { ... },
  "mcp_servers": [...]
}
```

### 配置文件

#### `.gitignore`
**类型**: Git 配置  
**用途**: 定义不提交到版本控制的文件

**忽略内容**:
- 编译产物 (`build/`, `*.exe`, `*.obj`)
- IDE 配置 (`.vs/`, `.vscode/`)
- 生成的 JSON 文件 (`cursor_*.json`)
- 保留参考文件 (`ItemTable.data`, `state.vscdb.txt`)

### 生成目录

#### `build/`
**类型**: 目录（编译后生成）  
**用途**: 存储编译产物

**结构**:
```
build/
├── bin/
│   └── Release/
│       └── cursor_info_collector.exe    # 最终可执行文件
├── CMakeFiles/                          # CMake 中间文件
├── CMakeCache.txt                       # CMake 缓存
└── ...                                  # 其他构建文件
```

**注意**: 此目录由 `build.bat` 自动创建，不包含在源代码中。

## 文件大小概览

| 文件类型 | 文件数量 | 总大小（估计） |
|---------|---------|--------------|
| 源代码 | 2 | ~30 KB |
| 构建脚本 | 3 | ~15 KB |
| 文档 | 5 | ~100 KB |
| 参考资料 | 3 | ~50 KB |
| 配置文件 | 1 | ~1 KB |
| **总计** | **14** | **~196 KB** |

## 编辑顺序建议

如果需要修改代码，建议按以下顺序阅读：

1. `QUICKSTART.md` - 了解基本用法
2. `README.md` - 理解技术细节
3. `cursor_info_collector.h` - 查看数据结构
4. `cursor_info_collector.c` - 阅读实现逻辑
5. `ItemTable.data` - 了解数据格式
6. `PROJECT_SUMMARY.md` - 掌握整体架构

## 依赖关系图

```
cursor_info_collector.exe
    ├── cursor_info_collector.c
    │   └── cursor_info_collector.h
    ├── sqlite3.lib (通过 CMake)
    └── Windows API
        ├── windows.h
        ├── shlobj.h
        └── time.h

build.bat
    └── CMakeLists.txt
        └── vcpkg (可选)

run.bat
    └── build.bat
```

## 使用流程图

```
用户
 │
 ├─→ [首次使用] 阅读 QUICKSTART.md
 │       │
 │       └─→ 安装依赖 (vcpkg + SQLite3)
 │
 ├─→ 双击 build.bat
 │       │
 │       └─→ 生成 cursor_info_collector.exe
 │
 └─→ 双击 run.bat 或运行 .exe
         │
         └─→ 生成 cursor_<PC名>_<时间>.json
```

## 维护指南

### 添加新功能
1. 在 `cursor_info_collector.h` 添加字段到 `CursorInfo` 结构
2. 在 `cursor_info_collector.c` 实现数据收集函数
3. 修改 `save_to_json()` 以输出新字段
4. 更新 `README.md` 文档

### 修复 Bug
1. 在 `cursor_info_collector.c` 定位问题代码
2. 修复并测试
3. 更新 `PROJECT_SUMMARY.md` 的已知限制章节

### 更新文档
- **功能变更**: 更新 `README.md` 和 `QUICKSTART.md`
- **新增限制**: 更新 `PROJECT_SUMMARY.md`
- **结构变更**: 更新本文件 `FILE_STRUCTURE.md`

## 许可证

所有文件采用 MIT 许可证（如有需要可添加 LICENSE 文件）。

---

**最后更新**: 2025-11-11  
**版本**: 1.0.0  
**作者**: [Your Name]

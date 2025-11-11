#include "cursor_info_collector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>
#include <shlobj.h>
#include <sqlite3.h>

// JSON解析辅助宏
#define JSON_BOOL_STR(b) ((b) ? "true" : "false")

// 检查Cursor是否安装
bool check_cursor_installed() {
    char cursor_path[MAX_PATH];
    
    // 获取 APPDATA 路径
    if (SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, cursor_path) != S_OK) {
        return false;
    }
    
    // 追加 Cursor 路径
    strcat_s(cursor_path, MAX_PATH, "\\Cursor");
    
    // 检查目录是否存在
    DWORD attrs = GetFileAttributesA(cursor_path);
    return (attrs != INVALID_FILE_ATTRIBUTES && (attrs & FILE_ATTRIBUTE_DIRECTORY));
}

// 获取计算机名
bool get_computer_name(char* buffer, size_t buffer_size) {
    DWORD size = (DWORD)buffer_size;
    return GetComputerNameA(buffer, &size);
}

// 获取当前时间戳
void get_timestamp(char* buffer, size_t buffer_size) {
    time_t now = time(NULL);
    struct tm timeinfo;
    localtime_s(&timeinfo, &now);
    strftime(buffer, buffer_size, "%Y%m%d_%H%M%S", &timeinfo);
}

// 从JSON字符串中提取布尔值
bool extract_json_bool(const char* json_str, const char* key) {
    if (!json_str || !key) return false;
    
    char search_key[256];
    snprintf(search_key, sizeof(search_key), "\"%s\":", key);
    
    const char* pos = strstr(json_str, search_key);
    if (!pos) return false;
    
    pos += strlen(search_key);
    // 跳过空白字符
    while (*pos == ' ' || *pos == '\t' || *pos == '\n' || *pos == '\r') pos++;
    
    return (strncmp(pos, "true", 4) == 0);
}

// 从数据库读取配置信息
bool read_database_config(CursorInfo* info) {
    sqlite3* db;
    sqlite3_stmt* stmt;
    char db_path[MAX_PATH];
    int rc;
    
    // 构建数据库路径
    if (SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, db_path) != S_OK) {
        strcpy_s(info->error_message, sizeof(info->error_message), 
                 "Failed to get APPDATA path");
        return false;
    }
    strcat_s(db_path, MAX_PATH, "\\Cursor\\User\\globalStorage\\state.vscdb");
    
    // 以只读模式打开数据库
    rc = sqlite3_open_v2(db_path, &db, SQLITE_OPEN_READONLY, NULL);
    if (rc != SQLITE_OK) {
        snprintf(info->error_message, sizeof(info->error_message),
                 "Cannot open database: %s", sqlite3_errmsg(db));
        return false;
    }
    
    // 初始化为Unknown状态
    strcpy_s(info->privacy_mode, sizeof(info->privacy_mode), "Unknown");
    info->use_claude_key = false;
    info->use_google_key = false;
    info->use_openai_key = false;
    info->use_azure = false;
    info->use_bedrock = false;
    
    // 准备SQL查询
    const char* sql = "SELECT key, value FROM ItemTable WHERE key IN ("
                     "'cursorai/donotchange/privacyMode', "
                     "'src.vs.platform.reactivestorage.browser.reactiveStorageServiceImpl.persistentStorage.applicationUser'"
                     ")";
    
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        snprintf(info->error_message, sizeof(info->error_message),
                 "Failed to prepare statement: %s", sqlite3_errmsg(db));
        sqlite3_close(db);
        return false;
    }
    
    // 执行查询
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        const char* key = (const char*)sqlite3_column_text(stmt, 0);
        const char* value = (const char*)sqlite3_column_text(stmt, 1);
        
        if (strcmp(key, "cursorai/donotchange/privacyMode") == 0) {
            // 解析隐私模式
            if (strcmp(value, "true") == 0) {
                strcpy_s(info->privacy_mode, sizeof(info->privacy_mode), "privacy");
            } else if (strcmp(value, "false") == 0) {
                strcpy_s(info->privacy_mode, sizeof(info->privacy_mode), "shared");
            }
        }
        else if (strstr(key, "persistentStorage.applicationUser") != NULL) {
            // 这是包含所有配置的大JSON，需要解析
            info->use_claude_key = extract_json_bool(value, "useClaudeKey");
            info->use_google_key = extract_json_bool(value, "useGoogleKey");
            info->use_openai_key = extract_json_bool(value, "useOpenAIKey");
            
            // 解析 useAzure
            if (strstr(value, "\"useAzure\":true") != NULL) {
                info->use_azure = true;
            }
            
            // 解析 bedrockState
            const char* bedrock_pos = strstr(value, "\"bedrockState\":");
            if (bedrock_pos != NULL) {
                info->use_bedrock = extract_json_bool(bedrock_pos, "useBedrock");
            }
        }
    }
    
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    
    return true;
}

// 读取MCP配置
bool read_mcp_config(CursorInfo* info) {
    char mcp_path[MAX_PATH];
    char user_profile[MAX_PATH];
    
    // 获取用户目录
    if (GetEnvironmentVariableA("USERPROFILE", user_profile, MAX_PATH) == 0) {
        strcpy_s(info->mcp_servers, sizeof(info->mcp_servers), "[]");
        return true; // 不算错误，只是没有配置
    }
    
    // 构建 mcp.json 路径
    snprintf(mcp_path, sizeof(mcp_path), "%s\\.cursor\\mcp.json", user_profile);
    
    // 打开文件
    FILE* file = NULL;
    errno_t err = fopen_s(&file, mcp_path, "r");
    if (err != 0 || file == NULL) {
        strcpy_s(info->mcp_servers, sizeof(info->mcp_servers), "[]");
        return true; // 文件不存在不算错误
    }
    
    // 读取文件内容
    char buffer[4096] = {0};
    size_t bytes_read = fread(buffer, 1, sizeof(buffer) - 1, file);
    fclose(file);
    
    if (bytes_read == 0) {
        strcpy_s(info->mcp_servers, sizeof(info->mcp_servers), "[]");
        return true;
    }
    
    // 简单解析：提取所有mcpServers下的server name
    // 这里做简化处理，提取 "mcpServers" 对象中的所有键名
    const char* mcp_servers_pos = strstr(buffer, "\"mcpServers\"");
    if (mcp_servers_pos == NULL) {
        strcpy_s(info->mcp_servers, sizeof(info->mcp_servers), "[]");
        return true;
    }
    
    // 寻找 mcpServers 对象的开始
    const char* obj_start = strchr(mcp_servers_pos, '{');
    if (obj_start == NULL) {
        strcpy_s(info->mcp_servers, sizeof(info->mcp_servers), "[]");
        return true;
    }
    
    // 提取服务名称
    char server_names[4096] = "[";
    int bracket_count = 1;
    obj_start++; // 跳过第一个 {
    
    bool first = true;
    while (*obj_start && bracket_count > 0) {
        if (*obj_start == '{') bracket_count++;
        else if (*obj_start == '}') bracket_count--;
        
        // 简化处理：查找所有的 "server_name": 模式
        if (*obj_start == '"' && bracket_count == 1) {
            const char* name_start = obj_start + 1;
            const char* name_end = strchr(name_start, '"');
            if (name_end) {
                char name[256];
                size_t len = name_end - name_start;
                if (len < sizeof(name)) {
                    strncpy_s(name, sizeof(name), name_start, len);
                    name[len] = '\0';
                    
                    // 跳过 mcpServers 本身
                    if (strcmp(name, "mcpServers") != 0) {
                        if (!first) strcat_s(server_names, sizeof(server_names), ",");
                        strcat_s(server_names, sizeof(server_names), "\"");
                        strcat_s(server_names, sizeof(server_names), name);
                        strcat_s(server_names, sizeof(server_names), "\"");
                        first = false;
                    }
                }
                obj_start = name_end;
            }
        }
        obj_start++;
    }
    
    strcat_s(server_names, sizeof(server_names), "]");
    strcpy_s(info->mcp_servers, sizeof(info->mcp_servers), server_names);
    
    return true;
}

// 收集Cursor信息
bool collect_cursor_info(CursorInfo* info) {
    memset(info, 0, sizeof(CursorInfo));
    
    // 获取计算机名
    if (!get_computer_name(info->pc_name, sizeof(info->pc_name))) {
        strcpy_s(info->pc_name, sizeof(info->pc_name), "Unknown");
    }
    
    // 获取时间戳
    get_timestamp(info->timestamp, sizeof(info->timestamp));
    
    // 检查Cursor是否安装
    info->cursor_installed = check_cursor_installed();
    
    if (!info->cursor_installed) {
        strcpy_s(info->error_message, sizeof(info->error_message),
                 "Cursor is not installed");
        strcpy_s(info->privacy_mode, sizeof(info->privacy_mode), "Unknown");
        strcpy_s(info->mcp_servers, sizeof(info->mcp_servers), "[]");
        return true; // 不算错误，只是没安装
    }
    
    // 读取数据库配置
    if (!read_database_config(info)) {
        // 错误信息已经在函数内设置
        return false;
    }
    
    // 读取MCP配置
    read_mcp_config(info);
    
    return true;
}

// 保存到JSON文件
bool save_to_json(const CursorInfo* info) {
    char filename[512];
    snprintf(filename, sizeof(filename), "cursor_%s_%s.json", 
             info->pc_name, info->timestamp);
    
    FILE* file = NULL;
    errno_t err = fopen_s(&file, filename, "w");
    if (err != 0 || file == NULL) {
        return false;
    }
    
    // 写入JSON内容
    fprintf(file, "{\n");
    fprintf(file, "  \"pc_name\": \"%s\",\n", info->pc_name);
    fprintf(file, "  \"timestamp\": \"%s\",\n", info->timestamp);
    fprintf(file, "  \"cursor_installed\": %s,\n", JSON_BOOL_STR(info->cursor_installed));
    
    if (info->cursor_installed) {
        fprintf(file, "  \"privacy_mode\": \"%s\",\n", info->privacy_mode);
        fprintf(file, "  \"model_keys\": {\n");
        fprintf(file, "    \"use_claude_key\": %s,\n", JSON_BOOL_STR(info->use_claude_key));
        fprintf(file, "    \"use_google_key\": %s,\n", JSON_BOOL_STR(info->use_google_key));
        fprintf(file, "    \"use_openai_key\": %s,\n", JSON_BOOL_STR(info->use_openai_key));
        fprintf(file, "    \"use_azure\": %s,\n", JSON_BOOL_STR(info->use_azure));
        fprintf(file, "    \"use_bedrock\": %s\n", JSON_BOOL_STR(info->use_bedrock));
        fprintf(file, "  },\n");
        fprintf(file, "  \"mcp_servers\": %s", info->mcp_servers);
    } else {
        fprintf(file, "  \"error\": \"%s\"", info->error_message);
    }
    
    if (strlen(info->error_message) > 0) {
        fprintf(file, ",\n  \"warning\": \"%s\"", info->error_message);
    }
    
    fprintf(file, "\n}\n");
    
    fclose(file);
    
    printf("Information saved to: %s\n", filename);
    return true;
}

// 主函数
int main() {
    CursorInfo info;
    
    printf("=== Cursor Information Collector ===\n\n");
    
    // 收集信息
    if (!collect_cursor_info(&info)) {
        fprintf(stderr, "Error: %s\n", info.error_message);
        return 1;
    }
    
    // 显示收集的信息
    printf("PC Name: %s\n", info.pc_name);
    printf("Timestamp: %s\n", info.timestamp);
    printf("Cursor Installed: %s\n", info.cursor_installed ? "Yes" : "No");
    
    if (info.cursor_installed) {
        printf("Privacy Mode: %s\n", info.privacy_mode);
        printf("Model Keys:\n");
        printf("  - Claude Key: %s\n", JSON_BOOL_STR(info.use_claude_key));
        printf("  - Google Key: %s\n", JSON_BOOL_STR(info.use_google_key));
        printf("  - OpenAI Key: %s\n", JSON_BOOL_STR(info.use_openai_key));
        printf("  - Azure: %s\n", JSON_BOOL_STR(info.use_azure));
        printf("  - Bedrock: %s\n", JSON_BOOL_STR(info.use_bedrock));
        printf("MCP Servers: %s\n", info.mcp_servers);
    }
    
    if (strlen(info.error_message) > 0) {
        printf("\nWarning: %s\n", info.error_message);
    }
    
    // 保存到JSON
    if (!save_to_json(&info)) {
        fprintf(stderr, "Failed to save JSON file\n");
        return 1;
    }
    
    printf("\n=== Collection Complete ===\n");
    return 0;
}

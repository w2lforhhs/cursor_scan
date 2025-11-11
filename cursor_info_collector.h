#ifndef CURSOR_INFO_COLLECTOR_H
#define CURSOR_INFO_COLLECTOR_H

#include <stdbool.h>

// 收集结果的数据结构
typedef struct {
    char pc_name[256];
    char timestamp[64];
    bool cursor_installed;
    char privacy_mode[32];       // "shared", "privacy", "Unknown"
    bool use_claude_key;
    bool use_google_key;
    bool use_openai_key;
    bool use_azure;
    bool use_bedrock;
    char mcp_servers[4096];      // JSON字符串形式的MCP服务列表
    char error_message[1024];    // 错误信息
} CursorInfo;

// 函数声明
bool check_cursor_installed();
bool get_computer_name(char* buffer, size_t buffer_size);
void get_timestamp(char* buffer, size_t buffer_size);
bool collect_cursor_info(CursorInfo* info);
bool save_to_json(const CursorInfo* info);

#endif // CURSOR_INFO_COLLECTOR_H

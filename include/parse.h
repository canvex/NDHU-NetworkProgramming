// parse.h

#ifndef PARSE_H
#define PARSE_H

#define MAX_COMMAND_PARAMS 20
#define MAX_COMMAND_PARAM_LENGTH 50

// 定義 command_t 結構
typedef struct {
    char command[MAX_COMMAND_PARAMS][MAX_COMMAND_PARAM_LENGTH];  // 命令
    char parameter[MAX_COMMAND_PARAMS]
                  [MAX_COMMAND_PARAM_LENGTH];  // 儲存最多 10 個參數
    int param_count;                           // 參數個數
    int command_count;
    char unknown_command[MAX_COMMAND_PARAM_LENGTH];
    int exeflag;  // 判斷命令類型
    int pipe_flag;
} command_t;

command_t* parser(char* commandStr);
void isBuildin(command_t* cmd);

#endif

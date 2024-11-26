#include "../include/parse.h"

#include <dirent.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
char buildCmd[100][100] = {"printenv", "setenv", "quit", "exit", "help"};
char NonbuildCmd[100][100];
int buildCmdCount = 5;     // same as buildCmd
int NonbuildCmdCount = 0;  // same as buildCmd
// scan the bin directory for executables, use it if exist, or else use the
// system executables.
void loadBin() {
    // scan the bin directory for executables, use it if exist, or else use the
    // system executables.
    DIR* d;
    struct dirent* dir;
    d = opendir("./bin");
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if (dir->d_type == 8) {
                strcpy(NonbuildCmd[NonbuildCmdCount], dir->d_name);
                NonbuildCmdCount++;
                // printf("%s\n", dir->d_name);
            }
        }
        for (int i = 0; i < NonbuildCmdCount; i++) {
            printf("%s,", NonbuildCmd[i]);
        }
        closedir(d);
    }
}
void isBuildin(command_t* cmd) {
    for (int i = 0; i < cmd->command_count; i++) {
        for (int j = 0; j < (buildCmdCount > NonbuildCmdCount ? buildCmdCount : NonbuildCmdCount); j++) {
            if (strcmp(cmd->command[i], buildCmd[j]) == 0) {
                if (cmd->pipe_flag == 1)
                    cmd->exeflag = 2;  // 內建指令且是pipe
                else
                    cmd->exeflag = 0;  // 內建指令
                break;
            }
            if (strcmp(cmd->command[i], NonbuildCmd[j]) == 0) {
                if (cmd->pipe_flag == 1)
                    cmd->exeflag = 2;  // bin裡的指令且是pipe
                else
                    cmd->exeflag = 1;  // bin裡的指令
                break;
            } else {
                cmd->exeflag = 87;  // 未知指令
            }
        }
        if (cmd->exeflag != 0) {
            strcpy(cmd->unknown_command, cmd->command[i]);
            return;
        }
    }
    return;
}

command_t* parser(char* commandStr) {
    command_t* cmd = (command_t*)malloc(sizeof(command_t));
    cmd->command_count = 0;
    cmd->param_count = 0;  // 初始化參數數量
    cmd->pipe_flag = 0;    // 初始化 pipe_flag
    int head = 1;
    // 清空參數陣列，確保不會保留之前的數據
    for (int i = 0; i < MAX_COMMAND_PARAMS; i++) {
        cmd->parameter[i][0] = '\0';  // 設定每個參數為空字串
        cmd->command[i][0] = '\0';
    }
    // 使用 strtok 提取命令
    char* token = strtok(commandStr, " ");
    while (token != NULL) {
        // 如果是第一個單字
        if (strcmp(token, "|") != 0 && head == 1) {
            strcpy(cmd->command[cmd->command_count], token);
            cmd->command_count++;
            head = 0;  // 設置為 0，表示接下來的單字是參數
        } else if (strcmp(token, "|") == 0) {
            // 當遇到 | 時，將 head 設回 1，表示下個單字是新的命令
            head = 1;
            strcpy(cmd->parameter[cmd->param_count], token);
            cmd->param_count++;

        } else if (strcmp(token, "|") != 0 && head == 0) {
            // 否則將單字放入參數陣列
            strcpy(cmd->parameter[cmd->param_count], token);
            cmd->param_count++;
        }

        // 繼續獲取下個單字
        token = strtok(NULL, " ");
    }

    cmd->exeflag = 1;  // 預設為未知命令
    // 檢查是否有普通管道
    for (int i = 0; i < cmd->param_count; i++) {
        if (strcmp(cmd->parameter[i], "|") == 0) {
            cmd->pipe_flag = 1;
            break;
        }
    }
    isBuildin(cmd);  // 去判斷是否為內建命令。

    return cmd;
}
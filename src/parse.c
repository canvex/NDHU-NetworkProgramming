#include "../include/parse.h"

#include <dirent.h>
#include <math.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
char buildCmd[100][100] = {"printenv", "setenv", "quit", "exit", "help"};

char NonbuildCmd[100][100];
char pythonCmd[100][100];
int buildCmdCount = 5;  // same as buildCmd
int NonbuildCmdCount = 0;
int pythonCmdCount = 0;
// scan the bin directory for executables, use it if exist, or else use the
// system executables.
void loadBin() {
    // scan the bin directory for executables, use it if exist, or else use the
    // system executables.
    // NonbuildCmdCount = 0;
    // NonbuildCmd[100][100];
    // memset(NonbuildCmd, 0, sizeof(NonbuildCmd));
    DIR* d;
    struct dirent* dir;
    d = opendir("./bin");
    // d = opendir("/home/brian/brian-HW/hw3/bin");
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            // 檢查檔案是否為普通檔案
            if (dir->d_type == DT_REG) {
                // 如果檔案是 .py 檔案
                if (strstr(dir->d_name, ".py") != NULL) {
                    // 去除 .py 副檔名
                    char* dot = strrchr(dir->d_name, '.');
                    if (dot != NULL) {
                        *dot = '\0';  // 將 '.' 及其後的部分去除
                    }

                    // 將處理過的檔案名稱加入 pythonCmd
                    if (pythonCmdCount < 100) {
                        strcpy(pythonCmd[pythonCmdCount], dir->d_name);
                        pythonCmdCount++;
                    }

                    // 避免將 Python 指令重複加入到 NonbuildCmd，直接繼續下一個檔案
                    continue;
                }

                // 其他檔案加入 NonbuildCmd
                if (NonbuildCmdCount < 100) {
                    strcpy(NonbuildCmd[NonbuildCmdCount], dir->d_name);
                    NonbuildCmdCount++;
                }
            }
        }

        // 列印 NonbuildCmd (普通可執行檔案)
        printf("Non-build Commands: ");
        for (int i = 0; i < NonbuildCmdCount; i++) {
            printf("%s, ", NonbuildCmd[i]);
        }

        // 列印 pythonCmd (.py 檔案)
        printf("\nPython Commands: ");
        for (int i = 0; i < pythonCmdCount; i++) {
            printf("%s, ", pythonCmd[i]);
        }

        closedir(d);
    }
}
void isBuildin(command_t* cmd) {
    int found = 0;  // 標記是否找到匹配指令

    for (int i = 0; i < cmd->command_count; i++) {
        // 比對內建指令
        for (int j = 0; j < buildCmdCount; j++) {
            if (strcmp(cmd->command[i], buildCmd[j]) == 0) {
                if (cmd->pipe_flag == 1)
                    cmd->exeflag = 2;  // 內建指令且是 pipe
                else
                    cmd->exeflag = 0;  // 內建指令
                found = 1;
                break;
            }
        }

        // 如果已找到匹配指令，跳過其他檢查
        if (found) continue;

        // 比對 nonbuildin 指令
        for (int j = 0; j < NonbuildCmdCount; j++) {
            if (strcmp(cmd->command[i], NonbuildCmd[j]) == 0) {
                if (cmd->pipe_flag == 1)
                    cmd->exeflag = 2;  // bin 裡的指令且是 pipe
                else
                    cmd->exeflag = 1;  // bin 裡的指令
                found = 1;
                break;
            }
        }

        if (found) continue;

        // 比對 python 指令
        for (int j = 0; j < pythonCmdCount; j++) {
            if (strcmp(cmd->command[i], pythonCmd[j]) == 0) {
                if (cmd->pipe_flag == 1)
                    cmd->exeflag = 2;  // Python 裡的指令且是 pipe
                else
                    cmd->exeflag = 3;  // Python 裡的指令
                found = 1;
                break;
            }
        }

        // 如果都沒找到，設置為未知指令
        if (!found) {
            cmd->exeflag = 87;  // 未知指令
            strcpy(cmd->unknown_command, cmd->command[i]);
        }
        return;  // 直接結束函數，因為已確定未知指令
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
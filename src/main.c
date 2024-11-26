#include <ctype.h>
#include <dirent.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "../include/exeBuildin.h"
#include "../include/myhdr.h"
#include "../include/parse.h"

command_t* parser(char* commandStr);

void init() {
    // 設置 PATH 環境變數
    setenv("PATH", "bin:.", 1);  // 1 表示覆蓋已存在的 PATH

    // 你可以在這裡添加其他初始化代碼
}

int main() {
    system("clear");  // 或 system("cls"); 根據你的操作系統選擇
    init();
    loadBin();
    //   char commandStr[256] = {0};
    char* commandStr;
    command_t* cmd = NULL;
    do {
        // 使用 readline 取代 fgets
        commandStr = readline("\nMyShell% ");
        commandStr[strcspn(commandStr, "\n")] = '\0';
        // 若使用者輸入的是空字串，直接繼續下一次迴圈
        if (commandStr == NULL || strlen(commandStr) == 0) {
            free(commandStr);
            continue;
        }
        // 儲存到歷史紀錄
        add_history(commandStr);
        // 解析命令
        cmd = parser(commandStr);

        // printf("\nMyShell%% ");
        // fgets(commandStr, sizeof(commandStr), stdin);
        // commandStr[strcspn(commandStr, "\n")] = '\0';
        // cmd = parser(commandStr);

        // -1 is no command
        // 0 is builtin command
        // 1 is a simple command
        // 2 is including a pipe
        // 3 is including a num pipe

        // 印出解析結果
        printf("\nCommand Count: %d\n", cmd->command_count);
        printf("Parameters Count: %d\n", cmd->param_count);
        printf("Commands:\n");
        for (int i = 0; i < cmd->command_count; i++) {
            printf("  Command %d: %s\n", i + 1, cmd->command[i]);
        }
        printf("Parameters:\n");
        for (int i = 0; i < cmd->param_count; i++) {
            printf("  Parameter %d: %s\n", i + 1, cmd->parameter[i]);
        }

        switch (cmd->exeflag) {
            case 0:
                printf("Buildin command\n");
                exeBuildin(cmd);
                break;
            case 1:
                printf("NonBuildin command: [%s].\n", cmd->unknown_command);
                exeNonBuildin(cmd);  // 調用新函數處理非內建指令
                break;
            case 2:
                printf("pipe command\n");
                exeNormalPipe(cmd);
                break;
            case 87:
                printf("I don't know who are you [%s].\n", cmd->unknown_command);
        }
        if (cmd != NULL) {
            free(cmd);
        }

    } while (strcmp(commandStr, "quit") != 0);
    return 0;
}

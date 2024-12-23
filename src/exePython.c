#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../include/exeBuildin.h"
#include "../include/myhdr.h"
#include "../include/parse.h"

#define READ_END 0
#define WRITE_END 1

extern char pythonCmd[100][100];
extern int pythonCmdCount;

void exepython(command_t* cmd) {
    // 獲取環境變數 loginname 的值
    char* loginname = getenv("loginname");

    pid_t pid = fork();

    if (pid == 0) {  // 子進程
        char* args[MAX_ARGS + 2];
        args[0] = cmd->command;  // 第一個參數是命令名
        char tmpCmd[100];
        int pos = 0;
        args[0] = "/usr/bin/python3";
        // 分配足夠空間來儲存檔案名稱並附加 .py 副檔名
        char python_filename[200];
        snprintf(python_filename, sizeof(python_filename), "./bin/%s.py", cmd->command);
        // snprintf(python_filename, sizeof(python_filename), "%s.py", cmd->command);

        args[1] = python_filename;
        args[2] = loginname;

        // args[1] = "/home/brian/brian-HW/hw3/bin/login.py";
        pos = 2;
        // 填充參數
        for (int i = 0; i < cmd->param_count; i++) {
            args[pos + i + 1] = cmd->parameter[i];
        }
        args[pos + cmd->param_count + 1] = NULL;  // 最後一個元素設置為 NULL

        // 印出傳遞給 execvp 的參數
        // printf("在C中即將執行的命令和參數:\n");
        // for (int i = 0; args[i] != NULL; i++) {
        //     printf("args[%d]: %s\n", i, args[i]);
        // }

        // 執行命令
        execvp(args[0], args);

        // 如果 execvp 返回，說明出錯了
        perror("execvp");
        exit(EXIT_FAILURE);
    } else if (pid > 0) {  // 父進程
        int status;
        waitpid(pid, &status, 0);  // 等待子進程結束
    } else {
        perror("fork 失敗");
    }
}
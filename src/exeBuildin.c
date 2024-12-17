#include "../include/exeBuildin.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../include/myhdr.h"
#include "../include/parse.h"

#define READ_END 0
#define WRITE_END 1

// void doChild(int fd[], char* commandArgv[]) {
//     // 關閉讀管道，子進程只需要寫管道
//     close(fd[READ_END]);
//     // 將標準輸出重定向到管道
//     dup2(fd[WRITE_END], STDOUT_FILENO);
//     close(fd[WRITE_END]);

//     // 執行第一個命令
//     if (execvp(commandArgv[0], commandArgv) < 0) {
//         perror("execvp");
//         exit(EXIT_FAILURE);
//     }
// }

// void doParent(int fd[], char* commandArgv[]) {
//     // 關閉寫管道，父進程只需要讀管道
//     close(fd[WRITE_END]);
//     // 將標準輸入重定向到管道
//     dup2(fd[READ_END], STDIN_FILENO);
//     close(fd[READ_END]);

//     // 執行第二個命令
//     if (execvp(commandArgv[0], commandArgv) < 0) {
//         perror("execvp");
//         exit(EXIT_FAILURE);
//     }
// }

void quit() {
    printf("\nGoodBye\n");
    exit(EXIT_SUCCESS);
}

void help() {
    printf(
        "\n***Welcome to Myshell program***\n"
        "This is NDHU Networking Programming Course homework\n"
        "List of commands supported:\n"
        "  > cd\n"
        "  > help\n"
        "  > printenv\n"
        "  > setenv\n"
        "  > quit\n"
        "  > all other general commands available in bin\n"
        "  > normal pipe handling\n"
        "  > number pipe handling\n\n");
}

void printenv(char* var) {
    char* value = getenv(var);
    if (value != NULL) {
        printf("    %s\n", value);
    } else {
        // printf("%s not set\n", var);
        printf("\n");
    }
}

void mysetenv(char* var, char* value) {
    if (var != NULL && value != NULL)
        if (setenv(var, value, 1) == 0)
            printf("env variable %s is set to %s \n", var, value);
}

void exeNonBuildin(command_t* cmd) {
    pid_t pid = fork();

    if (pid == 0) {  // 子進程
        char* args[MAX_ARGS + 2];
        args[0] = cmd->command;  // 第一個參數是命令名

        // 填充參數
        for (int i = 0; i < cmd->param_count; i++) {
            args[i + 1] = cmd->parameter[i];
        }
        args[cmd->param_count + 1] = NULL;  // 最後一個元素設置為 NULL

        // 如果命令是 python3，設置檔案的完整路徑
        if (strcmp(args[0], "python3") == 0) {
            strcpy(args[0], "/usr/bin/python3");

            // 構造檔案完整路徑
            char path[100] = "./src/";
            strcat(path, args[1]);
            args[1] = path;
        }

        // 執行命令
        execvp(args[0], args);

        // 如果 execvp 返回，說明出錯了
        perror("execvp");
        exit(EXIT_FAILURE);
    } else if (pid > 0) {  // 父進程
        int status;
        waitpid(pid, &status, 0);
    } else {
        perror("fork");
    }
}

void exeBuildin(command_t* cmd) {
    if (strcmp(cmd->command, "help") == 0) {
        help();
    } else if (strcmp(cmd->command, "printenv") == 0) {
        printenv(cmd->parameter);
    } else if (strcmp(cmd->command, "setenv") == 0) {
        mysetenv(cmd->parameter[0], cmd->parameter[1]);
    } else if (strcmp(cmd->command, "quit") == 0 ||
               strcmp(cmd->command, "exit") == 0) {
        quit();
    } else if (strcmp(cmd->command, "python3") == 0) {
        exeNonBuildin(cmd);
    } else {
        exeNonBuildin(cmd);
    }
}
void exeNormalPipe(command_t* cmd) {
    int fd[2];
    pid_t pid1, pid2;
    char* exeArgv0[50] = {NULL};  // 初始化所有元素為 NULL
    char* exeArgv1[50] = {NULL};
    // 使用 strtok 分割字串
    int i, index = 0;

    // 遍歷陣列找到 '|'
    for (i = 0; i < cmd->param_count; i++) {
        if (strcmp(cmd->parameter[i], "|") == 0) {
            index = i;  // 記錄 '|' 的位置
            break;
        }
    }

    // 將 '|' 左邊的字串存入 ans1
    for (i = 0; i < index; i++) {
        exeArgv0[i + 1] = cmd->parameter[i];
    }
    exeArgv0[i + 2] = NULL;

    int ans2_count = 0;
    // 將 '|' 右邊的字串存入 ans2
    for (ans2_count = 0, i = index + 1; i < cmd->param_count; i++, ans2_count++) {
        exeArgv1[ans2_count + 1] = cmd->parameter[i];
    }
    exeArgv1[ans2_count + 2] = NULL;

    for (int i = 0; i < cmd->param_count; i++) {
        if (strcmp(cmd->parameter[i], "|") == 0) {
            for (int j = i; j < cmd->param_count - 1; j++) {
                strcpy(cmd->parameter[j], cmd->parameter[j + 1]);
            }
            // 減少參數計數
            cmd->param_count--;
            // 將最後一個參數清空
            cmd->parameter[cmd->param_count][0] = '\0';
            // 因為參數已經向前移動，i 應該回退，重新檢查當前位置
            i--;
        }
    }

    exeArgv0[0] = cmd->command[0];  // argv[1]
    exeArgv1[0] = cmd->command[1];  // argv[2];

    if ((pipe(fd) == -1)) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid1 = fork();
    if (pid1 == 0) {
        // 第一個子進程
        close(fd[0]);
        dup2(fd[1], STDOUT_FILENO);
        close(fd[1]);

        // 執行第一個命令
        execvp(exeArgv0[0], exeArgv0);

        perror("execlp");
        exit(1);
    }

    pid2 = fork();
    if (pid2 == 0) {
        // 第二個子進程
        close(fd[1]);
        dup2(fd[0], STDIN_FILENO);
        close(fd[0]);

        // 執行第二個命令
        execvp(exeArgv1[0], exeArgv1);
        perror("execlp");
        exit(1);
    }

    // 父進程
    close(fd[0]);
    close(fd[1]);

    // 等待兩個子進程完成
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
}
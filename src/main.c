#include <arpa/inet.h>
#include <ctype.h>
#include <dirent.h>
#include <netinet/in.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>

// 自定義標頭檔
#include "../include/exeBuildin.h"
#include "../include/myhdr.h"
#include "../include/parse.h"
command_t *parser(char *commandStr);
// 宏定義
#define LISTEN_PORT 2486  // 假設的監聽埠號，實際可根據需求修改

// 全域變數
char pwd[256];  // 儲存工作目錄的變數
char myIP[INET_ADDRSTRLEN];
int myPORT;
int uid = 0;
extern int uid;                       // 全域的UID，假設從其他地方管理
extern char userFile[20];             // 用戶檔案路徑
char userFile[20] = "/tmp/userlist";  // 定義 userFile 變數

void init() {
    // 設置 PATH 環境變數
    setenv("PATH", "bin:.", 1);  // 1 表示覆蓋已存在的 PATH

    // 你可以在這裡添加其他初始化代碼
}
// 註冊新用戶到 userFile
void registerUser(int uid, const char *ip, int port, int pid) {
    FILE *file = fopen(userFile, "a");  // 打開檔案以附加模式
    if (file == NULL) {
        perror("Error opening user file");
        return;
    }

    // 格式化用戶資料並寫入檔案
    fprintf(file, "%d no_name %s %d %d\n", uid, ip, port, pid);

    // 關閉檔案
    fclose(file);

    printf("User %d registered with IP:Port %s:%d, PID: %d\n\r", uid, ip, port, pid);
}

// 刪除使用者檔案
void removeUserFile() {
    if (access(userFile, F_OK) == 0) {  // 檢查檔案是否存在
        if (remove(userFile) == 0) {    // 嘗試刪除檔案
            printf("User file '%s' removed successfully.\n", userFile);
        } else {
            perror("Error removing user file");
        }
    } else {
        printf("No user file to remove at '%s'.\n", userFile);
    }
}

// 信號處理函數
static void sig_handler(int sig) {
    int status;
    if (sig == SIGCHLD) {
        int pid = waitpid(-1, &status, WNOHANG);  // WNOHANG 使其不會阻塞
        if (pid > 0) {
            fprintf(stderr, "Child PID=%d finished.\n", pid);
        } else if (pid == 0) {
            // 沒有子進程結束
        } else {
            perror("waitpid error");
        }
    }
}

// 伺服器邏輯
void server(int connfd, struct sockaddr_in *cliaddr) {
    command_t *cmd = NULL;
    char *commandStr = NULL;  // 使用指標來儲存 readline() 返回的字串

    // 獲取客戶端 IP 和埠號
    inet_ntop(AF_INET, &cliaddr->sin_addr, myIP, sizeof(myIP));
    myPORT = ntohs(cliaddr->sin_port);

    // 初始化環境
    // system("clear");  // 或 system("cls"); 根據你的操作系統選擇
    getcwd(pwd, sizeof(pwd));
    dup2(connfd, STDIN_FILENO);
    dup2(connfd, STDOUT_FILENO);
    setbuf(stdout, NULL);
    close(connfd);
    printf("MyIP=%s, MyPORT=%d\n", myIP, myPORT);
    init();
    loadBin();

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
        // printf("\nCommand Count: %d\n", cmd->command_count);
        // printf("Parameters Count: %d\n", cmd->param_count);
        // printf("Commands:\n");
        // for (int i = 0; i < cmd->command_count; i++) {
        //     printf("  Command %d: %s\n", i + 1, cmd->command[i]);
        // }
        // printf("Parameters:\n");
        // for (int i = 0; i < cmd->param_count; i++) {
        //     printf("  Parameter %d: %s\n", i + 1, cmd->parameter[i]);
        // }

        switch (cmd->exeflag) {
            case 0:
                printf("Buildin command\n\r");
                exeBuildin(cmd);
                break;
            case 1:
                printf("NonBuildin command: [%s].\n\r", cmd->unknown_command);
                exeNonBuildin(cmd);  // 調用新函數處理非內建指令
                break;
            case 2:
                printf("pipe command\n\r");
                exeNormalPipe(cmd);
                break;
            case 87:
                printf("I don't know who are you [%s].\n\r", cmd->unknown_command);
        }
        if (cmd != NULL) {
            free(cmd);
        }

    } while (strcmp(commandStr, "quit") != 0);
}

// 主程式
int main() {
    int listenfd, connfd;
    struct sockaddr_in servaddr, cliaddr;
    char buff[INET_ADDRSTRLEN];
    socklen_t len;
    system("clear");  // 或 system("cls"); 根據你的操作系統選擇
    // 設置信號處理
    // signal(SIGCHLD, sig_handler);

    // 建立監聽 socket
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    // 設置 SO_REUSEADDR 選項，不必等待port
    int opt = 1;
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt error");
        exit(EXIT_FAILURE);
    }

    // 初始化伺服器地址
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(LISTEN_PORT);

    // 綁定地址到 socket
    if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("Bind error");
        exit(EXIT_FAILURE);
    }

    // 開始監聽
    if (listen(listenfd, 5) != 0) {
        perror("Listen error");
        exit(EXIT_FAILURE);
    }

    // 初始化使用者環境
    uid = 0;
    removeUserFile();

    // 主循環處理連線
    while (1) {
        len = sizeof(cliaddr);
        connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &len);

        if (connfd < 0) {
            perror("Accept error");
            continue;
        }

        // 打印連接訊息
        fprintf(stderr, "Connection from %s, port %d\n",
                inet_ntop(AF_INET, &cliaddr.sin_addr, buff, sizeof(buff)),
                ntohs(cliaddr.sin_port));

        // 建立子行程處理客戶端
        int child = fork();
        if (child == 0) {
            // 子行程執行伺服器邏輯
            close(listenfd);  // 關閉父行程的監聽 socket
            server(connfd, &cliaddr);
            exit(0);
        } else {
            // 父行程註冊使用者
            fprintf(stderr, "Server PID %d\n", child);
            registerUser(uid++, buff, ntohs(cliaddr.sin_port), child);
        }

        // 關閉子行程 socket 描述符
        close(connfd);
    }

    close(listenfd);
    return 0;
}

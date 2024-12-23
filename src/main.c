#include <arpa/inet.h>
#include <ctype.h>
#include <dirent.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
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

#define TARGET_PREFIX "np_sv"
#define LOGINNAME_PREFIX "loginname"
#define SERVER_FIFO "/tmp/np_sv"
#define SERVER_FIFO_TEMPLATE "/tmp/np_sv.%ld"
#define SERVER_FIFO_NAME_LEN (sizeof(SERVER_FIFO_TEMPLATE) + 20)

// 全域變數
char pwd[256];  // 儲存工作目錄的變數
char myIP[INET_ADDRSTRLEN];
int myPORT;
int uid = 0;
extern int uid;                       // 全域的UID，假設從其他地方管理
extern char userFile[20];             // 用戶檔案路徑
char userFile[20] = "/tmp/userlist";  // 定義 userFile 變數
static char serverFifo[SERVER_FIFO_NAME_LEN];

void init() {
    // 清除所有環境變數
    // clearenv();
    // 設置 PATH 環境變數
    // setenv("PATH", "bin:.", 1);  // 1 表示覆蓋已存在的 PATH
    setenv("PATH", "/home/brian/brian-HW/hw3/bin", 1);

    // 你可以在這裡添加其他初始化代碼
}

/* Invoked on exit to delete client FIFO */
static void removeFifo(void) {
    unlink(serverFifo);
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
void initremoveFIFO() {
    DIR *dir;
    struct dirent *entry;
    char path[1024];

    // 開啟目錄
    dir = opendir("/tmp");
    if (!dir) {
        perror("opendir failed");
        exit(EXIT_FAILURE);
    }

    // 遍歷目錄檔案
    while ((entry = readdir(dir)) != NULL) {
        // 檢查檔案名稱是否以 TARGET_PREFIX 或 LOGINNAME_PREFIX 開頭
        if (strncmp(entry->d_name, TARGET_PREFIX, strlen(TARGET_PREFIX)) == 0 ||
            strncmp(entry->d_name, LOGINNAME_PREFIX, strlen(LOGINNAME_PREFIX)) == 0) {
            // 拼接完整路徑
            snprintf(path, sizeof(path), "/tmp/%s", entry->d_name);

            // 刪除檔案
            if (remove(path) == 0) {
                printf("Deleted: %s\n", path);
            } else {
                perror("Failed to delete file");
            }
        }
    }

    closedir(dir);
}

void delete_line_by_pid(int pid) {
    const char *filename = "/tmp/userlist";  // 檔案路徑
    FILE *file = fopen(filename, "r+");
    if (file == NULL) {
        perror("Unable to open file");
        return;
    }

    // 創建一個暫時的檔案用來存放更新後的資料
    FILE *tempFile = fopen("/tmp/tempfile.txt", "w");
    if (tempFile == NULL) {
        perror("Unable to create temporary file");
        fclose(file);
        return;
    }

    char line[256];
    int target_pid;
    while (fgets(line, sizeof(line), file)) {
        // 假設 PID 在行的最後，並且 PID 是由空白分隔的
        if (sscanf(line, "%*d %*s %*s %*d %d", &target_pid) == 1) {
            // 如果 PID 匹配，則跳過這一行
            if (target_pid == pid) {
                continue;
            }
        }
        // 寫入到臨時檔案
        fputs(line, tempFile);
    }

    fclose(file);
    fclose(tempFile);

    // 用新的檔案覆蓋舊的檔案
    if (remove(filename) != 0) {
        perror("Unable to delete original file");
        return;
    }

    if (rename("/tmp/tempfile.txt", filename) != 0) {
        perror("Unable to rename temporary file");
        return;
    }

    printf("PID %d has been removed from %s\n", pid, filename);
}

// 信號處理函數
static void sig_handler(int sig) {
    int status;
    if (sig == SIGCHLD) {
        int pid = waitpid(-1, &status, WNOHANG);  // WNOHANG 使其不會阻塞
        if (pid > 0) {
            fprintf(stderr, "Child PID=%d finished.\n", pid);
            delete_line_by_pid(pid);

        } else if (pid == 0) {
            // 沒有子進程結束
        } else {
            // perror("waitpid error");
        }
    }
}

char *readName() {
    int mypid = getpid();                                      // 取得目前的 PID
    char path[100];                                            // 構建檔案路徑
    snprintf(path, sizeof(path), "/tmp/loginname.%d", mypid);  // 動態生成檔案路徑

    FILE *file = fopen(path, "r");  // 打開檔案進行讀取
    if (file == NULL) {             // 檢查檔案是否成功打開
        perror("無法打開檔案");
        return NULL;
    }

    char buffer[256];                                   // 暫存名稱的緩衝區
    if (fgets(buffer, sizeof(buffer), file) != NULL) {  // 讀取檔案內容
        fclose(file);                                   // 關閉檔案
        size_t len = strlen(buffer);

        // 移除換行符號（若存在）
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }

        // 動態分配記憶體儲存名稱
        char *name = malloc(len);  // 分配記憶體
        if (name == NULL) {
            perror("記憶體分配失敗");
            return NULL;
        }
        strcpy(name, buffer);  // 將名稱複製到新分配的記憶體中
        return name;
    } else {
        printf("檔案內容為空或讀取失敗。\n");
        fclose(file);
        return NULL;
    }
}

void makeFIFO() {
    umask(0); /* So we get the permissions we want */

    // 建服務的FIFO
    snprintf(serverFifo, SERVER_FIFO_NAME_LEN, SERVER_FIFO_TEMPLATE,
             (long)getpid());
    if (mkfifo(serverFifo, 0777) == -1 && errno != EEXIST) {
        fprintf(stderr, "mkfifo %s", serverFifo);
        exit(EXIT_FAILURE);
    }

    if (atexit(removeFifo) != 0) {
        perror("atexit");
        exit(EXIT_FAILURE);
    }
}

void loginShell() {
    command_t *cmd = (command_t *)malloc(sizeof(command_t));
    // 清空參數陣列，確保不會保留之前的數據
    for (int i = 0; i < MAX_COMMAND_PARAMS; i++) {
        cmd->parameter[i][0] = '\0';  // 設定每個參數為空字串
        cmd->command[i][0] = '\0';
    }

    strcpy(cmd->command[0], "login");
    cmd->command_count = 1;
    cmd->param_count = 0;  // 初始化參數數量
    cmd->pipe_flag = 0;    // 初始化 pipe_flag

    exepython(cmd);
}

// 伺服器邏輯
void shell(int connfd, struct sockaddr_in *cliaddr) {
    int serverFd;
    command_t *cmd = NULL;

    char commandStr[256];
    makeFIFO();
    if ((serverFd = open(serverFifo, O_RDONLY | O_NONBLOCK)) < 0) {
        perror("open server fifo error");
        exit(EXIT_FAILURE);
    }

    // 獲取客戶端 IP 和埠號
    inet_ntop(AF_INET, &cliaddr->sin_addr, myIP, sizeof(myIP));
    myPORT = ntohs(cliaddr->sin_port);

    // 初始化環境

    getcwd(pwd, sizeof(pwd));
    dup2(connfd, STDIN_FILENO);
    dup2(connfd, STDOUT_FILENO);
    dup2(connfd, STDERR_FILENO);  // 標準錯誤
    setbuf(stdout, NULL);
    system("clear");  // 或 system("cls"); 根據你的操作系統選擇
    // sleep(15);
    printf("MyIP=%s, MyPORT=%d ,MyPID=%d\n", myIP, myPORT, getpid());
    // sleep(15);
    init();
    loadBin();

    fd_set readfds;
    char buffer[256];

    printf("\nWelcome to Shell\n ");
    loginShell();
    char *name = readName();
    setenv("loginname", name, 1);
    printf("\n(%s)%% ", name);
    while (1) {
        FD_ZERO(&readfds);
        FD_SET(connfd, &readfds);
        FD_SET(serverFd, &readfds);

        int max_fd = connfd > serverFd ? connfd : serverFd;

        if (select(max_fd + 1, &readfds, NULL, NULL, NULL) < 0) {
            perror("select error");
            break;
        }

        // 處理telnet
        if (FD_ISSET(connfd, &readfds)) {
            int n = read(connfd, buffer, sizeof(buffer));
            if (n <= 0) {
                printf("Client disconnect socket %d\n", connfd);
                break;
            } else {
                buffer[strcspn(buffer, "\r\n")] = '\0';
                strcpy(commandStr, buffer);

                // printf("Received from client: %s\n", buffer);
                if (commandStr == NULL || strlen(commandStr) == 0) {
                    // free(commandStr);
                    printf("\n(%s)%% ", name);
                    continue;
                }
                // 解析命令
                cmd = parser(commandStr);
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
                    case 3:
                        printf("python command\n\r");
                        exepython(cmd);
                        break;
                    case 87:
                        printf("I don't know who are you [%s].\n\r", cmd->unknown_command);
                }
                if (cmd != NULL) {
                    free(cmd);
                }
                printf("\n(%s)%% ", name);
            }
        }
        if (FD_ISSET(serverFd, &readfds)) {
            int n = read(serverFd, buffer, sizeof(buffer));
            if (n > 0) {
                buffer[n] = '\0';
                send(connfd, buffer, n, 0);
                printf("\n(%s)%% ", name);
                // printf("FIFO Message Sent: %s\n", buffer);
            }
        }
    }
}

// 主程式
int main() {
    int listenfd, connfd;
    struct sockaddr_in servaddr, cliaddr;
    char buff[INET_ADDRSTRLEN];
    socklen_t len;
    system("clear");  // 或 system("cls"); 根據你的操作系統選擇
    // 設置信號處理
    signal(SIGCHLD, sig_handler);

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
    initremoveFIFO();

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
            // printf("Child started\n");
            // 子行程執行伺服器邏輯
            close(listenfd);  // 關閉父行程的監聽 socket
            shell(connfd, &cliaddr);
            exit(0);
        } else {
            // 父行程註冊使用者
            // fprintf(stderr, "Server PID %d\n", child);
            registerUser(uid++, buff, ntohs(cliaddr.sin_port), child);
        }

        // 關閉子行程 socket 描述符
        close(connfd);
    }

    close(listenfd);
    return 0;
}
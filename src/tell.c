#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define SERVER_FIFO_TEMPLATE "/tmp/np_sv.%ld"
#define SERVER_FIFO_NAME_LEN (sizeof(SERVER_FIFO_TEMPLATE) + 20)
char userFile[20] = "/tmp/userlist";

int main(int argc, char** argv) {
    int destTarget = atoi(argv[1]);
    int targetID = -1;
    unsigned int uid, port, pid;
    char ip[15], name[30];
    char buf[100];
    char sourceName[100];
    FILE* fin = fopen(userFile, "r");
    int mypid = getppid();

    if (argc < 3) {
        fprintf(stderr, "Usage: tell <id> <message>\n");
        exit(EXIT_FAILURE);
    }
    // 檢查訊息是否為空
    if (strlen(argv[2]) == 0) {
        fprintf(stderr, "Error: Message cannot be empty. Please enter a message.\n");
        exit(EXIT_FAILURE);
    }

    while (fscanf(fin, "%d %s %s %d %d", &uid, name, ip, &port, &pid) != EOF) {
        if (destTarget == uid) {  // 拿目標的pid
            targetID = pid;
            // printf("%4d\t%s\t%s:%d\t\t<-(targetID)\n\r", uid, name, ip, port);
        }
        if (pid == mypid) {  // 拿source的名字
            strcpy(sourceName, name);
            // printf("%4d\t%s\t%s:%d\t\t<-(myName)\n\r", uid, name, ip, port);
        }
    }
    fclose(fin);

    // 組合訊息
    char formatted_message[256] = {0};
    snprintf(formatted_message, sizeof(formatted_message), "\n<User %s(%d) told you>:", sourceName, destTarget);

    for (int i = 2; i < argc; i++) {
        strcat(formatted_message, " ");
        strcat(formatted_message, argv[i]);
    }

    // 建立 FIFO 名稱

    char targetFifo[SERVER_FIFO_NAME_LEN];
    snprintf(targetFifo, SERVER_FIFO_NAME_LEN, SERVER_FIFO_TEMPLATE, (long)targetID);

    // 打開 FIFO
    int fd = open(targetFifo, O_WRONLY | O_NONBLOCK);
    if (fd == -1) {
        printf("This user doesn't exist or offline\n");
        perror("Failed to open target FIFO");
        exit(EXIT_FAILURE);
    }

    // 寫入訊息
    ssize_t bytes_written = write(fd, formatted_message, strlen(formatted_message));
    if (bytes_written > 0) {
        printf("Send accept!\n");
    } else if (bytes_written == -1) {
        perror("Failed to write message");
        close(fd);
        exit(EXIT_FAILURE);
    }

    close(fd);
    return 0;
}

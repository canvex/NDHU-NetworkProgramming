#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

char userFile[20] = "/tmp/userlist";
char tempFile[20] = "/tmp/tempFile";

void show() {
    int fd;
    unsigned int uid, port, pid;
    char ip[15], name[30];
    char buf[100];

    FILE *fin = fopen(userFile, "r");
    int mypid = getppid();
    printf("mypid =%d\n\r", mypid);
    printf("<ID>\t<name>\t<IP:port>\t\t<indicate me>\n\r");

    while (fscanf(fin, "%d %s %s %d %d", &uid, name, ip, &port, &pid) != EOF) {
        if (pid == mypid)
            printf("%4d\t%s\t%s:%d\t\t<-(me)\n\r", uid, name, ip, port);
        else
            printf("%4d\t%s\t%s:%d\t\n\r", uid, name, ip, port);
    }
    fclose(fin);
}

bool checkExistName(char *name, char names[][30], int count) {
    for (int i = 0; i < count; i++) {
        if (strcmp(name, names[i]) == 0)

            return true;
    }
    return false;
}

int main(int argc, char **argv) {
    int fd;
    unsigned int uid, port, pid;
    char ip[15], name[30];
    char buf[100];
    char names[50][30];  // 儲存 50 個名字，每個名字最多 29 個字元
    int count = 0;

    if (argc < 2) {
        printf("You have to type your new name! \n\r");
        exit(EXIT_FAILURE);
    }
    remove(tempFile);
    FILE *fin = fopen(userFile, "r");
    FILE *temp_file = fopen(tempFile, "a");

    int mypid = getppid();
    // printf("mypid =%d\n\r", mypid);
    // printf("<ID>\t<name>\t<IP:port>\t\t<indicate me>\n\r");

    // 紀錄名字
    while (fscanf(fin, "%d %s %s %d %d", &uid, name, ip, &port, &pid) != EOF) {
        strcpy(names[count++], name);
    }
    // 使用 rewind() 將檔案指標移回開頭
    rewind(fin);

    while (fscanf(fin, "%d %s %s %d %d", &uid, name, ip, &port, &pid) != EOF) {
        if (pid == mypid) {
            if (checkExistName(argv[1], names, count)) {
                printf("User %s already exists! Please choose another \n\r", argv[1]);
                fprintf(temp_file, "%d %s %s %d %d\n", uid, name, ip, port, pid);
                // exit(EXIT_FAILURE);
            } else {
                printf("Name %s change accept! \n\r", argv[1]);
                fprintf(temp_file, "%d %s %s %d %d\n", uid, argv[1], ip, port, pid);
                // printf("%4d\t%s\t%s:%d\t\t<-(me)\n\r", uid, name, ip, port);
            }
        } else {
            fprintf(temp_file, "%d %s %s %d %d\n", uid, name, ip, port, pid);
        }
    }
    fclose(temp_file);
    fclose(fin);
    if (remove(userFile) != 0) {
        perror("Error deleting original file");
        return 1;
    }
    if (rename(tempFile, userFile) != 0) {
        perror("Error renaming temp file");
        return 1;
    }
    // printf("File modified successfully.\n");
    show();
}

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

char userFile[20] = "/tmp/userlist";

void main(int argc, char **argv) {
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
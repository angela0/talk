/*************************************************************************
    > File Name: talk.c
    > Author: Angela
    > Mail: zuchuang1@gmail.com
    > Created Time: 2016年08月11日 星期四 16时42分56秒
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/types.h>
#include <curses.h>
#include <poll.h>

#define MAX_LEN 10240

const uint32_t port = 44944;
int notarget = 0;

void dowhile(int sd)
{
    int stop = 0;
    char cmd[16];
    char peer[64];
    int ret;

    struct pollfd pfd;
    pfd.fd = sd;
    pfd.events = POLLIN;

    do {
        ret = poll(&pfd, 1, 10);    
        if (ret < 0) {
            stop = 1;
            break;
        } else if (ret > 0) {
            if (pfd.revents & POLLIN) {
                char buf[MAX_LEN+1];
                int size;
                struct sockaddr_in srcaddr;
                socklen_t len;
                printf(">");
                do {
                    size = recvfrom(sd, buf, MAX_LEN, 0, (struct sockaddr *)&srcaddr, &len);
                    if (size < 0) {
                        stop = 1;
                        break;
                    }
                    write(1, buf, size);
                } while (size != 0);
                printf("[ from %s ]", inet_ntop(AF_INET, &srcaddr.sin_addr, buf, INET_ADDRSTRLEN));
            }
        }

        printf(">");
        scanf("%s", cmd);
        if (strncmp(cmd, "send", 4) == 0) {
            scanf("%s", peer);
        } else if (strncmp(cmd, "alias", 5) == 0) {

        } else if (strncmp(cmd, "quit", 4) == 0) {
            stop = 1;
        }

    } while (!stop);
    if (stop) {
        printf("bye\n");
    }
}

int listen_port()
{
    int sd = socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in serv;
    bzero(&serv, sizeof(serv));

    serv.sin_family = AF_INET;
    serv.sin_port = htons(port);
    serv.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sd, (struct sockaddr *)&serv, sizeof(serv)) < 0) {
        printf("bind error: %s\n", strerror(errno));
    }

    return sd;
}


int main(int argc, char *argv[])
{
    if (argc == 1)
        notarget = 1;

    int sd = listen_port();
    if (notarget == 1) {
        dowhile(sd);
    } else {
        char msg[MAX_LEN+1];
        int size;
        struct sockaddr_in peer;
        bzero(&peer, sizeof(peer));
        peer.sin_family = AF_INET;
        peer.sin_port = htons(port);
        inet_pton(AF_INET, argv[1], &peer.sin_addr);
        do {
            size = read(0, msg, MAX_LEN);
            if (size < 0) {
                printf("read error: %s", strerror(errno));
                exit(1);
            }
            sendto(sd, msg, size, 0, (struct sockaddr *)&peer, sizeof(peer));
        } while (size != 0);
    }
    return 0;
}

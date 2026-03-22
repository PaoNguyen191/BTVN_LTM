#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

int main(int argc, char *argv[]){
    if(argc != 3){
        fprintf(stderr, "Cách dùng: %s <Cổng> <FILE_LOG>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int port = atoi(argv[1]);
    char *filename = argv[2];

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd < 0){
        perror("socket() failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if(bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
        perror("bind() failed");
        exit(EXIT_FAILURE);
    }

    if(listen(server_fd, 5) < 0){
        perror("listen() failed");
        exit(EXIT_FAILURE);
    }

    FILE *fp = fopen(filename, "a");
    if(!fp){
        perror("fopen() failed");
        exit(EXIT_FAILURE);
    }

    while(1){
        struct sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);

        int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &addr_len);
        if(client_fd < 0){
            perror("accept() failed");
            continue;
        }

        printf("Client: %s\n", inet_ntoa(client_addr.sin_addr));

        char buffer[1024];

        while(1){
            int bytes = recv(client_fd, buffer, sizeof(buffer)-1, 0);
            if(bytes <= 0) break;

            buffer[bytes] = '\0';

            char *id = strtok(buffer, "\n");
            char *name = strtok(NULL, "\n");
            char *dob = strtok(NULL, "\n");
            char *gpa = strtok(NULL, "\n");

            if(!id || !name || !dob || !gpa){
                printf("Dữ liệu lỗi!\n");
                continue;
            }

            // Lấy thời gian
            time_t now = time(NULL);
            struct tm *t = localtime(&now);

            char time_str[64];
            strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", t);

            // In ra màn hình
            printf("%s %s %s %s %s %s\n",
                inet_ntoa(client_addr.sin_addr),
                time_str, id, name, dob, gpa);

            // Ghi file
            fprintf(fp, "%s %s %s %s %s %s\n",
                inet_ntoa(client_addr.sin_addr),
                time_str, id, name, dob, gpa);

            fflush(fp);
        }

        close(client_fd);
    }

    fclose(fp);
    close(server_fd);
    return 0;
}
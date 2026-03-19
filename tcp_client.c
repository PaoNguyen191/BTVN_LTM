#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>

int main(int argc, char *argv[]){
    if(argc != 3){
        fprintf(stderr, "Cach su dung: %s <dia_chi_IP> <cong>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    // Tạo socket
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(client_fd < 0){
        perror("socket() failed");
        exit(EXIT_FAILURE);
    }

    // Cấu hình địa chỉ server
    struct sockaddr_in addr;;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(argv[1]);
    addr.sin_port = htons(atoi(argv[2]));

    // Kết nối đến server
    if(connect(client_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0){
        perror("connect() failed");
        exit(EXIT_FAILURE);
    }

    // Gui tin nhắn đến server
    char buffer[256];
    while(1){
        printf("Nhập tin nhắn (gõ 'exit' để thoát): ");
        fgets(buffer, sizeof(buffer), stdin);
        if(strncmp(buffer, "exit", 4) == 0){
            break;
        }
        if(send(client_fd, buffer, strlen(buffer), 0) < 0){
            perror("send() failed");
            exit(EXIT_FAILURE);
        }
    }
    // Đóng kết nối
    close(client_fd);
}
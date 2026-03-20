#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>

int main(int argc , char *argv[]){
    if(argc != 4){
        fprintf(stderr, "Cách sử dụng: %s <Cổng> <Tệp tin chứa câu chào> <Tệp tin lưu nội dung client gửi đến>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    int port = atoi(argv[1]);
    char *file_chao = argv[2];
    char *file_luu = argv[3];

    // Tạo socket
    int server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(server_fd < 0){
        perror("socket() failed");
        exit(EXIT_FAILURE);
    }

    // Cấu hình địa chỉ server
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if(bind(server_fd, (struct sockaddr *)&addr, sizeof(addr))){
        perror("bind() failed");
        exit(EXIT_FAILURE);
    }

    // Lắng nghe kết nối
    listen(server_fd, 5);

    char buffer[256];
    while(1){
        // Chấp nhận kết nối 
        int client = accept(server_fd, NULL, NULL);
        if(client < 0){
            perror("accept() failed");
            exit(EXIT_FAILURE);
        }
        // Gửi file chào
        FILE *f_chao = fopen(file_chao, "r");
        if(f_chao != NULL){
            while(fgets(buffer, sizeof(buffer), f_chao) != NULL){
                send(client, buffer, strlen(buffer), 0);
            }
            fclose(f_chao);
        }
        // Nhận dữ liệu từ client và lưu vào file
        FILE *f_luu = fopen(file_luu, "a");
        if(f_luu == NULL){
            perror("fopen() failed");
            exit(EXIT_FAILURE);
        }
        int bytes;
        while((bytes = recv(client, buffer, sizeof(buffer), 0)) > 0){
            fwrite(buffer, 1, bytes, f_luu);
        }
        fclose(f_luu);
        close(client);
    }
    close(server_fd);
}

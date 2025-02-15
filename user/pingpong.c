#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main() {
    int p[2];  // Pipe gồm 2 đầu: p[0] để đọc, p[1] để ghi
    char buf[1];

    // Tạo một pipe
    if (pipe(p) < 0) {
        printf("Error: Failed to create pipe\n");
        exit(1);
    }

    int pid = fork(); // Tạo tiến trình con

    if (pid < 0) { // Kiểm tra lỗi khi fork
        printf("Error: Fork failed\n");
        exit(1);
    }

    if (pid == 0) { // Tiến trình con
        read(p[0], buf, 1);  // Nhận dữ liệu từ tiến trình cha
        printf("%d: received ping\n", getpid());
        write(p[1], "P", 1);  // Gửi lại dữ liệu cho tiến trình cha
    } else { // Tiến trình cha
        write(p[1], "P", 1);  // Gửi dữ liệu đến tiến trình con
        wait(0); // Chờ tiến trình con hoàn thành
        read(p[0], buf, 1); // Nhận dữ liệu phản hồi từ tiến trình con
        printf("%d: received pong\n", getpid());
    }

    exit(0);
}

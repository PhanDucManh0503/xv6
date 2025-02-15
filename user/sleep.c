#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    if (argc != 2) { // Kiểm tra số lượng tham số
        printf("Usage: sleep <ticks>\n");
        exit(1);
    }

    int ticks = atoi(argv[1]); // Chuyển đổi tham số từ chuỗi sang số nguyên

    if (ticks < 1) { // Kiểm tra giá trị hợp lệ
        printf("Error: ticks must be a positive integer\n");
        exit(1);
    }

    sleep(ticks); // Gọi syscall sleep để tạm dừng tiến trình
    exit(0);
}

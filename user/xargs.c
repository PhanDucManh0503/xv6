// #include "kernel/types.h"
// #include "kernel/stat.h"
// #include "user/user.h"
// #include "kernel/param.h"

// #define MAX_LINE 512

// int main(int argc, char *argv[]) {
//     char buf[MAX_LINE];
//     char *new_argv[MAXARG];
//     int i, n;

//     // Sao chép các đối số của lệnh vào new_argv
//     for (i = 0; i < argc - 1; i++) {
//         new_argv[i] = argv[i + 1];
//     }
    
//     while ((n = read(0, buf, sizeof(buf))) > 0) {
//         int j = 0, start = 0;
//         while (j < n) {
//             if (buf[j] == '\n') {
//                 buf[j] = 0; // Kết thúc chuỗi
//                 new_argv[i] = buf + start;
//                 new_argv[i + 1] = 0;
                
//                 if (fork() == 0) {
//                     exec(new_argv[0], new_argv);
//                     exit(1);
//                 } else {
//                     wait(0);
//                 }
//                 start = j + 1;
//             }
//             j++;
//         }
//     }
//     exit(0);
// }


// #include "kernel/types.h"
// #include "kernel/stat.h"
// #include "user/user.h"

// #define MAXARG 10    // Số lượng tham số tối đa
// #define BUFSIZE 512  // Kích thước bộ đệm đầu vào

// int main(int argc, char *argv[]) {
//     char buf[BUFSIZE], *p = buf;  // Bộ đệm để lưu đầu vào
//     char *nargv[MAXARG];          // Mảng chứa danh sách tham số
//     int n = 0;                    // Số lượng tham số hiện tại

//     // Sao chép các đối số từ dòng lệnh vào nargv
//     for (int i = 1; i < argc; i++) {
//         nargv[n++] = argv[i];
//     }

//     // Đọc đầu vào từ stdin, từng ký tự một
//     while (read(0, p, 1) == 1) {
//         if (*p == '\n') {  // Khi gặp ký tự xuống dòng, thực thi lệnh
//             *p = 0;         // Thay ký tự xuống dòng bằng null để tạo chuỗi hợp lệ
//             nargv[n] = buf; // Thêm tham số mới từ đầu vào
//             nargv[n + 1] = 0; // Đặt NULL để kết thúc danh sách tham số

//             if (fork() == 0) { // Tạo tiến trình con để thực thi lệnh
//                 exec(nargv[0], nargv);
//                 exit(1); // Nếu exec thất bại, tiến trình con thoát với mã lỗi
//             }
//             wait(0); // Chờ tiến trình con hoàn thành
//             p = buf; // Đặt lại con trỏ để đọc dòng tiếp theo
//             n = argc - 1; // Reset số lượng tham số
//         } else {
//             p++; // Tiếp tục đọc ký tự tiếp theo
//         }
//     }
//     exit(0);
// }



#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"  // MAXARG

#define MAX_BUF 2048

int main(int argc, char *argv[]) {
    char buf[MAX_BUF];
    char *args[MAXARG];
    int i, n;
    int pos = 0;

    if (argc < 2) {
        fprintf(2, "usage: xargs <command> [args...]\n");
        exit(1);
    }

    // Sao chép các tham số lệnh vào args
    for (i = 0; i < argc - 1; i++) {
        args[i] = argv[i + 1];
    }

    while ((n = read(0, buf + pos, sizeof(buf) - pos - 1)) > 0) {
        buf[n + pos] = 0; // Kết thúc chuỗi
        char *p = buf;
        while (*p) {
            char *start = p;
            while (*p && *p != '\n') p++; // Tìm ký tự xuống dòng
            if (*p == '\n') {
                *p = 0; // Thay thế '\n' bằng NULL để tạo chuỗi riêng biệt
                args[i] = start;
                args[i + 1] = 0;

                if (fork() == 0) { // Tạo tiến trình con
                    exec(args[0], args);
                    exit(1); // Thoát nếu exec thất bại
                }
                wait(0); // Chờ tiến trình con kết thúc
                p++; // Tiếp tục xử lý chuỗi còn lại
            }
        }
        pos = 0; // Reset vị trí bộ đệm
    }
    exit(0);
}


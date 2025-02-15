// 


#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

/**
 * Hàm fmtname(path)
 * -----------------
 * Trích xuất tên file từ đường dẫn.
 * 
 * Tham số:
 *  - path: Đường dẫn đầy đủ của file/thư mục
 * 
 * Trả về:
 *  - Con trỏ đến tên file cuối cùng trong đường dẫn
 */
char* fmtname(char *path) {
    static char buf[DIRSIZ+1]; // Bộ đệm chứa tên file (giới hạn DIRSIZ ký tự)
    char *p;

    // Đưa con trỏ p đến ký tự '/' cuối cùng trong path
    for (p = path + strlen(path); p >= path && *p != '/'; p--);
    p++; // Trỏ đến ký tự đầu tiên của tên file thực tế

    // Nếu tên file dài hơn DIRSIZ, trả về trực tiếp
    if (strlen(p) >= DIRSIZ) return p;

    // Sao chép tên file vào buffer và gán ký tự kết thúc chuỗi
    memset(buf, 0, sizeof(buf));
    memmove(buf, p, strlen(p));

    return buf;
}

/**
 * Hàm searchDir(path, filename)
 * -----------------------------
 * Tìm kiếm file trong một thư mục và các thư mục con bằng cách duyệt hệ thống tập tin.
 * 
 * Tham số:
 *  - path: Đường dẫn thư mục cần tìm
 *  - filename: Tên file cần tìm
 */
void searchDir(char* path, char* filename) {
    int fd;
    struct stat st;
    struct dirent de;

    // Mở thư mục để lấy file descriptor
    if ((fd = open(path, 0)) < 0) {
        printf("find: cannot open %s\n", path);
        return;
    }

    // Lấy thông tin file/thư mục
    if (fstat(fd, &st) < 0) {
        printf("find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    // Nếu là file, so sánh tên với filename cần tìm
    if (st.type == T_FILE) {
        if (strcmp(fmtname(path), filename) == 0) {
            printf("%s\n", path);
        }
        close(fd);
        return;
    }

    // Nếu là thư mục, duyệt qua các file bên trong
    char buf[512], *p;
    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/'; // Thêm dấu '/' vào cuối đường dẫn

    while (read(fd, &de, sizeof(de)) == sizeof(de)) {
        if (de.inum == 0) continue; // Bỏ qua các mục không hợp lệ
        if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0) continue; // Tránh vòng lặp vô hạn

        // Sao chép tên file/thư mục vào buffer
        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0;

        if (stat(buf, &st) < 0) continue; // Bỏ qua nếu không lấy được thông tin file

        // Nếu là thư mục, gọi đệ quy để tìm kiếm trong thư mục con
        if (st.type == T_DIR) {
            searchDir(buf, filename);
        } else if (strcmp(fmtname(buf), filename) == 0) {
            printf("%s\n", buf); // In ra đường dẫn nếu tìm thấy file
        }
    }
    close(fd); // Đóng file descriptor
}

/**
 * Hàm find(path, filename)
 * ------------------------
 * Kiểm tra đường dẫn trước khi gọi searchDir().
 * 
 * Tham số:
 *  - path: Đường dẫn thư mục cần tìm kiếm
 *  - filename: Tên file cần tìm
 */
void find(char *path, char *filename) {
    int fd;
    struct stat st;

    // Kiểm tra xem đường dẫn có tồn tại không
    if ((fd = open(path, 0)) < 0) {
        printf("find: cannot open %s\n", path);
        return;
    }

    // Lấy thông tin của đường dẫn
    if (fstat(fd, &st) < 0) {
        printf("find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    // Nếu là thư mục, gọi searchDir để bắt đầu tìm kiếm
    if (st.type == T_DIR) {
        searchDir(path, filename);
    }

    close(fd);
}

/**
 * Hàm main(argc, argv)
 * --------------------
 * Kiểm tra đầu vào và gọi hàm `find()`.
 * 
 * Tham số:
 *  - argc: Số lượng tham số
 *  - argv: Mảng chứa các tham số
 */
int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: find <path> <filename>\n");
        exit(1);
    }

    if (argc == 2) {
        find(".", argv[1]);  // Tìm trong thư mục hiện tại nếu không chỉ định path
    } else {
        for (int i = 1; i < argc; i++) {
            find(argv[i], argv[2]);
        }
    }

    exit(0);
}

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
    // Lấy thời gian hiện tại
    time_t t;
    time(&t);

    // Sử dụng giá trị thời gian làm seed cho hàm srand
    srand((unsigned int) t);

    // Tạo các số ngẫu nhiên
    int i;
    for (i = 0; i < 10; i++) {
        printf("%d\n", rand());
    }

    return 0;
}

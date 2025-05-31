#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

int main() {
    printf("\n=== PROGRAM P1 BẮT ĐẦU ===\n");
    printf("P1: Chương trình tính toán số học\n");
    
    int counter = 0;
    time_t start_time = time(NULL);
    
    while (1) {
        // Mô phỏng công việc tính toán
        long sum = 0;
        for (int i = 0; i < 1000000; i++) {
            sum += i * i;
        }
        
        counter++;
        time_t current_time = time(NULL);
        
        printf("P1: Lần tính thứ %d, tổng = %ld, thời gian chạy: %ld giây\n", 
               counter, sum % 10000, current_time - start_time);
        
        sleep(1);  // Tạm dừng 1 giây
        
        // Kiểm tra tín hiệu dừng
        if (counter >= 20) {
            printf("P1: Hoàn thành sau %d lần tính!\n", counter);
            break;
        }
    }
    
    printf("=== PROGRAM P1 KẾT THÚC ===\n");
    return 0;
}

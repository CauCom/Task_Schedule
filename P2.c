#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

int main() {
    printf("\n=== PROGRAM P2 BẮT ĐẦU ===\n");
    printf("P2: Chương trình xử lý chuỗi\n");
    
    int counter = 0;
    time_t start_time = time(NULL);
    char buffer[1000];
    
    while (1) {
        // Mô phỏng xử lý chuỗi
        strcpy(buffer, "Hello World from P2 - ");
        
        for (int i = 0; i < 100; i++) {
            strcat(buffer, "X");
        }
        
        counter++;
        time_t current_time = time(NULL);
        
        printf("P2: Xử lý chuỗi lần %d, độ dài = %lu, thời gian: %ld giây\n", 
               counter, strlen(buffer), current_time - start_time);
        
        // Tạo output file để thể hiện process đang làm việc
        FILE* f = fopen("/tmp/p2_output.txt", "a");
        if (f) {
            fprintf(f, "P2 iteration %d at time %ld\n", counter, current_time);
            fclose(f);
        }
        
        sleep(1);
        
        if (counter >= 15) {
            printf("P2: Hoàn thành sau %d lần xử lý!\n", counter);
            break;
        }
    }
    
    printf("=== PROGRAM P2 KẾT THÚC ===\n");
    return 0;
}

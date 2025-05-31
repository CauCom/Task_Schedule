#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

int main() {
    printf("\n=== PROGRAM P3 BẮT ĐẦU ===\n");
    printf("P3: Chương trình tính toán khoa học\n");
    
    int counter = 0;
    time_t start_time = time(NULL);
    
    while (1) {
        // Mô phỏng tính toán phức tạp
        double result = 0.0;
        for (int i = 1; i <= 10000; i++) {
            result += sin(i) * cos(i) * sqrt(i);
        }
        
        counter++;
        time_t current_time = time(NULL);
        
        printf("P3: Tính toán lần %d, kết quả = %.2f, thời gian: %ld giây\n", 
               counter, result, current_time - start_time);
        
        // Ghi kết quả vào file
        FILE* f = fopen("/tmp/p3_results.txt", "a");
        if (f) {
            fprintf(f, "Iteration %d: result = %.6f at time %ld\n", 
                    counter, result, current_time);
            fclose(f);
        }
        
        sleep(1);
        
        if (counter >= 12) {
            printf("P3: Hoàn thành sau %d lần tính toán!\n", counter);
            break;
        }
    }
    
    printf("=== PROGRAM P3 KẾT THÚC ===\n");
    return 0;
}

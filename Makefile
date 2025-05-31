CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g
MATHFLAGS = -lm

# Targets
SCHEDULER = scheduler
PROGRAMS = programs/P1 programs/P2 programs/P3

.PHONY: all clean setup run restore

all: setup $(SCHEDULER) $(PROGRAMS)

# Tạo thư mục programs nếu chưa có
setup:
	@mkdir -p programs
	@echo "Đã tạo thư mục programs/"

# Compile scheduler chính
$(SCHEDULER): scheduler.c
	$(CC) $(CFLAGS) -o $@ $<
	@echo "Đã compile scheduler thành công!"

# Compile các chương trình mẫu
programs/P1: P1.c
	$(CC) $(CFLAGS) -o $@ $< $(MATHFLAGS)
	@echo "Đã compile P1"

programs/P2: P2.c  
	$(CC) $(CFLAGS) -o $@ $<
	@echo "Đã compile P2"

programs/P3: P3.c
	$(CC) $(CFLAGS) -o $@ $< $(MATHFLAGS)
	@echo "Đã compile P3"

# Chạy scheduler
run: all
	@echo "=== CHẠY TASK SCHEDULER ==="
	@echo "Nhấn Ctrl+C để dừng và lưu trạng thái"
	@echo "Sử dụng 'make restore' để khôi phục trạng thái đã lưu"
	@echo ""
	./$(SCHEDULER)

# Khôi phục từ trạng thái đã lưu  
restore: all
	@echo "=== KHÔI PHỤC TRẠNG THÁI SCHEDULER ==="
	./$(SCHEDULER) --restore

# Dọn dẹp
clean:
	rm -f $(SCHEDULER) $(PROGRAMS)
	rm -f scheduler_state.dat
	rm -f /tmp/p2_output.txt /tmp/p3_results.txt
	@echo "Đã dọn dẹp tất cả files"

# Hiển thị trạng thái
status:
	@echo "=== TRẠNG THÁI HỆ THỐNG ==="
	@echo "Scheduler executable: $(shell [ -f $(SCHEDULER) ] && echo 'CÓ' || echo 'KHÔNG')"
	@echo "Programs compiled:"
	@for prog in $(PROGRAMS); do \
		echo "  $$prog: $(shell [ -f $$prog ] && echo 'CÓ' || echo 'KHÔNG')"; \
	done
	@echo "State file: $(shell [ -f scheduler_state.dat ] && echo 'CÓ ($(shell stat -f%z scheduler_state.dat 2>/dev/null || stat -c%s scheduler_state.dat 2>/dev/null) bytes)' || echo 'KHÔNG')"
	@echo "Running processes:"
	@ps aux | grep -E "(P1|P2|P3|scheduler)" | grep -v grep || echo "  Không có process nào đang chạy"

# Debug mode - compile với debug symbols
debug: CFLAGS += -DDEBUG -O0
debug: all

# Tạo thêm chương trình mẫu
create-more-programs:
	@echo "Tạo thêm các chương trình mẫu..."
	@echo '#include <stdio.h>\n#include <unistd.h>\nint main() { \
		printf("P4: File I/O Program\\n"); \
		for(int i=0; i<10; i++) { \
			printf("P4: Writing file iteration %d\\n", i+1); \
			FILE* f = fopen("/tmp/p4.txt", "a"); \
			if(f) { fprintf(f, "Line %d\\n", i+1); fclose(f); } \
			sleep(1); \
		} \
		return 0; \
	}' > P4.c
	$(CC) $(CFLAGS) -o programs/P4 P4.c
	@echo "Đã tạo P4 (File I/O Program)"

help:
	@echo "=== TASK SCHEDULER - HƯỚNG DẪN SỬ DỤNG ==="
	@echo ""
	@echo "Các lệnh có sẵn:"
	@echo "  make all              - Compile tất cả (scheduler + programs)"
	@echo "  make run              - Chạy scheduler"  
	@echo "  make restore          - Khôi phục từ trạng thái đã lưu"
	@echo "  make clean            - Dọn dẹp tất cả files"
	@echo "  make status           - Hiển thị trạng thái hệ thống"
	@echo "  make debug            - Compile với debug mode"
	@echo "  make create-more-programs - Tạo thêm chương trình mẫu"
	@echo "  make help             - Hiển thị hướng dẫn này"
	@echo ""
	@echo "Cách sử dụng:"
	@echo "  1. make all           # Compile tất cả"
	@echo "  2. make run           # Chạy scheduler"
	@echo "  3. Ctrl+C             # Dừng và lưu trạng thái"  
	@echo "  4. make restore       # Khôi phục và tiếp tục"
	@echo ""

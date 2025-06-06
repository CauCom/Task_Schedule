#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>
#include <dirent.h>
#include <time.h>
#include <sys/stat.h>

#define MAX_PROCESSES 10
#define MAX_FILENAME 256
#define TIME_QUANTUM 3  // seconds
#define STATE_FILE "scheduler_state.dat"

typedef enum {
    READY,
    RUNNING, 
    TERMINATED,
    WAITING
} ProcessState;

typedef struct {
    pid_t pid;
    char program_name[MAX_FILENAME];
    char executable_path[MAX_FILENAME];
    int burst_time;
    int remaining_time;
    ProcessState state;
    time_t start_time;
} Process;

typedef struct {
    Process processes[MAX_PROCESSES];
    int front;
    int rear;
    int count;
    int time_quantum;
    int current_running;
} ProcessQueue;

ProcessQueue pq;
volatile sig_atomic_t scheduler_running = 1;

// Function prototypes
void init_queue();
void load_programs();
int enqueue_process(const char* program_name, const char* executable_path, int burst_time);
Process* dequeue_process();
void save_state();
void load_state();
void signal_handler(int sig);
void setup_signal_handlers();
void print_queue_status();
void run_scheduler();
int create_process(Process* proc);
void cleanup_processes();

void init_queue() {
    pq.front = 0;
    pq.rear = -1;
    pq.count = 0;
    pq.time_quantum = TIME_QUANTUM;
    pq.current_running = -1;

    // Initialize queue indices
    for (int i = 0; i < MAX_PROCESSES; i++) {
        pq.queue[i] = -1;
    }
}

void load_programs() {
    // Tự động tìm các executable files trong thư mục hiện tại
    DIR *dir;
    struct dirent *entry;
    struct stat file_stat;
    
    dir = opendir("./programs");
    if (dir == NULL) {
        printf("Tạo thư mục ./programs và đặt các chương trình P1, P2, P3... vào đó\n");
        return;
    }
    
    printf("Đang tải các chương trình từ thư mục ./programs/\n");
    
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.') continue;
        
        char full_path[512];
        snprintf(full_path, sizeof(full_path), "./programs/%s", entry->d_name);
        
        if (/* file executable */) {
            int idx = -1;
            // Find empty slot
            for (int i = 0; i < MAX_PROCESSES; i++) {
                if (pq.processes[i].pid == 0) {
                    idx = i;
                    break;
                }
            }

            if (idx != -1) {
                // Initialize process
                strcpy(pq.processes[idx].program_name, entry->d_name);
                // ... [các thông số khác] ...

                enqueue_process(idx); // Thêm vào hàng đợi
            }
        }
    }
    closedir(dir);
}

int enqueue_process(int index) {
    if (pq.count >= MAX_PROCESSES) return -1;

    pq.rear = (pq.rear + 1) % MAX_PROCESSES;
    pq.queue[pq.rear] = index;
    pq.count++;

    pq.processes[index].state = READY;
    return pq.rear;
}
int dequeue_process() {
    if (pq.count == 0) return -1;

    int index = pq.queue[pq.front];
    pq.front = (pq.front + 1) % MAX_PROCESSES;
    pq.count--;

    return index;
}

int create_process(Process* proc) {
    pid_t pid = fork();
    
    if (pid == 0) {
        // Child process
        printf("[CHILD] Bắt đầu thực thi: %s\n", proc->program_name);
        
        // Pause ngay sau khi fork để scheduler có thể kiểm soát
        raise(SIGSTOP);
        
        // Khi được resume, exec chương trình
        execl(proc->executable_path, proc->program_name, NULL);
        perror("execl failed");
        exit(1);
    } else if (pid > 0) {
        // Parent process
        proc->pid = pid;
        proc->state = READY;
        printf("[SCHEDULER] Đã tạo process %s với PID: %d\n", 
               proc->program_name, pid);
        
        // Đợi child process tự pause
        int status;
        waitpid(pid, &status, WUNTRACED);
        
        return 0;
    } else {
        perror("fork failed");
        return -1;
    }
}

void print_queue_status() {
    printf("\n=== TRẠNG THÁI HÀNG ĐỢI ===\n");
    printf("Số process trong queue: %d\n", pq.count);
    printf("Time quantum: %d giây\n", pq.time_quantum);
    
    if (pq.current_running >= 0) {
        Process* running = &pq.processes[pq.current_running];
        printf("Đang chạy: %s (PID: %d, còn lại: %d giây)\n", 
               running->program_name, running->pid, running->remaining_time);
    }
    
    printf("\nDanh sách processes:\n");
    for (int i = 0; i < MAX_PROCESSES; i++) {
        Process* p = &pq.processes[i];
        if (p->pid > 0) {
            const char* state_str[] = {"READY", "RUNNING", "TERMINATED", "WAITING"};
            printf("  %s (PID: %d) - %s - Còn lại: %d/%d giây\n",
                   p->program_name, p->pid, state_str[p->state], 
                   p->remaining_time, p->burst_time);
        }
    }
    printf("========================\n\n");
}

void save_state() {
    FILE* f = fopen(STATE_FILE, "wb");
    if (f) {
        fwrite(&pq, sizeof(ProcessQueue), 1, f);
        fclose(f);
        printf("Đã lưu trạng thái vào %s\n", STATE_FILE);
    }
}

void load_state() {
    FILE* f = fopen(STATE_FILE, "rb");
    if (f) {
        fread(&pq, sizeof(ProcessQueue), 1, f);
        fclose(f);
        printf("Đã khôi phục trạng thái từ %s\n", STATE_FILE);
    }
}

void cleanup_processes() {
    printf("\n[CLEANUP] Dọn dẹp tất cả processes...\n");
    
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (pq.processes[i].pid > 0) {
            kill(pq.processes[i].pid, SIGTERM);
            
            // Đợi process kết thúc
            int status;
            if (waitpid(p->pid, &status, WNOHANG) == 0) {
                // Nếu không kết thúc trong 2 giây, force kill
                sleep(2);
                kill(p->pid, SIGKILL);
                waitpid(p->pid, &status, 0);
            }
            p->state = TERMINATED;
        }
    }
}

void signal_handler(int sig) {
    switch(sig) {
        case SIGINT:
        case SIGTERM:
            printf("\n[SIGNAL] Nhận tín hiệu dừng scheduler...\n");
            scheduler_running = 0;
            save_state();
            cleanup_processes();
            break;
    }
}

void setup_signal_handlers() {
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
}

void run_scheduler() {
    printf("\n[SCHEDULER] Bắt đầu Round Robin Scheduling...\n");
    printf("Time Quantum: %d giây\n", pq.time_quantum);
    
    // Tạo tất cả processes trước
    for (int i = 0; i < MAX_PROCESSES; i++) {
        Process* p = &pq.processes[i];
        if (p->remaining_time > 0 && p->pid == 0) {
            if (create_process(p) != 0) {
                p->state = TERMINATED;
            }
        }
    }
    
    while (scheduler_running && pq.count > 0) {
        print_queue_status();
        
        // Tìm process READY tiếp theo
        

            // Dequeue next process
        int proc_index = dequeue_process();
        if (proc_index == -1) break;

        Process* current = &pq.processes[proc_index];
        pq.current_running = proc_index;
        current->state = RUNNING;
        current->start_time = time(NULL);
        
        // Resume process
        kill(current->pid, SIGCONT);
        
        // Đợi time quantum hoặc process kết thúc
        int runtime = (current->remaining_time < pq.time_quantum) ?
            current->remaining_time : pq.time_quantum;
        
        for (int t = 0; t < runtime && scheduler_running; t++) {
            sleep(1);
            
            // Kiểm tra xem process có còn sống không
            int status;
            pid_t result = waitpid(current->pid, &status, WNOHANG);
            
            if (result > 0) {
                // Process đã kết thúc
                printf("\n[SCHEDULER] Process %s đã hoàn thành!\n", 
                       current->program_name);
                current->state = TERMINATED;
                current->remaining_time = 0;
                pq.count--;
                break;
            }
            
            printf(".");
            fflush(stdout);
        }
        
        if (current->state == RUNNING) {
            kill(current->pid, SIGSTOP);
            current->remaining_time -= runtime;

            // RE-ENQUEUE UNFINISHED PROCESS
            if (current->remaining_time > 0) {
                enqueue_process(proc_index);
                printf("↻ Đưa %s vào cuối hàng đợi\n", current->program_name);
            }
            else {
                current->state = TERMINATED;
                printf("✓ %s hoàn thành!\n", current->program_name);
            }
        }
        
        pq.current_running = -1;
        
        // Lưu trạng thái định kỳ
        save_state();
        
        printf("\nChờ 1 giây trước khi chuyển process tiếp theo...\n");
        sleep(1);
    }
    
    printf("\n[SCHEDULER] Hoàn thành tất cả processes!\n");
}

int main(int argc, char* argv[]) {
    printf("=== TASK SCHEDULER - ROUND ROBIN ===\n");
    printf("Tác giả: CPU Scheduling Simulator\n\n");
    
    srand(time(NULL));
    setup_signal_handlers();
    
    // Kiểm tra xem có khôi phục từ state file không
    if (argc > 1 && strcmp(argv[1], "--restore") == 0) {
        load_state();
    } else {
        init_queue();
        load_programs();
    }
    
    if (pq.count == 0) {
        printf("Không có chương trình nào để thực thi!\n");
        printf("Hướng dẫn:\n");
        printf("1. Tạo thư mục './programs'\n");
        printf("2. Đặt các file executable (P1, P2, P3...) vào đó\n");
        printf("3. Chạy lại scheduler\n");
        return 1;
    }
    
    printf("Đã tải %d chương trình vào hàng đợi\n", pq.count);
    printf("Nhấn Ctrl+C để dừng và lưu trạng thái\n\n");
    
    run_scheduler();
    
    cleanup_processes();
    save_state();
    
    printf("\nTạm biệt!\n");
    return 0;
}

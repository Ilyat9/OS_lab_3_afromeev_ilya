#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h> 
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>
#include <string.h>

void handle_error(const char* msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

void process_data(char *data, size_t size) {
    long long current_sum = 0;
    int current_num = 0;
    int sign = 1;
    int reading_digits = 0; 

    for (size_t i = 0; i < size; i++) {
        char c = data[i];

        if (c == '-') {
            sign = -1;
        } else if (isdigit((unsigned char)c)) {
            current_num = current_num * 10 + (c - '0');
            reading_digits = 1;
        } else {
            if (reading_digits) {
                current_sum += (current_num * sign);
                current_num = 0;
                sign = 1;
                reading_digits = 0;
            }

            if (c == '\n') {
                printf("Сумма строки: %lld\n", current_sum);
                current_sum = 0;
            }
        }
    }

    if (reading_digits) {
        current_sum += (current_num * sign);
        printf("Сумма строки (конец файла): %lld\n", current_sum);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Использование: %s <имя_файла>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *filepath = argv[1];


    int fd = open(filepath, O_RDONLY);
    if (fd == -1) handle_error("Ошибка при открытии файла");

    
    struct stat sb;
    if (fstat(fd, &sb) == -1) handle_error("Ошибка fstat");

    if (sb.st_size == 0) {
        printf("Файл пуст.\n");
        close(fd);
        return 0;
    }

    
    char *addr = mmap(NULL, sb.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED) handle_error("Ошибка mmap");

    pid_t pid = fork();

    if (pid == -1) {
        handle_error("Ошибка fork");
    }

    if (pid == 0) {
        printf("[Child] PID: %d. Начинаю обработку данных...\n", getpid());
        
        process_data(addr, sb.st_size);
        
        printf("[Child] Завершение работы.\n");
        
        exit(EXIT_SUCCESS);
    } else {
        printf("[Parent] PID: %d. Ожидание завершения дочернего процесса (PID: %d)...\n", getpid(), pid);

        int status;
        waitpid(pid, &status, 0);

        if (WIFEXITED(status)) {
            printf("[Parent] Дочерний процесс завершился с кодом %d.\n", WEXITSTATUS(status));
        } else {
            printf("[Parent] Дочерний процесс завершился аварийно.\n");
        }

        if (munmap(addr, sb.st_size) == -1) handle_error("Ошибка munmap");
        close(fd);
    }

    return 0;
}

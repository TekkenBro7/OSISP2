#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <fcntl.h>

volatile sig_atomic_t keep_running = 1;

void handle_signal(int signal) {
    if (signal == SIGTERM || signal == SIGINT) {
        keep_running = 0;  // Устанавливаем флаг завершения
    }
}

// Функция записи текущего значения счетчика в файл состояния
void write_state(int count) {
    FILE *file = fopen("state.txt", "w");  // Открываем файл в режиме перезаписи
    if (file == NULL) {
        perror("Ошибка открытия файла state.txt");
        exit(EXIT_FAILURE);
    }
    fprintf(file, "%d\n", count);  
    fclose(file);
}

int read_state() {
    int count = 0;
    FILE *file = fopen("state.txt", "r");
    if (file) {
        fscanf(file, "%d", &count);  
        fclose(file);
    } else {
        count = 0;
    }
    return count;
}

// Функция записи счетчика в лог-файл
void write_counter(int count) {
    FILE *file = fopen("counter_log.txt", "a"); 
    if (file == NULL) {
        perror("Ошибка открытия файла counter_log.txt");
        exit(EXIT_FAILURE);
    }
    fprintf(file, "Счетчик %d\n", count);
    fclose(file);
}

int main() {
    signal(SIGTERM, handle_signal);
    signal(SIGINT, handle_signal);

    int count = read_state();
    count++; 

    while (keep_running) {
        write_counter(count);  
        write_state(count);    
        count++;               
        sleep(1);              
    }

    write_state(count);

    pid_t pid = fork();
    if (pid == 0) {
        execlp("./lab4", "./lab4", NULL);
        perror("Ошибка exec");
        exit(EXIT_FAILURE);
    }

    return 0;
}
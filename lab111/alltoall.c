#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
void all_to_all(int msg_size) {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Определение ранга (идентификатора) процесса
    MPI_Comm_size(MPI_COMM_WORLD, &size); // Определение общего числа процессов
    // Выделение памяти для буферов отправки и приема
    char* sendbuf = (char*)malloc(msg_size * size); // Буфер отправки
    char* recvbuf = (char*)malloc(msg_size * size); // Буфер приема
    MPI_Request* requests = (MPI_Request*)malloc(2 * size * sizeof(MPI_Request)); // Массив запросов
    MPI_Status* statuses = (MPI_Status*)malloc(2 * size * sizeof(MPI_Status)); // Массив статусов
    // Инициализация буфера отправки данными, зависящими от ранга процесса
    for (int i = 0; i < size * msg_size; i++) {
        sendbuf[i] = rank;
    }
    double start_time = MPI_Wtime(); // Замер времени начала коммуникации
    // Асинхронная отправка и прием сообщений между всеми процессами
    for (int i = 0; i < size; i++) {
        MPI_Isend(&sendbuf[i * msg_size], msg_size, MPI_CHAR, i, 0, MPI_COMM_WORLD, &requests[i]); // Асинхронная отправка
        MPI_Irecv(&recvbuf[i * msg_size], msg_size, MPI_CHAR, i, 0, MPI_COMM_WORLD, &requests[size + i]); // Асинхронный прием
    }
    // Ожидание завершения всех асинхронных операций
    MPI_Waitall(2 * size, requests, statuses);
    double end_time = MPI_Wtime(); // Замер времени окончания коммуникации
    // Вывод времени выполнения для каждого процесса
    printf("All-to-all: Процесс %d, размер сообщения %d байт, время: %f сек\n", rank, msg_size, end_time - start_time);
        free(sendbuf); // Освобождение памяти
    free(recvbuf);
    free(requests);
    free(statuses);
}
int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv); // Инициализация среды MPI
    all_to_all(1024); // Вызов функции для сообщения размером 1 КБ
    MPI_Finalize(); // Завершение работы MPI
    return 0;
}
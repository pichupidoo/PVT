#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
void ring_communication(int msg_size, int iterations) {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Определение ранга (идентификатора) процесса
    MPI_Comm_size(MPI_COMM_WORLD, &size); // Определение общего числа процессов
    char* sendbuf = (char*)malloc(msg_size); // Выделение памяти под буфер отправки
    char* recvbuf = (char*)malloc(msg_size); // Выделение памяти под буфер приема
    for (int i = 0; i < msg_size; i++) {
        sendbuf[i] = rank; // Инициализация буфера отправки значением ранга процесса
    }
    int next = (rank + 1) % size; // Индекс следующего процесса по кольцу
    int prev = (rank - 1 + size) % size; // Индекс предыдущего процесса по кольцу
    double start_time = MPI_Wtime(); // Замер времени начала коммуникации
    for (int i = 0; i < iterations; i++) {
        MPI_Sendrecv(sendbuf, msg_size, MPI_CHAR, next, 0, recvbuf, msg_size, MPI_CHAR, prev, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        for (int j = 0; j < msg_size; j++) {
            sendbuf[j] = recvbuf[j];
        }
    }
    double end_time = MPI_Wtime(); // Замер времени окончания коммуникации
    if (rank == 0) { // Вывод результата на экран только процессом с рангом 0
        printf("Ring: Размер сообщения %d байт, время: %f сек\n", msg_size,
            end_time - start_time);
    }
    free(sendbuf); // Освобождение памяти
    free(recvbuf);
}
int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv); // Инициализация среды MPI
    ring_communication(1, 15); // Вызов функции для сообщения размером 1 байт
    ring_communication(1024, 15); // Вызов функции для сообщения размером 1 КБ
    ring_communication(1024 * 1024, 15); // Вызов функции для сообщения размером 1 МБ
    MPI_Finalize(); // Завершение работы MPI
    return 0;
}
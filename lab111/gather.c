#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
void gather(int msg_size) {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Определение ранга (идентификатора) процесса
    MPI_Comm_size(MPI_COMM_WORLD, &size); // Определение общего числа процессов
    char* sendbuf = (char*)malloc(msg_size); // Буфер для отправки данных
    char* recvbuf = NULL; // Буфер для приема данных на корневом процессе
    // Инициализация буфера отправки данными, зависящими от ранга процесса
    for (int i = 0; i < msg_size; i++) {
        sendbuf[i] = rank;
    }
    if (rank == 0) {
        // Выделение памяти под буфер приема на процессе 0
        recvbuf = (char*)malloc(msg_size * size);
    }
    double start_time = MPI_Wtime(); // Замер времени начала коммуникации
    // Все процессы отправляют свои данные процессу 0 (корневому процессу)
    MPI_Gather(sendbuf, msg_size, MPI_CHAR, recvbuf, msg_size, MPI_CHAR, 0,
        MPI_COMM_WORLD);
    double end_time = MPI_Wtime(); // Замер времени окончания коммуникации
    // Вывод времени выполнения для каждого процесса
    printf("Gather: Процесс %d, размер сообщения %d байт, время: %f сек\n",
        rank, msg_size, end_time - start_time);
    free(sendbuf); // Освобождение памяти
    if (recvbuf) {
        free(recvbuf); // Освобождение памяти для приема на процессе 0
    }
}
int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv); // Инициализация среды MPI
    gather(1024); // Вызов функции для сообщения размером 1 КБ
    gather(1024 * 1024); // Вызов функции для сообщения размером 1 МБ
    MPI_Finalize(); // Завершение работы MPI
    return 0;
}
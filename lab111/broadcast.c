#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
void broadcast(int msg_size) {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Определение ранга (идентификатора) процесса
    MPI_Comm_size(MPI_COMM_WORLD, &size); // Определение общего числа процессов
    char* buffer = (char*)malloc(msg_size); // Выделение памяти под буфер
    if (rank == 0) {
        // Инициализация буфера сообщением для процесса 0
        for (int i = 0; i < msg_size; i++) {
            buffer[i] = 'A'; // Заполнение буфера символами 'A'
        }
    }
    double start_time = MPI_Wtime(); // Замер времени начала коммуникации
    if (rank == 0) {
        // Процесс 0 отправляет сообщение всем остальным процессам
        for (int i = 1; i < size; i++) {
            MPI_Send(buffer, msg_size, MPI_CHAR, i, 0, MPI_COMM_WORLD); //Отправка данных
        }
    }
    else {
        // Остальные процессы получают сообщение от процесса 0
        MPI_Recv(buffer, msg_size, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // Прием данных
    }
    double end_time = MPI_Wtime(); // Замер времени окончания коммуникации
    // Вывод времени выполнения для каждого процесса
    printf("Broadcast: Процесс %d, размер сообщения %d байт, время: %f сек\n", rank, msg_size, end_time - start_time);
        free(buffer); // Освобождение памяти
}
int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv); // Инициализация среды MPI
    broadcast(1024); // Вызов функции для сообщения размером 1 КБ
    broadcast(1024 * 1024); // Вызов функции для сообщения размером 1 МБ
    MPI_Finalize(); // Завершение работы MPI
    return 0;
}
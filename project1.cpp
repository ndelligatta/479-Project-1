#include <iostream>
#include <cstdlib>
#include <ctime>
#include <mpi.h>

int get_random_number(int rank);

int main(int argc, char *argv[]) {
    int rank, size;
    int numbers[2] = {0, 0};

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

   // Input validation
   if (size < 6 || size > 20) {
        if(rank == 0)
        std::cerr << "Size must be between [6, 20]!\n";
        MPI_Finalize();
        return 0;
    }

    int random_number = get_random_number(rank);

    if(rank != 0) {
        MPI_Recv(numbers, 2, MPI_INT, (rank - 1) % size, 0,
                 MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        std::cout << "Process " << rank << " #" << random_number
                  << " has recieved values [" << numbers[0] << ", "
                  << numbers[1] << "] from " << (rank - 1) % size
                  << std::endl;
    } else {
        numbers[0] = random_number;
    }

    if (rank == 1) {
        numbers[1] = random_number;
    }

    // Ensure largest number is first in array
    // while second largest is second in array
    if(numbers[1] > numbers[0]) {
        int temp;
        temp = numbers[0];
        numbers[0] = numbers[1];
        numbers[1] = temp;
    }

    // If rank 0 and 1 already put their numbers in,
    // then start comparing the three numbers for two highest
    if(rank > 1) {
        if(random_number > numbers[0]) {
            numbers[1] = numbers[0];
            numbers[0] = random_number;
        } else if(random_number > numbers[1]) {
            numbers[1] = random_number;
        }
    }

    MPI_Send(numbers, 2, MPI_INT, (rank + 1) % size, 0, MPI_COMM_WORLD);

    if(rank == 0) {
        MPI_Recv(numbers, 2, MPI_INT, size - 1, 0,
                 MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        std::cout << "Process " << rank << " #" << random_number
                  << " has recieved values [" << numbers[0] << ", "
                  << numbers[1] << "] from " << size - 1
                  << std::endl;
    }

    // End of getting top two numbers
    MPI_Barrier(MPI_COMM_WORLD);
    // Start of finding leader

    //  [1] = leader rank | [2] = second highest number
    int leader_and_number[2] = {0, 0};

    if(rank != 0) {
        MPI_Recv(leader_and_number, 2, MPI_INT, (rank - 1) % size, 0,
                 MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    } else {
        leader_and_number[0] = -1;
        leader_and_number[1] = numbers[1];
    }

    // Finding leader
    if(leader_and_number[1] == random_number) {
        leader_and_number[0] = rank;
        std::cout << "I, rank " << rank << " #" << random_number
                  << " have the 2nd highest number " << leader_and_number[1]
                  << " and I am the leader.\n";
   }

    MPI_Send(leader_and_number, 2, MPI_INT, (rank + 1) % size, 0, MPI_COMM_WORLD);

    if(rank == 0) {
        MPI_Recv(leader_and_number, 2, MPI_INT, size - 1, 0,
                 MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    // End of finding leader
    MPI_Barrier(MPI_COMM_WORLD);
    // Setting leader rank

    int leader;

    if (rank == 0) {
        leader = leader_and_number[0];
    }

    // Setting every process's leader to
    // rank with second highest number
    MPI_Bcast(&leader, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Ranks declaring their leader
    if(rank != leader)
        std::cout << "Rank " << rank << "'s leader is " << leader << std::endl;
    MPI_Finalize();
    return 0;
}
// Creates the unique 5 digit ID NNDRR
int get_random_number(int rank) {
    std::srand(std::time(nullptr) + rank);
    int random_number = std::rand() % 90 + 10;
    int d = random_number % 2;
    random_number *= 1000;
    d *= 100;
    random_number += d;
    random_number += rank;
    return random_number;
}

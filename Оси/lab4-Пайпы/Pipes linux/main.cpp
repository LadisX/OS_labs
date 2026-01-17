#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <vector>
#include <string>
#include <cstring>

// Массив для хранения файловых дескрипторов всех пайпов
// 5 пайпов, у каждого 2 конца: [0] - чтение, [1] - запись
int pipes[5][2];

void createPipes() {
    for (int i = 0; i < 5; ++i) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            exit(1);
        }
    }
}

void launchProcess(const char* progName, int readPipeIdx, int writePipeIdx) {
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(1);
    }

    if (pid == 0) { 
        
        if (dup2(pipes[readPipeIdx][0], STDIN_FILENO) == -1) {
            perror("dup2 stdin");
            exit(1);
        }

        // Перенаправляем STDOUT на запись в pipes[writePipeIdx]
        if (dup2(pipes[writePipeIdx][1], STDOUT_FILENO) == -1) {
            perror("dup2 stdout");
            exit(1);
        }

        for (int i = 0; i < 5; ++i) {
            close(pipes[i][0]);
            close(pipes[i][1]);
        }

        std::string path = "./";
        path += progName;
        execl(path.c_str(), progName, NULL);

        perror("execl");
        exit(1);
    }
}

int main() {
    createPipes();

    std::cout << "Launching processes..." << std::endl;

    launchProcess("M", 0, 1);
    launchProcess("A", 1, 2);
    launchProcess("P", 2, 3);
    launchProcess("S", 3, 4);

    for (int i = 0; i < 5; ++i) {
        if (i == 0) close(pipes[i][0]);
        else if (i == 4) close(pipes[i][1]);
        else {
            close(pipes[i][0]);
            close(pipes[i][1]);
        }
    }

    std::cout << "Enter numbers separated by spaces: ";
    std::string input;
    std::getline(std::cin, input);
    input += "\n";

    write(pipes[0][1], input.c_str(), input.length());


    close(pipes[0][1]);

    std::cout << "Processing..." << std::endl;

    for (int i = 0; i < 4; ++i) {
        wait(NULL);
    }

    char buffer[1024];
    int bytesRead = read(pipes[4][0], buffer, sizeof(buffer) - 1);
    if (bytesRead > 0) {
        buffer[bytesRead] = '\0';
        std::cout << "Result: " << buffer << std::endl;
    }

    close(pipes[4][0]);

    return 0;
}
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_CHAR 1024

int main() {
    int p1[2], p2[2], p3[2];
    int status;
    
    if (pipe(p1) < 0 || pipe(p2) < 0 || pipe(p3) < 0) {
        perror("Pipe failed");
        return 1;
    }

    pid_t child1 = fork();

    if (child1 < 0) {
        perror("Fork failed");
        return 1;
    }

    if (child1 == 0) {
        int num;
        int new_num;

        close(p1[1]);
        close(p2[0]);
        close(p3[0]);
        close(p3[1]);

        while (read(p1[0], &num, sizeof(num)) > 0) {
            new_num = num * num; 
            if (write(p2[1], &new_num, sizeof(new_num)) == -1) {
                perror("Write to p2 failed");
                break;
            }
        }

        close(p1[0]);
        close(p2[1]);
        exit(0);
    }

    pid_t child2 = fork();

    if (child2 < 0) {
        perror("Fork failed");
        return 1;
    }

    if (child2 == 0) {
        int num;
        int new_num;

        close(p1[0]);
        close(p1[1]);
        close(p2[1]);
        close(p3[0]);

        while (read(p2[0], &num, sizeof(num)) > 0) {
            new_num = num + 1; 
            if (write(p3[1], &new_num, sizeof(new_num)) == -1) {
                perror("Write to p3 failed");
                break;
            }
        }

        close(p2[0]);
        close(p3[1]);
        exit(0);
    }

    if (child1 != 0 && child2 != 0) {
        int num;

        close(p1[0]);
        close(p2[0]);
        close(p2[1]);
        close(p3[1]);

        while (scanf("%d", &num) != EOF) {
            if (write(p1[1], &num, sizeof(num)) == -1) {
                perror("Write to p1 failed");
                break;
            }

            if (read(p3[0], &num, sizeof(num)) > 0) {
                printf("Result: %d\n", num);
            }
            else {
                perror("Read from p3 failed");
                break;
            }
        }
        close(p1[1]);
        close(p3[0]);

        waitpid(child1, &status, 0);
        waitpid(child2, &status, 0);
    }
    
    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/time.h>

static void get_to_line(uint32_t line_num, FILE *stream) {
    char line[256];
    bool line_not_reached = true;
    uint32_t lines = 0;
   do {
        if (lines == line_num) {
            break;
        }
        lines++;
    }  while (fgets(line, sizeof(line), stream));
}

static uint32_t get_total_lines(void) {
    char line[256];
    uint32_t total_lines = 0;
    FILE *file_pointer = fopen("file1.dat", "r");
    while (fgets(line, sizeof(line), file_pointer)) { //count number of lines in file
        total_lines++;
    }
    fclose(file_pointer);
    return total_lines;
}

struct timeval t_start, t_stop, t_result;

int main () {
    char line[256];
    int num_children;
    uint32_t total_lines = get_total_lines();
    printf("Number of lines: %d\n", total_lines);
    printf("Number of child processes: ");
    scanf("%i", &num_children);
    uint32_t division = total_lines / num_children;
    gettimeofday(&t_start, NULL);
    int fds[2];
    pipe(fds);
    for (int i=0; i < num_children; i++) {
        if (fork() == 0) {
            FILE *file_pointer = fopen("file1.dat", "r");
            uint32_t line_index = division * i;
            get_to_line(line_index, file_pointer); //get to specific line
            unsigned int result = 0;
            uint32_t end = line_index + division;

            do { //iterate through section of file and sum up the values
                unsigned int temp;
                fscanf(file_pointer, "%i",&temp);
                result += temp;
                line_index++;
                
                if (line_index == end) {
                    break;
                }
            } while (fgets(line, sizeof(line), file_pointer));
            fclose(file_pointer);
            write(fds[1], &result, sizeof(unsigned int)); //write result to parent
            exit(0);
        }
    }

    uint32_t total_to_parent = 0;
    for (uint8_t i=0; i < num_children; i++) { //read results from children (order doesn't matter)
        uint32_t temp;
        read(fds[0],&temp,sizeof(uint32_t));
        total_to_parent += temp;    //sum all values
    }
    gettimeofday(&t_stop, NULL);
    timersub(&t_stop, &t_start, &t_result);
    printf("Total Value: %i\n",total_to_parent);
    printf("Elapsed Time: %ld microseconds\n", (long int)t_result.tv_usec);
}

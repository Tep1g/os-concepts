#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE *file_pointer = fopen("data1.dat","r");
    char line[256];
    unsigned int total = 0;

    do {
        unsigned int temp;
        fscanf(file_pointer, "%i", &temp);
        total += temp;
        printf("%u %u\n",temp, total);
    } while (fgets(line, sizeof(line), file_pointer));

    fclose(file_pointer);
    printf("%i", total);
 }
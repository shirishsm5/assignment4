/**************************************************************
* Class::  CSC-415-0# Spring 2025
* Name::
* Student ID::
* GitHub-Name::
* Project:: Assignment 4 – Processing FLR Data with Threads
*
* File:: flr_processor.c
*
* Description::
*   Parses the header file to determine field widths and record length.
*   Computes the number of records in the binary file.
*   Prepares for multi-threaded processing.
*
**************************************************************/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

#define HEADER_FILE "header.txt"
#define DATA_FILE "datafile.bin"

// Structure to store field metadata
typedef struct {
    int field_width;
    char field_name[50];
} Field;

// Function to parse the header file and determine record length
int parse_header(Field **fields, int *record_length) {
    FILE *file = fopen(HEADER_FILE, "r");
    if (!file) {
        perror("Error opening header file");
        return -1;
    }

    char line[100];
    int num_fields = 0, total_length = 0;

    *fields = NULL; // Dynamic allocation

    while (fgets(line, sizeof(line), file)) {
        Field temp;
        sscanf(line, "%d: %49[^\n]", &temp.field_width, temp.field_name);
        total_length += temp.field_width;

        *fields = realloc(*fields, (num_fields + 1) * sizeof(Field));
        (*fields)[num_fields++] = temp;
    }

    fclose(file);
    *record_length = total_length;
    return num_fields;
}

// Function to determine file size
long get_file_size(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Error opening data file");
        return -1;
    }
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fclose(file);
    return size;
}

int main(int argc, char *argv[]) {
    // ***TO DO***  Look at arguments, initialize application

    //**************************************************************
    // DO NOT CHANGE THIS BLOCK
    //Time stamp start
    struct timespec startTime;
    struct timespec endTime;

    clock_gettime(CLOCK_REALTIME, &startTime);
    //**************************************************************

    // *** TO DO *** Start your thread processing
    //              Wait for the threads to finish

    Field *fields;
    int record_length;
    int num_fields = parse_header(&fields, &record_length);

    if (num_fields < 0) {
        return EXIT_FAILURE;
    }

    long file_size = get_file_size(DATA_FILE);
    if (file_size < 0) {
        free(fields);
        return EXIT_FAILURE;
    }

    int num_records = file_size / record_length;

    // ***TO DO *** Display Data
    printf("Record Length: %d bytes\n", record_length);
    printf("Total Records: %d\n", num_records);

    //**************************************************************
    // DO NOT CHANGE THIS BLOCK
    //Clock output
    clock_gettime(CLOCK_REALTIME, &endTime);
    time_t sec = endTime.tv_sec - startTime.tv_sec;
    long n_sec = endTime.tv_nsec - startTime.tv_nsec;
    if (endTime.tv_nsec < startTime.tv_nsec) {
        --sec;
        n_sec = n_sec + 1000000000L;
    }

    printf("Total Time was %ld.%09ld seconds\n", sec, n_sec);
    //**************************************************************

    // ***TO DO *** Cleanup
    free(fields);

    return EXIT_SUCCESS;
}

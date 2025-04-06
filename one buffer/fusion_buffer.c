#include <stdio.h>
#include <stdlib.h>

// Constants
#define N 372
#define M 496
#define BUFFER_ROWS 32 // Rows in the buffer

// File names
#define INPUT_FILENAME "cherry_496x372_444.yuv"
#define OUTPUT_FILENAME "filtered_output.yuv"

// Global declarations for compatibility with C89
int current_y[N][M];
double max(double a, double b);
int access_count = 0;
int buffer_access_count = 0;
int access_z1 = 0, access_z2 = 0, access_z3 = 0, access_z4 = 0;

#pragma arm section zidata="cache"
int buffer[BUFFER_ROWS][M];
#pragma arm section

// Max function for C89 compatibility
double max(double a, double b) {
    return (a > b) ? a : b;
}

// Read and filter YUV file with a buffer
void read_and_filter_yuv_file_with_buffer(int input[N][M], double output[N][M]) {
    int i, j, a, b, c;
    double z1, z2, z3, z4, max_median;

    FILE *file = fopen(INPUT_FILENAME, "rb");
    if (!file) {
        printf("Error: Could not open input file.\n");
        exit(1);
    }

    // Initialize the buffer
    for (i = 0; i < BUFFER_ROWS && i < N; i++) {
        for (j = 0; j < M; j++) {
            buffer[i][j] = fgetc(file);
            access_count++;
        }
    }

    for (i = 0; i < N; i++) {
        if (i + BUFFER_ROWS < N) {
            // Load the next row into the buffer
            for (j = 0; j < M; j++) {
                buffer[(i + BUFFER_ROWS) % BUFFER_ROWS][j] = fgetc(file);
                access_count++;
            }
        }

        for (j = 0; j < M; j++) {
            // Apply the filter within the buffer
            a = buffer[(i - 1 < 0 ? 0 : (i - 1) % BUFFER_ROWS)][j];
            b = buffer[i % BUFFER_ROWS][j];
            c = buffer[(i + 1 >= N ? (N - 1) % BUFFER_ROWS : (i + 1) % BUFFER_ROWS)][j];
            buffer_access_count += 3;
            z1 = (a + b + c) / 3.0;

            a = buffer[i % BUFFER_ROWS][j - 1 < 0 ? 0 : j - 1];
            b = buffer[i % BUFFER_ROWS][j];
            c = buffer[i % BUFFER_ROWS][j + 1 >= M ? M - 1 : j + 1];
            buffer_access_count += 3;
            z2 = (a + b + c) / 3.0;

            a = buffer[(i + 1 >= N ? (N - 1) % BUFFER_ROWS : (i + 1) % BUFFER_ROWS)][j - 1 < 0 ? 0 : j - 1];
            b = buffer[i % BUFFER_ROWS][j];
            c = buffer[(i - 1 < 0 ? 0 : (i - 1) % BUFFER_ROWS)][j + 1 >= M ? M - 1 : j + 1];
            buffer_access_count += 3;
            z3 = (a + b + c) / 3.0;

            a = buffer[(i - 1 < 0 ? 0 : (i - 1) % BUFFER_ROWS)][j - 1 < 0 ? 0 : j - 1];
            b = buffer[i % BUFFER_ROWS][j];
            c = buffer[(i + 1 >= N ? (N - 1) % BUFFER_ROWS : (i + 1) % BUFFER_ROWS)][j + 1 >= M ? M - 1 : j + 1];
            buffer_access_count += 3;
            z4 = (a + b + c) / 3.0;

            max_median = max(max(z1, z2), max(z3, z4));
            output[i][j] = max_median;
        }
    }

    fclose(file);
}

// Write filtered YUV file
void write_yuv_file(int input[N][M], double filtered_y[N][M]) {
    int i, j;
    FILE *file = fopen(OUTPUT_FILENAME, "wb");
    if (!file) {
        printf("Error: Could not open output file.\n");
        exit(1);
    }

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            fputc((int)filtered_y[i][j], file);
        }
    }

    fclose(file);
}

// Main function
int main() {
    double filtered_y[N][M];

    // Read and filter input YUV file with a buffer
    read_and_filter_yuv_file_with_buffer(current_y, filtered_y);

    // Write the filtered output YUV file
    write_yuv_file(current_y, filtered_y);

    printf("Number of accesses to current_y array: %d\n", access_count);
    printf("Number of accesses to buffer: %d\n", buffer_access_count);
    printf("Filtered image saved to: %s\n", OUTPUT_FILENAME);
    return 0;
}


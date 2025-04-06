#include <stdio.h>
#include <stdlib.h>

// Constants
#define N 372
#define M 496
#define BLOCK_ROWS 32 // Size of the first buffer
#define SMALL_BLOCK_SIZE 3 // Size of the second buffer

// File names
#define INPUT_FILENAME "cherry_496x372_444.yuv"
#define OUTPUT_FILENAME "filtered_output.yuv"

// Global declarations
int current_y[N][M];

// Buffers
#pragma arm section zidata="L2cache"
int buffer[BLOCK_ROWS][M];
#pragma arm section

#pragma arm section zidata="L1cache"
int small_buffer[SMALL_BLOCK_SIZE][SMALL_BLOCK_SIZE];
#pragma arm section

// Counters gia tis prospelaseis
unsigned long current_y_accesses = 0;
unsigned long buffer_accesses = 0;
unsigned long small_buffer_accesses = 0;


double max(double a, double b);
void read_and_filter_yuv_file(int input[N][M], double output[N][M]);
void write_yuv_file(double filtered_y[N][M]);

// H C89 den yposthrizei th synarthsh fmax
double max(double a, double b) {
    return (a > b) ? a : b;
}

// synarthsh anagnoshs tou YUV arxeiou me xrhsh buffers
void read_and_filter_yuv_file(int input[N][M], double output[N][M]) {
    int i, j, x;
    double max_median;

    // Variables for filtering
    int dx, dy, nx, ny;
    double z1, z2, z3, z4;

    FILE *file = fopen(INPUT_FILENAME, "rb");
    if (!file) {
        printf("Error: Could not open input file.\n");
        exit(1);
    }

    // anagnosh tou pinaka BLOCK_ROWS 
    for (i = 0; i < N; i += BLOCK_ROWS) {
        int rows_to_read = (i + BLOCK_ROWS > N) ? (N - i) : BLOCK_ROWS;

        // Fortwsh twn grammwn tou BLOCK_ROWS ston buffer
        for (x = 0; x < rows_to_read; x++) {
            for (j = 0; j < M; j++) {
                buffer[x][j] = fgetc(file);
                current_y_accesses++; 
            }
        }

        // epeksergasia kathe grammhs tou buffer
        for (x = 0; x < rows_to_read; x++) {
            for (j = 0; j < M; j++) {

                // efarmogi filtrou ston mikro buffer + dynamiki prosthiki padding opou xreiazetai
                for (dx = -1; dx <= 1; dx++) {
                    for (dy = -1; dy <= 1; dy++) {
                        nx = x + dx;
                        ny = j + dy;

                        small_buffer[dx + 1][dy + 1] = (nx < 0 || nx >= rows_to_read || ny < 0 || ny >= M) ? 0 : buffer[nx][ny];
                        buffer_accesses++; 
                        small_buffer_accesses++;
                    }
                }

                // Ypologismos z1, z2, z3, z4 vasei tou mikrou buffer
                z1 = (small_buffer[0][1] + small_buffer[1][1] + small_buffer[2][1]) / 3.0;
                z2 = (small_buffer[1][0] + small_buffer[1][1] + small_buffer[1][2]) / 3.0;
                z3 = (small_buffer[2][0] + small_buffer[1][1] + small_buffer[0][2]) / 3.0;
                z4 = (small_buffer[0][0] + small_buffer[1][1] + small_buffer[2][2]) / 3.0;

                small_buffer_accesses += 12; // syllogika accesses apo ton ypologismo twn dianysmatwn

                max_median = max(max(z1, z2), max(z3, z4));
                output[i + x][j] = max_median;    
            }
        }
    }

    fclose(file);
}

// Synarthsh eggrafhs arxeiou YUV me loop fusion
void write_yuv_file(double filtered_y[N][M]) {
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

    read_and_filter_yuv_file(current_y, filtered_y);

    write_yuv_file(filtered_y);

    // Ektypwsh twn counters
    printf("Array Access Statistics:\n");
    printf("current_y accesses: %lu\n", current_y_accesses);
    printf("buffer accesses: %lu\n", buffer_accesses);
    printf("small_buffer accesses: %lu\n", small_buffer_accesses);

    printf("Filtered image saved to: %s\n", OUTPUT_FILENAME);
    return 0;
}


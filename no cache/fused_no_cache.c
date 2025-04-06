#include <stdio.h>
#include <stdlib.h>

// Constants
#define N 372
#define M 496

// File names
#define INPUT_FILENAME "cherry_496x372_444.yuv"
#define OUTPUT_FILENAME "filtered_output.yuv"

//Global dhlwseis logw C89
int current_y[N][M], current_u[N][M], current_v[N][M];
double max(double a, double b);
int access_count=0;
int access_z1 = 0, access_z2 = 0, access_z3 = 0, access_z4 = 0;
 
// H C89 den yposthrizei th synarthsh fmax
double max(double a, double b) {
    return (a > b) ? a : b;
}

// Synarthsh anagnwshs arxeiou YUV me loop fusion
void read_and_filter_yuv_file(int input[N][M], double output[N][M]) {
    int i, j, a, b, c;
    double z1, z2, z3, z4, max_median;

    FILE *file = fopen(INPUT_FILENAME, "rb");
    if (!file) {
        printf("Error: Could not open input file.\n");
        exit(1);
    }



    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            
            //edw to filtrarisma kai h anagnwsh ths eikonas ginontai sto idio loop
            input[i][j] = fgetc(file);

            a = input[i - 1 < 0 ? 0 : i - 1][j];
            b = input[i][j];
            c = input[i + 1 >= N ? N - 1 : i + 1][j];
            access_z1 += 3;
            z1 = (a + b + c) / 3.0;

            a = input[i][j - 1 < 0 ? 0 : j - 1];
            b = input[i][j];
            c = input[i][j + 1 >= M ? M - 1 : j + 1];
            access_z2 += 3;
            z2 = (a + b + c) / 3.0;

            a = input[i + 1 >= N ? N - 1 : i + 1][j - 1 < 0 ? 0 : j - 1];
            b = input[i][j];
            c = input[i - 1 < 0 ? 0 : i - 1][j + 1 >= M ? M - 1 : j + 1];
            access_z3 += 3;
            z3 = (a + b + c) / 3.0;

            a = input[i - 1 < 0 ? 0 : i - 1][j - 1 < 0 ? 0 : j - 1];
            b = input[i][j];
            c = input[i + 1 >= N ? N - 1 : i + 1][j + 1 >= M ? M - 1 : j + 1];
            access_z4 += 3;
			z4 = (a + b + c) / 3.0;

            max_median = max(max(z1, z2), max(z3, z4));
            output[i][j] = max_median;
        }
    }

    fclose(file);
    
    access_count = access_z1 + access_z2 + access_z3 + access_z4;
}

// Synarthsh eggrafhs arxeiou YUV me loop fusion
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

    // Read and filter input YUV file in a fused loop
    read_and_filter_yuv_file(current_y, filtered_y);

    // Write the filtered output YUV file
    write_yuv_file(current_y, filtered_y);
	
	printf("Number of accesses to current_y array: %d\n", access_count);
	 
    printf("Filtered image saved to: %s\n", OUTPUT_FILENAME);
    return 0;
}


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

/* Do not modify write_pgm() or read_pgm() */
int write_pgm(char *file, void *image, uint32_t x, uint32_t y)
{
  FILE *o;

  if (!(o = fopen(file, "w"))) {
    perror(file);

    return -1;
  }

  fprintf(o, "P5\n%u %u\n255\n", x, y);

  /* Assume input data is correctly formatted. *
   * There's no way to handle it, otherwise.   */

  if (fwrite(image, 1, x * y, o) != (x * y)) {
    perror("fwrite");
    fclose(o);

    return -1;
  }

  fclose(o);

  return 0;
}

/* A better implementation of this function would read the image dimensions *
 * from the input and allocate the storage, setting x and y so that the     *
 * user can determine the size of the file at runtime.  In order to         *
 * minimize complication, I've written this version to require the user to  *
 * know the size of the image in advance.                                   */
int read_pgm(char *file, void *image, uint32_t x, uint32_t y)
{
  FILE *f;
  char s[80];
  unsigned i, j;

  if (!(f = fopen(file, "r"))) {
    perror(file);

    return -1;
  }

  if (!fgets(s, 80, f) || strncmp(s, "P5", 2)) {
    fprintf(stderr, "Expected P6\n");

    return -1;
  }

  /* Eat comments */
  do {
    fgets(s, 80, f);
  } while (s[0] == '#');

  if (sscanf(s, "%u %u", &i, &j) != 2 || i != x || j != y) {
    fprintf(stderr, "Expected x and y dimensions %u %u\n", x, y);
    fclose(f);

    return -1;
  }

  /* Eat comments */
  do {
    fgets(s, 80, f);
  } while (s[0] == '#');

  if (strncmp(s, "255", 3)) {
    fprintf(stderr, "Expected 255\n");
    fclose(f);

    return -1;
  }

  if (fread(image, 1, x * y, f) != x * y) {
    perror("fread");
    fclose(f);

    return -1;
  }

  fclose(f);

  return 0;
}

int main(int argc, char *argv[])
{
  int8_t image[1024][1024];
  int8_t out[1024][1024];
  
  /* Example usage of PGM functions */
  /* This assumes that motorcycle.pgm is a pgm image of size 1024x1024 */
  read_pgm("motorcycle.pgm", image, 1024, 1024);

    int8_t sobel_x[3][3] = {
    {-1, 0, 1},
    {-2, 0, 2},
    {-1, 0, 1}
  };
  
  int8_t sobel_y[3][3] = {
    {-1, -2, -1},
    { 0,  0,  0},
    { 1,  2,  1}
  };
  
  int row, col, i, j;
  int acc_x, acc_y;

  /* Initialize the output to zero */
  memset(out, 0, sizeof(out));
  
  /* Apply Sobel filter */
  for (row = 1; row < 1023; ++row) {
    for (col = 1; col < 1023; ++col) {
      acc_x = 0;
      acc_y = 0;
      for (i = -1; i <= 1; ++i) {
        for (j = -1; j <= 1; ++j) {
          acc_x += image[row + i][col + j] * sobel_x[i + 1][j + 1];
          acc_y += image[row + i][col + j] * sobel_y[i + 1][j + 1];
        }
      }
      
     out[row][col] = sqrt(acc_x * acc_x + acc_y * acc_y);
      
    }
  }
  
  /* After processing the image and storing your output in "out", write *
   * to motorcycle.edge.pgm.                                            */
  write_pgm("motorcycle.pgm", out, 1024, 1024);
  
  return 0;
}

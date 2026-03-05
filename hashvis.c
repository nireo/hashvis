#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  uint32_t state;
} rng_t;

static uint32_t rng_next(rng_t *rng) {
  uint32_t x = rng->state;
  x ^= x << 13;
  x ^= x >> 17;
  x ^= x << 5;
  if (x == 0) {
    x = 0x6D2B79F5u;
  }
  rng->state = x;
  return x;
}

static int seed_from_file(const char *keyfile_path, rng_t *rng) {
  FILE *file = fopen(keyfile_path, "rb");
  int c;
  size_t bytes_read = 0;
  uint32_t hash = 2166136261u;

  if (file == NULL) {
    fprintf(stderr, "Error: Cannot open keyfile '%s': %s\n", keyfile_path,
            strerror(errno));
    return 1;
  }

  while ((c = fgetc(file)) != EOF) {
    hash ^= (uint8_t)c;
    hash *= 16777619u;
    bytes_read++;
  }

  if (ferror(file)) {
    fprintf(stderr, "Error: Could not read keyfile '%s'\n", keyfile_path);
    fclose(file);
    return 1;
  }

  fclose(file);

  if (bytes_read == 0) {
    fprintf(stderr, "Error: Keyfile is empty: %s\n", keyfile_path);
    return 1;
  }

  if (hash == 0) {
    hash = 0x9E3779B9u;
  }

  rng->state = hash;
  return 0;
}

static void generate_art_grid(int width, int height, rng_t *rng, int *grid) {
  int current_x = width / 2;
  int current_y = height / 2;
  int num_steps = (width * height) / 3;
  int step;

  memset(grid, 0, (size_t)(width * height) * sizeof(*grid));

  for (step = 0; step < num_steps; ++step) {
    int random_choice;
    int dx;
    int dy;
    int next_x;
    int next_y;

    if (current_y >= 0 && current_y < height && current_x >= 0 &&
        current_x < width) {
      grid[current_y * width + current_x]++;
    }

    random_choice = (int)(rng_next(rng) & 3u);
    dx = (random_choice & 1) ? 1 : -1;
    dy = (random_choice & 2) ? 1 : -1;

    next_x = current_x + dx;
    next_y = current_y + dy;

    if (next_x < 0 || next_x >= width) {
      dx *= -1;
    }
    if (next_y < 0 || next_y >= height) {
      dy *= -1;
    }

    current_x += dx;
    current_y += dy;

    if (current_x < 0) {
      current_x = 0;
    } else if (current_x >= width) {
      current_x = width - 1;
    }

    if (current_y < 0) {
      current_y = 0;
    } else if (current_y >= height) {
      current_y = height - 1;
    }
  }
}

static char map_count_to_ascii(int count) {
  static const char characters[] = ".o+=*BOX@%&#/^";
  int num_char_levels = (int)(sizeof(characters) - 1);

  if (count <= 0) {
    return ' ';
  }
  if (count > num_char_levels) {
    return characters[num_char_levels - 1];
  }
  return characters[count - 1];
}

int main(int argc, char *argv[]) {
  const int ascii_width = 80;
  const int ascii_height = 24;
  int *art_grid;
  rng_t rng;
  int i;
  int j;

  if (argc < 2) {
    fprintf(stderr, "Usage: %s <path_to_keyfile>\n", argv[0]);
    return 1;
  }

  if (seed_from_file(argv[1], &rng) != 0) {
    return 1;
  }

  art_grid = (int *)calloc((size_t)(ascii_width * ascii_height), sizeof(*art_grid));
  if (art_grid == NULL) {
    fprintf(stderr, "Error: Memory allocation failed\n");
    return 1;
  }

  generate_art_grid(ascii_width, ascii_height, &rng, art_grid);

  putchar('+');
  for (i = 0; i < ascii_width; ++i) {
    putchar('-');
  }
  printf("+\n");

  for (j = 0; j < ascii_height; ++j) {
    putchar('|');
    for (i = 0; i < ascii_width; ++i) {
      putchar(map_count_to_ascii(art_grid[j * ascii_width + i]));
    }
    printf("|\n");
  }

  putchar('+');
  for (i = 0; i < ascii_width; ++i) {
    putchar('-');
  }
  putchar('+');
  putchar('\n');

  free(art_grid);
  return 0;
}

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

std::mt19937 seed_from_file_modern(const std::filesystem::path &keyfilePath) {
  std::ifstream file(keyfilePath, std::ios::binary | std::ios::ate);
  if (!file.is_open()) {
    throw std::runtime_error("Cannot open keyfile: " + keyfilePath.string());
  }
  std::streamsize size = file.tellg();
  file.seekg(0, std::ios::beg);
  std::vector<char> buffer(static_cast<size_t>(size));
  if (size > 0) {
    if (!file.read(buffer.data(), size)) {
      throw std::runtime_error("Could not read keyfile: " +
                               keyfilePath.string());
    }
  }
  if (buffer.empty()) {
    throw std::runtime_error("Keyfile is empty: " + keyfilePath.string());
  }
  std::hash<std::string_view> hasher;
  std::size_t hashed_content =
      hasher(std::string_view(buffer.data(), buffer.size()));
  return std::mt19937(static_cast<std::uint32_t>(hashed_content));
}

std::vector<std::vector<int>> generate_art_grid(int width, int height,
                                                std::mt19937 &rng) {
  std::vector<std::vector<int>> grid(height, std::vector<int>(width, 0));

  int current_x = width / 2;
  int current_y = height / 2;

  int num_steps = (width * height) / 3;

  for (int step = 0; step < num_steps; ++step) {
    if (current_y >= 0 && current_y < height && current_x >= 0 &&
        current_x < width) {
      grid[current_y][current_x]++;
    }

    int random_choice = rng() % 4;
    int dx = (random_choice & 1) ? 1 : -1;
    int dy = (random_choice & 2) ? 1 : -1;

    int next_x = current_x + dx;
    int next_y = current_y + dy;

    if (next_x < 0 || next_x >= width) {
      dx *= -1;
    }
    if (next_y < 0 || next_y >= height) {
      dy *= -1;
    }

    current_x += dx;
    current_y += dy;

    current_x = std::max(0, std::min(width - 1, current_x));
    current_y = std::max(0, std::min(height - 1, current_y));
  }
  return grid;
}

char map_count_to_ascii(int count) {
  const char characters[] = ".o+=*BOX@%&#/^";
  const int num_char_levels = sizeof(characters) - 1;

  if (count <= 0) {
    return ' ';
  }
  if (count > num_char_levels) {
    return characters[num_char_levels - 1];
  }
  return characters[count - 1];
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <path_to_keyfile>" << std::endl;
    return 1;
  }

  std::filesystem::path privateKeyPath = argv[1];

  if (!std::filesystem::exists(privateKeyPath)) {
    std::cerr << "Error: Keyfile not found at " << privateKeyPath.string()
              << std::endl;
    return 1;
  }

  try {
    std::mt19937 rng = seed_from_file_modern(privateKeyPath);

    const int ascii_width = 80;
    const int ascii_height = 24;

    std::vector<std::vector<int>> art_grid =
        generate_art_grid(ascii_width, ascii_height, rng);

    std::cout << '+';
    for (int i = 0; i < ascii_width; ++i) {
      std::cout << '-';
    }
    std::cout << "+\n";

    for (int j = 0; j < ascii_height; ++j) {
      std::cout << '|';
      for (int i = 0; i < ascii_width; ++i) {
        std::cout << map_count_to_ascii(art_grid[j][i]);
      }
      std::cout << '|';
      std::cout << '\n';
    }

    std::cout << '+';
    for (int i = 0; i < ascii_width; ++i) {
      std::cout << '-';
    }
    std::cout << '+';
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }
  return 0;
}

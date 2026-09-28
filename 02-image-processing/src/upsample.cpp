#include <iostream>
#include <omp.h>

#include "image.h"

int main(int argc, char **argv) {
  if (argc != 4) {
    std::cerr << "Usage: " << argv[0] << " <input> <output> <scale_factor>"
              << std::endl;
    return 1;
  }

  int scale = atoi(argv[3]);
  Image img(argv[1]);
  // TODO: nearest-neighbor sampling using OpenMP

  return 0;
}

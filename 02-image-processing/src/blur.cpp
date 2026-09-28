#include <iostream>
#include <omp.h>

#include "image.h"

int main(int argc, char **argv) {
  if (argc != 5) {
    std::cerr << "Usage: " << argv[0]
              << " <input> <output> <num_threads> <blur_size>" << std::endl;
    return 1;
  }

  int num_threads = atoi(argv[3]);
  int blur_size = atoi(argv[4]);
  Image img(argv[1]);

  omp_set_num_threads(num_threads);

  // TODO: blur the image using a blur_size x blur_size filter (using OpenMP)

  return 0;
}

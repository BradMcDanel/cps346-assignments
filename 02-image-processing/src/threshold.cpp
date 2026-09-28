#include <iostream>
#include <omp.h>

#include "image.h"

int main(int argc, char **argv) {
  if (argc != 3) {
    std::cerr << "Usage: " << argv[0] << " <input> <output>" << std::endl;
    return 1;
  }

  Image img(argv[1]);          // This reads the image into the Image object
  Image out_img(img.w, img.h); // Creates an output image (img.w x img.h)

  // TODO: find the threshold (average of the image using OpenMP)

  // TODO: threshold the image (using OpenMP)

  out_img.write(argv[2]); // This writes the output image to a file

  return 0;
}

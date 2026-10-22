#include <cstdio>
#include <cstdlib>

#include "sound.h"

void update(Strip &s) {
  // TODO: implement
  // remove the following line (this prevents a compiler warning)
  (void)s;
  // end of TODO
}

int main(int argc, char **argv) {
  if (argc != 4) {
    fprintf(stderr, "Usage: %s <sources> <output_dir> <frame_every>\n", argv[0]);
    return 1;
  }

  std::vector<Source> sources = read_sources(argv[1]);
  Output output(argv[2], argv[1]);
  int frame_every = atoi(argv[3]);

  // One strip that holds the whole grid.
  Strip s(0, HEIGHT);

  for (int step = 1; step <= STEPS; step++) {
    update(s);
    add_sources(s, sources, step);
    finish_step(s);

    if (frame_every > 0 && step % frame_every == 0) {
      output.save_frame(step, &s.loud[WIDTH]);
    }
  }
  return 0;
}

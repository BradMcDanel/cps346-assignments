#include <mpi.h>

#include <algorithm>
#include <cstdio>
#include <cstdlib>

#include "sound.h"

void update(Strip &s) {
  // TODO: copy your update function from serial.cpp
  // remove the following line (this prevents a compiler warning)
  (void)s;
  // end of TODO
}

void split_rows(int rank, int size, int &first_row, int &rows) {
  // TODO: implement
  // remove the following lines (these prevent a compiler warning)
  (void)rank;
  (void)size;
  first_row = 0;
  rows = HEIGHT;
  // end of TODO
}

void exchange_halos(Strip &s, int rank, int size) {
  // TODO: implement
  // remove the following lines (these prevent a compiler warning)
  (void)s;
  (void)rank;
  (void)size;
  // end of TODO
}

void gather_loudness(Strip &s, int rank, int size, std::vector<double> &grid) {
  // TODO: implement
  // remove the following lines (these prevent a compiler warning)
  (void)s;
  (void)rank;
  (void)size;
  (void)grid;
  // end of TODO
}

int main(int argc, char **argv) {
  MPI_Init(&argc, &argv);
  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  if (argc != 4) {
    if (rank == 0)
      fprintf(stderr, "Usage: mpirun -np <ranks> %s <sources> <output_dir> <frame_every>\n",
              argv[0]);
    MPI_Finalize();
    return 1;
  }

  if (size > HEIGHT) {
    if (rank == 0) fprintf(stderr, "Use at most %d ranks (one row per rank)\n", HEIGHT);
    MPI_Finalize();
    return 1;
  }

  std::vector<Source> sources = read_sources(argv[1]);
  Output output(argv[2], argv[1]);  // only rank 0 should save pictures
  int frame_every = atoi(argv[3]);

  // Each rank only holds its own strip of the grid.
  int first_row, rows;
  split_rows(rank, size, first_row, rows);
  Strip s(first_row, rows);

  // Rank 0 puts the whole grid together here when saving a picture.
  std::vector<double> grid;
  if (rank == 0) grid.resize(WIDTH * HEIGHT);

  for (int step = 1; step <= STEPS; step++) {
    update(s);
    add_sources(s, sources, step);
    finish_step(s);
  }

  // TODO: add the halo exchange, saving pictures (every frame_every steps), and
  // timing to the loop above (see the assignment)
  // remove the following line (this prevents a compiler warning)
  (void)frame_every;
  // end of TODO

  MPI_Finalize();
  return 0;
}

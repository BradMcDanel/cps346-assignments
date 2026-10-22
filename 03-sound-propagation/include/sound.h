#ifndef _SOUND_H
#define _SOUND_H

#include <fstream>
#include <string>
#include <vector>

// Size of the Stager basement grid (each cell is 5 cm x 5 cm).
const int WIDTH = 1024;
const int HEIGHT = 800;

// Time step and number of steps (1 second of sound).
const double DT = 0.5 * 0.05 / 343.0;
const int STEPS = 13720;

// Constants used by the update (see the assignment).
const double SPREAD = 0.25;    // how fast pressure spreads to neighbors
const double DAMP = 2.0 * DT;  // how fast sound fades

// A horizontal strip of the grid that holds rows first_row to
// first_row + rows - 1. Row r of the strip is stored at index r * WIDTH, where
// rows 1 to rows are the rows this strip owns. Row 0 and row rows + 1 are halo
// rows that hold copies of the rows just above and below the strip.
struct Strip {
  int first_row;
  int rows;
  std::vector<unsigned char> wall; // 1 for a wall, 0 for air
  std::vector<double> prev;        // pressure at step n - 1
  std::vector<double> cur;         // pressure at step n
  std::vector<double> next;        // pressure at step n + 1
  std::vector<double> loud;        // loudness of each cell

  // Reads the rows of this strip (and its halo rows) from the floor plan.
  Strip(int first_row, int rows);
};

struct Source {
  std::string kind;  // pulse, tone, or chord
  int x, y;
  double amplitude, frequency, start, duration, radius;
};

std::vector<Source> read_sources(const std::string &path);

// Adds the sound from each source to s.next.
void add_sources(Strip &s, const std::vector<Source> &sources, int step);

// Updates s.loud from s.next. Then moves to the next step
// (prev <- cur, cur <- next).
void finish_step(Strip &s);

// Saves a picture of the loudness (frame_NNNNN.png) and the loudness at each
// microphone (probes.csv) to a folder. In the MPI version, only rank 0 should
// call save_frame.
class Output {
 public:
  Output(const std::string &dir, const std::string &sources_path);
  // loud holds all WIDTH * HEIGHT cells (no halo rows).
  void save_frame(int step, const double *loud);

 private:
  std::string dir, sources_path;
  std::vector<unsigned char> mask;
  std::vector<int> source_cells, probe_cells;
  std::ofstream probes;
};

#endif  // _SOUND_H

// Helper code for the sound simulation. You do not need to change this file.

#include "sound.h"

#include <sys/stat.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sstream>

const double PI = 3.14159265358979323846;

static void fail(const std::string &message) {
  std::cerr << "error: " << message << std::endl;
  std::exit(1);
}

// Reads a CSV file with a header line into rows of strings.
static std::vector<std::vector<std::string> > read_csv(const std::string &path) {
  std::ifstream in(path);
  if (!in) fail("cannot read " + path);
  std::vector<std::vector<std::string> > rows;
  std::string line;
  std::getline(in, line);  // header
  while (std::getline(in, line)) {
    if (line.empty()) continue;
    std::vector<std::string> fields;
    std::stringstream ss(line);
    std::string field;
    while (std::getline(ss, field, ',')) fields.push_back(field);
    rows.push_back(fields);
  }
  return rows;
}

// Reads the floor plan: 1 for air, 0 for a wall.
static std::vector<unsigned char> read_mask() {
  std::string path = std::string(DATA_DIR) + "/stager-basement-mask.pgm";
  std::ifstream in(path, std::ios::binary);
  std::string magic;
  int w, h, max_value;
  in >> magic >> w >> h >> max_value;
  in.get();
  if (!in || magic != "P5" || w != WIDTH || h != HEIGHT) fail("cannot read " + path);
  std::vector<unsigned char> mask(WIDTH * HEIGHT);
  in.read((char *)mask.data(), mask.size());
  if (!in) fail("cannot read " + path);
  for (size_t i = 0; i < mask.size(); i++) mask[i] = mask[i] ? 1 : 0;
  return mask;
}

Strip::Strip(int first_row, int rows) : first_row(first_row), rows(rows) {
  int cells = (rows + 2) * WIDTH;
  wall.assign(cells, 1);
  prev.assign(cells, 0.0);
  cur.assign(cells, 0.0);
  next.assign(cells, 0.0);
  loud.assign(cells, 0.0);

  // Copy rows first_row - 1 to first_row + rows (halo rows included). Rows
  // outside the grid stay as walls.
  std::vector<unsigned char> mask = read_mask();
  for (int r = 0; r < rows + 2; r++) {
    int y = first_row + r - 1;
    if (y < 0 || y >= HEIGHT) continue;
    for (int x = 0; x < WIDTH; x++) wall[r * WIDTH + x] = mask[y * WIDTH + x] ? 0 : 1;
  }
}

std::vector<Source> read_sources(const std::string &path) {
  std::vector<Source> sources;
  for (const auto &f : read_csv(path)) {
    if (f.size() != 9) fail("each line of " + path + " needs 9 values");
    Source s;
    s.x = std::atoi(f[1].c_str());
    s.y = std::atoi(f[2].c_str());
    s.kind = f[3];
    s.amplitude = std::atof(f[4].c_str());
    s.frequency = std::atof(f[5].c_str());
    s.start = std::atof(f[6].c_str());
    s.duration = std::atof(f[7].c_str());
    s.radius = std::atof(f[8].c_str());
    sources.push_back(s);
  }
  return sources;
}

// The sound a source makes at a given time (between -1 and 1).
static double signal(const Source &s, double time) {
  double t = time - s.start;
  if (t < 0 || t >= s.duration) return 0.0;
  if (s.kind == "pulse") {
    double q = PI * s.frequency * (t - 1.5 / s.frequency);
    return (1.0 - 2.0 * q * q) * std::exp(-q * q);
  }
  // Fade tones in and out so they do not click.
  double ramp = std::min(0.1 * s.duration, 2.0 / s.frequency);
  double edge = std::min(t, s.duration - t);
  double fade = edge < ramp ? 0.5 - 0.5 * std::cos(PI * edge / ramp) : 1.0;
  double phase = 2.0 * PI * s.frequency * t;
  if (s.kind == "chord")
    return fade * (std::sin(phase) + 0.5 * std::sin(1.25 * phase) +
                   0.3 * std::sin(1.5 * phase)) / 1.8;
  return fade * std::sin(phase);
}

void add_sources(Strip &s, const std::vector<Source> &sources, int step) {
  for (const Source &src : sources) {
    double value = DT * DT * src.amplitude * signal(src, (step - 1) * DT) / (1.0 + DAMP);
    if (value == 0.0) continue;
    int reach = (int)std::ceil(src.radius);
    for (int y = src.y - reach; y <= src.y + reach; y++) {
      if (y < s.first_row || y >= s.first_row + s.rows) continue;
      for (int x = src.x - reach; x <= src.x + reach; x++) {
        double d2 = (x - src.x) * (x - src.x) + (y - src.y) * (y - src.y);
        if (d2 > src.radius * src.radius) continue;
        int i = (y - s.first_row + 1) * WIDTH + x;
        if (s.wall[i]) continue;
        double weight = src.radius > 0 ? std::exp(-2.0 * d2 / (src.radius * src.radius)) : 1.0;
        s.next[i] += weight * value;
      }
    }
  }
}

void finish_step(Strip &s) {
  // Loudness is a running average of pressure squared over about 8 ms.
  const double alpha = -std::expm1(-DT / 0.008);
  for (int i = WIDTH; i < (s.rows + 1) * WIDTH; i++)
    s.loud[i] += alpha * (s.next[i] * s.next[i] - s.loud[i]);
  s.prev.swap(s.cur);
  s.cur.swap(s.next);
}

// Creates a folder and any missing parent folders (like mkdir -p).
static void make_dir(const std::string &path) {
  for (size_t i = 1; i <= path.size(); i++)
    if (i == path.size() || path[i] == '/') mkdir(path.substr(0, i).c_str(), 0755);
}

Output::Output(const std::string &dir, const std::string &sources_path)
    : dir(dir), sources_path(sources_path) {}

// Color for a loudness value: dark for quiet, bright yellow for loud.
static void color(double loud, unsigned char *rgb) {
  static const double stops[8][3] = {{9, 12, 35},    {43, 24, 83},   {99, 35, 111},
                                     {166, 47, 95},  {220, 80, 57},  {249, 142, 30},
                                     {249, 205, 74}, {253, 248, 192}};
  double db = loud > 0 ? 10.0 * std::log10(loud / 1e-4) : -1000.0;  // vs. 0.01
  double t = std::min(1.0, std::max(0.0, (db + 80.0) / 100.0)) * 7.0;
  int j = std::min((int)t, 6);
  for (int c = 0; c < 3; c++)
    rgb[c] = (unsigned char)(stops[j][c] + (stops[j + 1][c] - stops[j][c]) * (t - j) + 0.5);
}

// Fills a box of cells around a center cell (half width w, half height h).
static void paint(std::vector<unsigned char> &image, int center, int w, int h,
                  unsigned char r, unsigned char g, unsigned char b) {
  int cx = center % WIDTH, cy = center / WIDTH;
  for (int y = std::max(0, cy - h); y <= std::min(HEIGHT - 1, cy + h); y++)
    for (int x = std::max(0, cx - w); x <= std::min(WIDTH - 1, cx + w); x++) {
      unsigned char *rgb = &image[3 * (y * WIDTH + x)];
      rgb[0] = r, rgb[1] = g, rgb[2] = b;
    }
}

void Output::save_frame(int step, const double *loud) {
  if (!probes.is_open()) {
    make_dir(dir);
    mask = read_mask();
    for (const auto &f : read_csv(sources_path))
      source_cells.push_back(std::atoi(f[2].c_str()) * WIDTH + std::atoi(f[1].c_str()));
    probes.open(dir + "/probes.csv");
    if (!probes) fail("cannot write to " + dir);
    probes << "time";
    for (const auto &f : read_csv(std::string(DATA_DIR) + "/probes.csv")) {
      probes << ',' << f[0];
      probe_cells.push_back(std::atoi(f[2].c_str()) * WIDTH + std::atoi(f[1].c_str()));
    }
    probes << '\n';
  }

  // Loudness at each microphone (probe).
  double time = step * DT;
  probes << time;
  for (int cell : probe_cells) probes << ',' << std::sqrt(loud[cell]);
  probes << '\n';

  // Picture of the loudness with walls in gray.
  std::vector<unsigned char> image(WIDTH * HEIGHT * 3);
  for (int i = 0; i < WIDTH * HEIGHT; i++) {
    unsigned char *rgb = &image[3 * i];
    if (mask[i]) {
      color(loud[i], rgb);
    } else {
      rgb[0] = 216, rgb[1] = 221, rgb[2] = 228;
    }
  }
  // Sources are blue squares and microphones are white crosses. Both have a
  // dark outline so they stand out on loud and quiet cells.
  for (int cell : source_cells) {
    paint(image, cell, 9, 9, 25, 25, 25);
    paint(image, cell, 7, 7, 22, 135, 167);
  }
  for (int cell : probe_cells) {
    paint(image, cell, 12, 3, 25, 25, 25);
    paint(image, cell, 3, 12, 25, 25, 25);
    paint(image, cell, 11, 1, 255, 255, 255);
    paint(image, cell, 1, 11, 255, 255, 255);
  }

  char name[64];
  snprintf(name, sizeof(name), "/frame_%05d.png", step);
  stbi_write_png((dir + name).c_str(), WIDTH, HEIGHT, 3, image.data(), WIDTH * 3);
}

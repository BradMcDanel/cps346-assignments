## Data

* `stager-basement-mask.pgm` - the floor plan of the Stager Hall basement as a 1024 x 800 grid. Each cell is 5 cm across. White (255) cells are air and black (0) cells are walls.
* `sources-pulse.csv`, `sources-bass.csv`, `sources-duet.csv` - sound sources to try. Each line gives a name, a location (`x`, `y`), the kind of sound (`pulse`, `tone`, or `chord`), its amplitude, frequency (Hz), start time (s), duration (s), and radius (in cells).
* `probes.csv` - the microphone in STA029 (a classroom at the far end of the basement from STA002).

The floor plan was traced from the Stager Hall ground floor plan. Doors are left open. Furniture and stairs are ignored.

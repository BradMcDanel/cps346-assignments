## Sound Propagation with MPI

### Getting Started
In addition to CMake and C++11, you will also need to install MPI. This is already set up on the lab machines. If you want to install it locally, use one of the following (the same as in class):

Linux / WSL:
```
sudo apt update
sudo apt install openmpi-bin libopenmpi-dev
```

macOS:
```
brew install open-mpi
```

After installing these prerequisites, you should be able to compile the code provided in this repository. First, you need to clone this to your machine:

```
git clone git@github.com:BradMcDanel/cps346-assignments.git
cd cps346-assignments/03-sound-propagation/
```
Note: if you already cloned the repository for a previous assignment, you should be able to simply do `git pull` instead of the two above lines.

Now, we need to set up a `build/` directory for CMake:

```
mkdir build
cd build
```

Inside the build directory we can run CMake to generate a Makefile for the project.
```
cmake ..
```

Now, we can run make as usual to generate our executables (`serial` and `mpi` in this case). Afterwards, we can run both executables from inside the build directory.
```
make
./serial ../data/sources-pulse.csv ../output/serial 40
mpirun -np 4 ./mpi ../data/sources-pulse.csv ../output/mpi 40
```
The last argument saves a picture every 40 steps into the output folder (use 0 to save nothing). If `mpirun` says there are not enough slots, add `--oversubscribe`. Note that neither of these executables actually do very much - that is your job!

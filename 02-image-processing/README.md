## Image Processing

### Getting Started
In addition to CMake and C++11, you will also need to install OpenMP. This is already set up on the lab machines. If you want to install it locally, please look online and send me an email if you run into any issues!

After installing these prerequisites, you should be able to compile the code provided in this repositoy. First, you need to clone this to your machine:

```
git clone git@github.com:BradMcDanel/cps346-assignments.git
cd cps346-assignments/02-image-processing/
```
Note: if you already cloned the repository for the last assignment, you should be able to simply do `git pull` instead of the two above lines.

Now, we need to set up a `build/` directory for CMake:

```
mkdir build
cd build
```

Inside the build directory we can run CMake to generate a Makefile for the project.
```
cmake ..
```

Now, we can run make as usual to generate our executable (`serial` or `threaded` in this case). Afterwards, we can run both executables from inside the build directory.
```
make
./threshold ../data/mt-mitchell-nc.png ../data/threshold.png
./upsample ../data/mt-mitchell-nc.png ../data/upsample.png 4
./blur ../data/mt-mitchell-nc.png ../data/blur.png 4 15 
```
Note that none of these executables actually do very much - that is your job!

### Next Steps
* Look at examples in the cps346-lecture-code as a starting point.
* You can implement these algorithms serially and then add #pragma omp afterwards!!

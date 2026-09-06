## Writing a Web Crawler

### Getting Started
There are three requirements before getting started on this project: a C++11 compiler (likely the case for any computer < 5 years old), [CMake](https://cmake.org/download/) (3.10 or greater), and Python 3 (only to run the server). Follow the instructions on the CMake website to install it. Afterwards, when opening a new terminal, typing cmake should prompt you with a usage message.

Due to using the linux socket library, **this code will likely not compile on Windows.** Please use a lab machine, or a VM on your windows machine. You may also try to get [Windows Subsystem for Linux](https://docs.microsoft.com/en-us/windows/wsl/) working, but I make no guarantees it will work. The code should work fine on Mac OSX.

After installing these prerequisites, you should be able to compile the code provided in this repository. First, you need to clone this to your machine:

```
git clone git@github.com:BradMcDanel/cps346-assignments.git
cd cps346-assignments/01-webcrawler/
```

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
./serial -u articles/s/o/u/Sourdough.html -d 1 -o ../data/sourdough.txt
./threaded -u articles/s/o/u/Sourdough.html -d 1 -o ../data/sourdough.txt -t 4
```
Note that neither of these executables actually do very much - that is your job!

### Running the Site
The site you will be crawling is a snapshot of Wikipedia, about 3,100 pages. It is too big to keep in this repository, so download it once into `data/`:

```
cd cps346-assignments/01-webcrawler/
curl -L -o data/site.zip https://github.com/BradMcDanel/cps346-assignments/releases/download/site-v1/site.zip
```

That file is around 115 MB. Leave it zipped, the server reads pages straight out of the archive.

You serve the site yourself with `server.py`, which listens on `127.0.0.1:8080` and waits one second before answering each request. Open a second terminal, leave this running while you work, and stop it with ctrl-c when you are done.

```
python3 server.py
```

The one second delay is there on purpose and your reported timings should use it. While you are debugging you can speed things up with `python3 server.py --delay 0.1`, and `--port` will move it off 8080 if something else on your machine is already using that port. If you change the port, change it in `src/net.cpp` as well.

There is no home page, so `http://127.0.0.1:8080/` returns "Not Found". To look at the site you have to ask for a page by name. Any of these will get you started, and from there you can follow links like a normal web site:

```
http://127.0.0.1:8080/articles/s/o/u/Sourdough.html
http://127.0.0.1:8080/articles/c/h/i/Chicago.html
http://127.0.0.1:8080/articles/f/r/a/Franklin___Marshall_College.html
```

The pages are real Wikipedia articles with their markup intact, so the links are buried in the text the way they are on any web page. Links that leave the snapshot will 404, which is the same thing your crawler will see.

### Next Steps
* Click around the site to get a sense of what the content you will be crawling looks like
* Carefully read through the assignment (and ask me questions as they come up!)
* Get the serial version working 100% before starting the threaded version
* After finishing the threaded version, make sure that the output matches the serial version for several pages

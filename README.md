# Learning Tudat in C++

This repository contains some experiments with the C++ Tudat environment, in an attempt to get acquainted with the functionality and structure of the code base.


> :warning: **This repository has nothing to do with the official tudat team and is purely a personal project to gain experience.**

Below are some independent notes



## Important

* Don't forget to activate the correct Conda environment: `tudat-bundle`
* 

## Building projects

Tudat is built using `CMake`. It is not yet clear to me what the best way to build individual projects is, so for now I have used `make` instead.  Should look into how it is done in other projects but it is hard to find Tudat examples for C++.

to build perturbed-satellite-orbit, run the following

```bash
cd perturbed-satellite-orbit
conda activate tudat-bundle
make
```

The binary will be written to a subdirectory `build`.

### Binaries

I found that binaries for even very simple projects get very large. This is probably because of the many statically linked libraries that depend on each other. I have now basically included all the available *.a libraries in `$(TUDAT_BUNDLE)/build/lib` but I am not sure if this is the right thing to do. Maybe this would also be solved using Cmake

## Dependencies

* eigen3
* cspice
* SOFA
* tudat libraries

## Linking
The order of linking libraries is very imporant, and since all these libraries depend on one another, it took quite a while to find out an order where there wer no undefined references. Probably this is a problem that can be solved by CMake or better Makefile skills, but I have not figured it out yet.

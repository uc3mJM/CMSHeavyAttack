# Masquerating flows as Heavy Hitters in CMS

One Paragraph of project description goes here

## Description
The objetive of this implementation is to attack CMS in order to masquerate object as heavy hitters when they are not. In order to do this, a series of elements that overestimate the target element is found, adapting the inserts depending on the information obtained from the estimations.

Uses the Fingerprint Count Min Sketch (FCMS) implementation of Count-Min Sketch as a starting point  [https://github.com/alabid/countminsketch]( https://github.com/alabid/countminsketch)

## Compilation

This Visual Studio C++ project consists of 3 files: CMSF.hpp (declaration file), CMSF.cpp (source file) and main.cpp (test bench)

## Command line arguments

* w: table width, default value is w=1048576
* d: table depth or number of counters checked, default value is d=2
* b: fingerprint bits, default value is 0 for standard  Count-Min Sketch, when bits is not zero itindicates the bits used for the fingerprint related to the counter
* c: conservative update {0: normal update, 1: conservative update}, default value is c=0
* u: the fingerprint is used only if the counter is less than the given value, default value is u=0
* i: input item, default value is i="ASDFGHJKLZXCVBNM"
* n: number of runs, defaul value is n=1

```
example: cmsf.exe w=1048576 d=2, b=2, i=HOLAHOLAHOLA
```

## Output

The ouput will depend on the number of runs. in the case of having only one run, the number of operation is output along with the list of elements with which it is possible to carry out the attack. In the case of choosing more runs, the mean and standard deviation of the operations is shown.

With one run:

```
Number of updates needed -> 1704717
List of items found:
1. qBpM0xheA47ksEwu
2. CYHrCvexRDqDMutD

```

With more than one run:

```
Average of updates needed -> 145983
Deviation of updates needed -> 73192
```

## Authors

* **Josu Murua Salvador** -- [GitHub](https://github.com/uc3mJM)


## License

This project is licensed under the MIT Licens

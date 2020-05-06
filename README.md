# Masquerating flows as Heavy Hitters in Count-Min Sketch

## Description
The objective of this implementation is to attack CMS ([Count-Min Sketch](https://en.wikipedia.org/wiki/Count%E2%80%93min_sketch)) . The CMS is a data structures that is used to count the frequency of elements in a particular set,  widely used in networking and databases. This attack is based on masquerating objects like heavy hitters when they are not, a flow that surpasses a predefined threshold, therefore being considered a relevant element and disrupting it functionality of the network and potentially causing harm. In order to do this, a series of elements that overestimate the target element is found, adapting the inserts depending on the information obtained from the estimations.

This code is used in the master's degree final project "Masquerading Flows as Heavy Hitter in Count-Min Sketch" by Josu Murua Salvadore.

Uses the Fingerprint Count Min Sketch (FCMS) implementation of Count-Min Sketch as a starting point  [https://github.com/mladron/FCMS](https://github.com/mladron/FCMS)

## Compilation

This Visual Studio C++ project consists of 3 files: CMSF.hpp (declaration file), CMSF.cpp (source file) and main.cpp (test bench)

## Command line arguments

* w: table width, default value is w=1048576
* d: table depth or number of counters checked, default value is d=2
* b: fingerprint bits, default value is 0 for standard  Count-Min Sketch, when bits is not zero it indicates the bits used for the fingerprint related to the counter
* c: conservative update {0: normal update, 1: conservative update}, default value is c=0
* u: the fingerprint is used only if the counter is less than the given value, default value is u=0
* i: input item, default value is i="ASDFGHJKLZXCVBNM"
* n: number of runs, default value is n=1

```
example: CMSHeavyAttack.exe w=1048576 d=2, b=2, i=HOLAHOLAHOLA
```

## Output

The ouput will depend on the number of runs. in the case of having only one run, the number of operation is output along with the list of elements with which it is possible to carry out the attack. In the case of choosing more runs, the mean and standard deviation of the operations is shown.

With one run:

```
.\CMSHeavyAttack.exe n=1 w=4096 d=2
```

```
Heavy Hitter Masquerade attak to CMSF(Count-Min Sketch with Fingerprint)

Item                i = ASDFGHJKLZXCVBNM
Table width         w = 4096
Table depth         d = 2
Fingerprint bits    b = 0
Conservative update c = 0
Fingerprint limit   u = 0
Number of tries     n = 1

Number of updates needed -> 3090
List of items found:
1. sze1I0lJJzibyK8B
2. SLfMfcy0nwJv6lk0

```

With more than one run:

```
.\CMSHeavyAttack.exe n=10 w=4096 d=2
```

```
Heavy Hitter Masquerade attak to CMSF(Count-Min Sketch with Fingerprint)

Item                i = ASDFGHJKLZXCVBNM
Table width         w = 4096
Table depth         d = 2
Fingerprint bits    b = 0
Conservative update c = 0
Fingerprint limit   u = 0
Number of tries     n = 10

Average of updates needed -> 9480
Deviation of updates needed -> 2966
```

## Authors

* **Josu Murua Salvador** -- [GitHub](https://github.com/uc3mJM)


## License

This project is licensed under the MIT License

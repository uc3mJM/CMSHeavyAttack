# Masquerating flows as Heavy Hitters in Count-Min Sketch

## Description
The objective of this implementation is to attack CMS ([Count-Min Sketch](https://en.wikipedia.org/wiki/Count%E2%80%93min_sketch)). The CMS is a data structures that is used to count the frequency of elements in a particular set,  widely used in networking and databases. This attack is based on masquerating objects like heavy hitters when they are not, a flow that surpasses a predefined threshold, therefore being considered a relevant element and disrupting it functionality of the network and potentially causing harm. In order to do this, a series of elements that overestimate the target element is found, adapting the inserts depending on the information obtained from the estimations.

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
* i: input target item, default value is i="ASDFGHJKLZXCVBNM"
* n: number of runs, default value is n=1
* f: input keys

```
example: CMSHeavyAttack.exe w=1048576 d=2, b=2, i=HOLAHOLAHOLA
```

## Output

The ouput will depend on the number of runs. In the case of having only one run, the number of operation is output along with the list of elements with which it is possible to carry out the attack. In the case of choosing more runs, the mean and standard deviation of the operations is shown.

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

If the there is an input key, f="1000_ips.txt" for example, the CMS will be initialized with the flow in the file, an then te attack will be carried out to an random element in the file. In this case, two files are necessary to carry out the attack:
 * The one with all the element, each line corresponds to a element.
 * The one that has in each line the number of packets of the flows. This file ends in sorted _frequency.

## RedisBloom

To demonstrate that this attack is feasible in a real environment, it has been decided to implement a data base system that integrates a CMS, and for this a [Redis](https://redislabs.com/) database has been chosen. Redis is a database motor that relies on hash table storage, which additionally provides modules with various data types. One of these modules is [RedisBloom](https://oss.redislabs.com/redisbloom) which, among others, provides an implementation of Count-Min Sketch.This module provides an API with which you can perform operations such as initializing a Count-Min sketch to the dimensions specified by the user, increasing the object count, the return count for the items and more.

RedisBloomAttack.py has be implemented in Python 3 with [redisbloom-py](https://github.com/RedisBloom/redisbloom-py), a package that extens redis-py interface (Pyhton interface to Redis) with the RedisBloom API, in order to carry out the attack. This code is also provided.

## Authors

* **Josu Murua Salvador** -- [GitHub](https://github.com/uc3mJM)


## License

This project is licensed under the MIT License

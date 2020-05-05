//
// CMSF.hpp Count-Min Sketch with Fingerprints implemented by J. Martínez, this code used Daniel Alabi's Count-Min Sketch 
// as a starting point https://github.com/alabid/countminsketch 
//
// To improve accuracy of the counter, Count-Min Sketch stores a fingerprint of the last element x that has
// updated the counter, where fingerprint is calculated as f(x, count)
// 
// The counter of an element x is updated as follows:
//
// if the counter is 0, set the counter to 1
// else if the stored fingerprint matches f(x, count), add 1 to the counter else add 0.5 to the counter
// set the fingerprint to f(x, count)
//

//#include "pch.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include "CMSF.hpp"
#include <list>


/*
 * The class Counter is used in the update function when the conservative update is enabled
 */

class Counter {
public:

	int row;
	uint64_t column;
	short int fingerprint;
	float current;
	float updated;
};


/*
 * CMSF constructor initializes the count table and the array of hashes
*/

CMSF::CMSF(unsigned int width, short int depth, short int bits, short int conservative, short int fp_limit) {

	// width       : table width
	// depth       : table depth or number of counters checked
	// bits        : fingerprint bits, default value is 0 for standard Count-Min Sketch, when not zero it
	//               indicates the number of bits for the fingerprint used by Count-Min Sketch with Fingerprint 
	// conservative: conservative counter update
	// fp_limit    : fingerprint limit

	table_width = width;
	table_depth = depth;
	fingerprint_bits = bits;
	conservative_update = conservative;
	fingerprint_limit = fp_limit;
	total = 0.0;

	// initialize the counter table

	table = new KeyCounter * [table_depth];

	for (int i = 0; i < table_depth; i++)
		table[i] = new KeyCounter[table_width];

	for (int i = 0; i < table_depth; i++)
		for (int j = 0; j < table_width; j++) {
			table[i][j].count = 0.0;
			table[i][j].fingerprint = -1;
		}

	// initialize d pairwise independent hashes

	srand(time(NULL));
	hashes = new unsigned int* [table_depth];

	for (int i = 0; i < table_depth; i++) {
		hashes[i] = new unsigned int[2];
		genhash(hashes, i);
	}
}


/*
 * CMSF destructor deletes count table and the array of hashes
*/

CMSF::~CMSF() {
	for (int i = 0; i < table_depth; i++) {
		delete[] table[i];
	}

	delete table;

	for (int i = 0; i < table_depth; i++) {
		delete[] hashes[i];
	}

	delete hashes;
}


/*
 * updates the count of a numeric item
*/

void CMSF::update(uint64_t item) {
	std::list<Counter> counters;
	Counter c;

	total = total + 1.0;

	uint64_t hashval = 0;
	short int fingerprint;

	for (int i = 0; i < table_depth; i++) {
		hashval = (((long)hashes[i][0] * item + hashes[i][1]) % LONG_PRIME) % table_width;

		// Count-Min Sketch is used when fingerprint_bits == 0, otherwise the Count-Min Sketch with Fingerprint is used

		if (fingerprint_bits == 0) {
			// Standard Count-Min Sketch

			if (conservative_update == 0) {
				// normal update: the counters having zero are incremented by 1.0

				table[i][hashval].count = table[i][hashval].count + 1.0;
			}
			else {
				// conservative update: the counters to be updated are inserted in the counter list

				c = { i, hashval, -1, table[i][hashval].count, table[i][hashval].count + 1.0f };

				counters.push_front(c);
			}
		}
		else {
			// Count-Min Sketch with Fingerprint

			fingerprint = hash(item + table[i][hashval].count, 1, fingerprint_bits);

			if (conservative_update == 0) {
				// normal update: the counters having zero are incremented by 1.0, the counters less than the
				// fingerprint_limit are incremented by 1.0 when the fingerprint matches and 0.5 otherwise,
				// the counters having a value greater or equal than the fingerprint_limit are incremented by 1.0

				if (table[i][hashval].count == 0.0)
					table[i][hashval].count = 1.0;
				else
					if (table[i][hashval].count < fingerprint_limit) {
						if (table[i][hashval].fingerprint == fingerprint)
							table[i][hashval].count = table[i][hashval].count + 1.0;
						else
							table[i][hashval].count = table[i][hashval].count + 0.5;
					}
					else
						table[i][hashval].count = table[i][hashval].count + 1.0;

				table[i][hashval].fingerprint = fingerprint;
			}
			else {
				// conservative update: the counters to be updated are inserted in the counter list

				if (table[i][hashval].count == 0.0)
					c = { i, hashval, fingerprint, 0.0, 1.0 };
				else
					if (table[i][hashval].count < fingerprint_limit) {
						if (table[i][hashval].fingerprint == fingerprint)
							c = { i, hashval, fingerprint, table[i][hashval].count, table[i][hashval].count + 1.0f };
						else
							c = { i, hashval, fingerprint, table[i][hashval].count, table[i][hashval].count + 0.5f };
					}
					else
						c = { i, hashval, fingerprint, table[i][hashval].count, table[i][hashval].count + 1.0f };

				counters.push_front(c);
			}
		}
	}

	// conservative update: only the counters stored in the counter list having the minimum value are updated

	if (conservative_update == 1) {
		float minimum = std::numeric_limits<unsigned long long int>::max();

		// find the minimum value of the updated counters

		for (Counter c : counters)
			minimum = MIN(minimum, c.updated);

		// update the counters having the minimum value

		for (Counter c : counters) {
			if (c.updated == minimum || c.current < minimum) {
				table[c.row][c.column].count = c.updated;

				if (fingerprint_bits != 0)
					table[c.row][c.column].fingerprint = c.fingerprint;
			}
			else
				table[c.row][c.column].count = c.current;
		}
	}
}


/*
 * updates the count of a string item
*/

void CMSF::update(const char* str) {
	update(hashstr(str));
}


/*
 * estimates the count of a numeric item
*/

float CMSF::estimate(uint64_t item) {
	unsigned long long int min = std::numeric_limits<unsigned long long int>::max();

	uint64_t hashval = 0;

	for (int i = 0; i < table_depth; i++) {
		hashval = (((long)hashes[i][0] * item + hashes[i][1]) % LONG_PRIME) % table_width;
		min = MIN(min, table[i][hashval].count);
	}

	return min;
}


/*
 * estimates the count of a string item
*/

float CMSF::estimate(const char* str) {
	return estimate(hashstr(str));
}


/*
 * generates hash functions
*/

void CMSF::genhash(unsigned int** hashes, int i) {
	hashes[i][0] = JSHash((uint64_t)(float(rand()) * float(LONG_PRIME) / float(RAND_MAX) + 1));
	hashes[i][1] = RSHash((uint64_t)(float(rand()) * float(LONG_PRIME) / float(RAND_MAX) + 1));
}


/*
 * generates a hash value of a string
*/

uint64_t CMSF::hashstr(const char* str) {
	uint64_t hash = 5381;

	int c;

	while (c = *str++) {
		hash = ((hash << 5) + hash) + c;
	}

	return hash;
}


/*
 * hash functions
 */

int CMSF::hash(uint64_t key, int i, int s)
{
	unsigned int  val = RSHash(key) + i * JSHash(key);

	if (i == 2) val = RSHash(key);
	if (i == 1) val = JSHash(key);

	return (val % s);
}

// RSHash function

int CMSF::RSHash(uint64_t key)
{
	int b = 378551;
	int a = 63689;
	int hash = 0;
	int i = 0;
	char k[8];

	memcpy(k, &key, 8);

	for (i = 0; i < 8; i++)
	{
		hash = hash * a + k[i];
		a = a * b;
	}

	return hash;
}

// JSHash function

int CMSF::JSHash(uint64_t key)
{
	int hash = 1315423911;
	int i = 0;
	char k[8];

	memcpy(k, &key, 8);

	for (i = 0; i < 8; i++)
	{
		hash ^= ((hash << 5) + k[i] + (hash >> 2));
	}

	return hash;
}

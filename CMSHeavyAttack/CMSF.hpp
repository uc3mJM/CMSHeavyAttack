//
// CMSF.hpp Count-Min Sketch with Fingerprints implemented by J. Martínez, this code used Daniel Alabi's Count-Min Sketch 
// as a starting point https://github.com/alabid/countminsketch
//
// To improve accuracy of the counter, Count-Min Sketch stores a fingerprint of the last element x that has
// updated the counter, fingerprint = fp(x, count)
// 
// The counter of an element is updated as follows:
//
// if the counter is 0, set the counter to 1
// else if the stored fingerprint matches f(x, count), add 1 to the counter else add 0.5 to the counter
// set the fingerprint to f(x, count)
//


#define LONG_PRIME 4294967311l
#define MIN(a, b) (a < b ? a : b)

class CMSF {
public:

	CMSF(unsigned int width,
		short int depth,
		short int bits,
		short int conservative,
		short int fp_limit);	                                // class constructor

	virtual ~CMSF();						// class destructor

	unsigned int width() { return table_width; };			// gets table width
	unsigned int depth() { return table_depth; };			// gets table depth

	void update(uint64_t item);					// updates the count of a numeric item
	void update(const char* item);					// updates the count of a string item

	float estimate(uint64_t item);					// estimates the count of a numeric item
	float estimate(const char* item);				// estimates the count of a string item

	float totalcount() { return total; };				// gets the count of all the items stored in the sketch		

private:

	class KeyCounter {
	public:

		float count;
		short int fingerprint;
	};

	short int fingerprint_bits;					// bits used for the fingerprint
	short int conservative_update;					// conservative update
	short int fingerprint_limit;					// fingerprint limit
	float total;							// total count
	unsigned int table_width, table_depth;				// width and depth of the count table

	KeyCounter** table;						// counter and fingerprint table

	unsigned int** hashes;						// hash values

	int hash(uint64_t key, int i, int s);
	int RSHash(uint64_t key);
	int JSHash(uint64_t key);

	uint64_t hashstr(const char* str);				// generates a hash value of a string
	void genhash(unsigned int** hashes, int i);			// generates hashes

};

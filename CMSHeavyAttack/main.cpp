//
// Masquerading Flows as Heavy Hitters attack implementation Count-Min Sketch
//
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <functional> //for std::function
#include <algorithm>  //for std::generate_n
#include "CMSF.hpp"
#include <list>
#include <time.h>
#include <chrono>

/*
 * test_setup stores the command line arguments
 */

struct test_setup {

	std::string item;
	unsigned int width;
	short int depth;
	short int fingerprint_bits;
	short int conservative_update;
	short int fingerprint_limit;
	short int n;
	std::string inputKeys;
	std::string inputUniqueKeys;
	std::string frequency;
};


test_setup setup_CMSF_test(int argc, char* argv[]);
void display_CMSF_test_parameters(test_setup setup);
void CMSF_Value_Guess(const char* item, unsigned int width, short int depth, short int fingerprint_bits, short int conservative, short int fp_limit, short int n);
void CMSF_Value_Guess(unsigned int width, short int depth, short int fingerprint_bits, short int conservative, short int fp_limit, std::string keys, std::string uniqueKeys, std::string frequency);
bool validateInputFiles(std::string keys, std::string uniqueKeys, std::string frequency);
void gen_random(const int len, char* s);
std::string random_string(size_t length);


int main(int argc, char** argv) {
	test_setup setup;


	//
	// w: table width, default value is w=1048576
	// d: table depth or number of counters checked, default value is d=2
	// b: fingerprint bits, default value is 0 for standard Count-Min Sketch, when bits is not zero it
	//    indicates the bits used for the fingerprint related to the counter
	// c: conservative update {0: normal update, 1: conservative update}, default value is c=0
	// u: the fingerprint is used only if the counter is less than the given value, default value is u=0
	// i: input item, default value is i="ASDFGHJKLZXCVBNM"
	// n: number of runs, defaul value is n=1
	//
	// example: cmsf.exe w=1048576 d=2, b=2, i=HOLAHOLAHOLA


	// Setup of the  commnad line input arguments
	setup = setup_CMSF_test(argc, argv);

	// Display the parameters of CMS
	display_CMSF_test_parameters(setup);

	// funtion to find a valid set of elemets to carry out the attack
	if (setup.inputKeys.compare("") == 0) {
		CMSF_Value_Guess(setup.item.c_str(), setup.width, setup.depth, setup.fingerprint_bits, setup.conservative_update, setup.fingerprint_limit, setup.n);
	}
	else {
		//if (validateInputFiles(setup.inputKeys, setup.inputUniqueKeys, setup.frequency)) {
			CMSF_Value_Guess(setup.width, setup.depth, setup.fingerprint_bits, setup.conservative_update, setup.fingerprint_limit, setup.inputKeys, setup.inputUniqueKeys, setup.frequency);
		//}
	}
}

test_setup setup_CMSF_test(int argc, char* argv[]) {
	test_setup setup;

	setup.width = 0;
	setup.depth = -1;
	setup.fingerprint_bits = -1;
	setup.conservative_update = -1;
	setup.fingerprint_limit = -1;
	setup.inputKeys = "";
	setup.inputUniqueKeys = "";
	setup.n = 1;


	for (int i = 1; i < argc; i++) {
		std::stringstream str;
		str << argv[i];

		std::string argument = str.str();

		// argument 'w' is used for table width, default value is w=1048576 

		if (argument[0] == 'w' || argument[0] == 'W') {
			int len = argument.size();
			setup.width = std::stof(argument.substr(2, len - 1).c_str());
		}

		// argument 'd' is used for table depth, default value is d=2

		if (argument[0] == 'd' || argument[0] == 'D') {
			int len = argument.size();
			setup.depth = std::stof(argument.substr(2, len - 1).c_str());
		}
		// argument 'b' is used for fingerprint bits, default value is b=0, valid values are {4, 5, 6, 7, 8}

		if (argument[0] == 'b' || argument[0] == 'B') {
			int len = argument.size();
			setup.fingerprint_bits = std::stof(argument.substr(2, len - 1).c_str());
		}

		// argument 'c' is used for the conservative update, default value is c=0

		if (argument[0] == 'c' || argument[0] == 'C') {
			int len = argument.size();
			setup.conservative_update = std::stof(argument.substr(2, len - 1).c_str());
		}

		// argument 'u' is used for the fingerprint limit, default value is u=0, max value is 32767

		if (argument[0] == 'u' || argument[0] == 'U') {
			int len = argument.size();
			setup.fingerprint_limit = std::stof(argument.substr(2, len - 1).c_str());
		}

		// argument 'i' is used for the input item 

		if (argument[0] == 'i' || argument[0] == 'I') {

			int len = argument.size();
			setup.item = argument.substr(2, len - 1).c_str();

		}

		// argument 'n' is used for the number of times is going to run;

		if (argument[0] == 'n' || argument[0] == 'N') {
			int len = argument.size();
			setup.n = std::stof(argument.substr(2, len - 1).c_str());
		}
		// argument 'f' is used for the input file name

		if (argument[0] == 'f' || argument[0] == 'F') {
			int len = argument.size();
			setup.inputKeys = argument.substr(2, len - 1).c_str();
		}

	}

	// set default values for the arguments

	if (setup.width == 0)
		setup.width = 1048576;

	if (setup.depth == -1)
		setup.depth = 2;

	if (setup.fingerprint_bits == -1 || setup.fingerprint_bits == 0) {
		setup.fingerprint_bits = 0;
		setup.conservative_update = 0;
		setup.fingerprint_limit = 0;
	}
	else {
		// setup.fingerprint_bits valid values are {4, 5, 6, 7, 8}

		if (setup.fingerprint_bits < 4 || setup.fingerprint_bits > 8)
			setup.fingerprint_bits = 4;

		if (setup.conservative_update == -1)
			setup.conservative_update = 0;

		if (setup.fingerprint_limit == -1)
			setup.fingerprint_limit = 0;
	}

	if (setup.item.compare("") == 0) {
		setup.item = "ASDFGHJKLZXCVBNM";
	}

	if (setup.inputKeys.compare("") != 0) {
		setup.inputUniqueKeys = setup.inputKeys.substr(0, setup.inputKeys.length() - 4) + "_sorted_unique.txt";
		setup.frequency = setup.inputKeys.substr(0, setup.inputKeys.length() - 4) + "_sorted_frequency.txt";
	}

	return setup;
}


/*
 * shows the test setup on the console
 */

void display_CMSF_test_parameters(test_setup setup) {
	std::cout << "Heavy Hitter Masquerade attak to CMSF(Count-Min Sketch with Fingerprint)\n\n";

	std::cout << "Items               i = " << setup.item << "\n";
	std::cout << "Table width         w = " << setup.width << "\n";
	std::cout << "Table depth         d = " << setup.depth << "\n";
	std::cout << "Fingerprint bits    b = " << setup.fingerprint_bits << "\n";
	std::cout << "Conservative update c = " << setup.conservative_update << "\n";
	std::cout << "Fingerprint limit   u = " << setup.fingerprint_limit << "\n";
	std::cout << "Number of tries     n = " << setup.n << "\n";
	std::cout << "Input keys          f = '" << setup.inputKeys << "', '" << setup.inputUniqueKeys << "', " << setup.frequency << "\n";

	std::cout << "\n";
}


/*
* Function to find a valid set of element to carry of the attack
*/

void CMSF_Value_Guess(const char* item, unsigned int width, short int depth, short int fingerprint_bits, short int conservative, short int fp_limit, short int n) {

	//Variable initialization
	std::list<std::string> items;
	std::list<std::string>::iterator it;

	int* operations = new int[n];
	int average = 0, deviation = 0;
	long long sum = 0;

	unsigned int i, j, x = 0, total = 0, c = 0;

	bool finish = false;

	std::string random;
	std::string list_value;

	// loop to iterate all necesarry number of simulation
	for (j = 0; j < n; j++) {

		
		items.clear();
		// Instanciate CMS
		CMSF* cms = new CMSF(width, depth, 0, conservative, fp_limit);

		// First update of the target element
		cms->update(item);
		x = cms->estimate(item);
		
		// Update Operation count
		c = 0;
		finish = false;

		//New seed for the ramdon elements
		srand(j);

		std::cout << item <<" First estimate -> " << x << std::endl;

		//Loop to find valid set of elements
		do {

			// Select a new ramdon elemnet
			random = random_string(16);
			// Update CMS with ramdon element
			cms->update(random.c_str());

			c++;
			//If the estimate of the target element increments a colliding elemt have been found
			if ((cms->estimate(item)) == (x + 1)) {
				

				std::cout << "Found a colliding element -> " << random << std::endl;

				x = cms->estimate(item);

				// Add element to attack set
				items.push_back(random);
				finish = true;

				// Check attack set to know if it is complete
				for (i = 0; i < 10; i++) {
					
					// Update CMS with all element from the attack set
					for (it = items.begin(); it != items.end(); ++it) {
						list_value = *it;
						cms->update(list_value.c_str());
					}

					// If the estimate doesn't increment the algoritm countinues until it found another colliding element
					if ((cms->estimate(item)) != (x + 1)) {
						std::cout << "Attack set incomplete, keep looking!" << std::endl;
						finish = false;
						x = cms->estimate(item);
						break;
					}

					x = cms->estimate(item);
					
				}
			}

		} while (!finish);

		// List of updates needed in each iteration
		operations[j] = c;
		delete cms;

	}

	// If more than one simulation -> calculate average and standard derivation
	if (n > 1) {

		for (i = 0; i < n; ++i)
		{
			sum += operations[i];
		}

		average = sum / n;
		sum = 0;

		for (i = 0; i < n; i++)
		{
			sum += pow(abs((average - operations[i])), 2);
		}

		deviation = sqrt((sum / (n - 1)));

		std::cout << "Average of updates needed -> " << average << std::endl;
		std::cout << "Deviation of updates needed -> " << deviation << std::endl;
	}

	// If only one simulation -> show number of udates needed and the attack set
	else {
		std::cout << "Number of updates needed -> " << operations[0] << std::endl;
		std::cout << "List of items found: "<< std::endl;
		
		for (it = items.begin(); it != items.end(); ++it) {
			
			std::cout << std::distance(items.begin(), it) + 1  << ". " << *it << std::endl;

		}
	}

	delete[] operations;
}

/*Overloaded funtion*/
void CMSF_Value_Guess(unsigned int width, short int depth, short int figerprint_bits, short int conservative, short int fp_limit, std::string keys, std::string uniqueKeys, std::string frequency){

	std::ifstream inputFileKey;
	std::ifstream inputFileFrequency;
	std::ifstream inputFileUnique;

	auto t1 = std::chrono::high_resolution_clock::now();

	char ip[60];
	char frequecies[2];
	const char* item = "rand";
	int f = 2;

	bool finish = false;
	unsigned int i, j, c = 0;

	std::list<std::string> items;
	std::list<std::string>::iterator it;

	CMSF* cms = new CMSF(width, depth, 0, conservative, fp_limit);

	inputFileKey.open(keys);
	inputFileUnique.open(uniqueKeys);

	if (inputFileKey.is_open() && inputFileUnique.is_open()) {
		std::cout << "Building CMS with input keys" << std::endl;
		while (!inputFileKey.eof()) {
			inputFileKey.getline(ip, 60);
			cms->update(ip);
		}

		std::cout << "Finding an element with low frecuecy" << std::endl;
		while (f > 1 && !inputFileUnique.eof()) {
			inputFileUnique.getline(ip, 60);
			f = cms->estimate(ip);
			item = ip;
		}

		inputFileKey.close();
		inputFileUnique.close();
	}
	else
		std::cout << "Input file '" << keys << "' not found!\n";

	auto t2 = std::chrono::high_resolution_clock::now();

	auto duration = std::chrono::duration_cast<std::chrono::seconds>(t2 - t1).count();

	std::cout << "Time to build the set" << duration << std::endl;

	std::string random;
	std::string list_value;

	f = cms->estimate(item);
	// Update Operation count


	std::cout << "First estimate of item \""<< item << "\"-> " << f << std::endl;

		//Loop to find valid set of elements
		do {

			// Select a new ramdon elemnet
			random = random_string(60);
			// Update CMS with ramdon element
			cms->update(random.c_str());

			c++;
			//If the estimate of the target element increments a colliding elemt have been found
			if ((cms->estimate(item)) == (f + 1)) {


				std::cout << "Found a colliding element -> " << random << std::endl;

				f = cms->estimate(item);

				// Add element to attack set
				items.push_back(random);
				finish = true;

				// Check attack set to know if it is complete
				for (i = 0; i < 100; i++) {

					// Update CMS with all element from the attack set

					for (it = items.begin(); it != items.end(); ++it) {
						list_value = *it;
						cms->update(list_value.c_str());
					}

					// If the estimate doesn't increment the algoritm countinues until it found another colliding element
					if ((cms->estimate(item)) != (f + 1)) {
						std::cout << "Attack set incomplete, keep looking!" << std::endl;
						finish = false;
						f = cms->estimate(item);
						break;
					}

					f = cms->estimate(item);

				}
			}

		} while (!finish);


		std::cout << "Number of updates needed -> " << c << std::endl;
		std::cout << "List of items found: " << std::endl;

		for (it = items.begin(); it != items.end(); ++it) {

			std::cout << std::distance(items.begin(), it) + 1 << ". " << *it << std::endl;

		}

		std::cout << "Estimate of item \"" << item << "\"-> " << f << std::endl;
		std::cout << "Insertin the attack set 100 times " << std::endl;
		for (i = 0; i < 100; i++) {
			for (it = items.begin(); it != items.end(); ++it) {
				list_value = *it;
				cms->update(list_value.c_str());
			}
		}

		f = cms->estimate(item);
		std::cout << "Estimate of item \"" << item << "\"-> " << f << std::endl;

		auto t3 = std::chrono::high_resolution_clock::now();

		auto duration2 = std::chrono::duration_cast<std::chrono::seconds>(t3 - t2).count();

		std::cout << "Time to do the attack" << duration << std::endl;

		// List of updates needed in each iteration
		delete cms;
}

bool validateInputFiles(std::string keys, std::string uniqueKeys, std::string frequency) {
	std::ifstream inputFile;

	inputFile.open(keys);

	if (inputFile.is_open())
		inputFile.close();
	else {
		std::cout << "Input file '" << keys << "' not found!\n";

		return false;
	}

	inputFile.open(uniqueKeys);

	if (inputFile.is_open())
		inputFile.close();
	else {
		std::cout << "Input file '" << uniqueKeys << "' not found!\n";

		return false;
	}


	return true;
}


/*
 * Generate a random char*
 */
void gen_random(const int len, char* s) {

	/* Intializes random number generator */


	static const char alphanum[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";

	for (int i = 0; i < len; ++i) {
		s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
	}

	s[len] = 0;

}

std::string random_string(size_t length)
{
	auto randchar = []() -> char
	{
		const char charset[] =
			"0123456789"
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			"abcdefghijklmnopqrstuvwxyz";
		const size_t max_index = (sizeof(charset) - 1);
		return charset[rand() % max_index];
	};
	std::string str(length, 0);
	std::generate_n(str.begin(), length, randchar);
	return str;
}
	setup = setup_CMSF_test(argc, argv);
	display_CMSF_test_parameters(setup);
	CMSF_Value_Guess(setup.item.c_str(), setup.width, setup.depth, setup.fingerprint_bits, setup.conservative_update, setup.fingerprint_limit, setup.n);

}

test_setup setup_CMSF_test(int argc, char* argv[]) {
	test_setup setup;

	setup.width = 0;
	setup.depth = -1;
	setup.fingerprint_bits = -1;
	setup.conservative_update = -1;
	setup.fingerprint_limit = -1;
	setup.n = 1;


	for (int i = 1; i < argc; i++) {
		std::stringstream str;
		str << argv[i];

		std::string argument = str.str();

		// argument 'w' is used for table width, default value is w=1048576 

		if (argument[0] == 'w' || argument[0] == 'W') {
			int len = argument.size();
			setup.width = std::stof(argument.substr(2, len - 1).c_str());
		}

		// argument 'd' is used for table depth, default value is d=2

		if (argument[0] == 'd' || argument[0] == 'D') {
			int len = argument.size();
			setup.depth = std::stof(argument.substr(2, len - 1).c_str());
		}
		// argument 'b' is used for fingerprint bits, default value is b=0, valid values are {4, 5, 6, 7, 8}

		if (argument[0] == 'b' || argument[0] == 'B') {
			int len = argument.size();
			setup.fingerprint_bits = std::stof(argument.substr(2, len - 1).c_str());
		}

		// argument 'c' is used for the conservative update, default value is c=0

		if (argument[0] == 'c' || argument[0] == 'C') {
			int len = argument.size();
			setup.conservative_update = std::stof(argument.substr(2, len - 1).c_str());
		}

		// argument 'u' is used for the fingerprint limit, default value is u=0, max value is 32767

		if (argument[0] == 'u' || argument[0] == 'U') {
			int len = argument.size();
			setup.fingerprint_limit = std::stof(argument.substr(2, len - 1).c_str());
		}

		// argument 'i' is used for the input item 

		if (argument[0] == 'i' || argument[0] == 'I') {

			int len = argument.size();
			setup.item = argument.substr(2, len - 1).c_str();

		}

		// argument 'n' is used for the number of times is going to run;

		if (argument[0] == 'n' || argument[0] == 'N') {
			int len = argument.size();
			setup.n = std::stof(argument.substr(2, len - 1).c_str());
		}

	}

	// set default values for the arguments

	if (setup.width == 0)
		setup.width = 1048576;

	if (setup.depth == -1)
		setup.depth = 2;

	if (setup.fingerprint_bits == -1 || setup.fingerprint_bits == 0) {
		setup.fingerprint_bits = 0;
		setup.conservative_update = 0;
		setup.fingerprint_limit = 0;
	}
	else {
		// setup.fingerprint_bits valid values are {4, 5, 6, 7, 8}

		if (setup.fingerprint_bits < 4 || setup.fingerprint_bits > 8)
			setup.fingerprint_bits = 4;

		if (setup.conservative_update == -1)
			setup.conservative_update = 0;

		if (setup.fingerprint_limit == -1)
			setup.fingerprint_limit = 0;
	}

	if (setup.item.compare("") == 0) {
		setup.item = "ASDFGHJKLZXCVBNM";
	}

	return setup;
}


/*
 * shows the test setup on the console
 */

void display_CMSF_test_parameters(test_setup setup) {
	std::cout << " Heavy Hitter Masquerade attak to CMSF(Count-Min Sketch with Fingerprint)\n\n";

	std::cout << "Item                i = " << setup.item << "\n";
	std::cout << "Table width         w = " << setup.width << "\n";
	std::cout << "Table depth         d = " << setup.depth << "\n";
	std::cout << "Fingerprint bits    b = " << setup.fingerprint_bits << "\n";
	std::cout << "Conservative update c = " << setup.conservative_update << "\n";
	std::cout << "Fingerprint limit   u = " << setup.fingerprint_limit << "\n";
	std::cout << "Number of tries     n = " << setup.n << "\n";

	std::cout << "\n";
}

void CMSF_Value_Guess(const char* item, unsigned int width, short int depth, short int fingerprint_bits, short int conservative, short int fp_limit, short int n) {

	std::list<std::string> items;
	std::list<std::string>::iterator it;

	int* operations = new int[n];
	int average = 0, deviation = 0;
	long long sum = 0;

	unsigned int i, j, x = 0, total = 0, c = 0;

	bool finish = false;

	std::string random;
	std::string list_value;

	for (j = 0; j < n; j++) {

		
		items.clear();
		CMSF* cms = new CMSF(width, depth, 0, conservative, fp_limit);

		cms->update(item);
		x = cms->estimate(item);
		c = 0;
		finish = false;

		srand(j);

		do {

			random = random_string(16);
			cms->update(random.c_str());

			c++;

			if ((cms->estimate(item)) == (x + 1)) {

				x = cms->estimate(item);
				items.push_back(random);
				finish = true;

				for (i = 0; i < 10; i++) {

					for (it = items.begin(); it != items.end(); ++it) {
						list_value = *it;
						cms->update(list_value.c_str());
					}

					if ((cms->estimate(item)) != (x + 1)) {
						finish = false;
						x = cms->estimate(item);
						break;
					}

					x = cms->estimate(item);
				}
			}

		} while (!finish);


		operations[j] = c;
		delete cms;

	}

	if (n > 1) {

		for (i = 0; i < n; ++i)
		{
			sum += operations[i];
		}

		average = sum / n;
		sum = 0;

		for (i = 0; i < n; i++)
		{
			sum += pow(abs((average - operations[i])), 2);
		}

		deviation = sqrt((sum / (n - 1)));

		std::cout << "Average of updates needed -> " << average << std::endl;
		std::cout << "Deviation of updates needed -> " << deviation << std::endl;
	}
	else {
		std::cout << "Number of updates needed -> " << operations[0] << std::endl;
		std::cout << "List of items found: "<< std::endl;
		
		for (it = items.begin(); it != items.end(); ++it) {
			
			std::cout << std::distance(items.begin(), it) + 1  << ". " << *it << std::endl;

		}
	}

	delete[] operations;
}



/*
 * Generate a random char*
 */
void gen_random(const int len, char* s) {

	/* Intializes random number generator */


	static const char alphanum[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";

	for (int i = 0; i < len; ++i) {
		s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
	}

	s[len] = 0;

}

std::string random_string(size_t length)
{
	auto randchar = []() -> char
	{
		const char charset[] =
			"0123456789"
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			"abcdefghijklmnopqrstuvwxyz";
		const size_t max_index = (sizeof(charset) - 1);
		return charset[rand() % max_index];
	};
	std::string str(length, 0);
	std::generate_n(str.begin(), length, randchar);
	return str;
}

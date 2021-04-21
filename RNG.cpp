#include "stdafx.h"
#include "RNG.h"

bool generateRandomNumber(int shift) {
	if (shift < 0 && shift > 32) {
		throw "Shift must be in [0; 32]";
	}

	unsigned int random_s;
	errno_t err;

	while (true) {
		int* adr = new int;
		std::size_t address = reinterpret_cast<std::size_t>(adr);
		do{
			err = rand_s(&random_s);
		} while (err != 0);
		address = address ^ random_s;
		
		char c = 0;
		size_t cpyAddress = address;
		while (cpyAddress){
			c += cpyAddress & 1;
			cpyAddress >>= 1;
		}

		if (c == 16) {
			return (address >> shift) & 1;
		}
	}
}
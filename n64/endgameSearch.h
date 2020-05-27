#pragma once
#include "port.h"
#include "hash.h"

struct Empty {
	int sq;
	Empty* prev;
	Empty* next;

	inline void remove() {
		prev->next = next;
		if (next) {
			next->prev = prev;
		}
	}

	inline void insert() {
		prev->next = this;
		if (next) {
			next->prev = this;
		}
	}
};

class HashTable;

class EndgameSearch {
public:
	Empty start;
	Empty emptyArray[32];
	HashTable hashTable;
	bool useHash;

public:
	void init(u64 mover, u64 enemy);
	void validate();

private:
	void constructEmpties(u64 mover, u64 enemy);
};

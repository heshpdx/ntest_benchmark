#pragma once

#include "Moves.h"
#include "QPositionTest.h"
#include "StoreTest.h"

inline void testCore() {
	void TestBitBoard();

	TestBitBoard();
	TestQPosition();
	CMove::Test();
	CMoves::Test();

	void TestStore();
	TestStore();
}

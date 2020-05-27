#pragma once

#include "Moves.h"
#include "QPositionTest.h"

inline void testCore() {
  void TestBitBoard();

  TestBitBoard();
  TestQPosition();
  CMove::Test();
  CMoves::Test();
}

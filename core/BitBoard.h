// Copyright Chris Welty
//  All Rights Reserved
// This file is distributed subject to GNU GPL version 3. See the files
// GPLv3.txt and License.txt in the instructions subdirectory for details.

// board header file
#pragma once


#include <cassert>
#include <sstream>
#include <stdio.h>
#include "../n64/utils.h"
class CMoves;

//! The bitboard representation of a position, containing two u64s: one for the mover and one for empty squares.
class CBitBoard {
public:
    u64 empty, mover;

    CBitBoard() : empty(0), mover(0) {}
    // Set to starting position
    void Initialize();
    void Initialize(const char* boardText, bool fBlackMove=true);

    // flipping
    void FlipHorizontal();
    void FlipVertical();
    void FlipDiagonal();
    CBitBoard Symmetry(int sym) const;
    CBitBoard MinimalReflection() const;

    void InvertColors();

    // impossible board
    void SetImpossible();
    bool IsImpossible() const;

    // validation
    bool IsValid() const { return !(empty&mover); }

    // I/O
    void Print(bool fBlackMove) const;// Print the board in human-readable format
    void FPrint(FILE* fp, bool blackMove) const;// Print the board in human-readable format
    bool Write(FILE* fp) const;	// print in bitstream format
    bool Read(FILE* fp);		// read in bitstream format; return true if successful
    char* GetSBoard(char sBoard[NN+1], bool fBlackMove) const;

    // statistics
    u64 Hash() const {
      return hash_mover_empty(mover, empty);
    }

    int NEmpty() const { return bitCountInt(empty); }
    int NMover() const { return bitCountInt(mover); }
    void NDiscs(bool fBlackMove, int& nBlack, int& nWhite, int& nEmpty) const;
    int TerminalValue() const;

    // moving
    bool CalcMoves(CMoves& moves) const;
    int CalcMobility(u4& nMovesPlayer, u4& nMovesOpponent) const;
    int NMoverMobilities() const;
    bool operator==(const CBitBoard& b) const;
    bool operator!=(const CBitBoard& b) const;

    u64 getEnemy() const { return ~(u64(mover)|u64(empty)); }

protected:
    void FPrintHeader(std::stringstream &ss) const;
};

bool operator<(const CBitBoard& a, const CBitBoard& b);

//! Make the bitboard an impossible position. This is used when clearing the hashtable.
inline void CBitBoard::SetImpossible() {
    mover=empty=~0;
    assert(IsImpossible());
}

//! Check whether the bitboard is impossible. Impossible positions are stored in unused
//! hashtable locations.
inline bool CBitBoard::IsImpossible() const {
    return (mover&empty)?true:false;
}

inline bool CBitBoard::operator==(const CBitBoard& b) const { return !((mover^b.mover)|(empty^b.empty));};
inline bool CBitBoard::operator!=(const CBitBoard& b) const { return mover!=b.mover || empty!=b.empty;};
inline void CBitBoard::FlipVertical() { empty=flipVertical(empty); mover=flipVertical(mover);}
inline void CBitBoard::FlipHorizontal() { empty=flipHorizontal(empty); mover=flipHorizontal(mover);}
inline void CBitBoard::FlipDiagonal() { empty=flipDiagonal(empty); mover=flipDiagonal(mover);}
inline void CBitBoard::InvertColors() { mover^=~empty; }

//! This is just a bitboard that's guaranteed to be a minimal reflection.
//!
//! The class won't completely guarantee that you haven't messed up, since you can alter the data
//! elements to make it a nonminimal bitboard. Don't do this.
class CMinimalReflection: public CBitBoard {
public:
    CMinimalReflection(const CBitBoard& bb) : CBitBoard(bb.MinimalReflection()) {};
};

void TestBitBoard();


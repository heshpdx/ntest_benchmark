#pragma once

#include <iostream>
#include <string>

#include "port.h"

u64 flipHorizontal(u64 bits);

inline u64 flipDiagonal(u64 v) {
    // flip 4-by-4 bits
    v = (v & 0xf0f0f0f00f0f0f0fULL)
        | ((v >> 28) & 0xf0f0f0f0ULL)
        | ((v & 0xf0f0f0f0ULL) << 28);
    // flip 2-by-2 bits
    v = (v & 0xcccc3333cccc3333ULL)
        | ((v & 0x0000cccc0000ccccULL) << 14)
        | ((v & 0x3333000033330000ULL) >> 14);
    // flip 1-by-1 bits
    return (v & 0xaa55aa55aa55aa55ULL)
        | ((v & 0x00aa00aa00aa00aaULL) << 7)
        | ((v & 0x5500550055005500ULL) >> 7);
}

void printBitBoard(u64 bits);
void printBoard(u64 black, u64 white);
void printXorBoard(u64 expected, u64 actual);

const u64 MaskA = 0x0101010101010101;
const u64 MaskB = MaskA << 1;
const u64 MaskC = MaskA << 2;
const u64 MaskD = MaskA << 3;
const u64 MaskE = MaskA << 4;
const u64 MaskF = MaskA << 5;
const u64 MaskG = MaskA << 6;
const u64 MaskH = MaskA << 7;
const u64 MaskA1H8 = 0x8040201008040201;
const u64 MaskA8H1 = 0x0102040810204080;

// square type masks (for move ordering)
const u64 corners =  0x8100000000000081ULL;
const u64 cSquares = 0x4281000000008142ULL;
const u64 xSquares = 0x0042000000004200ULL;
const u64 other = ~(corners|xSquares);


std::string squareText(int row, int col);
std::string squareText(int sq);
std::string asHex(u64 bits);
std::string eng(double d);
std::string eng(double d, int precision);

u64 rand64();
u64 mobility(u64 mover, u64 enemy);
u64 koggeStoneFlips(int sq, u64 mover, u64 enemy);



/////////////////////////////////////////
// Generic types
/////////////////////////////////////////

// for compatibility with unix-like OS

inline void CHECKNEW(bool x) {
    if (!x) {
    	std::cerr << "New failed in " << __FILE__ << " line " << __LINE__ << "\n";
    	_exit(-1);
    }
}

/////////////////////////////////////////
// Sign
/////////////////////////////////////////

int Sign(int a);

/////////////////////////////////////////
// node value type
/////////////////////////////////////////

std::string StringFromValue(CValue value);
std::string StringFromValuePair(CValue value1, CValue value2);

//////////////////////////////////////
// Constants
//////////////////////////////////////

enum direction { north, south, east, west, northeast,southwest, northwest, southeast };

const CValue kStoneValue = 100;
const CValue kWipeout = 64*kStoneValue;
const CValue kMaxHeuristic = kWipeout-1;
const CValue kMaxBonus = 10000;
const CValue kInfinity = kWipeout+kMaxBonus;

enum {
    A1=000, B1=001, C1=002, D1=003, E1=004, F1=005, G1=006, H1=007,
    A2=010, B2=011, C2=012, D2=013, E2=014, F2=015, G2=016, H2=017,
    A3=020, B3=021, C3=022, D3=023, E3=024, F3=025, G3=026, H3=027,
    A4=030, B4=031, C4=032, D4=033, E4=034, F4=035, G4=036, H4=037,
    A5=040, B5=041, C5=042, D5=043, E5=044, F5=045, G5=046, H5=047,
    A6=050, B6=051, C6=052, D6=053, E6=054, F6=055, G6=056, H6=057,
    A7=060, B7=061, C7=062, D7=063, E7=064, F7=065, G7=066, H7=067,
    A8=070, B8=071, C8=072, D8=073, E8=074, F8=075, G8=076, H8=077,
};

///////////////////////////////////////////
// Coefficient type
///////////////////////////////////////////

typedef i4 TCoeff;


// square values
const int WHITE=0;
const int EMPTY=1;
const int BLACK=2;

// convert from various board characters to (0,1,2) and back again
int  TextToValue(char c);
char ValueToText(int item);

// convert from square number to row and column
inline int Row(int square) {
    if (N==8)
    	return square>>3;
    else
    	return square/N;
}

inline int Col(int square) {
    if (N==8)
    	return square&7;
    else
    	return square%N;
}

inline char RowText(int square) {
    return Row(square)+'1';
}

inline char ColText(int square) {
    return Col(square)+'A';
}

inline int Square(int row, int col) {
    if (N==8)
    	return (row<<3)+col;
    else
    	return row*N+col;
}

// directory where the program is located
extern std::string fnBaseDir;


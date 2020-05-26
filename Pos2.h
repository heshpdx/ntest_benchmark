// Copyright Chris Welty
//  All Rights Reserved
// This file is distributed subject to GNU GPL version 3. See the files
// GPLv3.txt and License.txt in the instructions subdirectory for details.

#pragma once

#ifndef H_POS2
#define H_POS2

#include <stdio.h>
#include <vector>
#include "n64/utils.h"
#include "core/Moves.h"
#include "core/BitBoard.h"
#include "pattern/patternJ.h"
#include "Stable.hpp"

// global variables

class Pos2 {
public:
    void InitializeToStartPosition();
    void Initialize(const char* sBoard, bool fBlackMove);
    void Initialize(const CBitBoard& bb, bool fBlackMove);

    // print the position
    void Print() const;
    void FPrint(FILE* logfile) const;
    void PrintStable() const;
    void FPrintStable(FILE* logfile) const;
    void PrintStableNext() const;
    void FPrintStableNext(FILE* logfile) const;

    // get the board text
    char* GetText(char* sBoard) const;

    // Moving
    void MakeMoveBB(int square);
    void PassBB();
    int CalcMovesAndPassBB(CMoves& moves);
    /**
    * Calculate moves and store them in moves
    *
    * @param [out] moves location to store moves
    * @return true if there are legal moves
    */

    bool CalcMoves(CMoves& moves) const {
        return m_bb.CalcMoves(moves);
    }
    void PassBase();

    // Info
    int NEmpty() const { return m_bb.NEmpty();}
    bool BlackMove() const { return m_fBlackMove; }
    const CBitBoard& GetBB() const { return m_bb; }
    bool IsValid() const { return m_bb.IsValid(); };

    int TerminalValue() const;

    int CalcMobility(u4& nMovesPlayer, u4& nMovesOpponent) const {
        return m_bb.CalcMobility(nMovesPlayer,nMovesOpponent);
    }

    uint64_t m_stable = 0;
    uint64_t m_stable_trigger = Corners;
    CBitBoard m_bb;
    uint8_t m_stable_mover = 0;
    uint8_t m_stable_opponent = 0;
    bool m_fBlackMove;
private:
    int CalcMovesAndPassBB(CMoves& moves, const CMoves& submoves);
};

inline int Pos2::TerminalValue() const {
    return m_bb.TerminalValue();
}

inline void Pos2::PassBase() {
    m_fBlackMove=!m_fBlackMove;
    m_bb.mover = ~(m_bb.mover | m_bb.empty);
    if (m_stable) {
        auto stable_swap = m_stable_mover;
        m_stable_mover = m_stable_opponent;
        m_stable_opponent = stable_swap;
    }
}

inline void Pos2::PassBB() {
    m_fBlackMove = !m_fBlackMove;
    m_bb.InvertColors();
    if (m_stable) {
        auto stable_swap = m_stable_mover;
        m_stable_mover = m_stable_opponent;
        m_stable_opponent = stable_swap;
    }
}
#endif // #ifdef H_POS2

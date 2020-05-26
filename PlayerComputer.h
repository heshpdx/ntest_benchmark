#include "core/QPosition.h"
#include "core/SearchInfo.h"
#include "core/MVK.h"

#include "game/Player.h"

class CEvaluator;
class CCache;
class CMPCStats;
class CCalcParams;

//! Default information for CPlayerComputer construction
class CComputerDefaults {
public:
	CComputerDefaults();

	std::string sCalcParams;
	char cEval, cCoeffSet;

	CValue vContempts[2];
	
	int iPruneEndgame, iPruneMidgame, nRandShifts[2];
	u4 iEdmund;
	u4 fsPrint, fsPrintOpponent;

	int MinutesOrDepth() const;

	static CValue FloatToContempt(double fContempt);
};

//! Computerized player of an othello game
class CPlayerComputer : public CPlayer {
public:
	CEvaluator* eval;
	CCache *caches[2];
	bool fHasCachedPos[2];
	CCalcParams *pcp;
	CMPCStats *mpcs;
	CComputerDefaults cd;
	bool toot;
	bool solved, hasBacktracked, fAverageTime;
	bool fAnalyzingDeferred;
	CValue solvedValue;

	CPlayerComputer(const CComputerDefaults& acd);
    CPlayerComputer() {}
	virtual ~CPlayerComputer();

	// game playing
	virtual void StartMatch(const COsMatch& match);
	virtual TCheatcode GetMoveAndTime(COsGame& game, int flags, COsMoveListItem& mli);
	virtual TCheatcode GetMove(COsGame& game, int flags, COsMoveListItem& mli);
	virtual void EndGame(const COsGame& game);


	// Routine to get a move
	void Clear();
	CSearchInfo DefaultSearchInfo(bool fBlackMove, u4 fNeeds, double tRemaining, int iCache) const;
	void GetChosen(const CSearchInfo& si, const CQPosition& pos, CMVK& chosen);
	void Hint(const CQPosition& pos, int nBest);


	// misc
	static u4 LogCacheSize(CCalcParams* pcp, int iPrune);
	int AdjustedHeight(const CQPosition& pos) const;
	void SetContempt(bool fDrawsToBlack);

	// information about the player
	virtual bool IsHuman() const;

protected:
	void SetParameters(const CQPosition& pos, int iCache);
	void SetParameters(int iCache);
	static int DefaultRandomness();
	virtual CCache* GetCache(int iCache);
	CQPosition posCached[2];
};

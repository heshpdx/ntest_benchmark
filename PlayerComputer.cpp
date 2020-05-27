#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>
#include "core/MPCStats.h"
#include "core/Cache.h"
#include "core/options.h"
#include "core/CalcParams.h"

#include "Pos2.h"
#include "SearchParams.h"
#include "Search.h"
#include "Evaluator.h"

#include "PlayerComputer.h"

using namespace std;

////////////////////////////////////////
// CComputerDefaults
////////////////////////////////////////

CComputerDefaults::CComputerDefaults() : sCalcParams("s12"), cEval('J'), cCoeffSet('A')
, iPruneEndgame(5), iPruneMidgame(4), iEdmund(1) {
	vContempts[0]=0;
	vContempts[1]=0;
	nRandShifts[0]=nRandShifts[1]=0;
	fsPrint=5;
	fsPrintOpponent=1;
}

int CComputerDefaults::MinutesOrDepth() const {
	if (sCalcParams.empty())
		return 0;
	else
		return atol(sCalcParams.c_str()+1);
}

CValue CComputerDefaults::FloatToContempt(double fContempt) {
	CValue v=int(fContempt*kStoneValue);
	if (v>kMaxBonus)
		v=kMaxBonus;
	else if (v<-kMaxBonus)
		v=-kMaxBonus;

	return -v;
}


////////////////////////////////////////
// CPlayerComputer
////////////////////////////////////////

CPlayerComputer::CPlayerComputer(const CComputerDefaults& acd) {
	cd=acd;
	pcp=CCalcParams::NewFromString(cd.sCalcParams);

	caches[0]=caches[1]=NULL;
	fHasCachedPos[0]=fHasCachedPos[1]=false;
	eval=CEvaluator::FindEvaluator(cd.cEval, cd.cCoeffSet);
	mpcs=CMPCStats::GetMPCStats(cd.cEval, cd.cCoeffSet, std::max(cd.iPruneMidgame, cd.iPruneEndgame));
	fAnalyzingDeferred=false;

	// get saved-game file name
	std::ostringstream os;
	os << fnBaseDir << "results/" << cd.cEval << cd.cCoeffSet << '_' << *pcp << ".ggf";
	m_fnSaveGame=os.str();
	
	toot=false;
	
	// calculate computer's name
	std::ostringstream osName;
	pcp->Name(osName);
	m_sName=osName.str();

	if (!mpcs) cd.iPruneMidgame=cd.iPruneEndgame=0;
}

CPlayerComputer::~CPlayerComputer() {
	int i;
	for (i=0; i<2; i++)
		if (caches[i])
			delete caches[i];

	delete mpcs;
	delete pcp;
}

u4 CPlayerComputer::LogCacheSize(CCalcParams* pcp, int aPrune) {
  return 21;
  /*
	u2 lgCacheSize;
	
	// basic suggestion from the calc params
	lgCacheSize=pcp->LogCacheSize(aPrune);

	// constrain lgCacheSize to be reasonable, due in part to Pentium handling of
	//	shift operators with right operand >= 32
	if (lgCacheSize<2)
		lgCacheSize=2;
	else if (lgCacheSize>30)
		lgCacheSize=30;

	// constrain lgCacheSize to fit within available RAM (from params.txt)
	if (maxCacheMem>sizeof(CCacheData)) {
		while (sizeof(CCacheData) > maxCacheMem>>lgCacheSize)
			lgCacheSize--;
	}

	// lgCacheSize must be at least 2, due to the way cache operates
	if (lgCacheSize<2)
		lgCacheSize=2;

	return lgCacheSize; */
}

//! Set the contempt such that draws always go to black or white, depending on fDrawsToBlack
void CPlayerComputer::SetContempt(bool fDrawsToBlack) {
	int vContempt = cd.FloatToContempt(100);
	if (fDrawsToBlack) {
		vContempt = -vContempt;
	}
	cd.vContempts[0]=vContempt; cd.vContempts[1]=-vContempt;
}

CCache* CPlayerComputer::GetCache(int iCache) {
	if (caches[iCache]==NULL)
		caches[iCache]=new CCache(1<<LogCacheSize(pcp, cd.iPruneMidgame && cd.iPruneEndgame));

	if (caches[iCache]==NULL) {
		std::cerr << "out of memory allocating cache " << iCache << " for computer " << Name() << "\n";
		assert(0);
		exit(-1);
	}

	return caches[iCache];
}

//! Get my move (if it's my move) or my recommended move (if it's the opponent move) and the time taken
//!
//! \return see CPlayer::TCheatcode for a description of cheat codes.
//!
//! \param[in] flags - combination of CPlayer::kMyMove and CPlayer::Game2
//! \param[in] game - game up to the position to evaluate
//! \param[out] mli - move eval and elapsed time are filled in.
CPlayer::TCheatcode CPlayerComputer::GetMoveAndTime(COsGame& game, int flags, COsMoveListItem& mli) {
	CPlayer::TCheatcode result;
	clock_t t=clock();
	result=GetMove(game, flags, mli);
	mli.tElapsed=double(clock()-t)/CLOCKS_PER_SEC;
	return result;
}

//! Get my move (if it's my move) or my recommended move (if it's the opponent move)
//!
//! \return see CPlayer::TCheatcode for a description of cheat codes.
//!
//! \param[in] flags - combination of CPlayer::kMyMove and CPlayer::Game2
//! \param[in] game - game up to the position to evaluate
//! \param[out] mli - move and eval are filled in; elapsed time calculated by the calling routine.
CPlayer::TCheatcode CPlayerComputer::GetMove(COsGame& game, int flags, COsMoveListItem& mli) {
	CMVK chosen;
	double tRemaining;

	COsPosition pos = game.GetPos();
	tRemaining=pos.cks[!pos.board.IsBlackMove()].tCurrent;
	const bool fMyMove=(flags&kMyMove)!=0;
	const bool fAnalyze=(flags&kAnalyze)!=0;

	CQPosition qpos(game.GetPos().board);
	u4 fNeeds=CSearchInfo::kNeedMove;
	if (game.mt.fRand)
		fNeeds|=CSearchInfo::kNeedRandSearch;
	if (fAnalyze)
		fNeeds|=CSearchInfo::kNeedValue;
	CSearchInfo si=DefaultSearchInfo(game.GetPos().board.IsBlackMove(), fNeeds, tRemaining, (flags&kGame2)?1:0);

	if (fMyMove | fAnalyze) {
		if (!fMyMove)
			si.SetPrintLevel(si.GetPrintLevel()|CSearchInfo::kPrintPondering);
		else
			si.SetPrintLevel(si.GetPrintLevel()&~CSearchInfo::kPrintPondering);
		
		GetChosen(si, qpos, chosen);

		extern bool fInTournament;
		if (chosen.fKnown && !fInTournament)
			mli.dEval=chosen.value/(double)kStoneValue;
		else
			mli.dEval=0;

		mli.mv=(std::string)chosen.move;
	}
	else {
		si.SetPrintLevel(cd.fsPrintOpponent);
	}

	return kCheatNone;
}

//! Display evals for the top nBest moves from a position.
void CPlayerComputer::Hint(const CQPosition& pos, int nBest) {
	CMoves moves;
	bool fHasMove = pos.CalcMoves(moves);


	if (nBest>0) {
		const u4 fNeeds=
				 CSearchInfo::kNeedMove
				|CSearchInfo::kNeedValue
				|CSearchInfo::kPrintRound
				|CSearchInfo::kPrintPondering;

		CSearchInfo si=DefaultSearchInfo(pos.BlackMove(), fNeeds, 1e6, 0);
		CMVK mvk;
		Pos2 pos2;
		pos2.Initialize(pos.BitBoard(),pos.BlackMove());
		IterativeValue(pos2, moves, *pcp, si, mvk, false, nBest);
	}
}

//! Return the computer's default search information
CSearchInfo CPlayerComputer::DefaultSearchInfo(bool fBlackMove, u4 fNeeds, double tRemaining, int iCache) const {
	int rs=DefaultRandomness() + cd.nRandShifts[!fBlackMove];
	return CSearchInfo(cd.iPruneMidgame, cd.iPruneEndgame, rs, cd.vContempts[fBlackMove], fNeeds, tRemaining, iCache, cd.fsPrint);
}

void CPlayerComputer::GetChosen(const CSearchInfo& si, const CQPosition& pos, CMVK& mvk) {
	SetParameters(pos, si.iCache);
	assert((mpcs==NULL) || cd.iPruneMidgame<=::mpcs->NPrunes());

	Pos2 pos2;
	pos2.Initialize(pos.BitBoard(),pos.BlackMove());
	TimedMVK(pos2, *pcp, si, mvk, false);

	// Print the move
	if (si.PrintMove()) {
		cout << (si.PrintPondering()?"Predict: ":"=== ")
			<< mvk
			<< setw(3) << pos.NEmpty() << "e: "
			<< (si.PrintPondering()?"":" ===") << "\n";
	}
}

void CPlayerComputer::Clear() {
	int i;
	for (i=0; i<2; i++)
		if (caches[i])
			caches[i]->SetStale();
	solved=false;
}

void CPlayerComputer::StartMatch(const COsMatch& match) {
	cerr << "s";
}

void CPlayerComputer::EndGame(const COsGame& game) {
	extern bool fInTournament;

	// save the game to the saved game file
	if (!m_fnSaveGame.empty()) {
		ofstream os(m_fnSaveGame.c_str(),ios::app);
		os << game << "\n";
	}

	if (!game.mt.fRand) {
		// defer analysis of the game if we're in a tournament
		if (fInTournament) {
			ofstream os((string(Name())+"_deferred.ggf").c_str(),ios::app);
			os << game << "\n";
		}
	}
}


static void prepareCache() {
	tSetStale=::cache->NBuckets()*1E-7/dGHz;
}

void CPlayerComputer::SetParameters(const CQPosition& pos, int iCache) {
	// Set up parameters
	::cache=GetCache(iCache);

	// we may want to copy the other cache, if the position is a predecessor
	posCached[iCache]=pos;
	fHasCachedPos[iCache]=true;
	if (caches[!iCache] && fHasCachedPos[!iCache] && posCached[!iCache].IsSuccessor(pos)) {
		caches[iCache]->CopyData(*(caches[!iCache]));
		cout << "Cache copy!\n";
	}

	prepareCache();
	//::iPruneMidgame = iPruneMidgame;
	//::iPruneEndgame = iPruneEndgame;
	::mpcs = mpcs;
	::evaluator = eval;
}

void CPlayerComputer::SetParameters(int iCache) {
	// Set up parameters
	::cache=GetCache(iCache);

	prepareCache();
	//::iPruneMidgame = iPruneMidgame;
	//::iPruneEndgame = iPruneEndgame;
	::mpcs = mpcs;
	::evaluator = eval;
}

bool CPlayerComputer::IsHuman() const {
	return false;
}

// default randomness for computer, dependent upon stone value
int CPlayerComputer::DefaultRandomness() {
	int result, sv;

	for (result=-4, sv=kStoneValue; sv; result++, sv>>=1);
	return result;
}

// Copyright Chris Welty
//  All Rights Reserved
// This file is distributed subject to GNU GPL version 3. See the files
// GPLv3.txt and License.txt in the instructions subdirectory for details.

// othello program

#ifdef _MSC_VER 
#pragma warning(disable: 4786)
#include <windows.h>
#endif

#ifdef __MINGW64__
#include <windows.h>
#endif

#include <ctime>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <ctype.h>
#include <sstream>
#include <iomanip>
#include <string>
#include "n64/n64.h"
#include "n64/test.h"
#include "core/NodeStats.h"
#include "core/CalcParams.h"
#include "pattern/FastFlip.h"
#include "PlayerComputer.h"

#include "SmartBook.h"
#include "options.h"
#include "Evaluator.h"
#include "EvalTest.h"
#include "SpeedTest.h"
#include "Pos2.h"
#include "Search.h"

#include "Pos2Test.h"
#include "SearchTest.h"
#include "core/coreTest.h"
#include "odk/odkTest.h"

using namespace std;

#define RANDSEED 8


static std::string fnOpening;

void Init() {
    setbuf(stdout, 0);
    srand(static_cast<unsigned int>(RANDSEED));

    InitFastFlip();
    InitConfigToPotMob();
    cout << setprecision(3);
    cerr << setprecision(3);

    void InitFFBonus();
    InitFFBonus();
    InitForcedOpenings();
}

void Clean() {
    CleanConfigToPotMob();
    CSmartBook::Clean();
}

void Test() {
    std::cerr << "Beginning standard test" << std::endl;

    int n64_main(int argc, char* argv[]);
    n64_main(1, 0); // test n64 package
    testCore();     // test core package
    testOdk();      // test ODK package

    TestPos2();
    TestSearch();
    GoldenValueEvalTest();
    std::cerr << "Ending standard test" << std::endl;
}

bool HasInput() { return false; }

int main(int argc, char**argv, char**envp) {

    try {
      time_t end_time;

      cout << "Ntest version as of " << __DATE__ << "\n";
      cout << "Copyright 1999-2020 Chris Welty and Vlad Petric\nAll Rights Reserved\n\n";

      Init();

      Test();

      CNodeStats start, end;

      start.Read();

      TestMoveSpeed(18, 16);

      time(&end_time);
      end.Read();
      cout << (end-start) << "\n";

      Clean();

      return 0;
    } catch(std::string exception) {
      cout << exception << "\n";
      return -1;
    }
}


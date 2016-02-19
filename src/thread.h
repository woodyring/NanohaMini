/*
  NanohaMini, a USI shogi(japanese-chess) playing engine derived from Stockfish 2.1
  Copyright (C) 2004-2008 Tord Romstad (Glaurung author)
  Copyright (C) 2008-2010 Marco Costalba, Joona Kiiski, Tord Romstad (Stockfish author)
  Copyright (C) 2014 Kazuyuki Kawabata

  NanohaMini is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  NanohaMini is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#if !defined(THREAD_H_INCLUDED)
#define THREAD_H_INCLUDED

#include <cstring>

#include "lock.h"
#if !defined(NANOHA)
#include "material.h"
#endif
#include "movepick.h"
#if !defined(NANOHA)
#include "pawns.h"
#endif
#include "position.h"

#if defined(NANOHA)
const int MAX_THREADS = 4;
#else
const int MAX_THREADS = 32;
#endif
const int MAX_ACTIVE_SPLIT_POINTS = 8;

struct SplitPoint {

  // Const data after splitPoint has been setup
  SplitPoint* parent;
  const Position* pos;
  Depth depth;
  Value beta;
  int nodeType;
  int ply;
  int master;
  Move threatMove;

  // Const pointers to shared data
  MovePicker* mp;
  SearchStack* ss;

  // Shared data
  Lock lock;
  volatile int64_t nodes;
  volatile Value alpha;
  volatile Value bestValue;
  volatile int moveCount;
  volatile bool is_betaCutoff;
  volatile bool is_slave[MAX_THREADS];
};


/// Thread struct is used to keep together all the thread related stuff like locks,
/// state and especially split points. We also use per-thread pawn and material hash
/// tables so that once we get a pointer to an entry its life time is unlimited and
/// we don't have to care about someone changing the entry under our feet.

struct Thread {

  void wake_up();
  bool cutoff_occurred() const;
  bool is_available_to(int master) const;
  void idle_loop(SplitPoint* sp);

  SplitPoint splitPoints[MAX_ACTIVE_SPLIT_POINTS];
#if !defined(NANOHA)
  MaterialInfoTable materialTable;
  PawnInfoTable pawnTable;
#endif
  int threadID;
  int maxPly;
  Lock sleepLock;
  WaitCondition sleepCond;
  SplitPoint* volatile splitPoint;
  volatile int activeSplitPoints;
  volatile bool is_searching;
  volatile bool do_sleep;
  volatile bool do_terminate;

#if defined(_MSC_VER)
  HANDLE handle;
#else
  pthread_t handle;
#endif
};


/// ThreadsManager class is used to handle all the threads related stuff like init,
/// starting, parking and, the most important, launching a slave thread at a split
/// point. All the access to shared thread data is done through this class.

class ThreadsManager {
  /* As long as the single ThreadsManager object is defined as a global we don't
     need to explicitly initialize to zero its data members because variables with
     static storage duration are automatically set to zero before enter main()
  */
public:
  Thread& operator[](int threadID) { return threads[threadID]; }
  void init();
  void exit();

  bool use_sleeping_threads() const { return useSleepingThreads; }
  int min_split_depth() const { return minimumSplitDepth; }
  int size() const { return activeThreads; }

  void set_size(int cnt);
  void read_uci_options();
  bool available_slave_exists(int master) const;

  template <bool Fake>
  Value split(Position& pos, SearchStack* ss, Value alpha, Value beta, Value bestValue,
              Depth depth, Move threatMove, int moveCount, MovePicker* mp, int nodeType);
private:
  Thread threads[MAX_THREADS];
  Lock threadsLock;
  Depth minimumSplitDepth;
  int maxThreadsPerSplitPoint;
  int activeThreads;
  bool useSleepingThreads;
};

extern ThreadsManager Threads;

#endif // !defined(THREAD_H_INCLUDED)

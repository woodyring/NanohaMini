/*
  NanohaMini, a USI shogi(japanese-chess) playing engine derived from Stockfish 2.1
  Copyright (C) 2004-2008 Tord Romstad (Glaurung author)
  Copyright (C) 2008-2010 Marco Costalba, Joona Kiiski, Tord Romstad (Stockfish author)
  Copyright (C) 2014-2015 Kazuyuki Kawabata

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

#if !defined MOVEPICK_H_INCLUDED
#define MOVEPICK_H_INCLUDED

#include "history.h"
#include "move.h"
#include "position.h"
#include "types.h"

struct SearchStack;

/// MovePicker is a class which is used to pick one pseudo legal move at a time
/// from the current position. It is initialized with a Position object and a few
/// moves we have reason to believe are good. The most important method is
/// MovePicker::get_next_move(), which returns a new pseudo legal move each time
/// it is called, until there are no moves left, when MOVE_NONE is returned.
/// In order to improve the efficiency of the alpha beta algorithm, MovePicker
/// attempts to return the moves which are most likely to get a cut-off first.

class MovePicker {

	MovePicker& operator=(const MovePicker&); // Silence a warning under MSVC

public:
	MovePicker(const Position&, Move, Depth, const History&, SearchStack*, Value);
	MovePicker(const Position&, Move, Depth, const History&, Square recaptureSq);
	MovePicker(const Position&, Move, const History&, PieceType parentCapture);
	Move get_next_move();

private:
	void score_captures();
	void score_noncaptures();
	void score_evasions();
	void go_next_phase();

	const Position& pos;
	const History& H;
	Depth depth;
	Move ttMove;
	MoveStack killers[2];
	Square recaptureSquare;
	int captureThreshold, phase;
	const uint8_t* phasePtr;
	MoveStack *curMove, *lastMove, *lastNonCapture, *badCaptures;
	MoveStack moves[MAX_MOVES];
};

#endif // !defined(MOVEPICK_H_INCLUDED)

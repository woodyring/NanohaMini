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

#if !defined(MOVEGEN_H_INCLUDED)
#define MOVEGEN_H_INCLUDED

#include "move.h"

enum MoveType {
	MV_CAPTURE,             // ‹î‚ğæ‚éè
	MV_NON_CAPTURE,         // ‹î‚ğæ‚ç‚È‚¢è
	MV_CHECK,               // ‰¤è
	MV_NON_CAPTURE_CHECK,   // ‹î‚ğæ‚ç‚È‚¢‰¤è
	MV_EVASION,             // ‰¤è‰ñ”ğè
	MV_NON_EVASION,         // ‰¤è‚ª‚©‚©‚Á‚Ä‚¢‚È‚¢‚Ì‡–@è¶¬
	MV_LEGAL                // ‡–@è¶¬
};

template<MoveType>
MoveStack* generate(const Position& pos, MoveStack* mlist);

/// The MoveList struct is a simple wrapper around generate(), sometimes comes
/// handy to use this class instead of the low level generate() function.
template<MoveType T>
struct MoveList {

	explicit MoveList(const Position& pos) : cur(mlist), last(generate<T>(pos, mlist)) {}
	void operator++() { cur++; }
	bool end() const { return cur == last; }
	Move move() const { return cur->move; }
	int size() const { return int(last - mlist); }

private:
	MoveStack mlist[MAX_MOVES];
	MoveStack *cur, *last;
};

#endif // !defined(MOVEGEN_H_INCLUDED)

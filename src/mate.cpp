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

#include <cassert>
#include "movegen.h"
#include "position.h"

#define USE_M3HASH

#if defined(USE_M3HASH)
#define MATE3_MASK              0x07ffffU	/* 8 * 64K = 512K entry */

namespace {
struct Mate3_Hash {
	uint64_t word1, word2;				// word1:key, word2:0-31=black_hand, word2:32-63=move
} mate3_hash_tbl[ MATE3_MASK + 1 ];

bool probe_m3hash(const Position& pos, Move &m)
{
	uint64_t word1, word2;
	const uint64_t key = pos.get_key();
	const uint32_t h = pos.handValue<BLACK>();
	const uint32_t index = static_cast<uint32_t>(key) ^ h ^ (h >> 15);
	const Mate3_Hash now = mate3_hash_tbl[index & MATE3_MASK];
	word1 = now.word1 ^ now.word2;
	word2 = now.word2;

	if (word1 != key || static_cast<uint32_t>(word2) != pos.handValue<BLACK>()) {
		return false;
	}
	m = static_cast<Move>(word2 >> 32);
	return true;
}

void store_m3hash(const Position& pos, const Move m)
{
	Mate3_Hash now;
	const uint64_t key = pos.get_key();
	const uint32_t h = pos.handValue<BLACK>();
	now.word2 = static_cast<uint64_t>(h) | (static_cast<uint64_t>(m) << 32);
	now.word1 = key ^ now.word2;

	const uint32_t index = static_cast<uint32_t>(key) ^ h ^ (h >> 15);
	mate3_hash_tbl[index & MATE3_MASK] = now;
}

}

#endif

//
//  �ʂƂ̈ʒu�֌W
//
// +---+---+---+---+---+
// |+30|+14| -2|-18|-34|
// +---+---+---+---+---+
// |+31|+15| -1|-17|-33|
// +---+---+---+---+---+
// |+32|+16| ��|-16|-32|
// +---+---+---+---+---+
// |+33|+17| +1|-15|-31|
// +---+---+---+---+---+
// |+34|+18| +2|-14|-30|
// +---+---+---+---+---+
// |+35|+19| +3|-13|-29|
// +---+---+---+---+---+
// |+36|+20| +4|-12|-28|
// +---+---+---+---+---+
//
//
// ���̃e�[�u���ł͕��A�j�A��A���A�ƁA�����A���j�A����݂̂�����.
// ���A�p�A��A�n�A���͕ʂŏ�������.
const struct Position::ST_OuteMove2 Position::OuteMove2[32] = {
	// +36�̈ʒu�̋�牤��.
	{+36, {
		{+18,	// �j�̂�
			(0<<FU)|(0<<KY)|(1<<KE)|(0<<GI)|(0<<KI)|(0<<KA)|(0<<HI)| (0x00 << TO) |(0<<UM)|(0<<RY),
			(0<<FU)|(0<<KY)|(0<<KE)|(0<<GI)        |(0<<KA)|(0<<HI)},
		{0,0,0},
	}},

	//  +4�̈ʒu�̋�牤��.
	{ +4, {
		{+18,	// �j�̂�
			(0<<FU)|(0<<KY)|(1<<KE)|(0<<GI)|(0<<KI)|(0<<KA)|(0<<HI)| (0x00 << TO) |(0<<UM)|(0<<RY),
			(0<<FU)|(0<<KY)|(0<<KE)|(0<<GI)        |(0<<KA)|(0<<HI)},
		{-14,	// �j�̂�
			(0<<FU)|(0<<KY)|(1<<KE)|(0<<GI)|(0<<KI)|(0<<KA)|(0<<HI)| (0x00 << TO) |(0<<UM)|(0<<RY),
			(0<<FU)|(0<<KY)|(0<<KE)|(0<<GI)        |(0<<KA)|(0<<HI)},
		{0,0,0},
	}},

	// -28�̈ʒu�̋�牤��.
	{-28, {
		{-14,	// �j�̂�
			(0<<FU)|(0<<KY)|(1<<KE)|(0<<GI)|(0<<KI)|(0<<KA)|(0<<HI)| (0x00 << TO) |(0<<UM)|(0<<RY),
			(0<<FU)|(0<<KY)|(0<<KE)|(0<<GI)        |(0<<KA)|(0<<HI)},
		{0,0,0},
	}},

	// +35�̈ʒu�̋�牤��.
	{+35, {
		{+17,	// �j���̂�
			(0<<FU)|(0<<KY)|(0<<KE)|(0<<GI)|(0<<KI)|(0<<KA)|(0<<HI)| (0x00 << TO) |(0<<UM)|(0<<RY),
			(0<<FU)|(0<<KY)|(1<<KE)|(0<<GI)        |(0<<KA)|(0<<HI)},
		{0,0,0},
	}},

	// +19�̈ʒu�̋�牤��.
	{+19, {
		{ +1,	// �j���̂�
			(0<<FU)|(0<<KY)|(0<<KE)|(0<<GI)|(0<<KI)|(0<<KA)|(0<<HI)| (0x00 << TO) |(0<<UM)|(0<<RY),
			(0<<FU)|(0<<KY)|(1<<KE)|(0<<GI)        |(0<<KA)|(0<<HI)},
		{0,0,0},
	}},

	//  +3�̈ʒu�̋�牤��.
	{ +3, {
		{+17,	// �j���̂�
			(0<<FU)|(0<<KY)|(0<<KE)|(0<<GI)|(0<<KI)|(0<<KA)|(0<<HI)| (0x00 << TO) |(0<<UM)|(0<<RY),
			(0<<FU)|(0<<KY)|(1<<KE)|(0<<GI)        |(0<<KA)|(0<<HI)},
		{-15,	// �j���̂�
			(0<<FU)|(0<<KY)|(0<<KE)|(0<<GI)|(0<<KI)|(0<<KA)|(0<<HI)| (0x00 << TO) |(0<<UM)|(0<<RY),
			(0<<FU)|(0<<KY)|(1<<KE)|(0<<GI)        |(0<<KA)|(0<<HI)},
		{0,0,0},
	}},

	// -13�̈ʒu�̋�牤��.
	{-13, {
		{ +1,	// �j���̂�
			(0<<FU)|(0<<KY)|(0<<KE)|(0<<GI)|(0<<KI)|(0<<KA)|(0<<HI)| (0x00 << TO) |(0<<UM)|(0<<RY),
			(0<<FU)|(0<<KY)|(1<<KE)|(0<<GI)        |(0<<KA)|(0<<HI)},
		{0,0,0},
	}},

	// -29�̈ʒu�̋�牤��.
	{-29, {
		{-15,	// �j���̂�
			(0<<FU)|(0<<KY)|(0<<KE)|(0<<GI)|(0<<KI)|(0<<KA)|(0<<HI)| (0x00 << TO) |(0<<UM)|(0<<RY),
			(0<<FU)|(0<<KY)|(1<<KE)|(0<<GI)        |(0<<KA)|(0<<HI)},
		{0,0,0},
	}},

	// +34�̈ʒu�̋�牤��.
	{+34, {
		{+17,
			(0<<FU)|(0<<KY)|(0<<KE)|(1<<GI)|(1<<KI)|(0<<KA)|(0<<HI)| (0x0F << TO) |(0<<UM)|(1<<RY),
			(0<<FU)|(0<<KY)|(0<<KE)|(1<<GI)        |(0<<KA)|(0<<HI)},
		{+16,	// �j���̂�
			(0<<FU)|(0<<KY)|(0<<KE)|(0<<GI)|(0<<KI)|(0<<KA)|(0<<HI)| (0x00 << TO) |(0<<UM)|(0<<RY),
			(0<<FU)|(0<<KY)|(1<<KE)|(0<<GI)        |(0<<KA)|(0<<HI)},
		{0,0,0},
	}},

	// +18�̈ʒu�̋�牤��.
	{+18, {
		{+17,
			(0<<FU)|(0<<KY)|(0<<KE)|(1<<GI)|(1<<KI)|(0<<KA)|(0<<HI)| (0x0F << TO) |(1<<UM)|(1<<RY),
			(1<<FU)|(0<<KY)|(0<<KE)|(1<<GI)        |(0<<KA)|(0<<HI)},
		{ +1,
			(0<<FU)|(0<<KY)|(0<<KE)|(1<<GI)|(1<<KI)|(0<<KA)|(0<<HI)| (0x0F << TO) |(1<<UM)|(0<<RY),
			(0<<FU)|(0<<KY)|(0<<KE)|(1<<GI)        |(0<<KA)|(0<<HI)},
		{0,0,0},
	}},

	//  +2�̈ʒu�̋�牤��.
	{ +2, {
		{+17,
			(0<<FU)|(0<<KY)|(0<<KE)|(1<<GI)|(1<<KI)|(0<<KA)|(0<<HI)| (0x0F << TO) |(0<<UM)|(1<<RY),
			(0<<FU)|(0<<KY)|(0<<KE)|(1<<GI)        |(0<<KA)|(0<<HI)},
		{-15,
			(0<<FU)|(0<<KY)|(0<<KE)|(1<<GI)|(1<<KI)|(0<<KA)|(0<<HI)| (0x0F << TO) |(0<<UM)|(1<<RY),
			(0<<FU)|(0<<KY)|(0<<KE)|(1<<GI)        |(0<<KA)|(0<<HI)},
		{+16,	// �j���̂�
			(0<<FU)|(0<<KY)|(0<<KE)|(0<<GI)|(0<<KI)|(0<<KA)|(0<<HI)| (0x00 << TO) |(0<<UM)|(0<<RY),
			(0<<FU)|(0<<KY)|(1<<KE)|(0<<GI)        |(0<<KA)|(0<<HI)},
		{-16,	// �j���̂�
			(0<<FU)|(0<<KY)|(0<<KE)|(0<<GI)|(0<<KI)|(0<<KA)|(0<<HI)| (0x00 << TO) |(0<<UM)|(0<<RY),
			(0<<FU)|(0<<KY)|(1<<KE)|(0<<GI)        |(0<<KA)|(0<<HI)},
		{ +1,
			(1<<FU)|(0<<KY)|(0<<KE)|(1<<GI)|(1<<KI)|(0<<KA)|(0<<HI)| (0x0F << TO) |(1<<UM)|(0<<RY),
			(1<<FU)|(0<<KY)|(0<<KE)|(1<<GI)        |(0<<KA)|(0<<HI)},
		{0,0,0},
	}},

	// 11:-14�̈ʒu�̋�牤��.
	{-14, {
		{-15,
			(0<<FU)|(0<<KY)|(0<<KE)|(1<<GI)|(1<<KI)|(0<<KA)|(0<<HI)| (0x0F << TO) |(1<<UM)|(1<<RY),
			(1<<FU)|(0<<KY)|(0<<KE)|(1<<GI)        |(0<<KA)|(0<<HI)},
		{ +1,
			(0<<FU)|(0<<KY)|(0<<KE)|(1<<GI)|(1<<KI)|(0<<KA)|(0<<HI)| (0x0F << TO) |(1<<UM)|(0<<RY),
			(0<<FU)|(0<<KY)|(0<<KE)|(1<<GI)        |(0<<KA)|(0<<HI)},
		{0,0,0},
	}},

	// 12:-30�̈ʒu�̋�牤��.
	{-30, {
		{-15,
			(0<<FU)|(0<<KY)|(0<<KE)|(1<<GI)|(1<<KI)|(0<<KA)|(0<<HI)| (0x0F << TO) |(0<<UM)|(1<<RY),
			(0<<FU)|(0<<KY)|(0<<KE)|(1<<GI)        |(0<<KA)|(0<<HI)},
		{-16,	// �j���̂�
			(0<<FU)|(0<<KY)|(0<<KE)|(0<<GI)|(0<<KI)|(0<<KA)|(0<<HI)| (0x00 << TO) |(0<<UM)|(0<<RY),
			(0<<FU)|(0<<KY)|(1<<KE)|(0<<GI)        |(0<<KA)|(0<<HI)},
		{0,0,0},
	}},

	// 13:+33�̈ʒu�̋�牤��.
	{+33, {
		{+17,
			(0<<FU)|(0<<KY)|(0<<KE)|(0<<GI)|(1<<KI)|(0<<KA)|(0<<HI)| (0x0F << TO) |(1<<UM)|(1<<RY),
			(0<<FU)|(0<<KY)|(0<<KE)|(0<<GI)        |(0<<KA)|(0<<HI)},
		{+16,
			(0<<FU)|(0<<KY)|(0<<KE)|(0<<GI)|(1<<KI)|(0<<KA)|(0<<HI)| (0x0F << TO) |(1<<UM)|(1<<RY),
			(0<<FU)|(0<<KY)|(0<<KE)|(1<<GI)        |(0<<KA)|(0<<HI)},
		{0,0,0},
	}},

	// 14:+17�̈ʒu�̋�牤��.
	{+17, {
		{+16,
			(0<<FU)|(0<<KY)|(0<<KE)|(0<<GI)|(0<<KI)|(0<<KA)|(0<<HI)| (0x00 << TO) |(0<<UM)|(0<<RY),
			(1<<FU)|(0<<KY)|(0<<KE)|(0<<GI)        |(0<<KA)|(0<<HI)},
		{ -1,	// �j���̂�
			(0<<FU)|(0<<KY)|(0<<KE)|(0<<GI)|(0<<KI)|(0<<KA)|(0<<HI)| (0x00 << TO) |(0<<UM)|(0<<RY),
			(0<<FU)|(0<<KY)|(1<<KE)|(0<<GI)        |(0<<KA)|(0<<HI)},
		{0,0,0},
	}},

	//  +1�̈ʒu�̋�牤��.	�˂Ȃ�
	// 15:-15�̈ʒu�̋�牤��.
	{-15, {
		{-16,
			(0<<FU)|(0<<KY)|(0<<KE)|(0<<GI)|(0<<KI)|(0<<KA)|(0<<HI)| (0x00 << TO) |(0<<UM)|(0<<RY),
			(1<<FU)|(0<<KY)|(0<<KE)|(0<<GI)        |(0<<KA)|(0<<HI)},
		{ -1,	// �j���̂�
			(0<<FU)|(0<<KY)|(0<<KE)|(0<<GI)|(0<<KI)|(0<<KA)|(0<<HI)| (0x00 << TO) |(0<<UM)|(0<<RY),
			(0<<FU)|(0<<KY)|(1<<KE)|(0<<GI)        |(0<<KA)|(0<<HI)},
		{0,0,0},
	}},

	// 16:-31�̈ʒu�̋�牤��.
	{-31, {
		{-15,
			(0<<FU)|(0<<KY)|(0<<KE)|(0<<GI)|(1<<KI)|(0<<KA)|(0<<HI)| (0x0F << TO) |(1<<UM)|(1<<RY),
			(0<<FU)|(0<<KY)|(0<<KE)|(0<<GI)        |(0<<KA)|(0<<HI)},
		{-16,
			(0<<FU)|(0<<KY)|(0<<KE)|(0<<GI)|(1<<KI)|(0<<KA)|(0<<HI)| (0x0F << TO) |(1<<UM)|(1<<RY),
			(0<<FU)|(0<<KY)|(0<<KE)|(1<<GI)        |(0<<KA)|(0<<HI)},
		{0,0,0},
	}},

	// 17:+32�̈ʒu�̋�牤��.
	{+32, {
		{+17,
			(0<<FU)|(0<<KY)|(0<<KE)|(1<<GI)|(0<<KI)|(0<<KA)|(0<<HI)| (0x00 << TO) |(0<<UM)|(1<<RY),
			(0<<FU)|(0<<KY)|(0<<KE)|(1<<GI)        |(0<<KA)|(0<<HI)},
		{+16,
			(0<<FU)|(0<<KY)|(0<<KE)|(0<<GI)|(1<<KI)|(0<<KA)|(0<<HI)| (0x0F << TO) |(1<<UM)|(0<<RY),
			(0<<FU)|(0<<KY)|(0<<KE)|(0<<GI)        |(0<<KA)|(0<<HI)},
		{+15,
			(0<<FU)|(0<<KY)|(0<<KE)|(1<<GI)|(0<<KI)|(0<<KA)|(0<<HI)| (0x00 << TO) |(0<<UM)|(1<<RY),
			(0<<FU)|(0<<KY)|(0<<KE)|(0<<GI)        |(0<<KA)|(0<<HI)},
		{0,0,0},
	}},

	// 18:+16�̈ʒu�̋�牤��.
	{+16, {
		{+15,
			(0<<FU)|(0<<KY)|(0<<KE)|(1<<GI)|(0<<KI)|(0<<KA)|(0<<HI)| (0x00 << TO) |(0<<UM)|(0<<RY),
			(0<<FU)|(0<<KY)|(0<<KE)|(0<<GI)        |(0<<KA)|(0<<HI)},
		{ +1,
			(0<<FU)|(0<<KY)|(0<<KE)|(1<<GI)|(0<<KI)|(0<<KA)|(0<<HI)| (0x00 << TO) |(0<<UM)|(0<<RY),
			(0<<FU)|(0<<KY)|(0<<KE)|(1<<GI)        |(0<<KA)|(0<<HI)},
		{ -1,
			(0<<FU)|(0<<KY)|(0<<KE)|(0<<GI)|(0<<KI)|(0<<KA)|(0<<HI)| (0x00 << TO) |(0<<UM)|(0<<RY),
			(0<<FU)|(0<<KY)|(0<<KE)|(1<<GI)        |(0<<KA)|(0<<HI)},
		{0,0,0},
	}},

	// 19:-16�̈ʒu�̋�牤��.
	{-16, {
		{-17,
			(0<<FU)|(0<<KY)|(0<<KE)|(1<<GI)|(0<<KI)|(0<<KA)|(0<<HI)| (0x00 << TO) |(0<<UM)|(0<<RY),
			(0<<FU)|(0<<KY)|(0<<KE)|(0<<GI)        |(0<<KA)|(0<<HI)},
		{ +1,
			(0<<FU)|(0<<KY)|(0<<KE)|(1<<GI)|(0<<KI)|(0<<KA)|(0<<HI)| (0x00 << TO) |(0<<UM)|(0<<RY),
			(0<<FU)|(0<<KY)|(0<<KE)|(1<<GI)        |(0<<KA)|(0<<HI)},
		{ -1,
			(0<<FU)|(0<<KY)|(0<<KE)|(0<<GI)|(0<<KI)|(0<<KA)|(0<<HI)| (0x00 << TO) |(0<<UM)|(0<<RY),
			(0<<FU)|(0<<KY)|(0<<KE)|(1<<GI)        |(0<<KA)|(0<<HI)},
		{0,0,0},
	}},

	// 20:-32�̈ʒu�̋�牤��.
	{-32, {
		{-15,
			(0<<FU)|(0<<KY)|(0<<KE)|(1<<GI)|(0<<KI)|(0<<KA)|(0<<HI)| (0x00 << TO) |(0<<UM)|(1<<RY),
			(0<<FU)|(0<<KY)|(0<<KE)|(1<<GI)        |(0<<KA)|(0<<HI)},
		{-16,
			(0<<FU)|(0<<KY)|(0<<KE)|(0<<GI)|(1<<KI)|(0<<KA)|(0<<HI)| (0x0F << TO) |(1<<UM)|(0<<RY),
			(0<<FU)|(0<<KY)|(0<<KE)|(0<<GI)        |(0<<KA)|(0<<HI)},
		{-17,
			(0<<FU)|(0<<KY)|(0<<KE)|(1<<GI)|(0<<KI)|(0<<KA)|(0<<HI)| (0x00 << TO) |(0<<UM)|(1<<RY),
			(0<<FU)|(0<<KY)|(0<<KE)|(0<<GI)        |(0<<KA)|(0<<HI)},
		{0,0,0},
	}},

	// 21:+31�̈ʒu�̋�牤��.
	{+31, {
		{+16,
			(0<<FU)|(0<<KY)|(0<<KE)|(0<<GI)|(0<<KI)|(0<<KA)|(0<<HI)| (0x00 << TO) |(1<<UM)|(1<<RY),
			(0<<FU)|(0<<KY)|(0<<KE)|(1<<GI)        |(0<<KA)|(0<<HI)},
		{+15,
			(0<<FU)|(0<<KY)|(0<<KE)|(0<<GI)|(0<<KI)|(0<<KA)|(0<<HI)| (0x00 << TO) |(1<<UM)|(1<<RY),
			(0<<FU)|(0<<KY)|(0<<KE)|(0<<GI)        |(0<<KA)|(0<<HI)},
		{0,0,0},
	}},

	// 22:+15�̈ʒu�̋�牤��.
	{+15, {
		{+16,
			(0<<FU)|(0<<KY)|(0<<KE)|(0<<GI)|(1<<KI)|(0<<KA)|(0<<HI)| (0x0F << TO) |(0<<UM)|(0<<RY),
			(0<<FU)|(0<<KY)|(0<<KE)|(0<<GI)        |(0<<KA)|(0<<HI)},
		{ -1,
			(0<<FU)|(0<<KY)|(0<<KE)|(0<<GI)|(1<<KI)|(0<<KA)|(0<<HI)| (0x0F << TO) |(0<<UM)|(0<<RY),
			(0<<FU)|(0<<KY)|(0<<KE)|(0<<GI)        |(0<<KA)|(0<<HI)},
		{0,0,0},
	}},

	// 23: -1�̈ʒu�̋�牤��.
	{ -1, {
		{+16,
			(0<<FU)|(0<<KY)|(0<<KE)|(0<<GI)|(0<<KI)|(0<<KA)|(0<<HI)| (0x00 << TO) |(0<<UM)|(0<<RY),
			(0<<FU)|(0<<KY)|(0<<KE)|(1<<GI)        |(0<<KA)|(0<<HI)},
		{-16,
			(0<<FU)|(0<<KY)|(0<<KE)|(0<<GI)|(0<<KI)|(0<<KA)|(0<<HI)| (0x00 << TO) |(0<<UM)|(0<<RY),
			(0<<FU)|(0<<KY)|(0<<KE)|(1<<GI)        |(0<<KA)|(0<<HI)},
		{0,0,0},
	}},

	// 24:-17�̈ʒu�̋�牤��.
	{-17, {
		{-16,
			(0<<FU)|(0<<KY)|(0<<KE)|(0<<GI)|(1<<KI)|(0<<KA)|(0<<HI)| (0x0F << TO) |(0<<UM)|(0<<RY),
			(0<<FU)|(0<<KY)|(0<<KE)|(0<<GI)        |(0<<KA)|(0<<HI)},
		{ -1,
			(0<<FU)|(0<<KY)|(0<<KE)|(0<<GI)|(1<<KI)|(0<<KA)|(0<<HI)| (0x0F << TO) |(0<<UM)|(0<<RY),
			(0<<FU)|(0<<KY)|(0<<KE)|(0<<GI)        |(0<<KA)|(0<<HI)},
		{0,0,0},
	}},

	// 25:-33�̈ʒu�̋�牤��.
	{-33, {
		{-16,
			(0<<FU)|(0<<KY)|(0<<KE)|(0<<GI)|(0<<KI)|(0<<KA)|(0<<HI)| (0x00 << TO) |(1<<UM)|(1<<RY),
			(0<<FU)|(0<<KY)|(0<<KE)|(1<<GI)        |(0<<KA)|(0<<HI)},
		{-17,
			(0<<FU)|(0<<KY)|(0<<KE)|(0<<GI)|(0<<KI)|(0<<KA)|(0<<HI)| (0x00 << TO) |(1<<UM)|(1<<RY),
			(0<<FU)|(0<<KY)|(0<<KE)|(0<<GI)        |(0<<KA)|(0<<HI)},
		{0,0,0},
	}},

	// 26:+30�̈ʒu�̋�牤��.
	{+30, {
		{+15,
			(0<<FU)|(0<<KY)|(0<<KE)|(1<<GI)|(0<<KI)|(0<<KA)|(0<<HI)| (0x00 << TO) |(0<<UM)|(1<<RY),
			(0<<FU)|(0<<KY)|(0<<KE)|(0<<GI)        |(0<<KA)|(0<<HI)},
		{0,0,0},
	}},

	// 27:+14�̈ʒu�̋�牤��.
	{+14, {
		{+15,
			(0<<FU)|(0<<KY)|(0<<KE)|(0<<GI)|(0<<KI)|(0<<KA)|(0<<HI)| (0x00 << TO) |(1<<UM)|(1<<RY),
			(0<<FU)|(0<<KY)|(0<<KE)|(0<<GI)        |(0<<KA)|(0<<HI)},
		{ -1,
			(0<<FU)|(0<<KY)|(0<<KE)|(0<<GI)|(0<<KI)|(0<<KA)|(0<<HI)| (0x00 << TO) |(1<<UM)|(1<<RY),
			(0<<FU)|(0<<KY)|(0<<KE)|(1<<GI)        |(0<<KA)|(0<<HI)},
		{0,0,0},
	}},

	// 28: -2�̈ʒu�̋�牤��.
	{ -2, {
		{+15,
			(0<<FU)|(0<<KY)|(0<<KE)|(1<<GI)|(0<<KI)|(0<<KA)|(0<<HI)| (0x00 << TO) |(0<<UM)|(1<<RY),
			(0<<FU)|(0<<KY)|(0<<KE)|(0<<GI)        |(0<<KA)|(0<<HI)},
		{-17,
			(0<<FU)|(0<<KY)|(0<<KE)|(1<<GI)|(0<<KI)|(0<<KA)|(0<<HI)| (0x00 << TO) |(0<<UM)|(1<<RY),
			(0<<FU)|(0<<KY)|(0<<KE)|(0<<GI)        |(0<<KA)|(0<<HI)},
		{ -1,
			(0<<FU)|(0<<KY)|(0<<KE)|(0<<GI)|(1<<KI)|(0<<KA)|(0<<HI)| (0x0F << TO) |(1<<UM)|(0<<RY),
			(0<<FU)|(0<<KY)|(0<<KE)|(0<<GI)        |(0<<KA)|(0<<HI)},
		{0,0,0},
	}},

	// 29:-18�̈ʒu�̋�牤��.
	{-18, {
		{-17,
			(0<<FU)|(0<<KY)|(0<<KE)|(0<<GI)|(0<<KI)|(0<<KA)|(0<<HI)| (0x00 << TO) |(1<<UM)|(1<<RY),
			(0<<FU)|(0<<KY)|(0<<KE)|(0<<GI)        |(0<<KA)|(0<<HI)},
		{ -1,
			(0<<FU)|(0<<KY)|(0<<KE)|(0<<GI)|(0<<KI)|(0<<KA)|(0<<HI)| (0x00 << TO) |(1<<UM)|(1<<RY),
			(0<<FU)|(0<<KY)|(0<<KE)|(1<<GI)        |(0<<KA)|(0<<HI)},
		{0,0,0},
	}},

	// 30:-34�̈ʒu�̋�牤��.
	{-34, {
		{-17,
			(0<<FU)|(0<<KY)|(0<<KE)|(1<<GI)|(0<<KI)|(0<<KA)|(0<<HI)| (0x00 << TO) |(0<<UM)|(1<<RY),
			(0<<FU)|(0<<KY)|(0<<KE)|(0<<GI)        |(0<<KA)|(0<<HI)},
		{0,0,0},
	}},

	// �I�[.
	{ 0, {
		{0,0,0},
	}},
};

// �Տ�̒��т̂����ł̉���𐶐�����.
MoveStack* Position::gen_check_long(const Color us, MoveStack* mlist) const
{
	// ��ԑ����猩�āA����̋ʂ̈ʒu
	const int enemyKing = (us == BLACK) ? kingG : kingS;
	if (enemyKing == 0) return mlist;

	const int ksuji = enemyKing & 0xF0;
	const int kdan  = enemyKing & 0x0F;
	int suji, dan;
	int i;
	int pos;
	int dir;
	int to, from;
	const int SorG = (us == BLACK) ? SENTE : GOTE;

	// ���e
	// ��E���ɂ�鉤��
		// 1. ��E���Ƌʂ̊Ԃ̋�𓮂��� or ���
		// 2. �򗳂��󂢂Ă����̓��ɂ̂�
		// 3. �򂪐��邱�Ƌy�ї��ɂ�鉤��

	// �p�E�n�ɂ�鉤��
		// 1. �p�E�n�Ƌʂ̊Ԃ̋�𓮂��� or ���
		// 2. �p�E�n���󂢂Ă��铹�ɂ̂邱�Ƃɂ�鉤��
		// 3. �p�����邱�Ƌy�єn�ɂ�鉤��

	// ���Ԃɂ�鉤��
		// 1. ���ԂƋʂ̊Ԃ̋�𓮂��� or ���
		// 2. �������邱�Ƃɂ�鉤��


	// ��E���ɂ�鉤��
	for (i = 0; i < 2; i++) {
		// ����͏������Ȃ�
		if (!OnBoard(hiPos[i])) continue;
		pos = hiPos[i];
		// ����̋�͏������Ȃ�
		if (IsNotOwn(ban[pos], SorG)) continue;

		suji = pos & 0xF0;
		dan  = pos & 0x0F;
		if (suji == ksuji || dan == kdan) {
			// 1. ��E���Ƌʂ̊Ԃ̋�𓮂��� or ��� (�����i or �����؂ɂ��āA�Ԃ̋1��)
			if (dan > kdan) {
				dir = 1;
			} else if (dan < kdan) {
				dir = -1;
			} else if (suji > ksuji) {
				dir = 16;
			} else {
				dir = -16;
			}
			to = enemyKing + dir;
			while (ban[to] == EMP) {
				to = to + dir;
			}
			from = to + dir;
			while (ban[from] == EMP) {
				from = from + dir;
			}
			if (from == hiPos[i]) {
				if (IsNotOwn(ban[to], SorG)) {
					// to�ɂ����͓G
					if ((pin[from]==0 || pin[from]==dir || pin[from]==-dir)) {
						if (ban[from] == SHI && ((to & 0x0F) <= 3 || (from & 0x0F) <= 3)) {
							// �����
							(mlist++)->move = cons_move(from, to, ban[from], ban[to], 1, MOVE_CHECK_LONG);
							// �s����
							(mlist++)->move = cons_move(from, to, ban[from], ban[to], 0, MOVE_CHECK_LONG | MOVE_CHECK_NARAZU);
						} else if ((ban[from] == GHI) && ((to & 0x0F) >= 7 || (from & 0x0F) >= 7)) {
							// �����
							(mlist++)->move = cons_move(from, to, ban[from], ban[to], 1, MOVE_CHECK_LONG);
							// �s����
							(mlist++)->move = cons_move(from, to, ban[from], ban[to], 0, MOVE_CHECK_LONG | MOVE_CHECK_NARAZU);
						} else {
							// �s����
							(mlist++)->move = cons_move(from, to, ban[from], ban[to], 0, MOVE_CHECK_LONG);
						}
					}
				} else {
					// to�ɂ����͖���
					mlist = gen_move_from(us, mlist, to, dir);
				}
			}
		} else {
			// 2. �򗳂��󂢂Ă����̓��ɂ̂�(�����i�⓯���؂ɂ��Ȃ�)
			int sx = Min(ksuji, suji);
			int lx = Max(ksuji, suji);
			int sy = Min(kdan, dan);
			int ly = Max(kdan, dan);
			int x, y;
			bool moveH, moveV;
			moveH = moveV = true;
			for (x = sx + 16; x < lx; x += 16) {
				if (ban[x +  dan] != EMP) moveH = false; 
				if (ban[x + kdan] != EMP) moveV = false;
			}
			for (y = sy + 1; y < ly; y++) {
				if (ban[ksuji + y] != EMP) moveH = false;
				if (ban[suji  + y] != EMP) moveV = false;
			}
			// ��Ԃ����ɓ�������
			if (moveH
				&& (ban[ksuji + dan] == EMP || IsNotOwn(ban[ksuji + dan], SorG)) 
				&& (pin[pos] == 0 || pin[suji+dan] == 16 || pin[suji+dan] == -16)) {
				if ((ban[pos] == SHI && dan <= 3)
					|| (ban[pos] == GHI && dan >= 7)) {
					(mlist++)->move = cons_move(pos, ksuji + dan, ban[pos], ban[ksuji + dan], 1, MOVE_CHECK_LONG);
					(mlist++)->move = cons_move(pos, ksuji + dan, ban[pos], ban[ksuji + dan], 0, MOVE_CHECK_LONG | MOVE_CHECK_NARAZU);
				} else {
					(mlist++)->move = cons_move(pos, ksuji + dan, ban[pos], ban[ksuji + dan], 0, MOVE_CHECK_LONG);
				}
			}
			// ��Ԃ��c�ɓ�������
			if (moveV
				&& (ban[suji + kdan] == EMP || IsNotOwn(ban[suji + kdan], SorG)) 
				&& (pin[pos] == 0 || pin[pos] == 1 || pin[pos] == -1)) {
				if (((ban[pos] == SHI) && (dan <= 3 || kdan <= 3))
					|| ((ban[pos] == GHI) && (dan >= 7 || kdan >= 7))) {
					(mlist++)->move = cons_move(pos, suji + kdan, ban[pos], ban[suji + kdan], 1, MOVE_CHECK_LONG);
					(mlist++)->move = cons_move(pos, suji + kdan, ban[pos], ban[suji + kdan], 0, MOVE_CHECK_LONG | MOVE_CHECK_NARAZU);
				} else {
					(mlist++)->move = cons_move(pos, suji + kdan, ban[pos], ban[suji + kdan], 0, MOVE_CHECK_LONG);
				}
			}
			// �����΂߂ɓ�����𐶐�����y�ۑ�z
			if (ban[pos] & PROMOTED)
			for (int dx = -0x10; dx <= 0x10; dx += 0x20) {
				for (int dy = -1; dy <= 1; dy += 2) {
					if (pin[pos] != 0 && pin[pos] != dx + dy && pin[pos] != -dx - dy) continue;
					if (ban[pos+dx+dy] == WALL || (ban[pos+dx+dy] != EMP && !IsNotOwn(ban[pos + dx + dy], SorG))) continue;	// �����̋�
					if (kdan == dan + dy) {
						//�΂߂ɓ����ĉ��ɉ���
						int sx = Min(ksuji, suji + dx);
						int lx = Max(ksuji, suji + dx);
						moveV = true;
						for (x = sx + 16; x < lx; x += 16) {
							if (ban[x + kdan] != EMP) moveV = false;
						}
						if (moveV) {
							(mlist++)->move = cons_move(pos, pos + dx + dy, ban[pos], ban[pos + dx + dy], 0, MOVE_CHECK_LONG);
						}
					}
					if (ksuji == suji + dx) {
						//�΂߂ɓ����ďc�ɉ���
						int sy = Min(kdan, dan + dy);
						int ly = Max(kdan, dan + dy);
						moveH = true;
						for (y = sy + 1; y < ly; y++) {
							if (ban[y + ksuji] != EMP) moveH = false;
						}
						if (moveH) {
							(mlist++)->move = cons_move(pos, pos + dx + dy, ban[pos], ban[pos + dx + dy], 0, MOVE_CHECK_LONG);
						}
					}
				}
			}

			// 3. �򂪐��邱�Ƌy�ї��ɂ�鉤��
			{
				static const int Directions[]={16,   1, -16, -1};
				static const int Positions[]={-17, -15,  17, 15};

				int i, j;
				for (i = 0; i < 4; i++) {
					to = enemyKing + Positions[i];
					if (ban[to] != EMP && (ban[to] == WALL || !IsNotOwn(ban[to], SorG))) {	// �����̋�
						continue;
					}
					for (j = 0; j < 4; j++) {
						dir = Directions[j];
						from = to + dir;
						while (ban[from] == EMP) {
							from += dir;
						}
						if (from == pos) {
							if (ban[from] == (SorG | RY) && (pin[from] == 0 || pin[from] == dir || pin[from] == -dir)) {
								(mlist++)->move = cons_move(from, to, ban[from], ban[to], 0);
							} else if (ban[from] == (SorG | HI) && (pin[from] == 0 || pin[from] == dir || pin[from] == -dir) && 
								((us == BLACK && ((from & 0x0f) <= 3 || (to & 0x0f) <= 3))
								 ||(us != BLACK && ((from & 0x0f) >= 7 || (to & 0x0f) >= 7)))
								 ) {
								(mlist++)->move = cons_move(from, to, ban[from], ban[to], 1);
							}
						}
					}
				}
			}
		}
	}

	// �p�E�n�ɂ�鉤��
	for (i = 0; i < 2; i++) {
		// ����͏������Ȃ�
		if (!OnBoard(kaPos[i])) continue;
		pos = kaPos[i];
		// ����̋�͏������Ȃ�
		if (IsNotOwn(ban[pos], SorG)) continue;

		suji = pos & 0xF0;
		dan  = pos & 0x0F;

		// 1. �p�E�n�Ƌʂ̊Ԃ̋�𓮂��� or ���
		static const int ka_dir[] = {+15, +17};
		{
			int i;
			for (i = 0; i < static_cast<int>(sizeof(ka_dir)/sizeof(ka_dir[0])); i++) {
				dir = ka_dir[i];
				if ((pos - enemyKing) % dir == 0) {
					// �p�E�n�� dir ����ɋʂ�����
					if (pos < enemyKing) dir = -dir;
					to = enemyKing + dir;
					while (ban[to] == EMP) {
						to += dir;
					}
					if (ban[to] != WALL) {
						from = to + dir;
						while (ban[from] == EMP) {
							from += dir;
						}
						if (from == pos) {
							// �p�E�n�Ƌʂ̊Ԃ�1�����
							if (IsNotOwn(ban[to], SorG)) {
								// to�ɂ����͓G
								if (pin[from] == 0 || pin[from] == dir || pin[from] == -dir) {
									if (ban[from] == SKA
										&& ((to & 0x0F) <= 3 || (from & 0x0F) <= 3)) {
										// �����
										(mlist++)->move = cons_move(from, to, ban[from], ban[to], 1, MOVE_CHECK_LONG);
										(mlist++)->move = cons_move(from, to, ban[from], ban[to], 0, MOVE_CHECK_LONG | MOVE_CHECK_NARAZU);
									} else if (ban[from] == GKA
										&& ((to & 0x0F) >= 7 || (from & 0x0F) >= 7)) {
										// �����
										(mlist++)->move = cons_move(from, to, ban[from], ban[to], 1, MOVE_CHECK_LONG);
										(mlist++)->move = cons_move(from, to, ban[from], ban[to], 0, MOVE_CHECK_LONG | MOVE_CHECK_NARAZU);
									} else {
										(mlist++)->move = cons_move(from, to, ban[from], ban[to], 0, MOVE_CHECK_LONG);
									}
								}
							} else {
								// to�ɂ����͖���
								mlist = gen_move_from(us, mlist, to, dir);
							}
						}
					}
				}
			}
		}

		// ���F2. �p�E�n���󂢂Ă��铹�ɂ̂邱�Ƃɂ�鉤��
		int kx = enemyKing >> 4;
		int ky = enemyKing & 0x0f;
		int bx = suji >> 4;
		int by = dan;
		int k1, k2, b1, b2;
		k1 = ky + kx;
		k2 = ky - kx;
		b1 = by + bx;
		b2 = by - bx;
		int s, l;
		// �꒼���Ȃ�΂����ł͐������Ȃ�
		if (k1 != b1 && k2 != b2) {
			// �p�E�n�Ƌʂ�������ɂȂ�
			if ((k1 & 1) == (b1 & 1)) {
				// �p�̋؂ɋʂ�����
				int x1, y1;
				x1 = (k1 - b2) / 2;
				y1 = (b2 + k1) / 2;
				dir = 17;
				if ((x1 >= 1) && (x1 <= 9) && (y1 >= 1) && (y1 <= 9)
					&& ((pin[pos] == 0) || (pin[pos] == dir) || (pin[pos] == -dir))
					&& (ban[to = ((x1 << 4)+y1)] == EMP || IsNotOwn(ban[to], SorG))) {
					bool moveF = true;
					s = Min(pos, to);
					l = Max(pos, to);
					int j;
					for (j = s + dir; j < l; j += dir) {
						if (ban[j] != EMP) moveF = false;
					}
					s = Min(to, enemyKing);
					l = Max(to, enemyKing);
					dir = 15;
					for (j = s + dir; j < l; j += dir) {
						if (ban[j] != EMP) moveF = false;
					}
					if (moveF) {
						if (((ban[pos] & PROMOTED) == 0)
							&& ((us == BLACK && (y1 <= 3 || dan <= 3))
							  ||(us != BLACK && (y1 >= 7 || dan >= 7)))) {
							(mlist++)->move = cons_move(pos, to, ban[pos], ban[to], 1, MOVE_CHECK_LONG);
							(mlist++)->move = cons_move(pos, to, ban[pos], ban[to], 0, MOVE_CHECK_LONG | MOVE_CHECK_NARAZU);
						} else {
							(mlist++)->move = cons_move(pos, to, ban[pos], ban[to], 0, MOVE_CHECK_LONG);
						}
					}
				}
				x1 = (b1 - k2) / 2;
				y1 = (k2 + b1) / 2;
				dir = 15;
				if ((x1 >= 1) && (x1 <= 9) && (y1 >= 1) && (y1 <= 9)
					&& ((pin[pos] == 0) || (pin[pos] == dir) || (pin[pos] == -dir))
					&& (ban[to = ((x1 << 4)+y1)] == EMP || IsNotOwn(ban[to], SorG))) {
					bool moveF = true;
					s = Min(pos, to);
					l = Max(pos, to);
					int j;
					for (j = s + dir; j < l; j += dir) {
						if (ban[j] != EMP) moveF = false;
					}
					s = Min(to, enemyKing);
					l = Max(to, enemyKing);
					dir = 17;
					for (j = s + dir; j < l; j += dir) {
						if (ban[j] != EMP) moveF = false;
					}
					if (moveF) {
						if (((ban[pos] & PROMOTED) == 0)
							&& ((us == BLACK && (y1 <= 3 || dan <= 3))
							  ||(us != BLACK && (y1 >= 7 || dan >= 7)))) {
							(mlist++)->move = cons_move(pos, to, ban[pos], ban[to], 1, MOVE_CHECK_LONG);
							(mlist++)->move = cons_move(pos, to, ban[pos], ban[to], 0, MOVE_CHECK_LONG | MOVE_CHECK_NARAZU);
						} else {
							(mlist++)->move = cons_move(pos, to, ban[pos], ban[to], 0, MOVE_CHECK_LONG);
						}
					}
				}
			} else {
				// �p�̋؂ɋʂ����Ȃ�
				// �˔n���c���ɓ�����𐶐�����
				if (ban[pos] & PROMOTED) {
					static const int dir[4] = {-16, -1, 1, 16};
					static const int p[4] = {-1, -1, +1, +1};	// y+x �̑���
					static const int m[4] = {+1, -1, +1, -1};	// y-x �̑���
					for (int dirID = 0; dirID < 4; dirID++) {
						if (pin[pos] != 0 && pin[pos] != dir[dirID] && pin[pos] != -dir[dirID]) continue;
						if (ban[pos + dir[dirID]] == WALL || (ban[pos + dir[dirID]] != EMP && !IsNotOwn(ban[pos + dir[dirID]], SorG))) continue;		// �����̋�
						if (k1 == b1 + p[dirID]) {
							//������p�����ɉ���
							int s = Min(enemyKing, pos + dir[dirID]);
							int l = Max(enemyKing, pos + dir[dirID]);
							bool moveF = true;
							for (int j = s + 15; j < l; j += 15) {
								if (ban[j] != EMP) moveF = false;
							}
							if (moveF) {
								(mlist++)->move = cons_move(pos, pos + dir[dirID], ban[pos], ban[pos + dir[dirID]], 0, MOVE_CHECK_LONG);
							}
						}
						if (k2 == b2 + m[dirID]) {
							//������m�����ɉ���
							int s = Min(enemyKing, pos + dir[dirID]);
							int l = Max(enemyKing, pos + dir[dirID]);
							bool moveF = true;
							for (int j = s + 17; j < l; j += 17) {
								if (ban[j] != EMP) moveF = false;
							}
							if (moveF) {
								(mlist++)->move = cons_move(pos, pos + dir[dirID], ban[pos], ban[pos + dir[dirID]], 0, MOVE_CHECK_LONG);
							}
						}
					}
				}
			}
		}

		// ���F3. �p�����邱�Ƌy�єn�ɂ�鉤��
		{
			static const int Directions[] = {-17, -15,  17, 15};
			static const int Positions[]  = { 16,   1, -16, -1};
		
			int i, j;
			for (i = 0; i < 4; i++) {
				to = enemyKing + Positions[i];
				if (ban[to] != EMP && (ban[to] == WALL || !IsNotOwn(ban[to], SorG))) {		// �����̋�
					continue;
				}
				for (j = 0; j < 4; j++) {
					dir = Directions[j];
					from = to + dir;
					while (ban[from] == EMP) {
						from += dir;
					}
					if (from == pos) {
						if (ban[from] == (SorG | UM)
							&& (pin[from] == 0 || pin[from] == dir || pin[from] == -dir)) {
							(mlist++)->move = cons_move(from, to, ban[from], ban[to], 0);
						} else if (ban[from] == (SorG | KA)
							&& (pin[from] == 0 || pin[from] == dir || pin[from] == -dir)
							&& (((((us == BLACK) && ((from & 0x0f) <= 3 || (to & 0x0f) <= 3)))
								|| ((((us != BLACK) && ((from & 0x0f) >= 7 || (to & 0x0f) >=7))))))
							) {
							(mlist++)->move = cons_move(from, to, ban[from], ban[to], 1);
						}
					}
				}
			}
		}
	}

	// ���Ԃɂ�鉤��
	for (i = 0; i < 4; i++) {
		// ����͏������Ȃ�
		if (!OnBoard(kyPos[i])) continue;
		pos = kyPos[i];

		// ����̋�͏������Ȃ�
		if (IsNotOwn(ban[pos], SorG)) continue;
		// �����͏������Ȃ�
		if (ban[pos] & PROMOTED) continue;

		suji = pos & 0xF0;
		dan  = pos & 0x0F;

		if (us == BLACK && dan > kdan) {
			dir = 1;
		} else if (us != BLACK && dan < kdan) {
			dir = -1;
		} else {
			continue;
		}

		// 1. ���ԂƋʂ̊Ԃ̋�𓮂��� or ���
		if (suji == ksuji) {
			to = enemyKing + dir;
			while (ban[to] == EMP) {
				to = to + dir;
			}
			from = to + dir;
			while (ban[from] == EMP) {
				from = from + dir;
			}
			if (from == pos) {
				if (IsNotOwn(ban[to], SorG)) {
					// to�ɂ����͓G
					if ((pin[from] == 0 || pin[from] == dir || pin[from] == -dir)) {
						// �����Ă�����=�ڂ̑O�ɉ������違�����Ȃ琬�����������܂��傤
						if (to - dir == enemyKing) {
							if ((us == BLACK && (to & 0x0F) <= 3)
								|| (us != BLACK && (to & 0x0F) >= 7)) {
								// �����
								(mlist++)->move = cons_move(from, to, ban[from], ban[to], 1);
							}
							if ((us == BLACK && (to & 0x0F) == 2)
								|| (us != BLACK && (to & 0x0F) == 8)) {
								// �s����
								(mlist++)->move = cons_move(from, to, ban[from], ban[to], 0, MOVE_CHECK_LONG | MOVE_CHECK_NARAZU);
							} else {
								// �s����
								(mlist++)->move = cons_move(from, to, ban[from], ban[to], 0, MOVE_CHECK_LONG);
							}
						} else {
							// �s����
							(mlist++)->move = cons_move(from, to, ban[from], ban[to], 0, MOVE_CHECK_LONG);
						}
					}
				} else {
					// to�ɂ����͖���
					mlist = gen_move_from(us, mlist, to, dir);
				}
			}
		} else {
			// 2. �������邱�Ƃɂ�鉤��
			to = 0;
			if (suji == ksuji + 16) {
				to = enemyKing + 16;
			} else if (suji == ksuji - 16) {
				to = enemyKing - 16;
			}
			if (to != 0) {
				from = to + dir;
				if (ban[to] != EMP && !IsNotOwn(ban[to], SorG)) {	// �����̋�
					to = from;
					from += dir;
				} else if (ban[from] != EMP && IsNotOwn(ban[from], SorG)) {	// �G�̋�
					to = from;
					from += dir;
				}
				while (ban[from] == EMP) {
					from += dir;
				}
				if (from == pos && (pin[from] == 0 || pin[from] == dir || pin[from] == -dir)) {
					if ((us == BLACK) && ((to & 0x0f) <= 3)) {
						if (ban[to] == EMP || IsNotOwn(ban[to], SorG)) {
							(mlist++)->move = cons_move(from, to, ban[from], ban[to], 1);
						}
						if ((to & 0x0F) == kdan && kdan <= 2 && (from & 0x0F) != kdan + 1) {
							(mlist++)->move = cons_move(from, to + dir, ban[from], ban[to+dir], 1);
						}
					}
					if ((us != BLACK) && ((to & 0x0f) >= 7)) {
						if (ban[to] == EMP || IsNotOwn(ban[to], SorG)) {
							(mlist++)->move = cons_move(from, to, ban[from], ban[to], 1);
						}
						if ((to & 0x0F) == kdan && kdan >= 8 && (from & 0x0F) != kdan - 1) {
							(mlist++)->move = cons_move(from, to + dir, ban[from], ban[to+dir], 1);
						}
					}
				}
			}
		}
	}
	return mlist;
}

// �Տ�̋�𓮂�����ŉ���ƂȂ���̂𐶐�����.Ver.2
MoveStack* Position::gen_check_short(const Color us, MoveStack* mlist) const
{
	int enemyKing; 				// ��ԑ����猩�āA����̋ʂ̈ʒu
	if (us == BLACK) {
		enemyKing = kingG;
	} else {
		enemyKing = kingS;
	}

	//
	//  �ʂƂ̈ʒu�֌W
	//
	// +---+---+---+---+---+
	// |+30|+14| -2|-18|-34|
	// +---+---+---+---+---+
	// |+31|+15| -1|-17|-33|
	// +---+---+---+---+---+
	// |+32|+16| ��|-16|-32|
	// +---+---+---+---+---+
	// |+33|+17| +1|-15|-31|
	// +---+---+---+---+---+
	// |+34|+18| +2|-14|-30|
	// +---+---+---+---+---+
	// |+35|+19| +3|-13|-29|
	// +---+---+---+---+---+
	// |+36|+20| +4|-12|-28|
	// +---+---+---+---+---+
	//
	//


	int i;
	int j;

	if (us == BLACK) {
		// ���
		i = -1;
		while (OuteMove2[++i].from) {
			const int from = enemyKing + OuteMove2[i].from;
			if (ban[from] == WALL) continue;
			const int koma = ban[from];
			if (koma != EMP && IsSente(koma)) {
				j = 0;
				while (OuteMove2[i].to[j].to) {
					int to = enemyKing + OuteMove2[i].to[j].to;
					if (ban[to] != WALL && (ban[to] == EMP || IsGote(ban[to]))) {
						if (pin[from] == 0 || pin[from] == to-from || pin[from] == from-to) {
							if (((OuteMove2[i].to[j].nari & (1 << (koma & 0x0F))) != 0) && ((to & 0x0f) <= 3 || (from & 0x0f) <= 3)) {
								(mlist++)->move = cons_move(from, to, ban[from], ban[to], 1);
							}
							if ((OuteMove2[i].to[j].narazu & (1 << (koma & 0x0F))) != 0) {
								(mlist++)->move = cons_move(from, to, ban[from], ban[to], 0);
							}
						}
					}
					j++;
				}
			}
		}
	} else {
		// ���
		i = -1;
		while (OuteMove2[++i].from) {
			const int from = enemyKing - OuteMove2[i].from;
			if (ban[from] == WALL) continue;
			const int koma = ban[from];
			if (koma != EMP && IsGote(koma)) {
				j = 0;
				while (OuteMove2[i].to[j].to) {
					int to = enemyKing - OuteMove2[i].to[j].to;
					if (ban[to] != WALL && (ban[to] == EMP || IsSente(ban[to]))) {
						if (pin[from] == 0 || pin[from] == to-from || pin[from] == from-to) {
							if (((OuteMove2[i].to[j].nari & (1 << (koma & 0x0F))) != 0) && ((to & 0x0f) >= 7 || (from & 0x0f) >= 7)) {
								(mlist++)->move = cons_move(from, to, ban[from], ban[to], 1);
							}
							if ((OuteMove2[i].to[j].narazu & (1 << (koma & 0x0F))) != 0) {
								(mlist++)->move = cons_move(from, to, ban[from], ban[to], 0);
							}
						}
					}
					j++;
				}
			}
		}
	}
	return mlist;
}

// ���ł�ŉ���ƂȂ���̂𐶐�����.
MoveStack* Position::gen_check_drop(const Color us, MoveStack* mlist, bool& bUchifudume) const
{
	int enemyKing = (us == BLACK) ? kingG : kingS;	// ��ԑ����猩�āA����̋ʂ̈ʒu
	int suji, dan;

	const int ksuji = enemyKing & 0xF0;
	const int kdan  = enemyKing & 0x0F;

	//
	//  �ʂƂ̈ʒu�֌W
	//
	//  +28 +12 -4 -20 -36
	//  +29 +13 -3 -19 -35
	//  +30 +14 -2 -18 -34
	//  +31 +15 -1 -17 -33
	//  +32 +16 �� -16 -32
	//  +33 +17 +1 -15 -31
	//  +34 +18 +2 -14 -30
	//  +35 +19 +3 -15 -29
	//  +36 +20 +4 -13 -28
	//
	//
	Piece piece;
	// ����ł�
	if (((us == BLACK) ? handS.getKI() : handG.getKI()) > 0) {
		if (us == BLACK) {
			piece = SKI;
			if (ban[enemyKing +  1] == EMP) {
				(mlist++)->move = cons_move(0, enemyKing +  1, piece, EMP);
			}
			if (ban[enemyKing + 17] == EMP) {
				(mlist++)->move = cons_move(0, enemyKing + 17, piece, EMP);
			}
			if (ban[enemyKing - 15] == EMP) {
				(mlist++)->move = cons_move(0, enemyKing - 15, piece, EMP);
			}
			if (ban[enemyKing + 16] == EMP) {
				(mlist++)->move = cons_move(0, enemyKing + 16, piece, EMP);
			}
			if (ban[enemyKing - 16] == EMP) {
				(mlist++)->move = cons_move(0, enemyKing - 16, piece, EMP);
			}
			if (ban[enemyKing -  1] == EMP) {
				(mlist++)->move = cons_move(0, enemyKing -  1, piece, EMP);
			}
		} else {
			piece = GKI;
			if (ban[enemyKing -  1] == EMP) {
				(mlist++)->move = cons_move(0, enemyKing  - 1, piece, EMP);
			}
			if (ban[enemyKing - 17] == EMP) {
				(mlist++)->move = cons_move(0, enemyKing - 17, piece, EMP);
			}
			if (ban[enemyKing + 15] == EMP) {
				(mlist++)->move = cons_move(0, enemyKing + 15, piece, EMP);
			}
			if (ban[enemyKing - 16] == EMP) {
				(mlist++)->move = cons_move(0, enemyKing - 16, piece, EMP);
			}
			if (ban[enemyKing + 16] == EMP) {
				(mlist++)->move = cons_move(0, enemyKing + 16, piece, EMP);
			}
			if (ban[enemyKing +  1] == EMP) {
				(mlist++)->move = cons_move(0, enemyKing  + 1, piece, EMP);
			}
		}
	}
	// ���ł�
	if (((us == BLACK) ? handS.getGI() : handG.getGI()) > 0) {
		if (us == BLACK) {
			piece = SGI;
			if (ban[enemyKing +  1] == EMP) {
				(mlist++)->move = cons_move(0, enemyKing +  1, piece, EMP);
			}
			if (ban[enemyKing - 17] == EMP) {
				(mlist++)->move = cons_move(0, enemyKing - 17, piece, EMP);
			}
			if (ban[enemyKing - 15] == EMP) {
				(mlist++)->move = cons_move(0, enemyKing - 15, piece, EMP);
			}
			if (ban[enemyKing + 15] == EMP) {
				(mlist++)->move = cons_move(0, enemyKing + 15, piece, EMP);
			}
			if (ban[enemyKing + 17] == EMP) {
				(mlist++)->move = cons_move(0, enemyKing + 17, piece, EMP);
			}
		}
		if (us != BLACK) {
			piece = GGI;
			if (ban[enemyKing -  1] == EMP) {
				(mlist++)->move = cons_move(0, enemyKing -  1, piece, EMP);
			}
			if (ban[enemyKing + 17] == EMP) {
				(mlist++)->move = cons_move(0, enemyKing + 17, piece, EMP);
			}
			if (ban[enemyKing + 15] == EMP) {
				(mlist++)->move = cons_move(0, enemyKing + 15, piece, EMP);
			}
			if (ban[enemyKing - 15] == EMP) {
				(mlist++)->move = cons_move(0, enemyKing - 15, piece, EMP);
			}
			if (ban[enemyKing - 17] == EMP) {
				(mlist++)->move = cons_move(0, enemyKing - 17, piece, EMP);
			}
		}
	}
	if (us == BLACK) {
		// ���Ԃ̂Ƃ�
		// �j��ł�
		if (handS.getKE() > 0 && kdan <= 7) {
			piece = SKE;
			if (ban[enemyKing - 14] == EMP) {
				(mlist++)->move = cons_move(0, enemyKing - 14, piece, EMP);
			}
			if (ban[enemyKing + 18] == EMP) {
				(mlist++)->move = cons_move(0, enemyKing + 18, piece, EMP);
			}
		}
		// ����ł�
		suji = ksuji;
		dan = kdan + 1;
		if (handS.getKY() > 0 && ban[dan+suji] == EMP) {
			for (; dan <= 9; dan++) {
				if (ban[dan+suji]==EMP) {
					(mlist++)->move = cons_move(0, suji+dan, SKY, EMP);
				} else {
					break;
				}
			}
		}
		// ����ł�
		suji = ksuji;
		dan = kdan + 1;
		if (handS.getFU() > 0 && ban[dan+suji] == EMP) {
			// ����`�F�b�N
			int nifu = is_double_pawn(us, suji);
			if (nifu == 0) {
				// �ł����l�߂��`�F�b�N
				dan = kdan + 1;
				if (!is_pawn_drop_mate(us, dan+suji)) {
					(mlist++)->move = cons_move(0, suji+dan, SFU, EMP);
				} else {
					bUchifudume = true;
				}
			}
		}
	}
	if (us != BLACK) {
		// ���Ԃ̂Ƃ�
		// �j��ł�
		if (handG.getKE() > 0 && kdan >= 3) {
			piece = GKE;
			if (ban[enemyKing + 14] == EMP) {
				(mlist++)->move = cons_move(0, enemyKing + 14, piece, EMP);
			}
			if (ban[enemyKing - 18] == EMP) {
				(mlist++)->move = cons_move(0, enemyKing - 18, piece, EMP);
			}
		}
		// ����ł�
		suji = ksuji;
		dan = kdan - 1;
		if (handG.getKY() > 0 && ban[dan+suji] == EMP) {
			for (; dan >= 1; dan--) {
				if (ban[dan+suji]==EMP) {
					(mlist++)->move = cons_move(0, suji+dan, GKY, EMP);
				} else {
					break;
				}
			}
		}
		// ����ł�
		suji = ksuji;
		dan = kdan - 1;
		if (handG.getFU() > 0 && ban[dan+suji] == EMP) {
			// ����`�F�b�N
			int nifu = is_double_pawn(us, suji);
			if (nifu == 0) {
				// �ł����l�߂��`�F�b�N
				dan = kdan - 1;
				if (!is_pawn_drop_mate(us, dan+suji)) {
					(mlist++)->move = cons_move(0, suji+dan, GFU, EMP);
				} else {
					bUchifudume = true;
				}
			}
		}
	}

	// �p�Ɣ�Ԃ͋ʎ��ӂ���ł悤�ɂ���
	// �p��ł�
	if (((us == BLACK) ? handS.getKA() : handG.getKA()) > 0) {
		static const int dir1 = -17;
		static const int dir2 = -15;
		static const int dir3 =  15;
		static const int dir4 =  17;
		int pos1, pos2, pos3, pos4;
		pos1 = pos2 = pos3 = pos4 = enemyKing;
		bool bl_1 = true;
		bool bl_2 = true;
		bool bl_3 = true;
		bool bl_4 = true;
		piece = (us == BLACK) ? SKA : GKA;
		while (bl_1 || bl_2 || bl_3 || bl_4) {
			if (bl_1 && ban[pos1 += dir1] == EMP) {
				(mlist++)->move = cons_move(0, pos1, piece, EMP);
			} else {
				bl_1 = false;
			}
			if (bl_2 && ban[pos2 += dir2] == EMP) {
				(mlist++)->move = cons_move(0, pos2, piece, EMP);
			} else {
				bl_2 = false;
			}
			if (bl_3 && ban[pos3 += dir3] == EMP) {
				(mlist++)->move = cons_move(0, pos3, piece, EMP);
			} else {
				bl_3 = false;
			}
			if (bl_4 && ban[pos4 += dir4] == EMP) {
				(mlist++)->move = cons_move(0, pos4, piece, EMP);
			} else {
				bl_4 = false;
			}
		}
	}
	// ��Ԃ�ł�
	if (((us == BLACK) ? handS.getHI() : handG.getHI()) > 0) {
		static const int dir1 = -16;
		static const int dir2 =  -1;
		static const int dir3 =   1;
		static const int dir4 =  16;
		int pos1, pos2, pos3, pos4;
		pos1 = pos2 = pos3 = pos4 = enemyKing;
		bool bl_1 = true;
		bool bl_2 = true;
		bool bl_3 = true;
		bool bl_4 = true;
		piece = (us == BLACK) ? SHI : GHI;
		while (bl_1 || bl_2 || bl_3 || bl_4) {
			if (bl_1 && ban[pos1 += dir1] == EMP) {
				(mlist++)->move = cons_move(0, pos1, piece, EMP);
			} else {
				bl_1 = false;
			}
			if (bl_2 && ban[pos2 += dir2] == EMP) {
				(mlist++)->move = cons_move(0, pos2, piece, EMP);
			} else {
				bl_2 = false;
			}
			if (bl_3 && ban[pos3 += dir3] == EMP) {
				(mlist++)->move = cons_move(0, pos3, piece, EMP);
			} else {
				bl_3 = false;
			}
			if (bl_4 && ban[pos4 += dir4] == EMP) {
				(mlist++)->move = cons_move(0, pos4, piece, EMP);
			} else {
				bl_4 = false;
			}
		}
	}
	return mlist;
}

// ���ł�ŉ���ƂȂ���̂𐶐�����(�c��3��p).
MoveStack* Position::gen_check_drop3(const Color us, MoveStack* mlist, bool& bUchifudume) const
{
	int enemyKing = (us == BLACK) ? kingG : kingS;	// ��ԑ����猩�āA����̋ʂ̈ʒu
	int suji, dan;

	const int ksuji = enemyKing & 0xF0;
	const int kdan  = enemyKing & 0x0F;

	//
	//  �ʂƂ̈ʒu�֌W
	//
	//  +28 +12 -4 -20 -36
	//  +29 +13 -3 -19 -35
	//  +30 +14 -2 -18 -34
	//  +31 +15 -1 -17 -33
	//  +32 +16 �� -16 -32
	//  +33 +17 +1 -15 -31
	//  +34 +18 +2 -14 -30
	//  +35 +19 +3 -15 -29
	//  +36 +20 +4 -13 -28
	//
	//
	Piece piece;
	// ����ł�
#define ADD_DROP(x)  if (ban[enemyKing + (x)] == EMP) (mlist++)->move = cons_move(0, enemyKing + (x), piece, EMP)
	if (((us == BLACK) ? handS.getKI() : handG.getKI()) > 0) {
		if (us == BLACK) {
			piece = SKI;
			ADD_DROP(  1);
			ADD_DROP( 17);
			ADD_DROP(-15);
			ADD_DROP( 16);
			ADD_DROP(-16);
			ADD_DROP( -1);
		} else {
			piece = GKI;
			ADD_DROP( -1);
			ADD_DROP(-17);
			ADD_DROP( 15);
			ADD_DROP(-16);
			ADD_DROP( 16);
			ADD_DROP(  1);
		}
	}
	// ���ł�
	if (((us == BLACK) ? handS.getGI() : handG.getGI()) > 0) {
		if (us == BLACK) {
			piece = SGI;
			ADD_DROP(  1);
			ADD_DROP(-17);
			ADD_DROP(-15);
			ADD_DROP( 15);
			ADD_DROP( 17);
		} else {
			piece = GGI;
			ADD_DROP( -1);
			ADD_DROP( 17);
			ADD_DROP( 15);
			ADD_DROP(-15);
			ADD_DROP(-17);
		}
	}
	if (us == BLACK) {
		//�j��ł�
		if (handS.getKE() > 0 && kdan <= 7) {
			piece = SKE;
			ADD_DROP(-14);
			ADD_DROP( 18);
		}
		// ����ł�
		suji = ksuji;
		dan = kdan + 1;
		if (handS.getKY() > 0 && ban[dan+suji] == EMP) {
			int n = 0;
			piece = SKY;
			for (; dan <= 9 && n < 3; dan++, n++) {
				if (ban[dan+suji]==EMP) {
					(mlist++)->move = cons_move(0, suji+dan, piece, EMP);
				} else {
					break;
				}
			}
		}
		// ����ł�
		suji = ksuji;
		dan = kdan + 1;
		if (handS.getFU() > 0 && ban[dan+suji] == EMP) {
			// ����`�F�b�N
			int nifu = is_double_pawn(us, suji);
			if (nifu == 0) {
				// �ł����l�߂��`�F�b�N
				dan = kdan + 1;
				if (!is_pawn_drop_mate(us, dan+suji)) {
					(mlist++)->move = cons_move(0, suji+dan, SFU, EMP);
				} else {
					bUchifudume = true;
				}
			}
		}
	} else {
		//�j��ł�
		if (handG.getKE() > 0 && kdan >= 3) {
			piece = GKE;
			ADD_DROP( 14);
			ADD_DROP(-18);
		}
		// ����ł�
		suji = ksuji;
		dan = kdan - 1;
		if (handG.getKY() > 0 && ban[dan+suji] == EMP) {
			int n = 0;
			for (; dan >= 1 && n < 3; dan--, n++) {
				if (ban[dan+suji]==EMP) {
					(mlist++)->move = cons_move(0, suji+dan, GKY, EMP);
				} else {
					break;
				}
			}
		}
		// ����ł�
		suji = ksuji;
		dan = kdan - 1;
		if (handG.getFU() > 0 && ban[dan+suji] == EMP) {
			// ����`�F�b�N
			int nifu = is_double_pawn(us, suji);
			if (nifu == 0) {
				// �ł����l�߂��`�F�b�N
				dan = kdan - 1;
				if (!is_pawn_drop_mate(us, dan+suji)) {
					(mlist++)->move = cons_move(0, suji+dan, GFU, EMP);
				} else {
					bUchifudume = true;
				}
			}
		}
	}

	// �p��ł�
	if (((us == BLACK) ? handS.getKA() : handG.getKA()) > 0) {
		static const int dir1 = -17;
		static const int dir2 = -15;
		static const int dir3 =  15;
		static const int dir4 =  17;
		int pos1, pos2, pos3, pos4;
		pos1 = pos2 = pos3 = pos4 = enemyKing;
		int n = 0;
		piece = (us == BLACK) ? SKA : GKA;
		while (n < 3) {
			if (ban[pos1 += dir1] == EMP) {
				(mlist++)->move = cons_move(0, pos1, piece, EMP);
				n++;
			} else {
				break;
			}
		}
		n = 0;
		while (n < 3) {
			if (ban[pos2 += dir2] == EMP) {
				(mlist++)->move = cons_move(0, pos2, piece, EMP);
				n++;
			} else {
				break;
			}
		}
		n = 0;
		while (n < 3) {
			if (ban[pos3 += dir3] == EMP) {
				(mlist++)->move = cons_move(0, pos3, piece, EMP);
				n++;
			} else {
				break;
			}
		}
		n = 0;
		while (n < 3) {
			if (ban[pos4 += dir4] == EMP) {
				(mlist++)->move = cons_move(0, pos4, piece, EMP);
				n++;
			} else {
				break;
			}
		}
	}
	// ��Ԃ�ł�
	if (((us == BLACK) ? handS.getHI() : handG.getHI()) > 0) {
		static const int dir1 = -16;
		static const int dir2 =  -1;
		static const int dir3 =   1;
		static const int dir4 =  16;
		int pos1, pos2, pos3, pos4;
		pos1 = pos2 = pos3 = pos4 = enemyKing;
		int n;
		n = 0;
		piece = (us == BLACK) ? SHI : GHI;
		while (n < 3) {
			if (ban[pos1 += dir1] == EMP) {
				(mlist++)->move = cons_move(0, pos1, piece, EMP);
				n++;
			} else {
				break;
			}
		}
		n = 0;
		while (n < 3) {
			if (ban[pos2 += dir2] == EMP) {
				(mlist++)->move = cons_move(0, pos2, piece, EMP);
				n++;
			} else {
				break;
			}
		}
		n = 0;
		while (n < 3) {
			if (ban[pos3 += dir3] == EMP) {
				(mlist++)->move = cons_move(0, pos3, piece, EMP);
				n++;
			} else {
				break;
			}
		}
		n = 0;
		while (n < 3) {
			if (ban[pos4 += dir4] == EMP) {
				(mlist++)->move = cons_move(0, pos4, piece, EMP);
				n++;
			} else {
				break;
			}
		}
	}
#undef ADD_DROP
	return mlist;
}


MoveStack* Position::generate_check(const Color us, MoveStack* mlist, bool& bUchifudume) const
{
	int enemyKing = (us == BLACK) ? kingG : kingS;	// ��ԑ����猩�āA����̋ʂ̈ʒu
	if (enemyKing == 0) return 0;

	// ��𐶐�.
	MoveStack* cur = mlist;
	MoveStack* last;

	// ���ʂɉ��肪�������Ă���Ƃ��͉���������������ɐ���悤�Ȏ��񋓂���.
	if ((us == BLACK && kingS != 0 && IsCheckS()) 
	   || (us != BLACK && kingG != 0 && IsCheckG())) {
		last = (us == BLACK) ? generate_evasion<BLACK>(mlist) :  generate_evasion<WHITE>(mlist);

		while (cur != last) {
			if (is_check_move(us, cur->move)) {
				(mlist++)->move = cur->move;
			}
			cur++;
		}

		return mlist;
	}

	// �Տ�̋�𓮂�����ŉ���ƂȂ���̂𐶐�����.
	last = gen_check_long(us, mlist);
	last = gen_check_short(us, last);

	// �d�����Ȃ���(�d���͔Տ�̋�𓮂�����Ɍ�����)
	if (last != mlist) {
		int j;
		int teNum = 1;
		MoveStack* top = mlist;
		for (cur = &top[1]; cur != last; cur++) {
			bool flag = false;
			for (j = 0; j < teNum; j++) {
				if (top[j].move == cur->move) {
					flag = true;
					break;
				}
			}
			if (flag == false) {
				top[teNum].move = cur->move;
				teNum++;
			}
		}
		mlist += teNum;
	}

	// ���ł�ŉ���ƂȂ���̂𐶐�����.
	mlist = gen_check_drop(us, mlist, bUchifudume);

	return mlist;
}

MoveStack* Position::generate_check3(const Color us, MoveStack* mlist, bool& bUchifudume) const
{
	int enemyKing = (us == BLACK) ? kingG : kingS;	// ��ԑ����猩�āA����̋ʂ̈ʒu
	if (enemyKing == 0) return 0;

	// ��𐶐�.
	MoveStack* cur = mlist;
	MoveStack* last;

	// ���ʂɉ��肪�������Ă���Ƃ��͉���������������ɐ���悤�Ȏ��񋓂���.
	if ((us == BLACK && kingS != 0 && IsCheckS())
	  || (us != BLACK && kingG != 0 && IsCheckG())) {
		last = (us == BLACK) ? generate_evasion<BLACK>(mlist) :  generate_evasion<WHITE>(mlist);

		while (cur != last) {
			if (is_check_move(us, cur->move)) {
				(mlist++)->move = cur->move;;
			}
			cur++;
		}

		return mlist;
	}

	// �Տ�̋�𓮂�����ŉ���ƂȂ���̂𐶐�����.
	last = gen_check_long(us, mlist);
	last = gen_check_short(us, last);

	// �d�����Ȃ���(�d���͔Տ�̋�𓮂�����Ɍ�����)
	if (last != mlist) {
		int j;
		int teNum = 1;
		MoveStack* top = mlist;
		for (cur = &top[1]; cur != last; cur++) {
			bool flag = false;
			for (j = 0; j < teNum; j++) {
				if (top[j].move == cur->move) {
					flag = true;
					break;
				}
			}
			if (flag == false) {
				top[teNum].move = cur->move;
				teNum++;
			}
		}
		mlist += teNum;
	}

	// ���ł�ŉ���ƂȂ���̂𐶐�����.
	mlist = gen_check_drop3(us, mlist, bUchifudume);

	return mlist;
}

// ������󂯂��̐���(�c��2��)
MoveStack *Position::generate_evasion_rest2(const Color us, MoveStack *mBuf, effect_t kiki, int &Ai)
{
	int kingPos;
	MoveStack *mlist = mBuf;

	kiki &= (EFFECT_LONG_MASK | EFFECT_SHORT_MASK);

	if (kiki == 0) {
		output_info("Error!:%s�ʂɉ��肪�������Ă��Ȃ��I\n", (us == BLACK) ? "���" : "���");
		this->print_csa();
MYABORT();
		return mlist;
	}
	if ((kiki & (kiki-1)) != 0) {
		//������͋ʂ𓮂��������Ȃ�
		mlist = gen_move_king(us, mlist);
	} else {
		kingPos = (us == BLACK) ? kingS : kingG;
		unsigned long id;
		int check;
		if (kiki & EFFECT_SHORT_MASK) {
			// ���т̂Ȃ������ɂ�鉤�� �� �����i�F���肵�Ă��������A�ʂ𓮂���
			_BitScanForward(&id, kiki);
			check = kingPos - NanohaTbl::Direction[id];
			//���������
			mlist = gen_move_to(us, mlist, check);
			//�ʂ𓮂���
			mlist = gen_move_king(us, mlist);
		} else {
			// ���ї����ɂ�鉤�� �� �����i�F���肵�Ă��������A�ʂ𓮂����A����
			_BitScanForward(&id, kiki);
			id -= EFFECT_LONG_SHIFT;
			check = SkipOverEMP(kingPos, -NanohaTbl::Direction[id]);
			//���������
			mlist = gen_move_to(us, mlist, check);
			//�ʂ𓮂���
			mlist = gen_move_king(us, mlist);
			//����������𐶐�����
			if (kingPos - NanohaTbl::Direction[id] != check) {
				Ai = 1;
			}
		}
	}
	return mlist;
}

// ������󂯂��̐���(�c��2��;�ړ�����)
MoveStack *Position::generate_evasion_rest2_MoveAi(const Color us, MoveStack *mBuf, effect_t kiki)
{
	int kingPos;
	MoveStack *mlist = mBuf;

	kiki &= (EFFECT_LONG_MASK | EFFECT_SHORT_MASK);

	if (kiki == 0) {
		output_info("Error!:%s�ʂɉ��肪�������Ă��Ȃ��I\n", (us == BLACK) ? "���" : "���");
		this->print_csa();
MYABORT();
		return mlist;
	}
	if ((kiki & (kiki-1)) != 0) {
		//������͋ʂ𓮂��������Ȃ� �� �����̐����Ȃ̂ŁA�����ɂ͗��Ȃ��͂�
MYABORT();
	} else {
		kingPos = (us == BLACK) ? kingS : kingG;
		unsigned long id;
		int check;
		if (kiki & EFFECT_SHORT_MASK) {
			// ���т̂Ȃ������ɂ�鉤�� �� �����̐����Ȃ̂ŁA�����ɂ͗��Ȃ��͂�
MYABORT();
		} else {
			// ���ї����ɂ�鉤�� �� ����
			_BitScanForward(&id, kiki);
			id -= EFFECT_LONG_SHIFT;
			check = SkipOverEMP(kingPos, -NanohaTbl::Direction[id]);
			//����������𐶐�����
			if (kingPos - NanohaTbl::Direction[id] != check) {
				int i;
				for (i = kingPos - NanohaTbl::Direction[id]; ban[i] == EMP; i -= NanohaTbl::Direction[id]) {
					mlist = gen_move_to(us, mlist, i);	//�ړ���
//					mlist = gen_drop_to(us, mlist, i);	//���ł�
				}
			}
		}
	}
	return mlist;
}

// ������󂯂��̐���(�c��2��;�ړ�����)
MoveStack *Position::generate_evasion_rest2_DropAi(const Color us, MoveStack *mBuf, effect_t kiki, int &check_pos)
{
	int kingPos;

	kiki &= (EFFECT_LONG_MASK | EFFECT_SHORT_MASK);
	MoveStack *mlist = mBuf;

	if (kiki == 0) {
		output_info("Error!:%s�ʂɉ��肪�������Ă��Ȃ��I\n", us == BLACK ? "���" : "���");
		this->print_csa();
MYABORT();
		return mlist;
	}

	if ((kiki & (kiki-1)) != 0) {
		//������͋ʂ𓮂��������Ȃ� �� �����̐����Ȃ̂ŁA�����ɂ͗��Ȃ��͂�
MYABORT();
	} else {
		kingPos = (us == BLACK) ? kingS : kingG;
		unsigned long id;
		int check;
		if (kiki & EFFECT_SHORT_MASK) {
			// ���т̂Ȃ������ɂ�鉤�� �� �����̐����Ȃ̂ŁA�����ɂ͗��Ȃ��͂�
MYABORT();
		} else {
			// ���ї����ɂ�鉤�� �� ����
			_BitScanForward(&id, kiki);
			id -= EFFECT_LONG_SHIFT;
			check = SkipOverEMP(kingPos, -NanohaTbl::Direction[id]);
			//����������𐶐�����
			check_pos = check;
			if (kingPos - NanohaTbl::Direction[id] != check) {
				int i;
				for (i = kingPos - NanohaTbl::Direction[id]; ban[i] == EMP; i -= NanohaTbl::Direction[id]) {
//					MoveTo(SorG, mNum, mBuf, i);	//�ړ���
//					int teNum2 = mNum;
//					DropTo(SorG, teNum2, mBuf, i);	//���ł�
//					if (teNum2 > mNum) mNum++;
					mlist = gen_drop_to(us, mlist, i);	//���ł�
				}
			}
		}
	}
	return mlist;
}

//
// �l�ނ��ǂ����𒲂ׂ�B
// �����FColor us				���(BLACK�F���AWHITE�F���)
//		 int maxDepth			�T������ő�[��
//		 Move &m					�l�܂����Ԃ�
//		 unsigned long limit	��������(ms)
// �߂�l�Fint					�l�ނ��ǂ���(VALUE_MATE:�l�ށA-VALUE_MATE�F�l�܂Ȃ��AVALUE_ZERO�F�s��)
//
int Position::Mate3(const Color us, Move &m)
{
	assert(us == side_to_move());
	// 1��l�߂��m�F
	{
		uint32_t refInfo;
		int val = (us == BLACK) ? Mate1ply<BLACK>(m, refInfo) :  Mate1ply<WHITE>(m, refInfo);
		if (val == VALUE_MATE) return val;
	}

#if defined(USE_M3HASH)
	if (probe_m3hash(*this, m)) {
		return m == MOVE_NONE ? -VALUE_MATE : VALUE_MATE;
	}
#endif

	MoveStack moves[256]; 	// �[��3���x�Ȃ�\���ȑ傫��
	MoveStack *cur, *last;
	bool bUchifudume = false;

	last = generate_check3(us, moves, bUchifudume);
	if (last == moves) {
		return -VALUE_MATE; 	//�l�܂Ȃ�
	}

	int valmax = -VALUE_MATE;
	for (cur = moves; cur != last; cur++) {
		StateInfo newSt;
		Move move = cur->move;
		// �c��R��ł͑ł����l�߂��������K�v�͂Ȃ����߁A�s���͓ǂ܂Ȃ�
		if ((move & MOVE_CHECK_NARAZU)) continue;
		do_move(move, newSt);
		int val;
		val = EvasionRest2(flip(us), last);
		undo_move(move);

		if (val > valmax) valmax = val;
		if (valmax == VALUE_MATE) {
			m = move;
#if defined(USE_M3HASH)
			store_m3hash(*this, move);
#endif
			return VALUE_MATE; //�l��
		}
	}

#if defined(USE_M3HASH)
			store_m3hash(*this, MOVE_NONE);
#endif
	return valmax;
}

//
// �ʕ����[(�c��2��)�ŋl�ނ��ǂ����𒲂ׂ�B
// �����FColor us				���(BLACK�F���AWHITE�F���)
//		 Move &m					�l�܂����Ԃ�
// �߂�l�Fint					�l�ނ��ǂ���(VALUE_MATE:�l�ށAVAL_FUDUMI�F�l�܂Ȃ��AVAL_FUMEI�F�s��)
//
//int Position::EvasionRest2(const Color us, MoveStack *antichecks, unsigned int &PP, unsigned int &DP, int &pn, int &dn, bool &isUchifudume)
int Position::EvasionRest2(const Color us, MoveStack *antichecks)
{
	if (!in_check()) {
		output_info("Error!:%s�ʂɉ��肪�������Ă��Ȃ��I\n", us == BLACK ? "���" : "���");
		this->print_csa();
		MYABORT();
		return -VALUE_MATE;
	}

	MoveStack *cur, *last;
	// �ł���ȊO�̉������萶��
	int Ai = 0;
	last = (us == BLACK) ? generate_evasion_rest2(BLACK, antichecks, exist_effect<WHITE>(kingS), Ai)
	                     : generate_evasion_rest2(WHITE, antichecks, exist_effect<BLACK>(kingG), Ai);

	if (last == antichecks && Ai == 0) {
		// �󂯂��Ȃ� �� �l��
		return VALUE_MATE;
	}

	int valmin = VALUE_MATE;
	StateInfo newSt;
	uint32_t refInfo;
	Move m;
	for (cur = antichecks; cur != last; cur++) {
		Move move = cur->move;
		do_move(move, newSt);
		int val = (us == BLACK) ? Mate1ply<WHITE>(m, refInfo) :  Mate1ply<BLACK>(m, refInfo);
		undo_move(move);

		if (val < valmin) {
			valmin = val;
		}
		// �l�܂Ȃ�����
		if (valmin != VALUE_MATE) {
			return valmin;
		}
	}
	if (Ai == 0) return valmin;

	// �ړ������̎�ŋl�ނ��m�F����
	last = (us == BLACK) ? generate_evasion_rest2_MoveAi(BLACK, antichecks, exist_effect<WHITE>(kingS))
	                     : generate_evasion_rest2_MoveAi(WHITE, antichecks, exist_effect<BLACK>(kingG));

	for (cur = antichecks; cur != last; cur++) {
		Move move = cur->move;
		do_move(move, newSt);
		int val = (us == BLACK) ? Mate1ply<WHITE>(m, refInfo) : Mate1ply<BLACK>(m, refInfo);
		undo_move(move);
		if (val < valmin) {
			valmin = val;
		}

		// �l�܂Ȃ�����
		if (valmin != VALUE_MATE) {
			return valmin;
		}
	}

	// �ł���ŋl�ނ��m�F����
	int check = 0;	// ����������Ă����̈ʒu
	last = (us == BLACK) ? generate_evasion_rest2_DropAi(BLACK, antichecks, exist_effect<WHITE>(kingS), check)
	                     : generate_evasion_rest2_DropAi(WHITE, antichecks, exist_effect<BLACK>(kingG), check);

	for (cur = antichecks; cur != last; cur++) {
		Move move = cur->move;

		// ���[�̋l�ݏ���
		do_move(move, newSt);
		int val = (us == BLACK) ? Mate1ply<WHITE>(m, refInfo) : Mate1ply<BLACK>(m, refInfo);
		undo_move(move);

		// �l�܂Ȃ�����
		if (val != VALUE_MATE) {
			return val;
		}
	}
	// �ǂ�����Ă��l��ł��܂�
	return VALUE_MATE;
}

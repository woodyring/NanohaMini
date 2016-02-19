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
#if !defined(EVAL_APERY)
#error "Error!"
#endif

#include <cassert>
#include <cstdio>

#include "position.h"
#include "evaluate.h"

// �]���֐��֘A��`
#include "param_apery.h"
#define FV_KK_BIN  "KK_synthesized.bin"
#define FV_KKP_BIN "KKP_synthesized.bin"
#define FV_KPP_BIN "KPP_synthesized.bin"

#define HANDLIST	14
#define NLIST	(38)

#define FV_SCALE                32

#define MATERIAL            (this->material)

#define SQ_BKING            NanohaTbl::z2sq[kingS]
#define SQ_WKING            NanohaTbl::z2sq[kingG]
#define HAND_B              (this->hand[BLACK].h)
#define HAND_W              (this->hand[WHITE].h)

#define Inv(sq)             (nsquare-1-sq)
//#define PcOnSq(k,i)         fv_kp[k][i]
//#define PcPcOn(i,j)         fv_pp[i][j]
#if defined(EVAL_FAST)
#define PcOnSq(k,i)         pc_on_sq[k][i][i]
#define PcPcOnSq(k,i,j)     pc_on_sq[k][i][j]
#else
#define PcOnSq(k,i)         pc_on_sq[k][(i)*((i)+3)/2]
#define PcPcOnSq(k,i,j)     pc_on_sq[k][(i)*((i)+1)/2+(j)]
#endif

#define I2HandPawn(hand)    (((hand) & HAND_FU_MASK) >> HAND_FU_SHIFT)
#define I2HandLance(hand)   (((hand) & HAND_KY_MASK) >> HAND_KY_SHIFT)
#define I2HandKnight(hand)  (((hand) & HAND_KE_MASK) >> HAND_KE_SHIFT)
#define I2HandSilver(hand)  (((hand) & HAND_GI_MASK) >> HAND_GI_SHIFT)
#define I2HandGold(hand)    (((hand) & HAND_KI_MASK) >> HAND_KI_SHIFT)
#define I2HandBishop(hand)  (((hand) & HAND_KA_MASK) >> HAND_KA_SHIFT)
#define I2HandRook(hand)    (((hand) & HAND_HI_MASK) >> HAND_HI_SHIFT)

enum {
	promote = 8, EMPTY = 0,	/* VC++��empty���Ԃ���̂ŕύX */
	pawn, lance, knight, silver, gold, bishop, rook, king, pro_pawn,
	pro_lance, pro_knight, pro_silver, piece_null, horse, dragon
};

enum { nhand = 7, nfile = 9,  nrank = 9,  nsquare = 81 };

// �]���֐��e�[�u���̃I�t�Z�b�g�B
// f_xxx �������̋�Ae_xxx ���G�̋�
// Bonanza �̉e���Ŏ����� 0 �̏ꍇ�̃C���f�b�N�X�����݂��邪�A�Q�Ƃ��鎖�͖����B
// todo: ������ 0 �̈ʒu���l�߂ăe�[�u���������ł�����������B(�L���b�V���ɏ����͏��₷��?)
enum {
	f_hand_pawn   = 0, // 0
	e_hand_pawn   = f_hand_pawn   + 19,
	f_hand_lance  = e_hand_pawn   + 19,
	e_hand_lance  = f_hand_lance  +  5,
	f_hand_knight = e_hand_lance  +  5,
	e_hand_knight = f_hand_knight +  5,
	f_hand_silver = e_hand_knight +  5,
	e_hand_silver = f_hand_silver +  5,
	f_hand_gold   = e_hand_silver +  5,
	e_hand_gold   = f_hand_gold   +  5,
	f_hand_bishop = e_hand_gold   +  5,
	e_hand_bishop = f_hand_bishop +  3,
	f_hand_rook   = e_hand_bishop +  3,
	e_hand_rook   = f_hand_rook   +  3,
	fe_hand_end   = e_hand_rook   +  3,

	f_pawn        = fe_hand_end,
	e_pawn        = f_pawn        + 81,
	f_lance       = e_pawn        + 81,
	e_lance       = f_lance       + 81,
	f_knight      = e_lance       + 81,
	e_knight      = f_knight      + 81,
	f_silver      = e_knight      + 81,
	e_silver      = f_silver      + 81,
	f_gold        = e_silver      + 81,
	e_gold        = f_gold        + 81,
	f_bishop      = e_gold        + 81,
	e_bishop      = f_bishop      + 81,
	f_horse       = e_bishop      + 81,
	e_horse       = f_horse       + 81,
	f_rook        = e_horse       + 81,
	e_rook        = f_rook        + 81,
	f_dragon      = e_rook        + 81,
	e_dragon      = f_dragon      + 81,
	fe_end        = e_dragon      + 81
};

enum { pos_n = fe_end * ( fe_end + 1 ) / 2 };

namespace {
	short p_value[31];

	short fv_kpp[nsquare][fe_end][fe_end];
	int fv_kkp[nsquare][nsquare][fe_end];
	int fv_kk[nsquare][nsquare];
}

namespace NanohaTbl {
	// Apery�͏c�^�Ȃ̂ŁA�ϊ��e�[�u�����Ⴄ
	const short z2sq[] = {
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1,  0,  1,  2,  3,  4,  5,  6,  7,  8, -1, -1, -1, -1, -1, -1,
		-1,  9, 10, 11, 12, 13, 14, 15, 16, 17, -1, -1, -1, -1, -1, -1,
		-1, 18, 19, 20, 21, 22, 23, 24, 25, 26, -1, -1, -1, -1, -1, -1,
		-1, 27, 28, 29, 30, 31, 32, 33, 34, 35, -1, -1, -1, -1, -1, -1,
		-1, 36, 37, 38, 39, 40, 41, 42, 43, 44, -1, -1, -1, -1, -1, -1,
		-1, 45, 46, 47, 48, 49, 50, 51, 52, 53, -1, -1, -1, -1, -1, -1,
		-1, 54, 55, 56, 57, 58, 59, 60, 61, 62, -1, -1, -1, -1, -1, -1,
		-1, 63, 64, 65, 66, 67, 68, 69, 70, 71, -1, -1, -1, -1, -1, -1,
		-1, 72, 73, 74, 75, 76, 77, 78, 79, 80, -1, -1, -1, -1, -1, -1,
	};
}

void Position::init_evaluate()
{
	int iret=0;
	FILE *fp;
	const char *fname ="�]���x�N�g��";

	do {
		size_t size;

		// KK
		fname = FV_KK_BIN;
		fp = fopen(fname, "rb");
		if ( fp == NULL ) { iret = -2; break;}

		size = nsquare * nsquare;
		if ( fread( fv_kk, sizeof(int), size, fp ) != size ) {
			iret = -2;
			break;
		}
		if (fgetc(fp) != EOF) {
			iret = -2;
			break;
		}
		fclose(fp);

		// KKP
		fname = FV_KKP_BIN;
		fp = fopen(fname, "rb");
		if ( fp == NULL ) { iret = -2; break;}

		size = nsquare * nsquare * fe_end;
		if ( fread( fv_kkp, sizeof(int), size, fp ) != size ) {
			iret = -2;
			break;
		}
		if (fgetc(fp) != EOF) {
			iret = -2;
			break;
		}
		fclose(fp);

		// KPP
		fname = FV_KPP_BIN;
		fp = fopen(fname, "rb");
		if ( fp == NULL ) { iret = -2; break;}

		size = nsquare * fe_end * fe_end;
		if ( (iret=fread( fv_kpp, sizeof(short), size, fp )) != size ) {
			fprintf(stderr, "%s:%d:iret = %d\n", __FILE__, __LINE__, iret);
			iret = -2;
			break;
		}
		if (fgetc(fp) != EOF) {
			iret = -2;
			break;
		}
		fclose(fp);
	} while (0);

	if (iret < 0) {
		std::cerr << "Can't load " << fname << "." << std::endl;
#if defined(CSADLL) || defined(CSA_DIRECT)
		::MessageBox(NULL, "�]���x�N�g�������[�h�ł��܂���\n�I�����܂�", "Error!", MB_OK);
#endif	// defined(CSA_DLL) || defined(CSA_DIRECT)
		exit(1);
	}

	int i;
	for ( i = 0; i < 31; i++) { p_value[i]       = 0; }

	p_value[15+pawn]       = DPawn;
	p_value[15+lance]      = DLance;
	p_value[15+knight]     = DKnight;
	p_value[15+silver]     = DSilver;
	p_value[15+gold]       = DGold;
	p_value[15+bishop]     = DBishop;
	p_value[15+rook]       = DRook;
	p_value[15+king]       = DKing;
	p_value[15+pro_pawn]   = DProPawn;
	p_value[15+pro_lance]  = DProLance;
	p_value[15+pro_knight] = DProKnight;
	p_value[15+pro_silver] = DProSilver;
	p_value[15+horse]      = DHorse;
	p_value[15+dragon]     = DDragon;

	p_value[15-pawn]          = p_value[15+pawn];
	p_value[15-lance]         = p_value[15+lance];
	p_value[15-knight]        = p_value[15+knight];
	p_value[15-silver]        = p_value[15+silver];
	p_value[15-gold]          = p_value[15+gold];
	p_value[15-bishop]        = p_value[15+bishop];
	p_value[15-rook]          = p_value[15+rook];
	p_value[15-king]          = p_value[15+king];
	p_value[15-pro_pawn]      = p_value[15+pro_pawn];
	p_value[15-pro_lance]     = p_value[15+pro_lance];
	p_value[15-pro_knight]    = p_value[15+pro_knight];
	p_value[15-pro_silver]    = p_value[15+pro_silver];
	p_value[15-horse]         = p_value[15+horse];
	p_value[15-dragon]        = p_value[15+dragon];
}

int Position::compute_material() const
{
	int v, item, itemp;
	int i;

	item  = 0;
	itemp = 0;
	for (i = KNS_FU; i <= KNE_FU; i++) {
		if (knkind[i] == SFU) item++;
		if (knkind[i] == GFU) item--;
		if (knkind[i] == STO) itemp++;
		if (knkind[i] == GTO) itemp--;
	}
	v  = item  * p_value[15+pawn];
	v += itemp * p_value[15+pro_pawn];

	item  = 0;
	itemp = 0;
	for (i = KNS_KY; i <= KNE_KY; i++) {
		if (knkind[i] == SKY) item++;
		if (knkind[i] == GKY) item--;
		if (knkind[i] == SNY) itemp++;
		if (knkind[i] == GNY) itemp--;
	}
	v += item  * p_value[15+lance];
	v += itemp * p_value[15+pro_lance];

	item  = 0;
	itemp = 0;
	for (i = KNS_KE; i <= KNE_KE; i++) {
		if (knkind[i] == SKE) item++;
		if (knkind[i] == GKE) item--;
		if (knkind[i] == SNK) itemp++;
		if (knkind[i] == GNK) itemp--;
	}
	v += item  * p_value[15+knight];
	v += itemp * p_value[15+pro_knight];

	item  = 0;
	itemp = 0;
	for (i = KNS_GI; i <= KNE_GI; i++) {
		if (knkind[i] == SGI) item++;
		if (knkind[i] == GGI) item--;
		if (knkind[i] == SNG) itemp++;
		if (knkind[i] == GNG) itemp--;
	}
	v += item  * p_value[15+silver];
	v += itemp * p_value[15+pro_silver];

	item  = 0;
	for (i = KNS_KI; i <= KNE_KI; i++) {
		if (knkind[i] == SKI) item++;
		if (knkind[i] == GKI) item--;
	}
	v += item  * p_value[15+gold];

	item  = 0;
	itemp = 0;
	for (i = KNS_KA; i <= KNE_KA; i++) {
		if (knkind[i] == SKA) item++;
		if (knkind[i] == GKA) item--;
		if (knkind[i] == SUM) itemp++;
		if (knkind[i] == GUM) itemp--;
	}
	v += item  * p_value[15+bishop];
	v += itemp * p_value[15+horse];

	item  = 0;
	itemp = 0;
	for (i = KNS_HI; i <= KNE_HI; i++) {
		if (knkind[i] == SHI) item++;
		if (knkind[i] == GHI) item--;
		if (knkind[i] == SRY) itemp++;
		if (knkind[i] == GRY) itemp--;
	}
	v += item  * p_value[15+rook];
	v += itemp * p_value[15+dragon];

	return v;
}

int Position::make_list_apery(int list0[NLIST], int list1[NLIST], int nlist) const
{
	static const struct {
		int f_pt, e_pt;
	} base_tbl[] = {
		{-1      , -1      },	//  0:---
		{f_pawn  , e_pawn  },	//  1:SFU
		{f_lance , e_lance },	//  2:SKY
		{f_knight, e_knight},	//  3:SKE
		{f_silver, e_silver},	//  4:SGI
		{f_gold  , e_gold  },	//  5:SKI
		{f_bishop, e_bishop},	//  6:SKA
		{f_rook  , e_rook  },	//  7:SHI
		{-1      , -1      },	//  8:SOU
		{f_gold  , e_gold  },	//  9:STO
		{f_gold  , e_gold  },	// 10:SNY
		{f_gold  , e_gold  },	// 11:SNK
		{f_gold  , e_gold  },	// 12:SNG
		{-1      , -1      },	// 13:--
		{f_horse , e_horse },	// 14:SUM
		{f_dragon, e_dragon},	// 15:SRY
		{-1      , -1      },	// 16:---
		{e_pawn  , f_pawn  },	// 17:GFU
		{e_lance , f_lance },	// 18:GKY
		{e_knight, f_knight},	// 19:GKE
		{e_silver, f_silver},	// 20:GGI
		{e_gold  , f_gold  },	// 21:GKI
		{e_bishop, f_bishop},	// 22:GKA
		{e_rook  , f_rook  },	// 23:GHI
		{-1      , -1      },	// 24:GOU
		{e_gold  , f_gold  },	// 25:GTO
		{e_gold  , f_gold  },	// 26:GNY
		{e_gold  , f_gold  },	// 27:GNK
		{e_gold  , f_gold  },	// 28:GNG
		{-1      , -1      },	// 29:---
		{e_horse , f_horse },	// 30:GUM
		{e_dragon, f_dragon}	// 31:GRY
	};
	int sq;

	// ��ԍ��F1�`2���ʁA3�`40���ʈȊO
	for (int kn = 3; kn <= 40; kn++) {
		const int z = knpos[kn];
		if (z < 0x11) continue;			// �������
		int piece = knkind[kn];
		sq = conv_z2sq(z);
		assert(piece <= GRY && sq < nsquare);
		assert(base_tbl[piece].f_pt != -1);
		list0[nlist] = base_tbl[piece].f_pt + sq;
		list1[nlist] = base_tbl[piece].e_pt + Inv(sq);
		nlist++;
	}

	assert( nlist == NLIST );

	return nlist;
}

int Position::evaluate(const Color us) const
{
	int list0[NLIST], list1[NLIST];
	int score, sq_bk, sq_wk, i, j;
	static int count=0;
	count++;
	int nlist=0;

	// ����������X�g������
#define FOO(hand, Piece, list0_index, list1_index)    \
	for (int i = I2Hand##Piece(hand); i >= 1; --i) {  \
		list0[nlist] = list0_index + i;               \
		list1[nlist] = list1_index + i;               \
		++nlist; \
	}

	FOO(HAND_B, Pawn  , f_hand_pawn  , e_hand_pawn  )
	FOO(HAND_W, Pawn  , e_hand_pawn  , f_hand_pawn  )
	FOO(HAND_B, Lance , f_hand_lance , e_hand_lance )
	FOO(HAND_W, Lance , e_hand_lance , f_hand_lance )
	FOO(HAND_B, Knight, f_hand_knight, e_hand_knight)
	FOO(HAND_W, Knight, e_hand_knight, f_hand_knight)
	FOO(HAND_B, Silver, f_hand_silver, e_hand_silver)
	FOO(HAND_W, Silver, e_hand_silver, f_hand_silver)
	FOO(HAND_B, Gold  , f_hand_gold  , e_hand_gold  )
	FOO(HAND_W, Gold  , e_hand_gold  , f_hand_gold  )
	FOO(HAND_B, Bishop, f_hand_bishop, e_hand_bishop)
	FOO(HAND_W, Bishop, e_hand_bishop, f_hand_bishop)
	FOO(HAND_B, Rook  , f_hand_rook  , e_hand_rook  )
	FOO(HAND_W, Rook  , e_hand_rook  , f_hand_rook  )
#undef FOO

	nlist = make_list_apery(list0, list1, nlist);

	sq_bk = SQ_BKING;
	sq_wk = SQ_WKING;
	assert(0 <= sq_bk && sq_bk < nsquare);
	assert(0 <= sq_wk && sq_wk < nsquare);
	const auto* ppkppb = fv_kpp[sq_bk     ];
	const auto* ppkppw = fv_kpp[Inv(sq_wk)];

	score = fv_kk[sq_bk][sq_wk];
	for ( i = 0; i < nlist; i++ ) {
		const int k0 = list0[i];
		const int k1 = list1[i];
		assert(0 <= k0 && k0 < fe_end);
		assert(0 <= k1 && k1 < fe_end);
		const auto* pkppb = ppkppb[k0];
		const auto* pkppw = ppkppw[k1];
		for ( j = 0; j < i; j++ ) {
			const int l0 = list0[j];
			const int l1 = list1[j];
			assert(0 <= l0 && l0 < fe_end);
			assert(0 <= l1 && l1 < fe_end);
			score += pkppb[l0];
			score -= pkppw[l1];
		}
		score += fv_kkp[sq_bk][sq_wk][k0];
	}

	score += MATERIAL * FV_SCALE;
	score /= FV_SCALE;

	score = (us == BLACK) ? score : -score;

	return score;
}

Value evaluate(const Position& pos, Value& margin)
{
	margin = VALUE_ZERO;
	const Color us = pos.side_to_move();
	return Value(pos.evaluate(us));
}

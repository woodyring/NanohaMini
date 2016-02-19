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

#if !defined(POSITION_H_INCLUDED)
#define POSITION_H_INCLUDED

#if defined(NANOHA)
#include <iostream>
#include <cstdio>
#else
#include "bitboard.h"
#endif
#include "move.h"
#include "types.h"

#if !defined(NANOHA)
/// The checkInfo struct is initialized at c'tor time and keeps info used
/// to detect if a move gives check.

struct CheckInfo {

	explicit CheckInfo(const Position&);

	Bitboard dcCandidates;
	Bitboard pinned;
	Bitboard checkSq[8];
};
#endif


/// The StateInfo struct stores information we need to restore a Position
/// object to its previous state when we retract a move. Whenever a move
/// is made on the board (by calling Position::do_move), an StateInfo object
/// must be passed as a parameter.
///
/// �� Position::do_move() �� struct ReducedStateInfo �ƒ�`�����킹��
///
class Position;

struct StateInfo {
#if defined(NANOHA)
	int gamePly;
	int pliesFromNull;

	Piece captured;
	uint32_t hand;
	uint32_t effect;
	Key key;
#else
	Key pawnKey, materialKey;
	Value npMaterial[2];
	int castleRights, rule50, pliesFromNull;
	Score value;
	Square epSquare;

	Key key;
	Bitboard checkersBB;
	PieceType capturedType;
#endif
	StateInfo* previous;
};

#if defined(NANOHA)
// �������֐�
extern void init_application_once();	// ���s�t�@�C���N�����ɍs��������.
#endif

/// The position data structure. A position consists of the following data:
///
///    * For each piece type, a bitboard representing the squares occupied
///      by pieces of that type.
///    * For each color, a bitboard representing the squares occupied by
///      pieces of that color.
///    * A bitboard of all occupied squares.
///    * A bitboard of all checking pieces.
///    * A 64-entry array of pieces, indexed by the squares of the board.
///    * The current side to move.
///    * Information about the castling rights for both sides.
///    * The initial files of the kings and both pairs of rooks. This is
///      used to implement the Chess960 castling rules.
///    * The en passant square (which is SQ_NONE if no en passant capture is
///      possible).
///    * The squares of the kings for both sides.
///    * Hash keys for the position itself, the current pawn structure, and
///      the current material situation.
///    * Hash keys for all previous positions in the game for detecting
///      repetition draws.
///    * A counter for detecting 50 move rule draws.

class Position {

	// No defaul, copy c'tor or assignment allowed, default c'tor will not be
	// generated anyhow because of user-defined c'tors.
	Position(const Position&);
	Position& operator=(const Position&);

public:
	Position(const Position& pos, int threadID);
#if defined(NANOHA)
	Position(const std::string& fen, int threadID);
#else
	Position(const std::string& fen, bool isChess960, int threadID);
#endif

	// Text input/output
#if defined(NANOHA)
	void from_fen(const std::string& fen);
#else
	void from_fen(const std::string& fen, bool isChess960);
#endif
	const std::string to_fen() const;
#if defined(NANOHA)
	void print_csa(Move m = MOVE_NONE) const;
	void print(Move m) const;
	void print() const {print(MOVE_NULL);}
	bool operator == (const Position &a) const;
	bool operator != (const Position &a) const;
#else
	void print(Move m = MOVE_NONE) const;
#endif

	// The piece on a given square
	Piece piece_on(Square s) const;
	bool square_is_empty(Square s) const;
#if defined(NANOHA)
	Value type_value_of_piece_on(Square s) const;
	Value promote_value_of_piece_on(Square s) const;
	int pin_on(Square s) const;
#endif

	// Side to move
	Color side_to_move() const;

#if !defined(NANOHA)
	// Bitboard representation of the position
	Bitboard empty_squares() const;
	Bitboard occupied_squares() const;
	Bitboard pieces(Color c) const;
	Bitboard pieces(PieceType pt) const;
	Bitboard pieces(PieceType pt, Color c) const;
	Bitboard pieces(PieceType pt1, PieceType pt2) const;
	Bitboard pieces(PieceType pt1, PieceType pt2, Color c) const;

	// Number of pieces of each color and type
	int piece_count(Color c, PieceType pt) const;

	// The en passant square
	Square ep_square() const;
#endif

	// Current king position for each color
	Square king_square(Color c) const;

#if !defined(NANOHA)
	// Castling rights
	bool can_castle(CastleRight f) const;
	bool can_castle(Color c) const;
	Square castle_rook_square(CastleRight f) const;
#endif

#if !defined(NANOHA)
	// Bitboards for pinned pieces and discovered check candidates
	Bitboard discovered_check_candidates() const;
	Bitboard pinned_pieces() const;

	// Checking pieces and under check information
	Bitboard checkers() const;
#endif
	bool in_check() const;
#if defined(NANOHA)
	bool at_checking() const;	// ����������Ă����Ԃ��H
#else
	// Piece lists
	const Square* piece_list(Color c, PieceType pt) const;

	// Information about attacks to or from a given square
	Bitboard attackers_to(Square s) const;
	Bitboard attackers_to(Square s, Bitboard occ) const;
	Bitboard attacks_from(Piece p, Square s) const;
	static Bitboard attacks_from(Piece p, Square s, Bitboard occ);
	template<PieceType> Bitboard attacks_from(Square s) const;
	template<PieceType> Bitboard attacks_from(Square s, Color c) const;
#endif

	// Properties of moves
#if defined(NANOHA)
	bool is_double_pawn(const Color us, int to) const;		// ������H
	bool is_pawn_drop_mate(const Color us, int to) const;	// �ł����l�߂��H
	bool move_gives_check(Move m) const;
	bool move_attacks_square(Move m, Square s) const;
	bool pl_move_is_legal(const Move m) const;
	bool is_pseudo_legal(const Move m) const {return pl_move_is_legal(m);}
#else
	bool move_gives_check(Move m, const CheckInfo& ci) const;
	bool move_attacks_square(Move m, Square s) const;
	bool pl_move_is_legal(Move m, Bitboard pinned) const;
	bool is_pseudo_legal(const Move m) const;
#endif
	bool is_capture(Move m) const;
	bool is_capture_or_promotion(Move m) const;
	bool is_passed_pawn_push(Move m) const;

	// Piece captured with previous moves
	PieceType captured_piece_type() const;

	// Information about pawns
	bool pawn_is_passed(Color c, Square s) const;

	// Doing and undoing moves
	void do_move(Move m, StateInfo& st);
#if defined(NANOHA)
	void do_drop(Move m);
	void undo_move(Move m);
	void undo_drop(Move m);
	void do_null_move(StateInfo& st);
	void undo_null_move();
	// ���i�߂��Ƀn�b�V���v�Z�̂ݍs��
	uint64_t calc_hash_no_move(const Move m) const;
#else
	void do_move(Move m, StateInfo& st, const CheckInfo& ci, bool moveIsCheck);
	void undo_move(Move m);
	void do_null_move(StateInfo& st);
	void undo_null_move();
#endif

#if defined(NANOHA)
	// �萶���Ŏg�����߂̊֐�
	// �w��ʒu����w������ɉ�������ʒu�܂ŒT��(WALL or ���� or ����̈ʒu�ɂȂ�)
	int SkipOverEMP(int pos, const int dir) const;
	// �����̍X�V
	void add_effect(const int z);					// �ʒuz�̋�̗����𔽉f����
	void del_effect(const int z, const Piece k);	
	// �����̉��Z
	template<Color>
	void add_effect_straight(const int z, const int dir, const uint32_t bit);
	template<Color>
	void del_effect_straight(const int z, const int dir, const uint32_t bit);
#define AddKikiDirS	add_effect_straight<BLACK>
#define AddKikiDirG	add_effect_straight<WHITE>
#define DelKikiDirS	del_effect_straight<BLACK>
#define DelKikiDirG	del_effect_straight<WHITE>
	// �s�����X�V
	template <Color> void add_pin_info(const int dir);
	template <Color> void del_pin_info(const int dir);
#define AddPinInfS	add_pin_info<BLACK>
#define AddPinInfG	add_pin_info<WHITE>
#define DelPinInfS	del_pin_info<BLACK>
#define DelPinInfG	del_pin_info<WHITE>

public:
	// �萶��
	template<Color> MoveStack* add_straight(MoveStack* mlist, const int from, const int dir) const;
	template<Color> MoveStack* add_move(MoveStack* mlist, const int from, const int dir) const;
#define add_straightB	add_straight<BLACK>
#define add_straightW	add_straight<WHITE>
#define add_moveB	add_move<BLACK>
#define add_moveW	add_move<WHITE>
	MoveStack* gen_move_to(const Color us, MoveStack* mlist, int to) const;		// to�ɓ�����̐���
	MoveStack* gen_drop_to(const Color us, MoveStack* mlist, int to) const;		// to�ɋ��ł�̐���
	template <Color> MoveStack* gen_drop(MoveStack* mlist) const;			// ���ł�̐���
	MoveStack* gen_move_king(const Color us, MoveStack* mlist, int pindir = 0) const;			//�ʂ̓�����̐���
	MoveStack* gen_king_noncapture(const Color us, MoveStack* mlist, int pindir = 0) const;			//�ʂ̓�����̐���
	MoveStack* gen_move_from(const Color us, MoveStack* mlist, int from, int pindir = 0) const;		//from���瓮����̐���

	template <Color> MoveStack* generate_capture(MoveStack* mlist) const;
	template <Color> MoveStack* generate_non_capture(MoveStack* mlist) const;
	template <Color> MoveStack* generate_evasion(MoveStack* mlist) const;
	template <Color> MoveStack* generate_non_evasion(MoveStack* mlist) const;
	template <Color> MoveStack* generate_legal(MoveStack* mlist) const;

	// ����֘A
	MoveStack* gen_check_long(const Color us, MoveStack* mlist) const;
	MoveStack* gen_check_short(const Color us, MoveStack* mlist) const;
	MoveStack* gen_check_drop(const Color us, MoveStack* mlist, bool &bUchifudume) const;
	MoveStack* generate_check(const Color us, MoveStack* mlist, bool &bUchifudume) const;

	// 3��l�ߗp�̎萶��
	MoveStack* gen_check_drop3(const Color us, MoveStack* mlist, bool &bUchifudume) const;
	MoveStack* generate_check3(const Color us, MoveStack* mlist, bool &bUchifudume) const;			// ���萶��
	// ��������̐���(3��l�ߎc��2��p)
	MoveStack *generate_evasion_rest2(const Color us, MoveStack *mBuf, effect_t effect, int &Ai);
	MoveStack *generate_evasion_rest2_MoveAi(const Color us, MoveStack *mBuf, effect_t effect);
	MoveStack *generate_evasion_rest2_DropAi(const Color us, MoveStack *mBuf, effect_t effect, int &check_pos);

	// �ǖʂ̕]��
	static void init_evaluate();
	int make_list(int * pscore, int list0[], int list1[] ) const;
	int evaluate(const Color us) const;

	// ��딻��(bInaniwa �ɃZ�b�g���邽�� const �łȂ�)
	bool IsInaniwa(const Color us);

	// �@�\�F�����錾�ł��邩�ǂ������肷��
	bool IsKachi(const Color us) const;

	// ���肩�ǂ����H
	bool is_check_move(const Color us, Move move) const;

	// ����1��l��
	static void initMate1ply();
	template <Color us> uint32_t infoRound8King() const;
	// ��ł��ɂ�藘�����Ղ邩�H
	template <Color us> uint32_t chkInterceptDrop(const uint32_t info, const int kpos, const int i, const int kind) const;
	// ��ړ��ɂ�藘�����Ղ邩�H
	template <Color us> uint32_t chkInterceptMove(const uint32_t info, const int kpos, const int i, const int from, const int kind) const;
	// ��łɂ����l�̔���
	template <Color us> uint32_t CheckMate1plyDrop(const uint32_t info, Move &m) const;
	int CanAttack(const int kind, const int from1, const int from2, const int to) const;
	// �ړ��ɂ����l�̔���
	template <Color us> uint32_t CheckMate1plyMove(const uint32_t info, Move &m, const int check = 0) const;
	template <Color us> int Mate1ply(Move &m, uint32_t &info);

	// 3��l��
	int Mate3(const Color us, Move &m);
//	int EvasionRest2(const Color us, MoveStack *antichecks, unsigned int &PP, unsigned int &DP, int &dn);
	int EvasionRest2(const Color us, MoveStack *antichecks);

	template<Color>
	effect_t exist_effect(int pos) const;				// ����
	template<Color us>
	int sq_king() const {return (us == BLACK) ? knpos[1] : knpos[2];}

	// �ǖʂ�Huffman����������
	int EncodeHuffman(unsigned char buf[32]) const;
#endif

	// Static exchange evaluation
	int see(Move m) const;
	int see_sign(Move m) const;

	// Accessing hash keys
	Key get_key() const;
	Key get_exclusion_key() const;
#if !defined(NANOHA)
	Key get_pawn_key() const;
	Key get_material_key() const;
#endif

	// Incremental evaluation
	Score value() const;
#if !defined(NANOHA)
	Value non_pawn_material(Color c) const;
	Score pst_delta(Piece piece, Square from, Square to) const;
#endif

	// Game termination checks
	bool is_mate() const;
#if defined(NANOHA)
	// �����͓���ǖʂ����Ȃ����������͂Ȃ�(�������͕ʂŔ��f)�Ȃ̂ŁASkipRepetition�͎g��Ȃ�
	// �߂�ltrue�͐����
	// �߂�lfalse��ret==0�͂Ȃɂ��Ȃ�
	// �߂�lfalse��ret==-1�͘A������̐����
	bool is_draw(int& ret) const;
#else
	template<bool SkipRepetition> bool is_draw() const;
#endif

	// Plies from start position to the beginning of search
	int startpos_ply_counter() const;

	// Other properties of the position
#if !defined(NANOHA)
	bool opposite_colored_bishops() const;
	bool has_pawn_on_7th(Color c) const;
	bool is_chess960() const;
#endif

	// Current thread ID searching on the position
	int thread() const;
#if defined(NANOHA)
	int64_t tnodes_searched() const;
	void set_tnodes_searched(int64_t n);
#if defined(CHK_PERFORM)
	unsigned long mate3_searched() const;
	void set_mate3_searched(unsigned long  n);
	void inc_mate3_searched(unsigned long  n=1);
#endif // defined(CHK_PERFORM)
#endif

	int64_t nodes_searched() const;
	void set_nodes_searched(int64_t n);

	// Position consistency check, for debugging
	bool is_ok(int* failedStep = NULL) const;
	void flip_me();
#if defined(NANOHA)
	uint32_t handValue_of_side() const {return hand[sideToMove].h; }
	template<Color us> uint32_t handValue() const {return hand[us].h; }
	int get_material() const { return material; }
#endif

	// Global initialization
	static void init();

#if defined(NANOHA)
	static unsigned char relate_pos(int z1, int z2) {return DirTbl[z1][z2];}	// z1��z2�̈ʒu�֌W.
#endif
private:

	// Initialization helper functions (used while setting up a position)
	void clear();
	void put_piece(Piece p, Square s);
#if !defined(NANOHA)
	void set_castle(int f, Square ksq, Square rsq);
	void set_castling_rights(char token);
#endif
	bool move_is_legal(const Move m) const;

#if defined(NANOHA)
	void init_position(const unsigned char board_ori[9][9], const int Mochigoma_ori[]);
	void make_pin_info();
	void init_effect();
#endif

	// Helper functions for doing and undoing moves
#if !defined(NANOHA)
	void do_capture_move(Key& key, PieceType capture, Color them, Square to, bool ep);
	void do_castle_move(Move m);
	void undo_castle_move(Move m);

	template<bool FindPinned>
	Bitboard hidden_checkers() const;
#endif

	// Computing hash keys from scratch (for initialization and debugging)
	Key compute_key() const;
#if defined(NANOHA)
	int compute_material() const;
#endif
#if !defined(NANOHA)
	Key compute_pawn_key() const;
	Key compute_material_key() const;

	// Computing incremental evaluation scores and material counts
	Score pst(Piece p, Square s) const;
	Score compute_value() const;
	Value compute_non_pawn_material(Color c) const;
#endif

#if !defined(NANOHA)
	// Board
	Piece board[64];             // [square]

	// Bitboards
	Bitboard byTypeBB[8];        // [pieceType]
	Bitboard byColorBB[2];       // [color]

	// Piece counts
	int pieceCount[2][8];        // [color][pieceType]

	// Piece lists
	Square pieceList[2][8][16];  // [color][pieceType][index]
	int index[64];               // [square]
#endif

#if defined(NANOHA)
	Piece banpadding[16*2];		// Padding
	Piece ban[16*12];			// �Տ�� (����)
	PieceNumber_t komano[16*12];		// �Տ�� (��ԍ�)
#define MAX_KOMANO	40
	effect_t effect[2][16*12];				// ����
#define effectB	effect[BLACK]
#define effectW	effect[WHITE]

#define IsCheckS()	EXIST_EFFECT(effectW[kingS])	/* ���ʂɉ��肪�������Ă��邩? */
#define IsCheckG()	EXIST_EFFECT(effectB[kingG])	/* ���ʂɉ��肪�������Ă��邩? */
	int pin[16*10];					// �s��(���ƌ�藼�p)
	Hand hand[2];					// ����
#define handS	hand[BLACK]
#define handG	hand[WHITE]
	PieceKind_t knkind[MAX_PIECENUMBER+1];	// knkind[num] : ��ԍ�num�̋���(EMP(0x00) �` GRY(0x1F))
	uint8_t knpos[MAX_PIECENUMBER+1];		// knpos[num]  : ��ԍ�num�̔Տ�̍��W(0:���g�p�A1:��莝��A2:��莝��A0x11-0x99:�Տ�)
									//    ��ԍ�
#define KNS_SOU	1
#define KNE_SOU	1
								//        1    : ����
#define KNS_GOU	2
#define KNE_GOU	2
								//        2    : ����
#define KNS_HI	3
#define KNE_HI	4
								//        3- 4 : ��
#define KNS_KA	5
#define KNE_KA	6
								//        5- 6 : �p
#define KNS_KI	7
#define KNE_KI	10
								//        7-10 : ��
#define KNS_GI	11
#define KNE_GI	14
								//       11-14 : ��
#define KNS_KE	15
#define KNE_KE	18
								//       15-18 : �j
#define KNS_KY	19
#define KNE_KY	22
								//       19-22 : ��
#define KNS_FU	23
#define KNE_FU	40
								//       23-40 : ��
#define kingS	sq_king<BLACK>()
#define kingG	sq_king<WHITE>()
#define hiPos	(&knpos[ 3])
#define kaPos	(&knpos[ 5])
#define kyPos	(&knpos[19])
#define IsHand(x)	((x) <  0x11)
#define OnBoard(x)	((x) >= 0x11)
	int material;
	bool bInaniwa;

#endif

	// Other info
#if !defined(NANOHA)
	int castleRightsMask[64];    // [square]
	Square castleRookSquare[16]; // [castleRight]
#endif
	StateInfo startState;
	int64_t nodes;
	int startPosPly;
	Color sideToMove;		// ��Ԃ̐F
	int threadID;
#if defined(NANOHA)
	int64_t tnodes;
	unsigned long count_Mate1plyDrop;		// ��ł��ŋl�񂾉�
	unsigned long count_Mate1plyMove;		// ��ړ��ŋl�񂾉�
	unsigned long count_Mate3ply;			// Mate3()�ŋl�񂾉�
#endif
	StateInfo* st;
#if !defined(NANOHA)
	int chess960;
#endif

	// Static variables
#if defined(NANOHA)
//  static Key zobrist[2][RY+1][0x100];
	static Key zobrist[GRY+1][0x100];
#else
	static Score pieceSquareTable[16][64]; // [piece][square]
	static Key zobrist[2][8][64];          // [color][pieceType][square]
	static Key zobEp[64];                  // [square]
	static Key zobCastle[16];              // [castleRight]
#endif
	static Key zobSideToMove;		// ��Ԃ���ʂ���
	static Key zobExclusion;		// NULL MOVE���ǂ�����ʂ���
#if defined(NANOHA)
	static unsigned char DirTbl[0xA0][0x100];	// �����p[from][to]

	// ���萶���p�e�[�u��
	static const struct ST_OuteMove2 {
		int from;
		struct {
			int to;
			int narazu;
			int nari;
		} to[6];
	} OuteMove2[32];
	static uint32_t TblMate1plydrop[0x10000];	// ��ł��ŋl�ޔ��f������e�[�u��.

	friend void init_application_once();	// ���s�t�@�C���N�����ɍs��������.
	friend class SearchMateDFPN;
#endif
};

inline int64_t Position::nodes_searched() const {
	return nodes;
}

inline void Position::set_nodes_searched(int64_t n) {
	nodes = n;
}

#if defined(NANOHA)
inline int64_t Position::tnodes_searched() const {
	return tnodes;
}

inline void Position::set_tnodes_searched(int64_t n) {
	tnodes = n;
}

#if defined(CHK_PERFORM)
inline unsigned long Position::mate3_searched() const {
	return count_Mate3ply;
}
inline void Position::set_mate3_searched(unsigned long  n) {
	count_Mate3ply = n;
}
inline void Position::inc_mate3_searched(unsigned long  n) {
	count_Mate3ply += n;
}
#endif // defined(CHK_PERFORM)

#endif

inline Piece Position::piece_on(Square s) const {
#if defined(NANOHA)
	return ban[s];
#else
	return board[s];
#endif
}

inline bool Position::square_is_empty(Square s) const {
#if defined(NANOHA)
	return ban[s] == EMP;
#else
	return board[s] == PIECE_NONE;
#endif
}

#if defined(NANOHA)
inline Value Position::promote_value_of_piece_on(Square s) const {
	return Value(NanohaTbl::KomaValuePro[type_of(piece_on(s))]);
}

inline int Position::pin_on(Square s) const {
	return pin[s];
}
#endif

inline Color Position::side_to_move() const {
	return sideToMove;
}

#if !defined(NANOHA)
inline Bitboard Position::occupied_squares() const {
	return byTypeBB[0];
}

inline Bitboard Position::empty_squares() const {
	return ~byTypeBB[0];
}

inline Bitboard Position::pieces(Color c) const {
	return byColorBB[c];
}

inline Bitboard Position::pieces(PieceType pt) const {
	return byTypeBB[pt];
}

inline Bitboard Position::pieces(PieceType pt, Color c) const {
	return byTypeBB[pt] & byColorBB[c];
}

inline Bitboard Position::pieces(PieceType pt1, PieceType pt2) const {
	return byTypeBB[pt1] | byTypeBB[pt2];
}

inline Bitboard Position::pieces(PieceType pt1, PieceType pt2, Color c) const {
	return (byTypeBB[pt1] | byTypeBB[pt2]) & byColorBB[c];
}

inline int Position::piece_count(Color c, PieceType pt) const {
	return pieceCount[c][pt];
}

inline const Square* Position::piece_list(Color c, PieceType pt) const {
	return pieceList[c][pt];
}

inline Square Position::ep_square() const {
	return st->epSquare;
}
#endif

inline Square Position::king_square(Color c) const {
#if defined(NANOHA)
	return (c == BLACK) ? Square(sq_king<BLACK>()) : Square(sq_king<WHITE>());
#else
	return pieceList[c][KING][0];
#endif
}

#if !defined(NANOHA)
inline bool Position::can_castle(CastleRight f) const {
	return st->castleRights & f;
}

inline bool Position::can_castle(Color c) const {
	return st->castleRights & ((WHITE_OO | WHITE_OOO) << c);
}

inline Square Position::castle_rook_square(CastleRight f) const {
	return castleRookSquare[f];
}

template<>
inline Bitboard Position::attacks_from<PAWN>(Square s, Color c) const {
	return StepAttacksBB[make_piece(c, PAWN)][s];
}

template<PieceType Piece> // Knight and King and white pawns
inline Bitboard Position::attacks_from(Square s) const {
	return StepAttacksBB[Piece][s];
}

template<>
inline Bitboard Position::attacks_from<BISHOP>(Square s) const {
	return bishop_attacks_bb(s, occupied_squares());
}

template<>
inline Bitboard Position::attacks_from<ROOK>(Square s) const {
	return rook_attacks_bb(s, occupied_squares());
}

template<>
inline Bitboard Position::attacks_from<QUEEN>(Square s) const {
	return attacks_from<ROOK>(s) | attacks_from<BISHOP>(s);
}

inline Bitboard Position::checkers() const {
	return st->checkersBB;
}
#endif

inline bool Position::in_check() const {
#if defined(NANOHA)
	int pos = king_square(sideToMove);
	if (pos == 0) return false;
	Color them = flip(sideToMove);
	return EXIST_EFFECT(effect[them][pos]);
#else
	return st->checkersBB != 0;
#endif
}

#if defined(NANOHA)
inline bool Position::at_checking() const {
	int pos = (sideToMove == BLACK) ? king_square(WHITE) : king_square(BLACK);
	if (pos == 0) return false;
	return EXIST_EFFECT(effect[sideToMove][pos]);
}
#endif

#if !defined(NANOHA)
inline bool Position::pawn_is_passed(Color c, Square s) const {
	return !(pieces(PAWN, flip(c)) & passed_pawn_mask(c, s));
}
#endif

inline Key Position::get_key() const {
#if defined(NANOHA) && !defined(NDEBUG)
	if (sideToMove == BLACK) {
		if ((st->key & 1) != 0) {
			MYABORT();
		}
	} else {
		if ((st->key & 1) == 0) {
			MYABORT();
		}
	}
#endif
	return st->key;
}

inline Key Position::get_exclusion_key() const {
#if defined(NANOHA) && !defined(NDEBUG)
	if (sideToMove == BLACK) {
		if ((st->key & 1) != 0) {
			MYABORT();
		}
	} else {
		if ((st->key & 1) == 0) {
			MYABORT();
		}
	}
#endif
	return st->key ^ zobExclusion;
}

#if !defined(NANOHA)
inline Key Position::get_pawn_key() const {
	return st->pawnKey;
}

inline Key Position::get_material_key() const {
	return st->materialKey;
}
#endif

#if !defined(NANOHA)
inline Score Position::pst(Piece p, Square s) const {
	return pieceSquareTable[p][s];
}
#endif

#if !defined(NANOHA)
inline Score Position::pst_delta(Piece piece, Square from, Square to) const {
	return pieceSquareTable[piece][to] - pieceSquareTable[piece][from];
}
#endif

#if !defined(NANOHA)
inline Score Position::value() const {
	return st->value;
}
#endif

#if !defined(NANOHA)
inline Value Position::non_pawn_material(Color c) const {
	return st->npMaterial[c];
}

inline bool Position::is_passed_pawn_push(Move m) const {

	return   board[move_from(m)] == make_piece(sideToMove, PAWN)
	      && pawn_is_passed(sideToMove, move_to(m));
}
#endif

inline int Position::startpos_ply_counter() const {
	return startPosPly + st->pliesFromNull; // HACK
}

#if !defined(NANOHA)
inline bool Position::opposite_colored_bishops() const {

	return   pieceCount[WHITE][BISHOP] == 1
	      && pieceCount[BLACK][BISHOP] == 1
	      && opposite_colors(pieceList[WHITE][BISHOP][0], pieceList[BLACK][BISHOP][0]);
}

inline bool Position::has_pawn_on_7th(Color c) const {
	return pieces(PAWN, c) & rank_bb(relative_rank(c, RANK_7));
}

inline bool Position::is_chess960() const {
	return chess960;
}
#endif

inline bool Position::is_capture_or_promotion(Move m) const {
#if defined(NANOHA)
	return move_captured(m) != EMP || is_promotion(m);
#else

	assert(is_ok(m));
	return is_special(m) ? !is_castle(m) : !square_is_empty(move_to(m));
#endif
}

inline bool Position::is_capture(Move m) const {
#if defined(NANOHA)
	return !square_is_empty(move_to(m));
#else
	// Note that castle is coded as "king captures the rook"
	assert(is_ok(m));
	return (!square_is_empty(move_to(m)) && !is_castle(m)) || is_enpassant(m);
#endif
}

inline PieceType Position::captured_piece_type() const {
#if defined(NANOHA)
	return type_of(st->captured);
#else
	return st->capturedType;
#endif
}

inline int Position::thread() const {
	return threadID;
}

#if defined(NANOHA)
// �ȉ��A�Ȃ̂͌ŗL����
template<Color us>
effect_t Position::exist_effect(int pos) const {
	return effect[us][pos];
}

// �w��ʒu����w������ɉ�������ʒu�܂ŒT��(WALL or ���� or ����̈ʒu�ɂȂ�)
inline int Position::SkipOverEMP(int pos, const int dir) const {
	if (ban[pos += dir] != EMP) return pos;
	if (ban[pos += dir] != EMP) return pos;
	if (ban[pos += dir] != EMP) return pos;
	if (ban[pos += dir] != EMP) return pos;
	if (ban[pos += dir] != EMP) return pos;
	if (ban[pos += dir] != EMP) return pos;
	if (ban[pos += dir] != EMP) return pos;
	if (ban[pos += dir] != EMP) return pos;
	return pos + dir;
}

// ����`�F�b�N(true:pos�̋؂ɕ������遁����ɂȂ�Afalse:pos�̋؂ɕ����Ȃ�)
inline bool Position::is_double_pawn(const Color us, const int pos) const
{
	const Piece *p = &(ban[(pos & ~0x0F)+1]);
	Piece pawn = (us == BLACK) ? SFU : GFU;
	for (int i = 0; i < 9; i++) {
		if (*p++ == pawn) {
			return true;
		}
	}
	return false;
}

// �����֘A
template<Color turn>
inline void Position::add_effect_straight(const int z, const int dir, const uint32_t bit)
{
	int zz = z;
	do {
		zz += dir;
		effect[turn][zz] |= bit;
	} while(ban[zz] == EMP);

	// �����͑���ʂ�������т�
	const int enemyKing = (turn == BLACK) ? GOU : SOU;
	if (ban[zz] == enemyKing) {
		zz += dir;
		if (ban[zz] != WALL) {
			effect[turn][zz] |= bit;
		}
	}
}
template<Color turn>
inline void Position::del_effect_straight(const int z, const int dir, const uint32_t bit)
{
	int zz = z;
	do {
		zz += dir; effect[turn][zz] &= bit;
	} while(ban[zz] == EMP);

	// �����͑���ʂ�������т�
	const int enemyKing = (turn == BLACK) ? GOU : SOU;
	if (ban[zz] == enemyKing) {
		zz += dir;
		if (ban[zz] != WALL) {
			effect[turn][zz] &= bit;
		}
	}
}

// �s�����X�V
template<Color turn>
inline void Position::add_pin_info(const int dir) {
	int z;
	const Color rturn = (turn == BLACK) ? WHITE : BLACK;
	z = (turn == BLACK) ? SkipOverEMP(kingS, -dir) : SkipOverEMP(kingG, -dir);
	if (ban[z] != WALL) {
		if ((turn == BLACK && (ban[z] & GOTE) == 0)
		 || (turn == WHITE && (ban[z] & GOTE) != 0)) {
			effect_t eft = (turn == BLACK) ? EFFECT_KING_S(z) : EFFECT_KING_G(z);
			if (eft & (effect[rturn][z] >> EFFECT_LONG_SHIFT)) pin[z] = dir;
		}
	}
}
template<Color turn>
void Position::del_pin_info(const int dir) {
	int z;
	z = (turn == BLACK) ? SkipOverEMP(kingS, -dir) : SkipOverEMP(kingG, -dir);
	if (ban[z] != WALL) {
		if ((turn == BLACK && (ban[z] & GOTE) == 0)
		 || (turn == WHITE && (ban[z] & GOTE) != 0)) {
			pin[z] = 0;
		}
	}
}
#endif
#endif // !defined(POSITION_H_INCLUDED)

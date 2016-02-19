#
# �Ȃ̂�mini�ɂ���Ƃ���NANOHAMINI=1�̑O��#�����A
# �Ȃ̂�micro�ɂ���Ƃ���NANOHAMINI=1�̑O��#��t���ANANOHAMICRO=1�̑O��#�����
#
NANOHAMINI=1
#NANOHAMICRO=1

!IFDEF NANOHAMINI
EVAL_TYPE=EVAL_OLD
EXE = nanohamini.exe
PGD = nanohamini.pgd
PGOLOG = nanohamini_prof.txt
!ELSEIFDEF NANOHAMICRO
EVAL_TYPE=EVAL_MICRO
EXE = nanohamicro.exe
PGD = nanohamicro.pgd
PGOLOG = nanohamicro_prof.txt
!ELSE
!ERROR undefined eval_type
!ENDIF

OBJS = mate1ply.obj misc.obj timeman.obj evaluate.obj position.obj \
	 tt.obj main.obj move.obj \
	 movegen.obj search.obj uci.obj movepick.obj thread.obj ucioption.obj \
	 benchmark.obj book.obj \
	 shogi.obj mate.obj problem.obj

CC=cl
LD=link



# Compile Options
#
# -DEVAL_OLD        �Ȃ̂�mini(2��֌W�̕]���֐�)
# -DEVAL_MICRO      �Ȃ̂�micro(��݂̂̕]���֐�)
#
# Visual C++�I�v�V����
#
# /D_CRT_SECURE_NO_WARNINGS
#                   secure�Ȋ֐����g���Ă��Ȃ��Ƃ��̌x�����o���Ȃ�
# /Zc:forScope      �X�R�[�v ���[�v�ɕW�� C++ ��K�p����
# /Wall             �x�������ׂėL���ɂ���
# /GS[-]            �Z�L�����e�B �`�F�b�N��L���ɂ���
# /favor:<blend|AMD64|EM64T> �œK������v���Z�b�T
# /GL[-]            �����N���̃R�[�h�������s��
# /RTCs             �X�^�b�N �t���[�� �����^�C�� �`�F�b�N
# /RTCu             ����������Ă��Ȃ����[�J���ϐ��̃`�F�b�N

FLAGS = -DNDEBUG -D$(EVAL_TYPE) -DNANOHA -DCHK_PERFORM  \
	-DOLD_LOCKS /favor:AMD64 /EHsc /D_CRT_SECURE_NO_WARNINGS \
	 /GL /Zc:forScope
#CXXFLAGS=$(FLAGS) /MT /W4 /Wall /nologo /Od /GS /RTCsu
CXXFLAGS=$(FLAGS) /MD /W3 /nologo /Ox /Ob2 /GS- /Gm /Zi
LDFLAGS=/NOLOGO /STACK:16777216,32768 /out:$(EXE) /LTCG /DEBUG
PGOLDFLAGS1=/NOLOGO /STACK:16777216,32768 /out:$(EXE) /LTCG:PGI
PGOLDFLAGS2=/NOLOGO /STACK:16777216,32768 /out:$(EXE) /LTCG:PGO


all: $(EXE)

$(EXE) : $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) User32.lib

.cpp.obj :
	$(CC) $(CXXFLAGS) /c $*.cpp

clean :
	del /q *.obj
	del /q *.idb
	del /q *.pdb
	del /q *.pgc
	del /q *.pgd
	del /q *.suo
	del    $(PGOLOG)
	del    $(EXE)

pgo: $(OBJS)
	$(LD) $(PGOLDFLAGS1) $(OBJS) User32.lib
	$(EXE) bench 128 2 9
	pgomgr /merge $(PGD)
	pgomgr /summary $(PGD) > $(PGOLOG)
	$(LD) $(PGOLDFLAGS2) $(OBJS) User32.lib

prof-clean:
	del /q *.pgc
	del /q *.pgd
	del    $(PGOLOG)
	del    $(EXE)

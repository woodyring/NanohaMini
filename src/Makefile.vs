EXE = nanohamini.exe
#EXE = nanohamicro.exe

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

FLAGS = -DNDEBUG -DEVAL_OLD -DNANOHA -DCHK_PERFORM  \
	-DOLD_LOCKS /favor:AMD64 /EHsc /D_CRT_SECURE_NO_WARNINGS \
	 /GL /Zc:forScope
#CXXFLAGS=$(FLAGS) /MT /W4 /Wall /nologo /Od /GS /RTCsu
CXXFLAGS=$(FLAGS) /MD /W3 /nologo /Ox /Ob2 /GS- /Gm /Zi
LDFLAGS=/NOLOGO /STACK:16777216,32768 /out:$(EXE) /LTCG


all: $(EXE)

$(EXE) : $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) User32.lib

.cpp.obj :
	$(CC) $(CXXFLAGS) /c $*.cpp

clean :
	del /q *.obj
	del /q *.idb
	del /q *.pdb
	del /q $(EXE)

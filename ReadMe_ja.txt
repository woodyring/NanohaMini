==================================================
【 ソフト名 】なのはmini
【バージョン】0.2.2.1
【  作成日  】2016/01/04
【  製作者  】かず＠なのは(川端一之)
==================================================

■はじめに
　本プログラムは、USIに対応した将棋プログラムです。
　強いチェスプログラムのStockfishをベースに作成した「なのは」や「Calamity」のミ
　ニ版です。
　※「なのは」や「Calamity」からの主な差分
　　・評価ベクトルを簡略化しました。
　　・長手数詰めルーチンを搭載していません。
　　・定跡データを減らしました。
　　・上記の差分によりたいぶ弱くなっています。


■動作環境
　・Windows(7以降)で動きます。
　・メモリは1GBもあれば十分です。
　・CPUは速いほうがいいです。

　必要なランタイム
  ・Visual Studio 2013 の Visual C++ 再頒布可能パッケージ (x86)
    http://www.microsoft.com/ja-jp/download/details.aspx?id=40784


■インストール
　本プログラムを使用するには、USI(Universal Shogi Interface)に対応した
　将棋所3.0.0が必要になります(おそらく他のバージョンでも動作しますが未確認)。
　
　--------------------------------------
　・将棋所
　　将棋GUIソフト「将棋所」のページ
　　http://www.geocities.jp/shogidokoro/
　--------------------------------------
　1. 同梱されるnanohamini.exe、book_40.jsk、fv_mini.binを将棋所のengineフォルダにコピーします。
　2. コピーしたnanohamini.exeを将棋所にエンジンとして登録します。
　
　登録の仕方は、将棋所の説明を参考にしてください。


■設定について
　将棋所のエンジン管理で設定が変更できます。
  OwnBook          … チェックを入れると定跡データを使います。
  RandomBookSelect … チェックを入れると定跡データをランダムに選択します。
  BookFile         … 定跡データを指定します(デフォルトは book_40.jsk です)
  Threads          … スレッド数を指定します(CPUのコア数を推奨)。
  Hash             … ハッシュサイズを指定します。
  ByoyomiMargin    … 秒読みで思考を指定した時間(ms単位)早めに打ち切ります。
                      (例:500と指定し、秒読み3秒の場合、約2.5秒で指します)


■アンインストール
　将棋所にエンジンとして登録している場合は、
　まずは将棋所から登録を解除してください。
　登録を解除したら、同梱ファイル（後述）を削除して完了です。


■コンパイル方法
　・Visual Studio Community 2013 で確認しています(2015ではコンパイルできません)。

  1. 適当なところに展開します。
  2. VS2013 x86 Native tools Command Prompt (または VS2013 x86 Native tools Command Prompt)を起動します。
  3. 展開したところにカレントディレクトリを移動します。
  4. nmake -f Makefile.vs とすれば、コンパイルできます。
     (nmake -f Makefile.vs pgo とすれば、PGOが効いてより高速になると思います)

  ※Cygwin でも make とすれば、コンパイルできると思います。

  ・AperyWCSC25の評価ファイルを使うようにコンパイルすることもできます。

  1. Makefile.vsのNANOPERY=1のみを有効にする
  2. 上に準じてコンパイルする
  3. 使うときは AperyWCSC25 の KK_synthesized.bin, KKP_synthesized.bin, KPP_synthesized.bin と
     nanopery.exe を同じフォルダに入れ、USIエンジン登録します。


■同梱ファイル
　実行に必要なファイルはbinに、ソースはsrcにまとめています。
　・Copying.txt
　　GNU GENERAL PUBLIC LICENSEです。
　・ReadMe_ja.txt
　　このファイルです。
　・bin/nanohamini.exe
　　なのはminiの本体です。
　　これをUSIエンジンとして将棋所に登録します。
　　単体では動作しないので、ご注意ください。
　・bin/fv_mini.bin
　　nanohamini用評価ベクトルファイルです。
　・bin/nanohanano.exe
　　なのはnanoの本体です。KPのみの評価関数です。
　・bin/fv_nano.bin
　　nanohanano用評価ベクトルファイルです。
　・bin/book_40.jsk
　　定跡データファイルです。
　・src ディレクトリ以下のファイル
　　なのはminiのソースファイルおよびmakefileです。


■使用条件および免責等
　・GPLに従います。詳細は同梱のCopying.txtを参照してください。

　・なのはminiの著作権は作者（かず＠なのは）が保有します。 
　・なのはminiを再配布するときは、GPLに従って行ってください。 
　・バグは内在していると思いますので、ご指摘いただければありがたいです。
　  (なるべくやさしくお願いします)


■支援等
　もし、このソフトを気に入って支援をしていただける方がおられましたら、アマゾン
のほしい物リストを公開していますのでそちらからご支援等頂ければ、それはとっても
うれしいなって思います。
また、アマゾンで買い物をするときには当方のブログのアマゾン検索から探していただ
くのも力になります。

  欲しいもの： http://www.amazon.co.jp/registry/wishlist/2UMG7J5WTXP3D/
  ブログ    ： http://vivio.blog.shinobi.jp/


■謝辞
　・ベースとなったStockfish開発者Marco Costalba, Joona Kiiski, Tord Romstadに感
　　謝します。
　・なのはminiの開発にあたって gpsfish を参考にしました。TeamGPS各位に感謝します。
　・Woodyさんのブログ記事もとても参考にしています。
　・れさぴょんも参考にしています。開発者の池さんに感謝します。


■参考文献
・小谷善行、他:「コンピュータ将棋」, サイエンス社, 1990.
・松原仁 編著:「コンピュータ将棋の進歩」, 共立出版, 1996.
・松原仁 編著:「コンピュータ将棋の進歩2」, 共立出版, 1998.
・松原仁 編著:「コンピュータ将棋の進歩3」, 共立出版, 2000.
・松原仁 編著:「アマ四段を超えるコンピュータ将棋の進歩4」, 共立出版, 2003.
・松原仁 編著:「アマトップクラスに迫るコンピュータ将棋の進歩5」, 共立出版, 2005.
・池泰弘:「コンピュータ将棋のアルゴリズム」, 工学社, 2005.
・金子知適, 田中哲朗, 山口和紀, 川合慧:「新規節点で固定深さの探索を併用するdf-pn アルゴリズム」, 第10回ゲーム・プログラミングワークショップ, pp.1-8, 2005.
・脊尾昌宏:「詰将棋を解くアルゴリズムにおける優越関係の効率的な利用について」, 第5回ゲーム・プログラミングワークショップ, pp. 129-136, 1999.
・保木邦仁:「局面評価の学習を目指した探索結果の最適制御」 http://www.geocities.jp/bonanza_shogi/gpw2006.pdf 
・岸本章宏:「IS 将棋の詰将棋解答プログラムについて」, http://www.is.titech.ac.jp/~kishi/pdf_file/csa.pdf, 2004.
・橋本剛, 上田徹, 橋本隼一:「オセロ求解へ向けた取り組み」, http://www.lab2.kuis.kyoto-u.ac.jp/~itohiro/Games/Game080307.html


■参考web(読めなくなったページも含みます)
・千里の道も一歩から: http://woodyring.blog.so-net.ne.jp/
・小宮日記: http://d.hatena.ne.jp/mkomiya/
・State of the Digital Shogics [最先端計数将棋学]: http://ameblo.jp/professionalhearts/
・ながとダイアリー: http://d.hatena.ne.jp/mclh46/
・毎日がEveryday: http://d.hatena.ne.jp/issei_y/
・Bonanzaソース完全解析ブログ: http://d.hatena.ne.jp/LS3600/
・aki.の日記: http://d.hatena.ne.jp/ak11/
・FPGA で将棋プログラムを作ってみるブログ: http://blog.livedoor.jp/yss_fpga/


■履歴
2016/01/04 Ver.0.2.2.1   いろいろバグ修正
2015/12/01 Ver.0.2.2     いろいろバグ修正、なのはnano追加
2014/12/26 Ver.0.2.1.1   ByoyomiMarginの追加、MultiPVの削除、致命的バグ修正
2014/12/23 Ver.0.2.1     初版

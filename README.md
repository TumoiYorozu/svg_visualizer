# p5visualizer
競プロ用。C++コードに描画コマンドを埋め込み、ファイルを出力し、それをブラウザ上で表示させる


## 特徴
提出コードに、描画コマンドが埋め込めるので、簡単！
* スコア判定などをビジュアライザ用と提出コード用2回書かずに済む。
* 提出コードの一時変数を表示させたいときに、直接描画できるので便利。
* TODO
* p5.jsに準拠しているので、それなりに機能も豊富。

### コードの例
* TODO
![image](https://github.com/shindannin/p5visualizer/assets/8682966/6c90c405-d893-48eb-8c66-2e3aa08903db)

### ビジュアライズの例
* TODO
![image](https://github.com/shindannin/p5visualizer/assets/8682966/e4837fad-0408-41c2-9cf5-0711cef2af3a)

## 使用方法
1. 自分のコードに、macro.cppを挿入する。
2. 描画コマンドを書いていく。
3. 自分のコードを実行すると、ファイルVisCommands.txtが出力される。
4. ビジュアライザindex.htmlをブラウザで開く。
* TODO
5. 「ファイル」で先ほど出力されたVisCommands.txtを開く。
6. 表示される。時間のスライダーで表示させたい時間を選べる。

※自分のコードを提出するときは、#define VISUALIZE をコメントアウトしてください。

## 描画命令一覧

### 時間命令
p5.jsにないオリジナルの命令です。ここで指定した時間は再変更するまで全ての描画で使用される。
* time(t) スライダーで時間指定したときに表示される時間を指定する。

### 基本図形
* TODO

### 描画スタイルや属性の一時退避
* push()
* pop()

### テキスト描画
* textAlign(alignX, alignY="")
* textLeading(leading)
* textSize(size)
* textStyle(style)
* textWidth(text)
* textAscent()
* textDescent()
* textWrap(wrap)
* text(str, x, y)
* textFont(fontName)

## サンプル
* TODO
AHC009 ( https://atcoder.jp/contests/ahc009 ) に提出可能です。
* ahc009_sample.cpp 提出コード
* ahc009_VisCommands.txt 出力された描画コマンド

## TODO
* 時間の範囲指定（追加しようか迷う…）
* 入力・出力などテキストを表示させるためのテキストボックス
* push, popなど名前の衝突が起きそうだけど使えそうな関数をどうするか？
* 3D描画

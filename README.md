# p5visualizer
競プロ用。C++コードに描画コマンドを埋め込み、ファイルを出力し、それをブラウザ上で表示させる

## 使用上の注意
- sketch.js内でevalを実行しているので、任意コードが実行できるので誰でも使える場所に置くのは危険です！ローカルやプライベートでの使用に留めておいてください。
- C++マクロを使っているので、他の関数名と衝突等のアクシデントが発生する可能性があります。適宜対応してください。

## 特徴
提出コードに、描画コマンドが埋め込めるので、簡単！
* スコア判定などをビジュアライザ用と提出コード用2回書かずに済む
* 提出コードの一時変数を表示させたいときに、直接描画できるので便利
* p5.jsに準拠しているので、それなりに機能も豊富

### コードの例
![image](https://github.com/shindannin/p5visualizer/assets/8682966/90549c16-58ed-49e3-97df-5945245f150b)

### ビジュアライズの例
![image](https://github.com/shindannin/p5visualizer/assets/8682966/e4837fad-0408-41c2-9cf5-0711cef2af3a)

## 使用方法
1. 自分のmain.cppに、macro.cppを挿入する。
2. main.cppに描画コマンドを書いていく。
3. 実行時にVisCommands.txtが出力される
4. ビジュアライザindex.htmlをブラウザで開く
5. 「ファイルを選択」で先ほどのVisCommands.txtを開く
6. 表示される。時間のスライダーで表示させたい時間を選べる

## 描画
https://p5js.org/reference/ の一部の描画命令をサポートしている。なお、https://editor.p5js.org/ にウェブ版のエディタがあるので、描画コマンドがおかしいときはここでも確認できる。

### 時間命令
 ここで指定した時間は再変更するまで全ての描画で使用される。
* time(t) スライダーで時間指定したときに表示される時間を指定する。
* always(t) スライダーの時間を無視し、いつでも表示される。

### 基本図形
* arc(x,y,w,h,start,stop)
* ellipse(x,y,w,h)
* circle(x,y,d)
* line(x1,y1,x2,y2)
* point(x,y)
* quad(x1,y1,x2,y2,x3,y3,x4,y4)
* rect(x,y,w,h)
* square(x,y,s)
* triangle(x1,y1,x2,y2,x3,y3)

### スタイル指定
* background(gray, alpha = 255)
* background(r, g, b, a)
* background(colorName)
* clear()
* colorMode(mode, max1, max2, max3, maxA)
* fill(gray, alpha = 255)
* fill(r, g, b, a)
* fill(colorName)
* noFill()
* noStroke()
* stroke(gray, alpha = 255)
* stroke(r, g, b, alpha = 255)
* stroke(colorName)
* erase()
* noErase()

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
AHC009 ( https://atcoder.jp/contests/ahc009 ) に提出可能
* ahc009_sample.cpp 提出コード
* ahc009_VisCommands.txt 出力された描画コマンド

## TODO
* 時間の範囲指定（追加しようか迷う…）
* 入力・出力などテキストを表示させるためのテキストボックス
* 関数名そのままで本当にいいのか？衝突事故がおきそう。
* push, popなど名前の衝突が起きそうだけど使えそうな関数をどうするか？
* 3D描画

# SVG Visualizer
競プロ用。C++コードに描画コマンドを埋め込み、ファイルを出力し、それをブラウザ上で表示させる


## 特徴
提出コードに、描画コマンドが埋め込めるので、簡単！
* スコア判定などをビジュアライザ用と提出コード用2回書かずに済む。
* 提出コードの一時変数を表示させたいときに、直接描画できるので便利。
* SVG に準拠しているので、それなりに機能も豊富。
* 画像をクリップボードにコピー、ファイルに保存、GIF動画化して保存、に対応

### コードの例
![image](https://github.com/TumoiYorozu/svg_visualizer/assets/12813429/bab0f1e0-997b-4949-ade6-b1dfde7084f5)
### ビジュアライズの例
![image](https://github.com/TumoiYorozu/svg_visualizer/assets/12813429/d5135771-c598-45c9-bad2-1fce608806e0)

## 簡単な使用方法
1. `python3 run.py [svgが出力されるディレクトリ] [PORT番号]` で、HTTP(,websocket) サーバーを起動する
2. 自分のコードに、macro.cppを挿入する。
3. 描画コマンドを書いていく。
4. 自分のコードを実行すると、ファイルVisCommands.svgが出力される。
4. ビジュアライザ [http://localhost:8080/](http://localhost:8080/) をブラウザで開く。
5. ビジュアライズしたい svg を選択。
6. 表示される。時間のスライダーで表示させたい時間を選べる。

※ ビジュアライザを無効化するときは、#define VISUALIZE をコメントアウトしてください。一応 AtCoder で提出しようとすると undef されます

## 設計
- 座標の指定は `std::complex<double>` を使用しています。
- (x, y) の順で指定します。(r, c) は対応していません
- `VISUALIZE` が宣言されていない場合、描画命令が早期 Return され、パフォーマンスに影響を与えません。
- svg を吐きますが、その管理は構造体では無く gloval 変数で行われます。
- visualizer_helper を使うことで、関数を抜けるときに自動で svg を close できます（正常終了しなくてもビジュアライザ側で補完するので動きます）
- `visualizer_helper visualizer_helper(Point{800, 600});` の様に呼ぶと、幅800, 高さ600 の仮想キャンパスが生成されます。最初に呼んで下さい

### 時間命令
svg にないオリジナルの命令です。ここで指定した時間は再変更するまで全ての描画で使用されます
* time(t) スライダーで時間指定したときに表示される時間を指定する。`-1` や引数を省略すると、全時間で表示されます

### 色と、color(v) 関数
- オプションで指定する色は、`#ffae30` `#999` `orange` の様なカラーコードを string で指定します。
- `color(v)` 関数を用いることで、[0,1]の値を、青→緑→赤のグラデーションに変換した string を得れます

### オプション
SVG の fill, stroke, stroke-width 等を管理する構造体 `Vopt` があります。
- Vopt("塗色")
- Vopt("線色", 線サイズ)
- Vopt("塗色", "線色", 線サイズ)  
のコンストラクタがあります。オプションの位置で `Vopt("red", 2)` の代わりに `{"red", 2}` と書いても良いし、塗りのみの指定なら `rect(p, size, "red")` の様にも書けます。  
メソッドチェーンで `Vopt("塗色").stroke("線色").swidth(線サイズ)` の様にも書けます。  
`Vopt fill(string s)` 関数なども用意されているので、単に `fill("塗色").stroke("線色").swidth(線サイズ)` の様にも書けます。  

### テキスト
* text(Point p, string str, float size, Vopt op={})
座標 p に、フォントサイズ size で str を描画します。string str の他に、int, double 版も定義されています。

- align オプション
デフォルトで左上起点で描画されますが、オプションに `align("C")` で中心に、`align("R")` を指定すれば右揃えで描画されます。  
中心で表示するための `alignC` が定義されているので、`text(p, "Hello", 10, alignC)` で、p を中心に文字を表示できます。


### 基本描画
* circle(Point c, float r, Vopt)  
  c を中心に、半径 r の円
* line(Point p1, Point p2, Vopt)  
  p1 から p2 への線
* rect(Point p, Point size, Vopt)  
  pを左上に、size の四角
* rect2p(Point p, Point q, Vopt)  
  座標 p, q を指定した四角
* rectc(Point c, Point size, float deg = 0, Vopt)  
  c を中心に、deg 度回転した四角
* polygon(const std::vector<Point>& ps, Vopt)  
  多角形
* polyline(const std::vector<Point>& ps, Vopt)  
  線分

### Grid 構造体
グリッドなどを扱いやすくする構造体です。  
`Grid G(W, H)` で W x H のグリッドを扱えるようになります。
デフォルトでキャンパス全体を使用しますが、2個以上並べたいなどのときは、 `Grid(W, H, whole_size, origin)` でGridのキャンパス上の場所を指定できます。

- line(G)
  グリッドに線を引きます。`line(G, "#bbb")` 

- Grid(int x, int y)
  座標(x,y) のセル(後述)が取得できます。
- 

## サンプル
* TODO
AHC009 ( https://atcoder.jp/contests/ahc009 ) に提出可能です。
* ahc009_sample.cpp 提出コード
* ahc009_VisCommands.txt 出力された描画コマンド


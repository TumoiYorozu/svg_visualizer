#include <vector>
#include <string>
#include <algorithm>
#include <complex>
#include <utility>
#include <unordered_map>
#include <optional>
#include <cmath>

namespace visualizer {
using std::string;
using std::vector;
using std::optional;
using std::nullopt;
using Point = std::complex<double>;
Point visualizer_campus_size;
bool visualizer_internal_y_upper = false;

#define VISUALIZE // 提出時にはここをコメントアウトすること。そうしないとTLEする。

#if (defined(ONLINE_JUDGE) || defined(ATCODER) || defined(NOVIS)) && defined(VISUALIZE)
#undef VISUALIZE // 誤提出防止
#endif


#ifdef VISUALIZE
#define VRET ;
FILE* visFile = nullptr;

struct visualizer_helper {
    visualizer_helper(Point size, Point origin = Point(), bool y_upper = false) {
        if (size.imag() <= 0) { size.imag(size.real());}
        visualizer_campus_size = size;
        visFile = fopen("VisCommands.svg", "w");

        fprintf(visFile, "<svg xmlns='http://www.w3.org/2000/svg' version='1.1' width='800' viewBox='%g %g %g %g'", origin.real(), origin.imag(), size.real(), size.imag());
        if (y_upper) {
            visualizer_internal_y_upper = true;
            fprintf(visFile, " transform='scale(1, -1)' transform-origin='0 %g'", size.imag()/2);
            fprintf(visFile, "><style>text {transform: scale(1, -1);}</style");
        }
        fprintf(visFile, ">\n");
    }
    ~visualizer_helper(){
        fprintf(visFile, "</svg>\n");
        fclose(visFile);
    }
};


#include <functional>



namespace buf_internal {
    constexpr int buf_len = 2048;
    char buf[buf_len];
    int buf_i = 0;
    int line_number = 1;
    std::size_t get_buf_hash() {
        std::size_t hash = buf_i;
        const char* str = buf;
        while (*str) {
            hash ^= static_cast<std::size_t>(*str++) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        }
        return hash;
    }
    std::unordered_map<std::size_t, int> compressor;
}

template<typename... Args>
void print(const char* format, Args... args) {
    VRET;

    using namespace buf_internal;
    if constexpr (sizeof...(args) > 0) {
        buf_i += std::snprintf(buf + buf_i, buf_len - buf_i, format, args...);
    } else {
        buf_i += std::snprintf(buf + buf_i, buf_len - buf_i, "%s", format);
    }

    if (buf[buf_i-1] != '\n') return;
    ++line_number;

    bool ignore = (buf[0] == '<' && buf[1] == '!');
    std::size_t hash = get_buf_hash();
    buf_i = 0;
    
    if (ignore || compressor.count(hash) == 0) {
        if (!ignore) compressor.emplace(hash, line_number);
        fprintf(visFile, "%s", buf);
    } else {
        fprintf(visFile, "L%d\n", compressor[hash]);
    }
}

#else
#define VRET return;
struct visualizer_helper { template<typename... Args> visualizer_helper(Args... args) {} };
template<typename... Args>
void print(const char* format, Args... args) { return; }
#endif


struct Vopt {
    struct v {
        optional<string> fill = nullopt;
        optional<string> stroke = nullopt;
        optional<float> swidth = nullopt;

        optional<string> text_anchor = nullopt;
        optional<string> dominant_baseline = nullopt;
        optional<string> title = nullopt;
        optional<string> desc_text = nullopt;
    } v;
    Vopt(){};
    Vopt(const Vopt& op) : v{op.v} {};
    Vopt(const string fill) { v.fill = fill; }
    Vopt(const char*  fill) { v.fill = fill; }
    Vopt(const string stroke, double width) { v.fill = ""; v.stroke=stroke; v.swidth=width; }
    Vopt(const char*  stroke, double width) { v.fill = ""; v.stroke=stroke; v.swidth=width; }
    Vopt(const string fill, const string stroke, double width) { v.fill = fill; v.stroke=stroke; v.swidth=width; }
    Vopt(const char*  fill, const char*  stroke, double width) { v.fill = fill; v.stroke=stroke; v.swidth=width; }


    void p(const optional<string>& a, const char* label, const string empty = "") {
        if (a) print(" %s='%s'", label, (a.value().size()?a.value():empty).c_str());
    }
    void p(const optional<float>& a, const char* label) {
        if (a) print(" %s='%g'", label, a.value());
    }
    void operator()(bool close = true) {
        VRET;
        p(v.fill, "fill", "none");
        p(v.stroke, "stroke");
        p(v.swidth, "stroke-width");
        p(v.text_anchor, "text-anchor");
        p(v.dominant_baseline, "dominant-baseline");
        if (v.desc_text) {
            print(" data-txt='%s'", v.desc_text.value().c_str());
        }
        if (v.title) {
            print("/></g>\n");
            if (!close) {printf("Error! %s:%d\n", __FILE__, __LINE__); abort(); }
        } else {
            if (close) print("/>\n");
        }
    }
    void pre() {
        VRET;
        if (v.title) {
            print("<g><title>%s</title>", v.title.value().c_str());
        }
    }
    Vopt fill(string s)            { Vopt res(*this); res.v.fill = s; return res; }
    Vopt stroke(string s)          { Vopt res(*this); res.v.stroke = s; return res; }
    Vopt swidth(float a)           { Vopt res(*this); res.v.swidth = a; return res; }
    Vopt stroke(string s, float v) { return stroke(s).swidth(v); }

    Vopt title(string s)           { Vopt res(*this); res.v.title = s; return res; } // 改行は &#x0D;
    Vopt desc(string s)            { Vopt res(*this); res.v.desc_text = s; return res; } // 改行は<br>
    template<typename T> optional<T>& over_write(optional<T>& a, const optional<T>& g) {
        if (!a && g) a = g;
        return a;
    }
};

Vopt fill(string s) { Vopt op; return op.fill(s);}
Vopt stroke(string s) { Vopt op; return op.stroke(s);}
Vopt swidth(float a) { Vopt op; return op.swidth(a);}
Vopt stroke(string s, float v) { Vopt op; return op.stroke(s, v);}


string color(double val) { // [0,1]の値を、青→緑→赤のグラデーションに変換
    val = std::clamp(val, 0.0, 1.0);
    double r, g, b;
    if (val < 0.5) {
        double x = val * 2.0;
        r = std::round(30.0 * (1.0 - x) + 144.0 * x);
        g = std::round(144.0 * (1.0 - x) + 255.0 * x);
        b = std::round(255.0 * (1.0 - x) + 30.0 * x);
    } else {
        double x = val * 2.0 - 1.0;
        r = std::round(144.0 * (1.0 - x) + 255.0 * x);
        g = std::round(255.0 * (1.0 - x) + 30.0 * x);
        b = std::round(30.0 * (1.0 - x) + 70.0 * x);
    }
    char buffer[8]; // Buffer for the formatted string
    std::sprintf(buffer, "#%02x%02x%02x", int(r),int(g),int(b));
    return std::string(buffer);
}


void circle(Point c, float r, Vopt op={}) { VRET;
    print("<circle cx='%g' cy='%g' r='%g'", c.real(), c.imag(), r);
    op();
}

void circles(const vector<Point>& cs, float r, Vopt op={}) { VRET;
    for(auto p : cs) circle(p, r, op);
}

void line(Point p1, Point p2, Vopt op={}) { VRET;
    if (op.v.fill && !op.v.stroke && !op.v.swidth) {
        op.v.stroke.swap(op.v.fill);
        op.v.swidth = 1;
    }
    print("<line x1='%g' y1='%g' x2='%g' y2='%g' stroke-linecap='round' stroke-linejoin='round'", p1.real(), p1.imag(), p2.real(), p2.imag());
    op();
}


void rect(Point p, Point size, Vopt op={}) { VRET;
    print("<rect x='%g' y='%g' width='%g' height='%g'", p.real(), p.imag(), size.real(), size.imag());
    op();
}

void rect2p(Point p, Point q, Vopt op={}) { VRET;
    rect(p, q-p, op);
}

void rectc(Point c, Point size, float deg = 0, Vopt op={}) { VRET;
    const Point p = c - size * 0.5;
    print("<rect x='%g' y='%g' width='%g' height='%g' transform='rotate(%g %g %g)'", p.real(), p.imag(), size.real(), size.imag(), deg, c.real(), c.imag());
    op();
}


void polygon(const std::vector<Point>& ps, Vopt op={}) { VRET;
    print("<polygon points='");
    for (auto p : ps) print("%g,%g ", p.real(), p.imag());
    print("' ");
    op();
}

void polyline(const std::vector<Point>& ps, Vopt op={}) { VRET;
    op.pre();
    print("<polyline points='");
    for (auto p : ps) print("%g,%g ", p.real(), p.imag());
    print("'");
    op();
}

void text(Point p, string str, float size, Vopt op={}) { VRET;
    print("<text x='%g' y='%g' font-size='%g'", p.real(), p.imag(), size);

    if (visualizer_internal_y_upper) {
        print(" transform-origin='%g %g'", p.real(), p.imag());
    }
    
    // if (!op.v.text_anchor) op.v.text_anchor = "middle";
    // if (!op.v.dominant_baseline) op.v.dominant_baseline = "middle";
    op(false);
    print(">%s</text>\n", str.c_str());
}

void text(Point p, int num, float size, Vopt op={}) { VRET;
    text(p, std::to_string(num), size, op);
}
void text(Point p, double num, float size, Vopt op={}) { VRET;
    text(p, std::to_string(num), size, op);
}

Vopt align(string align) { // LR, TBI(UD)
    Vopt op;
    if (align.find("R") != string::npos) {
        op.v.text_anchor = "end"; // R
    } else if (align.find("L") != string::npos) {
        op.v.text_anchor = "start"; // L
    } else {
        op.v.text_anchor = "middle"; // C
    }
    if (align.find("T") != string::npos || align.find("U") != string::npos) {
        op.v.dominant_baseline = "hanging "; // T
    } else if (align.find("B") != string::npos || align.find("D") != string::npos) {
        op.v.dominant_baseline = "text-bottom"; // B
    } else if (align.find("I") != string::npos) {
        op.v.dominant_baseline = "ideographic"; // Baseline より下
    } else {
        op.v.dominant_baseline = "middle"; // C
    }
    return op;
}
const Vopt alignC = align("");

void vtime(int a = -1, int b = -1){ VRET;
    if (b < 0) return print("<!--time=%d-->\n",a);
    else       return print("<!--time=%d:%d-->\n",a,b);
}

namespace internal {
    struct Segment : public std::pair<Point, Point>{
        Segment(const Point &p1, const Point &p2) : pair(p1, p2) {}
    };
    struct Box {
        Point p, s;
        Box(Point p, Point s) : p(p), s(s) {};
        
        Point center() const {
            return p + s * 0.5;
        }
        operator Point() const { return center(); }

        Point UpLeft() const {  return p; }
        Point UpRight() const { return p + Point(s.real(), 0); }
        Point BtLeft() const { return p + Point(0, s.imag()); }
        Point BtRight() const  { return p + s; }
        Segment segL()const { return Segment(UpLeft(), BtLeft()); }
        Segment segR() const { return Segment(UpRight(), BtRight()); }
        Segment segU() const { return Segment(UpLeft(), UpRight()); }
        Segment segD() const { return Segment(BtLeft(), BtRight()); }
        Box shrink(double v) const {
            return {p + Point(v, v), s - Point(v*2, v*2)};
        }
    };
    void line(const Segment& seg, Vopt op={}) { VRET;
        line(seg.first, seg.second, op);
    }
    void line(const Box& box, Vopt op={}) { VRET;
        line(box.UpLeft(), box.UpRight(), op);
        line(box.UpRight(), box.BtRight(), op);
        line(box.BtRight(), box.BtLeft(), op);
        line(box.BtLeft(), box.UpLeft(), op);
    }
    void rect(const Box& box, Vopt op={}) { VRET;
        rect(box.p, box.s, op);
    }
} // namespace internal

struct Grid {
    int W_num, H_num;
    Point whole_size{-1,-1}, origin={};
    double cell_w, cell_h;
    Point cell_sz;
    Grid(int WH_num) : W_num(WH_num), H_num(WH_num) { init(); }
    Grid(int W_num, int H_num, Point whole_size={-1,-1}, Point origin={}) : W_num(W_num), H_num(H_num), whole_size(whole_size), origin(origin) {
        init();
    }
    void init() {
        if (whole_size.real() < 0) whole_size = visualizer_campus_size;
        cell_w = whole_size.real() / W_num;
        cell_h = whole_size.imag() / H_num;
        cell_sz = {cell_w, cell_h};
    }
    double world_x(int col) const { return origin.real() + cell_w * col; }
    double world_y(int row) const { return origin.imag() + cell_h * row; }
    double world_top() const { return world_y(0); }
    double world_bottom() const { return world_y(H_num); }
    double world_left() const { return world_x(0); }
    double world_right() const { return world_x(W_num); }

    internal::Segment seg_horizontal(int y, int x, int len = 1) const {
        return {
            Point{world_x(x),  world_y(y)},
            Point{world_x(x+len), world_y(y)}
        };
    }
    internal::Segment seg_horizontal(int y) const {
        return seg_horizontal(y, 0, W_num);
    }
    internal::Segment seg_vertical(int x, int y, int len = 1) const {
        return {
            Point{world_x(x), world_y(y)},
            Point{world_x(x), world_y(y+len)}
        };
    }
    internal::Segment seg_vertical(int x) const {
        return seg_vertical(x, 0, H_num);
    }
    internal::Box _internal_cell(int x, int y, int w, int h) const {
        return {Point(world_x(x), world_y(y)), Point(cell_w*w, cell_h*h) };
    }
    ///
    internal::Box cell(int x, int y, int w, int h) const {
        return _internal_cell(x, y, h, w);
    }
    internal::Box cell(int x, int y) const {
        return _internal_cell(x, y, 1, 1);
    }
    internal::Box whole() const {
        return _internal_cell(0, 0, W_num, H_num);
    }
    internal::Box operator()(int x, int y) const {
        return cell(x, y);
    }
    internal::Box operator()() const {
        return whole();
    }
    ///
};

void line(const Grid& grid, Vopt op={}) { VRET;
    for (int y = 0; y <= grid.H_num; ++y) {
        line(grid.seg_horizontal(y), op);
    }
    for (int x = 0; x <= grid.W_num; ++x) {
        line(grid.seg_vertical(x), op);
    }
}
void rect(const Grid& grid, Vopt op={}) { VRET;
    rect(grid(), op);
    op.v.fill.reset();
    line(grid, op);
}


} // namespace visualizer

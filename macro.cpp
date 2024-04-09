
#include <vector>
#include <string>
#include <algorithm>
#include <complex>
#include <utility>
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

#if defined(ONLINE_JUDGE) || defined(ATCODER)
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

        fprintf(visFile, "<svg xmlns='http://www.w3.org/2000/svg' version='1.1' width='800' viewBox='%f %f %f %f'", origin.real(), origin.imag(), size.real(), size.imag());
        if (y_upper) {
            visualizer_internal_y_upper = true;
            fprintf(visFile, " transform='scale(1, -1)' transform-origin='0 %f'", size.imag()/2);
            fprintf(visFile, "><style>text {transform: scale(1, -1);}</style");
        }
        fprintf(visFile, ">\n");
    }
    ~visualizer_helper(){
        fprintf(visFile, "</svg>\n");
        fclose(visFile);
    }
};
template<typename... Args>
void print(const char* format, Args... args) {
    VRET;
    if constexpr (sizeof...(args) > 0) {
        fprintf(visFile, format, args...);
    } else {
        fprintf(visFile, "%s", format);
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
        if (a) print(" %s='%.2f'", label, a.value());
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
    print("<circle cx='%.2f' cy='%.2f' r='%.2f'", c.real(), c.imag(), r);
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
    print("<line x1='%.2f' y1='%.2f' x2='%.2f' y2='%.2f' stroke-linecap='round' stroke-linejoin='round'", p1.real(), p1.imag(), p2.real(), p2.imag());
    op();
}


void rect(Point p, Point size, Vopt op={}) { VRET;
    print("<rect x='%.2f' y='%.2f' width='%.2f' height='%.2f'", p.real(), p.imag(), size.real(), size.imag());
    op();
}

void rect2p(Point p, Point q, Vopt op={}) { VRET;
    rect(p, q-p, op);
}

void rectc(Point c, Point size, float deg = 0, Vopt op={}) { VRET;
    const Point p = c - size * 0.5;
    print("<rect x='%.2f' y='%.2f' width='%.2f' height='%.2f' transform='rotate(%.2f %.2f %.2f)'", p.real(), p.imag(), size.real(), size.imag(), deg, c.real(), c.imag());
    op();
}


void polygon(const std::vector<Point>& ps, Vopt op={}) { VRET;
    print("<polygon points='");
    for (auto p : ps) print("%.2f,%.2f ", p.real(), p.imag());
    print("' ");
    op();
}

void polyline(const std::vector<Point>& ps, Vopt op={}) { VRET;
    op.pre();
    print("<polyline points='");
    for (auto p : ps) print("%.2f,%.2f ", p.real(), p.imag());
    print("'");
    op();
}

void text(Point p, string str, float size, Vopt op={}) { VRET;
    print("<text x='%.2f' y='%.2f' font-size='%.2f'", p.real(), p.imag(), size);

    if (visualizer_internal_y_upper) {
        print(" transform-origin='%.2f %.2f'", p.real(), p.imag());
    }
    
    op(false);
    print(">%s</text>\n", str.c_str());
}

void text(Point p, int num, float size, Vopt op={}) { VRET;
    text(p, std::to_string(num), size, op);
}
void text(Point p, double num, float size, Vopt op={}) { VRET;
    text(p, std::to_string(num), size, op);
}

Vopt align(string align) { // LR, TBI
    Vopt op;
    if (align.find("R") != string::npos) {
        op.v.text_anchor = "end"; // R
    } else if (align.find("L") != string::npos) {
        op.v.text_anchor = "start"; // L
    } else {
        op.v.text_anchor = "middle"; // C
    }
    if (align.find("T") != string::npos) {
        op.v.dominant_baseline = "hanging "; // T
    } else if (align.find("B") != string::npos) {
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

} // namespace visualizer

#include <cstdint>
#include <string>
#include <emscripten/bind.h>
/*
em++ svg_manager.cpp -O3 -o http/svg_manager.js --bind -s INITIAL_MEMORY=200MB -s MAXIMUM_MEMORY=4GB -sALLOW_MEMORY_GROWTH -s EXPORTED_RUNTIME_METHODS='["ccall", "cwrap"]' -s EXPORTED_FUNCTIONS='["_get_svg", "_malloc", "_free"]'

em++ svg_manager.cpp -o http/svg_manager.js --bind -s MAXIMUM_MEMORY=2GB -sALLOW_MEMORY_GROWTH && sed -i "s$'requestPointerLock'$//'requestPointerLock'$" http/svg_manager.js

em++ svg_manager.cpp -O3 -s ASSERTIONS=0 -s DISABLE_EXCEPTION_CATCHING=1 -o http/svg_manager.js --bind -s INITIAL_MEMORY=1GB -s MAXIMUM_MEMORY=3GB -sALLOW_MEMORY_GROWTH && sed -i "s$'requestPointerLock'$//'requestPointerLock'$" http/svg_manager.js
*/


using std::string;
using std::vector;


struct svg_line {
    int line_num;
    int16_t begin_time, end_time;
    int start_p;
    uint16_t len;
    int16_t z;
    // string svg;
};

vector<svg_line> svg_lines;
string internal_svg;

vector<vector<svg_line>> turn_svgs;
vector<svg_line>         gloval_svgs;
int svg_max_time = 0;

int set_svg(const string &svg) {
    svg_lines.clear();
    gloval_svgs.clear();
    turn_svgs.clear();


    string::size_type i = 0;
    int line_num = 0;
    int16_t begin_time = -1;
    int16_t end_time = -1;
    int max_time = 0;

    constexpr svg_line dummy = {-2, -2, -2, 0, 0};;
    svg_lines.push_back(dummy);;

    for (;;) {
        string::size_type br = i;
        while(br < svg.size()) {
            char c = svg[br];
            ++br;
            if (c == '\r' || c == '\n') break;
        }
        if (br - i > 1) {
            if (svg[i] == 'L') {
                int ln = 0;
                for (int j = i+1; j < br; ++j) {
                    char c = svg[j];
                    if ('0' <= c && c <= '9') {
                        ln = ln * 10 + (c - '0');
                    } else {
                        break;
                    }
                }
                auto tmp = svg_lines[ln];
                tmp.line_num = line_num;
                tmp.begin_time = begin_time;
                tmp.end_time = end_time;
                svg_lines.emplace_back(tmp);
                (begin_time>=0&&begin_time == end_time ? turn_svgs[begin_time] : gloval_svgs).emplace_back(tmp);
            } else if (svg.substr(i, 9) == "<!--time=") {
                if (svg[i+9] == '-') {
                    begin_time = end_time = -1;
                } else {
                    begin_time = end_time = 0;
                    int j = i+9;
                    for (; j < br; ++j) {
                        char c = svg[j];
                        if ('0' <= c && c <= '9') {
                            begin_time = begin_time * 10 + (c - '0');
                        } else {
                            break;
                        }
                    }
                    if (svg[j] != ':') {
                        end_time = begin_time;
                    } else {
                        for (; j < br; ++j) {
                            ++j;
                            char c = svg[j];
                            if ('0' <= c && c <= '9') {
                                end_time = end_time * 10 + (c - '0');
                            } else {
                                break;
                            }
                        }
                    }
                    max_time = std::max<int>(max_time, end_time);
                }
                if (max_time >= turn_svgs.size()) {
                    turn_svgs.resize(max_time + 1);
                }
                svg_lines.emplace_back(dummy);
            } else if (svg.substr(i, 6) == "</svg>") {
                break;
            } else {
                int16_t z = 0;
                if (svg[i] == 'z') {
                    ++i;
                    const bool is_minus = (svg[i] == '-');
                    if (is_minus) ++i;
                    while('0' <= svg[i] && svg[i] <= '9') {
                        z = z * 10 + (svg[i] - '0');
                        ++i;
                    }
                    if (is_minus) z = -z;
                }
                svg_line tmp{line_num, begin_time, end_time, int(i), uint16_t(br - i), z};
                svg_lines.emplace_back(tmp);
                (begin_time>=0&&begin_time == end_time ? turn_svgs[begin_time] : gloval_svgs).emplace_back(tmp);
            }
            ++line_num;
        }
        i = br;
        if (br >= svg.size()) break;
        // if (line_num >= 100) break;
    }
    internal_svg = std::move(svg);
    svg_max_time = max_time;
    return max_time;
}



extern "C" {
string get_svg_res;
char* get_svg(int t) {
    t = std::clamp(t, 0, svg_max_time);

    get_svg_res.clear();

    vector<svg_line> slines;
    slines.reserve(gloval_svgs.size() + turn_svgs[t].size());

    for (const auto &line : gloval_svgs) {
        if (line.begin_time == -1 || (line.begin_time <= t && t <= line.end_time)) {
            slines.emplace_back(line);
        }
    }
    slines.reserve(slines.size() + turn_svgs[t].size());
    for (const auto &line : turn_svgs[t]) {
        slines.emplace_back(line);
    }
    std::sort(slines.begin(), slines.end(), [](const svg_line &a, const svg_line &b) {
        if (a.z != b.z) return a.z < b.z;
        return a.line_num < b.line_num;
    });

    for (const auto line : slines) {
        get_svg_res += internal_svg.substr(line.start_p, line.len);
    }

    get_svg_res += "</svg>\n";
    return const_cast<char*>(get_svg_res.c_str());
}
}

// Emscriptenのバインディング
EMSCRIPTEN_BINDINGS(my_module) {
    emscripten::function("set_svg", &set_svg);
    // emscripten::function("get_svg", &get_svg);
}

#include <cstdint>
#include <string>
#include <emscripten/bind.h>
/*
em++ svg_manager.cpp -o http/svg_manager.js --bind -s MAXIMUM_MEMORY=2GB -sALLOW_MEMORY_GROWTH && sed -i "s$'requestPointerLock'$//'requestPointerLock'$" http/svg_manager.js

em++ svg_manager.cpp -O3 -s ASSERTIONS=0 -s DISABLE_EXCEPTION_CATCHING=1 -o http/svg_manager.js --bind -s INITIAL_MEMORY=1GB -s MAXIMUM_MEMORY=3GB -sALLOW_MEMORY_GROWTH && sed -i "s$'requestPointerLock'$//'requestPointerLock'$" http/svg_manager.js
*/


using std::string;
using std::vector;


struct svg_line {
    int line_num;
    int16_t begin_time, end_time;
    int start_p, len;
    // string svg;
};

vector<svg_line> svg_lines;
string internal_svg;

vector<vector<svg_line>> turn_svgs;
vector<svg_line>         gloval_svgs;

int set_svg(const string &svg) {
    // internal_svg = svg;
    // return svg.length();

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
            } else {
                // svg_lines.push_back({line_num, begin_time, end_time, svg.substr(i, br - i)});
                svg_line tmp{line_num, begin_time, end_time, int(i), int(br - i)};
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
    return max_time;
}

string get_svg(int t) {
    string res;
    int ti = 0;
    int gi = 0;
    while (ti < turn_svgs[t].size() && gi < gloval_svgs.size()) {
        if (gloval_svgs[gi].begin_time != -1 && (t < gloval_svgs[gi].begin_time || gloval_svgs[gi].end_time < t)) {
            ++gi;
            continue;
        }
        if (turn_svgs[t][ti].line_num < gloval_svgs[gi].line_num) {
            res += internal_svg.substr(turn_svgs[t][ti].start_p, turn_svgs[t][ti].len);
            ++ti;
        } else {
            res += internal_svg.substr(gloval_svgs[gi].start_p, gloval_svgs[gi].len);
            ++gi;
        }
    }
    for (; ti < turn_svgs[t].size(); ++ti) {
        res += internal_svg.substr(turn_svgs[t][ti].start_p, turn_svgs[t][ti].len);
    }
    for (; gi < gloval_svgs.size(); ++gi) {
        if (gloval_svgs[gi].begin_time != -1 && (t < gloval_svgs[gi].begin_time || gloval_svgs[gi].end_time < t)) {
            ++gi;
            continue;
        }
        res += internal_svg.substr(gloval_svgs[gi].start_p, gloval_svgs[gi].len);
    }
    // for (auto &line : svg_lines) {
    //     if (line.begin_time == -1 || (line.begin_time <= t && t <= line.end_time)) {
    //         // res += line.svg;
    //         res += internal_svg.substr(line.start_p, line.len);
    //     }
    // }
    return res;
}

// Emscriptenのバインディング
EMSCRIPTEN_BINDINGS(my_module) {
    emscripten::function("set_svg", &set_svg);
    emscripten::function("get_svg", &get_svg);
}

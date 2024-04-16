#include <string>
#include <emscripten/bind.h>
/*
em++ svg_manager.cpp -o http/svg_manager.js --bind -s MAXIMUM_MEMORY=2GB -sALLOW_MEMORY_GROWTH && sed -i "s$'requestPointerLock'$//'requestPointerLock'$" http/svg_manager.js
*/


using std::string;
using std::vector;


struct svg_line {
    int line_num;
    int begin_time;
    int end_time;
    string svg;
};

vector<svg_line> svg_lines;



int set_svg(const string &svg) {
    // internal_svg = svg;
    // return svg.length();

    svg_lines.clear();
    string::size_type i = 0;
    int line_num = 0;
    int begin_time = -1;
    int end_time = -1;
    int max_time = 0;
    for (;;) {
        string::size_type br = i;
        while(br < svg.size()) {
            char c = svg[br];
            ++br;
            if (c == '\r' || c == '\n') break;
        }
        if (br - i > 1) {
            if (svg.substr(i, 9) == "<!--time=") {
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
                        end_time = begin_time + 1;
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
                    max_time = std::max(max_time, end_time-1);
                }
            } else {
                string line = svg.substr(i, br - i);
                svg_lines.push_back({line_num, begin_time, end_time, line});
                ++line_num;
            }
        }
        i = br;
        if (br >= svg.size()) break;

        if (line_num >= 100) break;
    }
    return max_time;
}

string get_svg(int t) {
    string res;
    for (auto &line : svg_lines) {
        // res += std::to_string(line.begin_time) + ":" + std::to_string(line.end_time) + line.svg; continue;

        if (line.begin_time < 0 || (line.begin_time <= t && t < line.end_time)) {
            res += line.svg;
        }
    }
    return res;
}

// Emscriptenのバインディング
EMSCRIPTEN_BINDINGS(my_module) {
    emscripten::function("set_svg", &set_svg);
    emscripten::function("get_svg", &get_svg);
}

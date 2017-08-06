#include <algorithm>
#include <gtkmm.h>
#include <iostream>
#include <map>
#include <util.hpp>
#include <vector>

namespace util {
    const int KEYCODE_ENTER = 65293;
    const int BUTTONCODE_MOUSE_LEFT = 1;
    const long double pi = 3.1415926535897932384626433832795L;
    const long double e  = 2.7182818284590452353602874713527L;

    void print(Glib::ustring text) {
        std::cout << text << "\n";
    }

    std::map<char, std::array<std::vector<unsigned int>, 5>> char_map = {
        {
            '0', {{
                {{1, 1, 1}},
                {{1, 0, 1}},
                {{1, 0, 1}},
                {{1, 0, 1}},
                {{1, 1, 1}}
            }}
        },
        {
            '1', {{
                {{0, 1, 1}},
                {{0, 0, 1}},
                {{0, 0, 1}},
                {{0, 0, 1}},
                {{0, 0, 1}}
            }}
        },
        {
            '2', {{
                {{1, 1, 1}},
                {{0, 0, 1}},
                {{1, 1, 1}},
                {{1, 0, 0}},
                {{1, 1, 1}}
            }}
        },
        {
            '3', {{
                {{1, 1, 1}},
                {{0, 0, 1}},
                {{1, 1, 1}},
                {{0, 0, 1}},
                {{1, 1, 1}}
            }}
        },
        {
            '4', {{
                {{1, 0, 1}},
                {{1, 0, 1}},
                {{1, 1, 1}},
                {{0, 0, 1}},
                {{0, 0, 1}}
            }}
        },
        {
            '5', {{
                {{1, 1, 1}},
                {{1, 0, 0}},
                {{1, 1, 1}},
                {{0, 0, 1}},
                {{1, 1, 1}}
            }}
        },
        {
            '6', {{
                {{1, 1, 1}},
                {{1, 0, 0}},
                {{1, 1, 1}},
                {{1, 0, 1}},
                {{1, 1, 1}}
            }}
        },
        {
            '7', {{
                {{1, 1, 1}},
                {{1, 0, 1}},
                {{0, 0, 1}},
                {{0, 0, 1}},
                {{0, 0, 1}}
            }}
        },
        {
            '8', {{
                {{1, 1, 1}},
                {{1, 0, 1}},
                {{1, 1, 1}},
                {{1, 0, 1}},
                {{1, 1, 1}}
            }}
        },
        {
            '9', {{
                {{1, 1, 1}},
                {{1, 0, 1}},
                {{1, 1, 1}},
                {{0, 0, 1}},
                {{1, 1, 1}}
            }}
        },
        {
            '-', {{
                {{0, 0, 0}},
                {{0, 0, 0}},
                {{1, 1, 1}},
                {{0, 0, 0}},
                {{0, 0, 0}}
            }}
        },
        {
            '.', {{
                {0},
                {0},
                {0},
                {0},
                {1}
            }}
        }
    };

    Glib::ustring str(long double i) {
        Glib::ustring num = std::to_string(i);
        int size = num.size();
        for(int i = 0; i < size; ++i) {
            char c = num[size - 1 - i];
            if(num.size() - 1 && (c == '0' || c == '.')) {
                num.erase(size - 1 - i);
            }
            else {
                break;
            }
        }

        return num;
    }

    Glib::ustring str(std::vector<Glib::ustring> v) {
        Glib::ustring msg = "[";
        for(std::vector<Glib::ustring>::size_type i = 0; i != v.size(); i++) {
            msg += v[i];
            if(i < v.size() - 1) {
                msg += ", ";
            }
        }
        msg += "]";
        return msg;
    }

    /**
    * @brief      remove first occurence of a character in a string
    *
    * @param[in]  text    string
    * @param[in]  filter  The character to be removed
    *
    * @return     string
    */
    std::string lremove_char(std::string text, char filter) {
        auto iter = std::find(text.begin(), text.end(), filter);
        if(iter != text.end()) {
            text.erase(iter);
        }
        return text;
    }

    bool is_number(std::string text) {
        if(text == "e" || text == "pi" || text == "π") {
            return true;
        }
        text = lremove_char(lremove_char(text, '-'), '.');
        return !text.empty() && std::all_of(text.begin(), text.end(), ::isdigit);
    }

    long double parse_number(std::string text) {
        if(!is_number(text)) {
            throw std::invalid_argument(text + " is not a valid number!");
        }
        else if(text == "e") {
            return e;
        }
        else if(text == "pi" || text == "π") {
            return pi;
        }

        return std::stold(text);
    }

    int sign(int x) {
        return  x ? 1 - 2 * std::signbit(x) : 0;
    }
}

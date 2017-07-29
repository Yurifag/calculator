#pragma once
#include <gtkmm.h>
#include <map>
#include <vector>

namespace util {
    extern const int KEYCODE_ENTER;
    extern const int BUTTONCODE_MOUSE_LEFT;
    extern const long double pi;
    extern const long double e;

    extern std::map<char, std::array<std::vector<unsigned int>, 5>> char_map;

    Glib::ustring str(long double i);
    Glib::ustring str(std::vector<Glib::ustring> v);

    void                       print(Glib::ustring msg);
    void                       print(std::string msg);
    template <typename T> void print(T v) {
        print(str(v));
    }

    template <typename T> void print(std::vector<T> v) {
        print(str(v));
    }

    /**
     * @brief      remove first occurence of a character in a string
     *
     * @param[in]  text    string
     * @param[in]  filter  The character to be removed
     *
     * @return     string
     */
    std::string lremove_char(std::string text, char filter);

    bool is_number(std::string text);
    long double parse_number(std::string input);

        /**
    * @brief      copies a portion of a vector
    *
    * @param[in]  v      vector to slice
    * @param[in]  begin  index at which to begin
    * @param[in]  end    index before which to end
    *
    * @return     new vector containing the extracted elements
    */
    template <typename T> std::vector<T> slice(const std::vector<T> v, int begin, int end) {
        std::vector<T> u;

        if(begin < 0) {
            begin = v.size() + begin;
        }

        if(end < 0) {
            end = v.size() + end;
        }

        if(begin == end) {
            u = v;
        }
        else if(begin < end) {
            while(begin < end) {
                u.push_back(v[begin]);
                begin++;
            }
        }
        else if(end < begin) {
            while(begin < v.size()) {
                u.push_back(v[begin]);
                begin++;
            }
            begin = 0;
            while(begin < end) {
                u.push_back(v[begin]);
                begin++;
            }
        }
        return u;
    }

    /**
    * @brief      combines a vector with another one
    *
    * @param[in]  v     vector to be extended
    * @param[in]  u     vector containing the elements to be appended
    *
    * @return     new vector containing the elements of both vectors
    */
    template <typename T> std::vector<T> extend(std::vector<T> v, std::vector<T> u) {
        v.reserve(v.size() + distance(u.begin(), u.end()));
        v.insert(v.end(), u.begin(), u.end());
        return v;
    }

    /**
    * @brief      reverse a vector
    *
    * @param[in]  v     vector to be reversed
    *
    * @return     new vector containing all elements in reversed order
    */
    template <typename T> std::vector<T> reverse(std::vector<T> v) {
        std::vector<T> u;
        for(int i = 0; i < v.size(); ++i) {
            u.push_back(v[v.size() - 1 - i]);
        }
        return u;
    }

    int sign(int x);
}

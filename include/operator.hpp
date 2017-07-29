#pragma once
#include <gtkmm.h>
#include <vector>

/**
 * @brief      Operator class for doing calculations with two numbers
 */
class Operator {
  public:
    enum OPERATOR {
        ADD,
        SUBTRACT,
        MULTIPLY,
        DEVIDE,
        POTENTIATE
    };

    static std::vector<Glib::ustring> operators;
    int precedence;
    int op_type;

    Operator(const int op_type, const int precedence);
    ~Operator();

    static Operator*   get(const int op_type);
    static Operator*   get(const Glib::ustring op_symbol);
    static bool        is_operator(const Glib::ustring);
    long double        calc(const long double u, const long double v);
    static long double add(const long double u, const long double v);
    static long double subtract(const long double u, const long double v);
    static long double multiply(const long double u, const long double v);
    static long double devide(const long double u, const long double v);
    static long double potentiate(const long double u, const long double v);
    Glib::ustring      to_string();
};

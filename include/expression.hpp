#pragma once
#include <gtkmm.h>
#include <operator.hpp>
#include <vector>

/**
 * @brief      base class for polymorphic math expressions
 */
class Operand {
  public:
    virtual ~Operand();
    virtual Glib::ustring to_string()           = 0;
    virtual double long calc(double long x)     = 0;
    virtual Operand* derive()                   = 0;
    virtual std::vector<Glib::ustring> unwind() = 0;
    virtual Operand* simplify()                         = 0;
};

/**
 * @brief      object holding a number of type long double
 */
class Number : public Operand {
  public:
    Number(long double value);
    ~Number();

    long double                get();
    long double                calc(long double x);
    Operand*                   simplify();
    Operand*                   derive();
    std::vector<Glib::ustring> unwind();
    Glib::ustring              to_string();

  private:
    long double value;
};

/**
 * @brief      class representing the variable "x"
 */
class X : public Operand {
  public:
    static X& getInstance();
    static X* getReference();
    X(X const &) = delete;
    void operator=(X const &) = delete;

    long double                calc(long double x);
    Operand*                   simplify();
    Operand*                   derive();
    std::vector<Glib::ustring> unwind();
    Glib::ustring              to_string();

  private:
    X();
};

/**
 * @brief      expression holding an operator and 2 operands (Number, X, Expression)
 */
class Expression : public Operand {
  public:
    Operator *op;
    Operand  *operand1;
    Operand  *operand2;

    Expression(Operator *op, Operand *operand1, Operand *operand2);
    ~Expression();

    /**
     * @brief      calculate the y value of expression with a given x value recursively
     *
     * @param[in]  x     value of x
     *
     * @return     value of y
     */
    long double                calc(long double x);
    Operand*                   simplify();
    Operand*                   derive();
    std::vector<Glib::ustring> unwind();
    Glib::ustring              to_string();
};

class Function : public Operand {
  public:
    int func;
    Operand *expression;
    enum FUNC {
        SIN,
        COS,
        TAN,
        COT,
        SEC,
        CSC,
        ARCSIN,
        ARCCOS,
        ARCTAN,
        ARCCOT,
        ARCSEC,
        ARCCSC,
        SINH,
        COSH,
        TANH,
        COTH,
        SECH,
        CSCH,
        ARCSINH,
        ARCCOSH,
        ARCTANH,
        ARCCOTH,
        ARCSECH,
        ARCCSCH,
        SQRT,
        LOG10,
        LOG,
        LN,
        SIGN,
        ABS
    };

    static const std::vector<Glib::ustring> names;

    Function(int func, Operand *expression);
    ~Function();

    long double                calc(long double x);
    Operand*                   simplify();
    Operand*                   derive();
    std::vector<Glib::ustring> unwind();
    Glib::ustring              get_name();
    Glib::ustring              to_string();
    static bool                is_func(Glib::ustring name);
    static int                 get_func(Glib::ustring name);
};

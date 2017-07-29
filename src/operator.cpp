#include <operator.hpp>
#include <util.hpp>
#include <vector>

using namespace util;

std::vector<Glib::ustring> Operator::operators = {
    "+",
    "-",
    "*",
    "/",
    "^"
};

Operator::Operator(const int op_type, const int precedence) {
    this->op_type     = op_type;
    this->precedence = precedence;
}
Operator::~Operator() {}
Operator* Operator::get(const int op_type) {
    switch(op_type) {
        case ADD:
            return new Operator(op_type, 0);
        case SUBTRACT:
            return new Operator(op_type, 0);
        case MULTIPLY:
            return new Operator(op_type, 1);
        case DEVIDE:
            return new Operator(op_type, 1);
        case POTENTIATE:
            return new Operator(op_type, 2);
        default:
            throw std::invalid_argument(str(op_type) + " is not a valid operator enum!");
    }
}
Operator* Operator::get(const Glib::ustring op_symbol) {
    if(op_symbol == "+") {
        return Operator::get(ADD);
    }
    else if(op_symbol == "-") {
        return Operator::get(SUBTRACT);
    }
    else if(op_symbol == "*") {
        return Operator::get(MULTIPLY);
    }
    else if(op_symbol == "/") {
        return Operator::get(DEVIDE);
    }
    else if(op_symbol == "^") {
        return Operator::get(POTENTIATE);
    }

    throw std::invalid_argument(op_symbol + " is not a valid operator symbol!");
}
long double Operator::add(const long double u, const long double v) {
    return u + v;
}
long double Operator::subtract(const long double u, const long double v) {
    return u - v;
}
long double Operator::multiply(const long double u, const long double v) {
    return u * v;
}
long double Operator::devide(const long double u, const long double v) {
    return u / v;
}
long double Operator::potentiate(const long double u, const long double v) {
    return std::pow(u, v);
}
long double Operator::calc(const long double u, const long double v) {
    switch(this->op_type) {
        case ADD:
            return Operator::add(u, v);
        case SUBTRACT:
            return Operator::subtract(u, v);
        case MULTIPLY:
            return Operator::multiply(u, v);
        case DEVIDE:
            return Operator::devide(u, v);
        case POTENTIATE:
            return Operator::potentiate(u, v);
        default:
            throw std::invalid_argument(str(op_type) + " is not a valid operator enum!");
    }
}
Glib::ustring Operator::to_string() {
    return this->operators[this->op_type];
}
bool Operator::is_operator(const Glib::ustring op) {
    if(op == "+" || op == "-" || op == "*" || op == "/" || op == "^") {
        return true;
    }
    return false;
}

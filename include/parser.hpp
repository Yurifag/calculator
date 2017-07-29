#pragma once
#include <expression.hpp>
#include <gtkmm.h>
#include <vector>

class Parser {
  public:
    static Glib::RefPtr<Glib::Regex> math_pattern;
    static Operand* create_expression(std::vector<Glib::ustring> matches);
    static Operand* parse(Glib::ustring text);
    static bool     validate(Glib::ustring text);
};

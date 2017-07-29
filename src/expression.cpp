#include <expression.hpp>
#include <gtkmm.h>
#include <util.hpp>
#include <vector>

using namespace util;

Operand::~Operand() {}

Number::Number(long double value) {
    this->value = value;
}
Number::~Number() {}
long double Number::get() {
    return this->value;
}
long double Number::calc(long double x) {
    return this->value;
}
Operand* Number::simplify() {
    return this;
}
Operand* Number::derive() {
    return new Number(0);
}
std::vector<Glib::ustring> Number::unwind() {
    return {{this->to_string()}};
}
Glib::ustring Number::to_string() {
    return str(this->value);
}

X::X() {};
X& X::getInstance() {
    static X instance;
    return instance;
}
X* X::getReference() {
    return &X::getInstance();
}
long double X::calc(long double x) {
    return x;
}
Operand* X::simplify() {
    return this;
}
Operand* X::derive() {
    return new Number(1);
}
std::vector<Glib::ustring> X::unwind() {
    return {{this->to_string()}};
}
Glib::ustring X::to_string() {
    return "x";
}

Expression::Expression(Operator *op, Operand *operand1, Operand *operand2) {
    this->op       = op;
    this->operand1 = operand1;
    this->operand2 = operand2;
}
Expression::~Expression() {
    delete this->op;
    delete this->operand1;
    delete this->operand2;
}
long double Expression::calc(long double x) {
    return op->calc(operand1->calc(x), operand2->calc(x));
}
Operand* Expression::simplify() {
    this->operand1 = this->operand1->simplify();
    this->operand2 = this->operand2->simplify();

    if(Number *num1 = dynamic_cast<Number*>(this->operand1)) {
        if(Number *num2 = dynamic_cast<Number*>(this->operand2)) {
            return new Number(this->op->calc(num1->get(), num2->get()));
        }
        else if(num1->get() == 0) {
            if(this->op->op_type == Operator::ADD) {
                return this->operand2;
            }
            else if(this->op->op_type == Operator::SUBTRACT) {
                return new Expression(Operator::get(Operator::MULTIPLY),
                    new Number(-1),
                    this->operand2
                );
            }
            else if(this->op->op_type == Operator::MULTIPLY || this->op->op_type == Operator::DEVIDE) {
                return this->operand1;
            }
        }
        else if(num1->get() == 1) {
            if(this->op->op_type == Operator::MULTIPLY) {
                return operand2;
            }
            else if(this->op->op_type == Operator::POTENTIATE) {
                return operand1;
            }
        }
        else if(Expression *expr = dynamic_cast<Expression*>(this->operand2)) {
            if(Number *num3 = dynamic_cast<Number*>(expr->operand1)) {
                if(expr->op->op_type == Operator::POTENTIATE) {
                    if(num1->get() == -1) {
                        expr->operand1 = new Number(-num3->get());
                        return expr;
                    }
                }
                else {
                    expr->operand1 = new Number(this->op->calc(num1->get(), num3->get()));
                    return expr;
                }
            }
        }
    }
    else if(Number *num2 = dynamic_cast<Number*>(this->operand2)) {
        if(num2->get() == 0) {
            if(this->op->op_type == Operator::ADD || this->op->op_type == Operator::SUBTRACT) {
                return this->operand1;
            }
            else if(this->op->op_type == Operator::MULTIPLY) {
                return operand2;
            }
            else if(this->op->op_type == Operator::POTENTIATE) {
                return new Number(1);
            }
        }
        else if(num2->get() == 1 && (this->op->op_type == Operator::MULTIPLY|| this->op->op_type == Operator::POTENTIATE || this->op->op_type == Operator::DEVIDE)) {
            return operand1;
        }
        else if(Expression *expr = dynamic_cast<Expression*>(this->operand1)) {
            if(num2->get() == -1 && (this->op->op_type == Operator::MULTIPLY || this->op->op_type == Operator::DEVIDE)) {
                if(Number *num4 = dynamic_cast<Number*>(expr->operand1)) {
                    expr->operand1 = new Number(-num4->get());
                    return expr;
                }
            }
        }
    }
    else if(this->operand1->to_string() == this->operand2->to_string()) {
        if(this->op->op_type == Operator::ADD) {
            return new Expression(Operator::get(Operator::MULTIPLY),
                new Number(2),
                this->operand1
            );
        }
        else if(this->op->op_type == Operator::SUBTRACT) {
            return new Number(0);
        }
        else if(this->op->op_type == Operator::MULTIPLY) {
            return new Expression(Operator::get(Operator::POTENTIATE),
                this->operand1,
                new Number(2)
            );
        }
        else if(this->op->op_type == Operator::DEVIDE) {
            return new Number(1);
        }
    }

    return this;
}
Operand* Expression::derive() {
    Operand*      u  = this->operand1;
    Operand*      v  = this->operand2;
    Operand*      udx  = this->operand1->derive();
    Operand*      vdx  = this->operand2->derive();
    Operand*      derivative;

    if(this->op->op_type == Operator::POTENTIATE) {
        /*
            f(x)  = u^v
            f'(x) = v * u^(v-1) * u' + u^v * ln(u) * v'
         */
        derivative = new Expression(Operator::get(Operator::ADD),
            new Expression(Operator::get(Operator::MULTIPLY),
                v,
                new Expression(Operator::get(Operator::MULTIPLY),
                    new Expression(Operator::get(Operator::POTENTIATE),
                        u,
                        new Expression(Operator::get(Operator::SUBTRACT),
                            v,
                            new Number(1)
                        )
                    ),
                    udx
                )
            ),
            new Expression(Operator::get(Operator::MULTIPLY),
                new Expression(Operator::get(Operator::POTENTIATE),
                    u,
                    v
                ),
                new Expression(Operator::get(Operator::MULTIPLY),
                    new Function(Function::LN, u),
                    vdx
                )
            )
        );

        /*
            f(x)  = u^n
            f'(x) = n * u^(n-1) * u'
         */

        /*
            f(x)  = c^u
            f'(x) = c^u * ln(c) * u'
         */
    }
    else if(this->op->op_type == Operator::MULTIPLY) {
        /*
            f(x)  = u * v
            f'(x) = u * v' + v * u'
         */
        derivative = new Expression(Operator::get(Operator::ADD),
            new Expression(Operator::get(Operator::MULTIPLY),
                u,
                vdx
            ),
            new Expression(Operator::get(Operator::MULTIPLY),
                v,
                udx
            )
        );

        /*
            f(x)  = c * u
            f'(x) = c * u'
         */
    }
    else if(this->op->op_type == Operator::DEVIDE) {
        /*
            f(x)  = u / v
            f'(x) = (v * u' - u * v') / v^2
         */
        derivative = new Expression(Operator::get(Operator::DEVIDE),
            new Expression(Operator::get(Operator::SUBTRACT),
                new Expression(Operator::get(Operator::MULTIPLY),
                    v,
                    udx
                ),
                new Expression(Operator::get(Operator::MULTIPLY),
                    u,
                    vdx
                )
            ),
            new Expression(Operator::get(Operator::POTENTIATE),
                v,
                new Number(2)
            )
        );
    }
    else {
        /*
            f(x)  = u  + v
            f'(x) = u' + v'

            OR

            f(x)  = u  - v
            f'(x) = u' - v'
         */

        derivative = new Expression(Operator::get(this->op->op_type),
            udx,
            vdx
        );
    }
    return derivative->simplify();
}
std::vector<Glib::ustring> Expression::unwind() {
    if(this->op->to_string() == "^") {
        return extend(extend(extend(extend(this->operand1->unwind(), {{this->op->to_string()}}), {{"("}}), this->operand2->unwind()), {{")"}});
    }

    return extend(extend(this->operand1->unwind(), {{this->op->to_string()}}), this->operand2->unwind());
}
Glib::ustring Expression::to_string() {
    std::vector<Glib::ustring> symbols = this->unwind();
    Glib::ustring text = "";
    for(int i = 0; i < symbols.size(); ++i) {
        if(!(symbols[i] == "*" && symbols.size() > i + 1 && symbols[i + 1] == "x")) {
            text += symbols[i];
        }
    }
    return text;
}

Function::Function(int func, Operand *expression) {
    this->func       = func;
    this->expression = expression;
}
Function::~Function() {}
const std::vector<Glib::ustring> Function::names = {
    "sin",
    "cos",
    "tan",
    "cot",
    "sec",
    "csc",
    "arcsin",
    "arccos",
    "arctan",
    "arccot",
    "arcsec",
    "arccsc",
    "sinh",
    "cosh",
    "tanh",
    "coth",
    "sech",
    "csch",
    "arcsinh",
    "arccosh",
    "arctanh",
    "arccoth",
    "arcsech",
    "arccsch",
    "sqrt",
    "log10",
    "log",
    "ln",
    "sign",
    "abs"
};
long double Function::calc(long double x) {
    x = this->expression->calc(x);
    switch(this->func) {
        case SIN:
            return std::sin(x);
        case COS:
            return std::cos(x);
        case TAN:
            return std::tan(x);
        case COT:
            return 1 / std::tan(x);
        case SEC:
            return 1 / std::cos(x);
        case CSC:
            return 1 / std::sin(x);
        case SINH:
            return std::sinh(x);
        case COSH:
            return std::cosh(x);
        case TANH:
            return std::tanh(x);
        case COTH:
            if(x) return std::cosh(x) / std::sinh(x);
            break;
        case SECH:
            return 1 / std::cosh(x);
        case CSCH:
            if(x) return 1 / std::sinh(x);
            break;
        case ARCSIN:
            return std::asin(x);
        case ARCCOS:
            return std::acos(x);
        case ARCTAN:
            return std::atan(x);
        case ARCCOT:
            if(x) return x > 0 ? std::atan(1 / x) : std::atan(1 / x) + pi;
            break;
        case ARCSEC:
            return std::acos(1 / x);
        case ARCCSC:
            return 1 / std::asin(x);
        case ARCSINH:
            return std::asinh(x);
        case ARCCOSH:
            return std::acosh(x);
        case ARCTANH:
            return std::atanh(x);
        case ARCCOTH:
            return std::atanh(1 / x);
        case ARCSECH:
            return std::cosh(1 / x);
        case ARCCSCH:
            return 1 / std::asinh(x);
        case SQRT:
            return std::sqrt(x);
        case LOG10:
            return std::log10(x);
        case LOG:
        case LN:
            return std::log(x);
        case SIGN:
            return  x ? 1 - 2 * std::signbit(x) : 0;
        case ABS:
            return std::abs(x);
    }
    throw std::invalid_argument(str(this->func) + " is not a valid Function enum!");
}
Operand* Function::simplify() {
    this->expression = this->expression->simplify();
    return this;
}
Operand* Function::derive() {
    if(Operand *inner = this->expression->derive()) {
        Operand *outer;
        switch(this->func) {
            case SIN:
                outer = new Function(COS, this->expression);
                break;
            case COS:
                outer = new Expression(Operator::get(Operator::MULTIPLY),
                    new Number(-1),
                    new Function(SIN, this->expression)
                );
                break;
            case TAN:
                outer = new Expression(Operator::get(Operator::POTENTIATE),
                    new Function(SEC, this->expression),
                    new Number(2)
                );
                break;
            case COT:
                outer = new Expression(Operator::get(Operator::MULTIPLY),
                    new Number(-1),
                    new Expression(Operator::get(Operator::POTENTIATE),
                        new Function(CSC, this->expression),
                        new Number(2)
                    )
                );
                break;
            case SEC:
                outer = new Expression(Operator::get(Operator::MULTIPLY),
                    new Function(SEC, this->expression),
                    new Function(TAN, this->expression)
                );
                break;
            case CSC:
                outer = new Expression(Operator::get(Operator::MULTIPLY),
                    new Number(-1),
                    new Expression(Operator::get(Operator::MULTIPLY),
                        new Function(CSC, this->expression),
                        new Function(COT, this->expression)
                    )
                );
                break;
            case ARCSIN:
                outer = new Expression(Operator::get(Operator::DEVIDE),
                    new Number(1),
                    new Function(SQRT, new Expression(Operator::get(Operator::SUBTRACT),
                        new Number(1),
                        new Expression(Operator::get(Operator::POTENTIATE),
                            this->expression,
                            new Number(2)
                        )
                    ))
                );
                break;
            case ARCCOS:
                outer = new Expression(Operator::get(Operator::DEVIDE),
                    new Number(-1),
                    new Function(SQRT, new Expression(Operator::get(Operator::SUBTRACT),
                        new Number(1),
                        new Expression(Operator::get(Operator::POTENTIATE),
                            this->expression,
                            new Number(2)
                        )
                    ))
                );
                break;
            case ARCTAN:
                outer = new Expression(Operator::get(Operator::DEVIDE),
                    new Number(1),
                    new Expression(Operator::get(Operator::ADD),
                        new Expression(Operator::get(Operator::POTENTIATE),
                            this->expression,
                            new Number(2)
                        ),
                        new Number(1)
                    )
                );
                break;
            case ARCCOT:
                outer = new Expression(Operator::get(Operator::DEVIDE),
                    new Number(-1),
                    new Expression(Operator::get(Operator::ADD),
                        new Number(1),
                        new Expression(Operator::get(Operator::POTENTIATE),
                            this->expression,
                            new Number(2)
                        )
                    )
                );
                break;
            case ARCSEC:
                outer = new Expression(Operator::get(Operator::DEVIDE),
                    new Number(1),
                    new Expression(Operator::get(Operator::MULTIPLY),
                        new Function(ABS, this->expression),
                        new Function(SQRT, new Expression(Operator::get(Operator::SUBTRACT),
                            new Expression(Operator::get(Operator::POTENTIATE),
                                this->expression,
                                new Number(2)
                            ),
                            new Number(1)
                        ))
                    )
                );
                break;
            case ARCCSC:
                outer = new Expression(Operator::get(Operator::DEVIDE),
                    new Number(-1),
                    new Expression(Operator::get(Operator::MULTIPLY),
                        new Function(ABS, this->expression),
                        new Function(SQRT, new Expression(Operator::get(Operator::SUBTRACT),
                            new Expression(Operator::get(Operator::POTENTIATE),
                                this->expression,
                                new Number(2)
                            ),
                            new Number(1)
                        ))
                    )
                );
                break;
            case SINH:
                outer = new Function(COSH, this->expression);
                break;
            case COSH:
                outer = new Function(SINH, this->expression);
                break;
            case TANH:
                outer = new Expression(Operator::get(Operator::POTENTIATE),
                    new Function(SECH, this->expression),
                    new Number(2)
                );
                break;
            case COTH:
                outer = new Expression(Operator::get(Operator::MULTIPLY),
                    new Number(-1),
                    new Expression(Operator::get(Operator::POTENTIATE),
                        new Function(CSCH, this->expression),
                        new Number(2)
                    )
                );
                break;
            case SECH:
                outer = new Expression(Operator::get(Operator::MULTIPLY),
                    new Function(SECH, this->expression),
                    new Function(TANH, this->expression)
                );
                break;
            case CSCH:
                outer = new Expression(Operator::get(Operator::MULTIPLY),
                    new Function(CSCH, this->expression),
                    new Function(COTH, this->expression)
                );
                break;
            case ARCSINH:
                outer = new Expression(Operator::get(Operator::DEVIDE),
                    new Number(1),
                    new Function(SQRT, new Expression(Operator::get(Operator::ADD),
                        new Expression(Operator::get(Operator::POTENTIATE),
                            this->expression,
                            new Number(2)
                        ),
                        new Number(1)
                    ))
                );
                break;
            case ARCCOSH:
                outer = new Expression(Operator::get(Operator::DEVIDE),
                    new Number(1),
                    new Function(SQRT, new Expression(Operator::get(Operator::SUBTRACT),
                        new Expression(Operator::get(Operator::POTENTIATE),
                            this->expression,
                            new Number(2)
                        ),
                        new Number(1)
                    ))
                );
                break;
            case ARCTANH:
                outer = new Expression(Operator::get(Operator::DEVIDE),
                    new Number(1),
                    new Expression(Operator::get(Operator::SUBTRACT),
                        new Number(1),
                        new Expression(Operator::get(Operator::POTENTIATE),
                            this->expression,
                            new Number(2)
                        )
                    )
                );
                break;
            case ARCCOTH:
                outer = new Expression(Operator::get(Operator::DEVIDE),
                    new Number(1),
                    new Expression(Operator::get(Operator::SUBTRACT),
                        new Number(1),
                        new Expression(Operator::get(Operator::POTENTIATE),
                            this->expression,
                            new Number(2)
                        )
                    )
                );
                break;
            case ARCSECH:
                outer = new Expression(Operator::get(Operator::DEVIDE),
                    new Number(-1),
                    new Expression(Operator::get(Operator::MULTIPLY),
                        this->expression,
                        new Function(SQRT, new Expression(Operator::get(Operator::ADD),
                            new Number(1),
                            new Expression(Operator::get(Operator::POTENTIATE),
                                this->expression,
                                new Number(2)
                            )
                        ))
                    )
                );
                break;
            case ARCCSCH:
                outer = new Expression(Operator::get(Operator::DEVIDE),
                    new Number(-1),
                    new Expression(Operator::get(Operator::MULTIPLY),
                        this->expression,
                        new Function(SQRT, new Expression(Operator::get(Operator::ADD),
                            new Number(1),
                            new Expression(Operator::get(Operator::POTENTIATE),
                                this->expression,
                                new Number(2)
                            )
                        ))
                    )
                );
                break;
            case SQRT:
                outer = new Expression(Operator::get(Operator::DEVIDE),
                    new Number(1),
                    new Expression(Operator::get(Operator::MULTIPLY),
                        new Number(2),
                        new Function(SQRT, this->expression)
                    )
                );
                break;
            case LOG10:
                outer = new Expression(Operator::get(Operator::DEVIDE),
                    new Number(1),
                    new Expression(Operator::get(Operator::MULTIPLY),
                        this->expression,
                        new Function(LN, new Number(10))
                    )
                );
                break;
            case LOG:
            case LN:
                outer = new Expression(Operator::get(Operator::DEVIDE),
                    new Number(1),
                    this->expression
                );
                break;
            case ABS:
                outer = new Function(SIGN, this->expression);
                break;
        }

        return (new Expression(Operator::get(Operator::MULTIPLY),
            inner,
            outer
        ))->simplify();
    }

    throw std::invalid_argument(str(this->func) + " is not a valid Function enum!");
}
std::vector<Glib::ustring> Function::unwind() {
    return extend(extend({{this->get_name(), "("}}, this->expression->unwind()), {{")"}});
}
Glib::ustring Function::get_name() {
    switch(this->func) {
        case SIN:
            return "sin";
        case COS:
            return "cos";
        case TAN:
            return "tan";
        case COT:
            return "cot";
        case SEC:
            return "sec";
        case CSC:
            return "csc";
        case ARCSIN:
            return "arcsin";
        case ARCCOS:
            return "arccos";
        case ARCTAN:
            return "arctan";
        case ARCCOT:
            return "arccot";
        case ARCSEC:
            return "arcsec";
        case ARCCSC:
            return "arccsc";
        case SINH:
            return "sinh";
        case COSH:
            return "cosh";
        case TANH:
            return "tanh";
        case COTH:
            return "coth";
        case SECH:
            return "sech";
        case CSCH:
            return "csch";
        case ARCSINH:
            return "arcsinh";
        case ARCCOSH:
            return "arccosh";
        case ARCTANH:
            return "arctanh";
        case ARCCOTH:
            return "arccoth";
        case ARCSECH:
            return "arcsech";
        case ARCCSCH:
            return "arccsch";
        case SQRT:
            return "sqrt";
        case LOG10:
            return "log10";
        case LOG:
        case LN:
            return "ln";
        case SIGN:
            return "sign";
        case ABS:
            return "abs";
        default:
            throw std::invalid_argument(str(this->func) + " is not a valid Function enum!");
    }
}
bool Function::is_func(Glib::ustring name) {
    for(int i = 0; i < Function::names.size(); ++i) {
        if(name == Function::names[i]) {
            return true;
        }
    }
    return false;
}
int Function::get_func(Glib::ustring name) {
    for(int i = 0; i < Function::names.size(); ++i) {
        if(name == Function::names[i]) {
            return i;
        }
    }
    throw std::invalid_argument(name + " is not a function!");
}
Glib::ustring Function::to_string() {
    return this->get_name() + "(" + this->expression->to_string() + ")";
}

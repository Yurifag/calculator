#include <expression.hpp>
#include <parser.hpp>
#include <util.hpp>
#include <vector>

using namespace util;

Glib::RefPtr<Glib::Regex> Parser::math_pattern = Glib::Regex::create(
    "(?:(?<=^|[\\(\\*\\/^])-)?\\d+(?:[.,]\\d+)?"
    "|(?!^)[*/^](?![+*/^]|$)"
    "|-(?![-+*/^]|$)"
    "|(?!^)\\+(?![-+*/^]|$)"
    "|(((arc)?(sin|cos|tan|cot|sec|csc)h?)|sqrt|log(10)?|ln|sign|abs|e|pi|π)"
    "|[()x]"
);

Operand* Parser::create_expression(std::vector<Glib::ustring> matches) {
    int i, indent;

    if(matches[0] == "(" && (matches[matches.size() - 1] == ")" || Function::is_func(matches[matches.size() - 1]))) {
        indent = 0;
        for(i = 0; i < matches.size(); ++i) {
            if(matches[i] == "(") {
                indent++;
            }
            else if(matches[i] == ")") {
                indent--;
            }

            if(!indent) {
                break;
            }
        }

        if(i == matches.size() - 1) {
            matches = slice(matches, 1, matches.size() - 1);
        }
        else if(Function::is_func(matches[matches.size() - 1]) && i == matches.size() - 2) {
            return new Function(
                Function::get_func(matches[matches.size() - 1]),
                create_expression(slice(matches, 1, matches.size() - 2))->simplify()
            );
        }
    }

    if(matches.size() < 3) {
        if(is_number(matches[0])) {
            double long num = parse_number(matches[0]);
            if(matches.size() == 2 && matches[1] == "-") {
                return new Number(-num);
            }
            return new Number(num);

        }
        else if(matches[0] == "x") {
            if(matches.size() == 2 && matches[1] == "-") {
                return new Expression(Operator::get(Operator::MULTIPLY),
                    new Number(-1),
                    X::getReference()
                );
            }
            return X::getReference();
        }
    }
    else {
        // iterate over operators from lowest to highest precedence
        for(const Glib::ustring op : Operator::operators) {
            // iterate over all matches
            indent = 0;
            for(i = 0; i < matches.size(); ++i) {
                if(matches[i] == "(") {
                    indent++;
                }
                else if(matches[i] == ")") {
                    indent--;
                }
                // if match is the operator in question
                if(matches[i] == op && !indent && i && matches.size() > i + 1) {

                    Operand *operand1 = create_expression(slice(matches, i + 1, matches.size()));
                    Operand *operand2 = create_expression(slice(matches, 0, i));

                    return (new Expression(Operator::get(op), operand1, operand2))->simplify();
                }
            }
        }
    }
    throw std::invalid_argument(str(matches) + " is not a valid expression!");
}

Operand* Parser::parse(Glib::ustring text) {
    text = text.lowercase();
    std::vector<Glib::ustring> matches;
    Glib::MatchInfo            match_info;
    math_pattern->match(text, match_info);
    if(match_info.get_match_count()) {
        do {
            Glib::ustring match = match_info.fetch(0);
            if(!(Operator::is_operator(match) || match == "(") && matches.size() && !Operator::is_operator(matches[matches.size() - 1])) {
                matches.push_back("*");
            }
            /*
            if((match == "(" || match == "x" || match == "e" || Function::is_func(match)) && matches.size() && is_number(matches[matches.size() - 1])) {
                matches.push_back("*");
            }
            */
            matches.push_back(match);
        } while(match_info.next());

        matches = reverse(matches);
        std::vector<int> opening;
        std::vector<int> closing;
        int              i;
        for(i = 0; i < matches.size(); ++i) {
            if(matches[i] == "(") {
                opening.push_back(i);
            }
            else if(matches[i] == ")") {
                closing.push_back(i);
            }
        }
        for(i = 0; i < opening.size(); ++i) {
            matches[opening[i]] = ")";
        }
        for(i = 0; i < closing.size(); ++i) {
            matches[closing[i]] = "(";
        }

        return create_expression(matches);
    }

    throw std::invalid_argument("Parser error: " + str(matches));
}

bool Parser::validate(Glib::ustring text) {
    text = text.lowercase();
    text = math_pattern->replace(text.c_str(), text.size(), 0, "");
    return !text.size();
}

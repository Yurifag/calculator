package de.grzb;

import java.util.ArrayList;
import java.util.List;

import static de.grzb.Symbol.CLOSING_PARENTHESIS;
import static de.grzb.Symbol.OPENING_PARENTHESIS;
import static de.grzb.Symbol.X;

public class ExpressionFactory {

    public static Expression createExpression(List<String> matches) {
        ArrayList<IChainable> expression = new ArrayList<IChainable>();

        int indent = 0;
        int start = 0;
        int i = 0;
        while(matches.size() > i) {
            String match = matches.get(i);
            if(match.equals(OPENING_PARENTHESIS.getName())) {
                indent++; // current parenthesis nesting
                if(indent == 1) start = i;
            }
            else if(match.equals(CLOSING_PARENTHESIS.getName())) {
                indent--;
                if(indent == 0) expression.add(ExpressionFactory.createExpression(matches.subList(start + 1, i)));
            }
            else if(indent == 0) {
                Operator operator = Operator.getByName(match);
                if(operator != null) {
                    expression.add(operator);
                }
                else if(match.equals(X.getName())) {
                    expression.add(X);
                }
                else {
                    expression.add(new Number(match));
                }
            }

            i++;
        }

        return new Expression(expression.toArray(new IChainable[expression.size()]));
    }

}

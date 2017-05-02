package de.grzb;

import static de.grzb.IOParser.print;

public class Main {

    public static void main(String[] args) {
        Expression expression = ExpressionFactory.createExpression(IOParser.mathInput());
        Function f = new Function(expression);
        print(f);
        print(f.ofX(3.0D));
    }

}

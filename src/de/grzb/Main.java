package de.grzb;

import static de.grzb.IOParser.print;

public class Main {
    /**
     * Creates the expression from user's input, creates the function and prints the function and result for specific x-Value
     */
    public static void main(String[] args) {
        Expression expression = ExpressionFactory.createExpression(IOParser.mathInput());
        Function f = new Function(expression);
        print(f);
        print(f.ofX(3.0D));
    }

}

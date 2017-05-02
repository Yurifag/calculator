package de.grzb;

import java.util.List;

import static de.grzb.Symbol.X;

public class Function {
    private final Expression function;

    public Function(Expression expression) {
        this.function = expression;
    }

    public Number ofX(Double x) {
        return Function.calculate(Function.ofX(this.function.copy(), x));
    }

    private static Expression ofX(Expression expression, Double x) {

        for(int i = 0; i < expression.size(); i++) {
            if(expression.get(i) instanceof Expression) {
                Function.ofX((Expression) expression.get(i), x);
            }
            else if(expression.get(i).equals(X)) {
                expression.set(i, new Number(x));
            }
        }

        return expression;
    }

    private static Number calculate(Expression expression) {

        for(int i = 0; i < expression.size(); i++) {
            if(expression.get(i) instanceof Expression) {
                expression.set(i, Function.calculate(((Expression) expression.get(i))));
            }
        }

        expression.reverse();

        List<Operator> operators = Operator.values();
        operators.sort((operator1, operator2) -> {
            if(operator1.getPrecedence() > operator2.getPrecedence()) {
                return -1;
            }
            else if(operator1.getPrecedence() < operator2.getPrecedence()) {
                return 1;
            }

            return 0;
        });

        Number result = new Number(0.0D);
        if(expression.size() == 1 && expression.get(0) instanceof Number) {
            return (Number) expression.get(0);
        }

        for(int i = operators.size() - 1; i >= 0; i--) {
            for(int j = expression.size() - 1; j >= 0; j--) {
                if(j > 0 && j < expression.size() - 1) {
                    IChainable operator = expression.get(j);
                    IChainable number1 = expression.get(j - 1);
                    IChainable number2 = expression.get(j + 1);

                    if(operator instanceof Operator && operator.equals(operators.get(i)) && number1 instanceof Number && number2 instanceof Number) {

                        Number operand1 = (Number) number2;
                        Number operand2 = (Number) number1;
                        result = ((Operator) operator).calc(operand1, operand2);

                        expression.set(j - 1, result);
                        expression.remove(j + 1);
                        expression.remove(j);
                    }
                }
            }
        }

        //Collections.reverse(expressionCopy);

        return result;
    }

    public String toString() {
        return this.function.toString();
    }

}

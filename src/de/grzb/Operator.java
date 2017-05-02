package de.grzb;

import java.util.Arrays;
import java.util.List;

class Operator implements IChainable {
    private static Operator ADD        = new Operator("+", 2);
    private static Operator SUBTRACT   = new Operator("-", 2);
    private static Operator MULTIPLY   = new Operator("*", 1);
    private static Operator DIVIDE     = new Operator("/", 1);
    private static Operator POTENTIATE = new Operator("^", 0);

    private final String name;
    private final int precedence;
    private static List<Operator> values;

    static {
        Operator.values = Arrays.asList(ADD, SUBTRACT, MULTIPLY, DIVIDE, POTENTIATE);
    }

    private Operator(String name, int precedence) {
        this.name = name;
        this.precedence = precedence;
    }

    public int getPrecedence() {
        return this.precedence;
    }

    public static List<Operator> values() {
        return Operator.values;
    }

    public static Operator getByName(String name) {
        for(Operator value : Operator.values()) {
            if(value.name.equals(name)) {
                return value;
            }
        }

        return null;
    }

    public Number calc(Number operand1, Number operand2) {
        if(this.equals(ADD))             return Operator.add(operand1, operand2);
        else if(this.equals(SUBTRACT))   return Operator.subtract(operand1, operand2);
        else if(this.equals(MULTIPLY))   return Operator.multiply(operand1, operand2);
        else if(this.equals(DIVIDE))     return Operator.divide(operand1, operand2);
        else /* if(this.equals(POTENTIATE))*/ return Operator.potentiate(operand1, operand2);
    }

    public static Number add(Number operand1, Number operand2) {
        return new Number(operand1.getValue() + operand2.getValue());
    }

    public static Number subtract(Number operand1, Number operand2) {
        return new Number(operand1.getValue() - operand2.getValue());
    }

    public static Number multiply(Number operand1, Number operand2) {
        return new Number(operand1.getValue() * operand2.getValue());
    }

    public static Number divide(Number operand1, Number operand2) {
        return new Number(operand1.getValue() / operand2.getValue());
    }
    public static Number potentiate(Number operand1, Number operand2) {
        return new Number(Math.pow(operand1.getValue(), operand2.getValue()));
    }

    public String toString() {
        return this.name;
    }

    @Override
    public IChainable copy() {
        return this;
    }

}

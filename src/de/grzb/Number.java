package de.grzb;

import java.util.ArrayList;

public class Number extends ArrayList<IChainable> implements IChainable {
    private Double value;

    public Number(Double number) {
        this.value = number;
    }

    public Number(String number) {
        this(Double.parseDouble(number));
    }

    public String toString() {
        return Double.toString(this.value);
    }

    public Double getValue() {
        return this.value;
    }

    @Override
    public IChainable copy() {
        return new Number(this.value);
    }
}

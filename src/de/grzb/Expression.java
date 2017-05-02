package de.grzb;

public class Expression implements IChainable {
    private IChainable[] expressionArray;

    public Expression(IChainable[] list) {
        this.expressionArray = list;
    }

    public IChainable get(int index) {
        return this.expressionArray[index];
    }

    public void set(int index, IChainable value) {
        this.expressionArray[index] = value;
    }

    public int size() {
        return this.expressionArray.length;
    }

    public void remove(int index) {
        IChainable[] list = new IChainable[this.expressionArray.length - 1];
        for(int i = 0; i < this.expressionArray.length; i++) {
            if(i < index) {
                list[i] = this.expressionArray[i];
            }
            else if(i > index) {
                list[i - 1] = this.expressionArray[i];
            }
        }
        this.expressionArray = list;
    }

    public void reverse() {
        IChainable[] list = new IChainable[this.expressionArray.length];
        for(int i = this.expressionArray.length - 1; i >= 0; i--) {
            list[this.expressionArray.length - 1 - i] = this.expressionArray[i];
        }
        this.expressionArray = list;
    }

    public Expression copy() {
        IChainable[] expCopy = new IChainable[this.expressionArray.length];

        for(int i = 0; i < this.expressionArray.length; i++) {
            expCopy[i] = this.expressionArray[i].copy();
        }

        return new Expression(expCopy);
    }

    @Override
    public String toString() {
        StringBuilder stringBuilder = new StringBuilder();
        //stringBuilder.append("(");
        for(int i = 0; i < this.expressionArray.length; i++) {
            stringBuilder.append(this.expressionArray[i].toString().concat(" "));
        }
        //stringBuilder.append(")");
        return stringBuilder.toString();
    }

}

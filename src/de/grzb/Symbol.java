package de.grzb;


public enum Symbol implements IChainable {
    OPENING_PARENTHESIS("("),
    CLOSING_PARENTHESIS(")"),
    X("x");

    private final String name;

    Symbol(String name) {
        this.name = name;
    }

    public String getName() {
        return this.name;
    }

    @Override
    public IChainable copy() {
        return this;
    }

}

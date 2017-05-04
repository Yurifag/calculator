package de.grzb;

import java.util.ArrayList;
import java.util.Scanner;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class IOParser {
    private static final Pattern mathPattern = Pattern.compile("\\d+|[+\\-*/^()]|x");

    public static void print(Object object) {
        System.out.printf(object.toString() + "\n");
    }

    public static String input() {
        return new Scanner(System.in).nextLine();
    }

    /**
     * Modifies the User's input, creates a List and adds the equation (modified by regex parameter)
     */
    public static ArrayList<String> mathInput() {
        String input = input().toLowerCase().replace(" ", "").replace(",", ".");
        ArrayList<String> matches = new ArrayList<String>();
        Matcher matcher = mathPattern.matcher(input);
        while(matcher.find()) {
            matches.add(matcher.group());
        }

        return matches;
    }

}

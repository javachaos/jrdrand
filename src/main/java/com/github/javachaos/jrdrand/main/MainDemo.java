package com.github.javachaos.jrdrand.main;

import com.github.javachaos.jrdrand.RdRand;

/**
 * Main Demo class.
 */
public class MainDemo {

    /**
     * Unused Constructor.
     */
    private MainDemo() {}

    /**
     * Main entry point.
     * @param args unused.
     */
    public static void main(String[] args) {
        RdRand rd = new RdRand();
        long s = rd.seed();
        System.out.println("RDSEED: " + s);
        System.out.println("RDRAND: " + rd.rand());
    }
}

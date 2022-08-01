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
        System.out.println("RDSEED: " + rd.seed());
        System.out.println("RDRAND: " + rd.rand());
    }
}

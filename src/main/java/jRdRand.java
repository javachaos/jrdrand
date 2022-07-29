public class jRdRand {

    /**
     * Call c function which returns the result of a call to RDRAND
     * the function will attempt to get a value from the cpu 33 times.
     * In the unlikely event this does not return a value after 33 calls to RDRAND
     * this function will return 0
     * If the CPU is not an intel processor or does not support DRNG
     * return a value less than zero.
     *
     * @param seed initial seed value for rdrand
     * @return a 64-bit DRNG value from a supported intel processor
     */
    public native long rdrand(long seed);

    /**
     * Attempt to get a true random seed using the Intel RDSEED cpu instruction.
     * In the unlikely event that this function cannot return a seed it returns 0
     *
     * @return a true random seed 64-bits wide
     */
    public native long rdseed();

    static {
        try
        {
            System.loadLibrary ("rdrand");
        }
        catch ( Throwable e )
        {
            System.out.println ( "Native code library failed to load.\n" + e ) ;
            System.exit ( 1 ) ;
        }
    }

    public static void main(String[] args) {
        jRdRand jrd = new jRdRand();
        long seed = jrd.rdseed();
        long randomNum = jrd.rdrand(seed);
        System.out.println("RDSEED: " + seed);
        System.out.println("RDRAND: " + randomNum);
    }
}

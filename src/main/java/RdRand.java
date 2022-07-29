public class RdRand {

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
    private native long rdrand(long seed);

    /**
     * Attempt to get a true random seed using the Intel RDSEED cpu instruction.
     * In the unlikely event that this function cannot return a seed it returns 0
     *
     * @return a true random seed 64-bits wide
     */
    private native long rdseed();

    public long rand(long seed) {
        long s = 0;
        while (s <= 0) {
            s = rdrand(seed);
        }
        return s;
    }

    public long seed() {
        long s = 0;
        while (s <= 0) {
            s = rdseed();
        }
        return s;
    }

    static {//TODO Replace with System.load() and a switch statement per OS
        try
        {
            //must set VM option -Djava.library.path=build/libs/rdrand/shared
            System.loadLibrary ("rdrand");
        }
        catch ( Throwable e )
        {
            System.out.println ( "Native code library failed to load.\n" + e ) ;
            System.exit ( 1 ) ;
        }
    }
}

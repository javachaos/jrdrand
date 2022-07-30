import java.util.Locale;
import java.util.Objects;

/**
 * A simple RdRand class that has two functions
 * 1. rand() - get a random long (using RDRAND CPU instruction)
 * 2. seed() - get a random long seed value (using RDSEED CPU instruction)
 */
public class RdRand {

    /**
     * Create a new RdRand() instance
     */
    public RdRand() {
    }

    /**
     * Call c function which returns the result of a call to RDRAND
     * the function will attempt to get a value from the cpu until a result is returned.
     * In the unlikely event this does not return a value in a reasonable time frame (<1-2s)
     * there is likely a hardware issue with the Intel CPU.
     *
     * @return a 64-bit DRNG value from a supported intel processor
     */
    private native long rdrand();

    /**
     * Attempt to get a true random seed using the Intel RDSEED cpu instruction.
     * In the unlikely event that this function cannot return a seed it returns 0
     *
     * @return a true random seed 64-bits wide
     */
    private native long rdseed();

    /**
     * Execute RDRAND cpu instruction and return the value.
     *
     * @return a positive 64bit random value
     */
    public long rand() {
        long s = 0;
        while (s <= 0) {
            s = rdrand();
        }
        return s;
    }

    /**
     * Execute RDSEED cpu instruction and return the value.
     *
     * @return a positive 64bit random seed value
     */
    public long seed() {
        long s = 0;
        while (s <= 0) {
            s = rdseed();
        }
        return s;
    }

    static {
        String OS = System.getProperty("os.name", "generic").toLowerCase(Locale.ENGLISH);
        String protocol = Objects.requireNonNull(RdRand.class.getResource("RdRand.class")).getProtocol();
        if(Objects.equals(protocol, "jar")){
            if (OS.startsWith("windows")) {
                NativeUtils.loadWindows();
            }
            if (OS.startsWith("linux")) {
                NativeUtils.loadLinux();
            }
            //TODO Add support for more OS's
        } else if(Objects.equals(protocol, "file")) {
            System.loadLibrary ("rdrand");
        }
    }
}

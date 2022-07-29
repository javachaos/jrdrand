import java.io.IOException;
import java.util.Locale;
import java.util.Objects;

public class RdRand {

    /**
     * Call c function which returns the result of a call to RDRAND
     * the function will attempt to get a value from the cpu 33 times.
     * In the unlikely event this does not return a value after 33 calls to RDRAND
     * this function will return 0
     * If the CPU is not an intel processor or does not support DRNG
     * return a value less than zero.
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

    public long rand() {
        long s = 0;
        while (s <= 0) {
            s = rdrand();
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
            //must set VM option -Djava.library.path=build/libs/rdrand/shared
            //for running in IDE load from build dir.
            System.loadLibrary ("rdrand");
        }
    }
}

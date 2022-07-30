import java.io.*;

public class NativeUtils {

    public static void loadLinux() {
        loadLibraryFromJar("native/x86_64/linux/librdrand.so", "librdrand", ".so");
    }

    public static void loadWindows() {
        loadLibraryFromJar("native/x86_64/windows/rdrand.dll", "rdrand", ".dll");
    }

    private static void loadLibraryFromJar(final String path, final String prefix, final String suffix) {
        InputStream is = ClassLoader.getSystemResourceAsStream(path);
        if (is == null) {
            return;
        }
        try {
            File file = File.createTempFile(prefix, suffix);
            OutputStream os = new FileOutputStream(file);
            byte[] buffer = new byte[1024];
            int length;
            while ((length = is.read(buffer)) != -1) {
                os.write(buffer, 0, length);
            }
            is.close();
            os.close();

            System.load(file.getAbsolutePath());
            file.deleteOnExit();
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }
}

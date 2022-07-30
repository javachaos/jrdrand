package com.github.javachaos.jrdrand.utils;

import java.io.*;

/**
 * Native Utilities Class
 */
public class NativeUtils {

    /**
     * Unused Constructor
     */
    private NativeUtils() {}

    /**
     * Loads the linux shared library into the JVM.
     */
    public static void loadLinux() {
        loadLibraryFromJar("native/x86_64/linux/librdrand.so", "librdrand", ".so");
    }

    /**
     * Loads the linux shared library into the JVM
     */
    public static void loadWindows() {
        loadLibraryFromJar("native/x86_64/windows/rdrand.dll", "rdrand", ".dll");
    }

    /**
     * Load a library from within our jar into a temp file and then to
     * the current running JVM instance.
     * @param path the path to the native library
     * @param prefix the name prefix of the native library
     * @param suffix the name suffix of the native library (e.g. .so or .dll)
     */
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

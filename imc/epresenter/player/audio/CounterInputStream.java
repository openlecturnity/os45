package imc.epresenter.player.audio;                                                                   

import java.io.*;

public class CounterInputStream extends InputStream {
    private long counter;
    private InputStream input;

    public CounterInputStream(InputStream in) {
        input = in;
        counter = 0;
    }

    public long bytesProcessed() {
        return counter;
    }

    public void resetCounter() {
        counter = 0;
    }

    public int available() throws IOException {
        return input.available();
    }

    public int read() throws IOException {
        counter++;
        return input.read();
    }

    public int read(byte[] array) throws IOException {
        return read(array, 0, array.length);
    }

    public int read(byte[] array, int off, int len) throws IOException {
        int r = input.read(array, off, len);
        if (r != -1) counter+=r;
        return r;
    }

    public long skip(long length) throws IOException {
        long s = input.skip(length);
        counter += s;
        return s;
    }

    public void close() throws IOException {
        input.close();
    }
}
    

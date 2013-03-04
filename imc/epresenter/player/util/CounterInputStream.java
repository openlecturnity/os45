package imc.epresenter.player.util;

import java.io.*;

public class CounterInputStream extends InputStream {
	private long readBytes = 0;
	private InputStream input;
	
	public CounterInputStream(InputStream is) {
		input = is;
	}
	
	public int read() throws IOException {
		readBytes++;
		return input.read();
	}
	
	public int read(byte[] b) throws IOException {
		int r = input.read(b);
		readBytes += r;
		return r;
	}
	
	public int read(byte[] b, int off, int len) throws IOException {
		int r = input.read(b, off, len);
		readBytes += r;
		return r;
	}
	
	public long readBytes() {
		return readBytes;
	}
}

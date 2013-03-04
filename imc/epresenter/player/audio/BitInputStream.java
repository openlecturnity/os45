package imc.epresenter.player.audio;

import java.io.*;

public class BitInputStream extends InputStream {
	private InputStream input;
	private int[] mask;
	private int buffer;
	private int bufferedBits;
	private int maxBufferedBits;

	public BitInputStream(InputStream in) {
		this(in, 8);
	}

	public BitInputStream(InputStream in, int maxBits) {
		if (maxBits <= 0 || maxBits > 32 || maxBits%8 != 0) throw new IllegalArgumentException(
			"Maximum number of buffer bits ("+maxBits+") must be out of [8,16,24,32].");
		input = in;
		maxBufferedBits = maxBits;
		mask = new int[33];
		for (int i=0; i<mask.length; i++) mask[i] = (1 << i) - 1;
	}

	public int readBitString(int length) throws IOException {
		if (length <= 0 || length > 32) throw new IllegalArgumentException(
			"The number of bits to read ("+length+") must be out of [1, 32].");
		int bitString = 0;
		int copiedBits = 0;

		while(copiedBits < length) {
			if (bufferedBits == 0) { // fill buffer with (maxBufferdBits / 8) bytes
				buffer = 0;
				int i = 0;
				buffer = read(); // at least one byte should be readable
				if (buffer == -1) throw new EOFException();
				for (i=0; i<maxBufferedBits/8-1; i++) {
					try {
						buffer = buffer << 8 | read();
					} catch (IOException e) { 
						break;
					}
				}
				bufferedBits = 8+i*8;
			}
			int toCopy = Math.min(length-copiedBits, bufferedBits);
			bitString = bitString << toCopy; // make room
			bitString |= buffer >> (bufferedBits-toCopy) & mask[toCopy];
			bufferedBits -= toCopy;
			copiedBits += toCopy;
		}

		return bitString;
	}

	public void discardBufferedBits() {
		bufferedBits = 0;
	}

	public int read() throws IOException {
		if (bufferedBits > 0) throw new IllegalStateException(
			"There may be no buffered bits when trying to do a normal read().");
		return input.read();
	}

	public long skip(long n) throws IOException {
		if (bufferedBits > 0) throw new IllegalStateException(
			"There may be no buffered bits when trying to do skip().");
		return input.skip(n);
	}


	public static void main(String[] args) throws IOException {
		byte[] array = { (byte)0xff, (byte)0x0f, (byte)0x11 };
		ByteArrayInputStream bin = new ByteArrayInputStream(array);
		BitInputStream rin = new BitInputStream(bin);
		for (int i=0; i<8; i++) System.out.println(Integer.toBinaryString(rin.readBitString(3)));

	}
}



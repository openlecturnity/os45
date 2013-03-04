package imc.epresenter.player.audio;

import java.io.*;

public class BitOutputStream extends OutputStream {
	private OutputStream output;
	private int[] mask;
	private int buffer;
	private int bufferedBits;
	private int maxBufferedBits;

	public BitOutputStream(OutputStream out) {
		this(out, 8);
	}

	public BitOutputStream(OutputStream out, int maxBits) {
		if (maxBits <= 0 || maxBits > 32 || maxBits%8 != 0) throw new IllegalArgumentException(
			"Maximum number of buffer bits ("+maxBits+") must be out of [8,16,24,32].");
		output = out;
		maxBufferedBits = maxBits;
		mask = new int[33];
		for (int i=0; i<mask.length; i++) mask[i] = (1 << i) - 1;
	}

	public void writeBitString(int value, int length) throws IOException {
		if (length <= 0 || length > 32) throw new IllegalArgumentException(
			"The number of bits to write ("+length+") must be out of [1, 32].");
		int copiedBits = 0;
		
		while(copiedBits < length) {
			int toCopy = Math.min(length-copiedBits, maxBufferedBits-bufferedBits);
			buffer = buffer << toCopy; // make room
			buffer = buffer | ((value >> (length-(toCopy+copiedBits))) & mask[toCopy]);
			copiedBits += toCopy;
			bufferedBits += toCopy;
			if (bufferedBits == maxBufferedBits) flushBufferedBits();
		}
	}

	public void flushBufferedBits() throws IOException {
		while (bufferedBits > 0) {
			if (bufferedBits > 8) {
				output.write((buffer >> (bufferedBits-8)) & mask[8]); 
			} else if (bufferedBits == 8) {
				output.write(buffer & mask[8]);
			} else {
				output.write((buffer << (8-bufferedBits)) & mask[8]);
			}
			bufferedBits -= Math.min(bufferedBits, 8);
		}
		buffer = 0;
	}

	public void write(int b) throws IOException {
		if (bufferedBits > 0) throw new IllegalStateException(
			"There may be no buffered bits when trying to do a normal write().");
		output.write(b);
	}

	public void flush() throws IOException {
		flushBufferedBits();
		output.flush();
	}

	public void close() throws IOException {
		flushBufferedBits();
		output.close();
	}

	public static void main(String[] args) throws IOException {
		int[] mas = new int[17];
		for (int i=0; i<mas.length; i++) mas[i] = (1 << i) - 1;
		int output = 0xfd << 24 | 0x7a << 16 | 0x6e << 8 | 0xa2;
		ByteArrayOutputStream bon = new ByteArrayOutputStream(8);
		BitOutputStream ron = new BitOutputStream(bon);
		int writtenBits = 0;
		while(writtenBits < 4*8) {
			int toWrite = Math.min((int)(Math.random()*16), 4*8-writtenBits);
			ron.writeBitString((output >> 32-writtenBits-toWrite) & mas[toWrite], toWrite);
			writtenBits += toWrite;
		}

		ByteArrayInputStream bin = new ByteArrayInputStream(bon.toByteArray());
		for (int i=0; i<4; i++) {
			System.out.print(Integer.toBinaryString(bin.read())+" ");
		}
		System.out.println();

	}
}


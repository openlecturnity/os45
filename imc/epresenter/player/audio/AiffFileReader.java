package imc.epresenter.player.audio;

import java.io.*;
import java.net.URL;
import javax.sound.sampled.*;
import javax.sound.sampled.spi.AudioFileReader;

public class AiffFileReader extends AudioFileReader {
	public AudioFileFormat getAudioFileFormat(InputStream inputstream)
		throws UnsupportedAudioFileException, IOException
	{
      
		AudioFormat audioformat = null;
		AiffFileFormat aifffileformat = null;
		byte byte0 = 8;
		int i = 0;
		int j = 0;
		int k = 0;
		DataInputStream datainputstream = new DataInputStream(inputstream);
		datainputstream.mark(byte0);
		i = datainputstream.readInt();
		j = datainputstream.readInt();
		if(i != 0x464f524d) {
			//System.err.println("AR: no magic number ("+Integer.toHexString(i)+")");
			datainputstream.reset();
			throw new UnsupportedAudioFileException("not an AIFF file");
		}
		if(j <= 0) {
			j = -1;
			k = -1;
		} else {
			k = j + 8;
		}
		audioformat = new AudioFormat(-1F, -1, -1, true, true);
		aifffileformat = new AiffFileFormat(AudioFileFormat.Type.AIFF, k, audioformat, -1);
		datainputstream.reset();
		return aifffileformat;
	}

	public AudioFileFormat getAudioFileFormat(URL url)
		throws UnsupportedAudioFileException, IOException
	{
		InputStream inputstream = null;
		BufferedInputStream bufferedinputstream = null;
		DataInputStream datainputstream = null;
		AudioFileFormat audiofileformat = null;
		inputstream = url.openStream();
		bufferedinputstream = new BufferedInputStream(inputstream, 4096);
		audiofileformat = getAudioFileFormat(((InputStream) (bufferedinputstream)));
		datainputstream = new DataInputStream(bufferedinputstream);
		audiofileformat = getCOMM(datainputstream, audiofileformat);
		bufferedinputstream.close();
		return audiofileformat;
	}

	public AudioFileFormat getAudioFileFormat(File file)
		throws UnsupportedAudioFileException, IOException
	{
		FileInputStream fileinputstream = null;
		BufferedInputStream bufferedinputstream = null;
		DataInputStream datainputstream = null;
		AudioFileFormat audiofileformat = null;
		fileinputstream = new FileInputStream(file);
		bufferedinputstream = new BufferedInputStream(fileinputstream, 4096);
		audiofileformat = getAudioFileFormat(((InputStream) (bufferedinputstream)));
		datainputstream = new DataInputStream(bufferedinputstream);
		audiofileformat = getCOMM(datainputstream, audiofileformat);
		bufferedinputstream.close();
		return audiofileformat;
	}

	public AudioInputStream getAudioInputStream(InputStream inputstream)
		throws UnsupportedAudioFileException, IOException
	{
      DataInputStream datainputstream = null;
		AudioFileFormat audiofileformat = null;
		audiofileformat = getAudioFileFormat(inputstream);
		datainputstream = new DataInputStream(inputstream);
		audiofileformat = getCOMM(datainputstream, audiofileformat);
		return new AudioInputStream(datainputstream, audiofileformat.getFormat(), audiofileformat.getFrameLength());
	}

	public AudioInputStream getAudioInputStream(URL url)
		throws UnsupportedAudioFileException, IOException
	{
		InputStream inputstream = null;
		BufferedInputStream bufferedinputstream = null;
		inputstream = url.openStream();
		bufferedinputstream = new BufferedInputStream(inputstream, 4096);
		return getAudioInputStream(((InputStream) (bufferedinputstream)));
	}

	public AudioInputStream getAudioInputStream(File file)
		throws UnsupportedAudioFileException, IOException
	{
		FileInputStream fileinputstream = null;
		BufferedInputStream bufferedinputstream = null;
		fileinputstream = new FileInputStream(file);
		bufferedinputstream = new BufferedInputStream(fileinputstream, 4096);
		return getAudioInputStream(((InputStream) (bufferedinputstream)));
	}

	private AudioFileFormat getCOMM(DataInputStream datainputstream, AudioFileFormat audiofileformat)
		throws UnsupportedAudioFileException, IOException
	{
		AudioFormat.Encoding encoding = null;
		short sampleSize = -1;
		short channels = -1;
		int frameSize = -1;
		float sampleRate = -1F;
		float frameRate = -1F;
		int l = 0;
		boolean aifc = false;
		byte abyte0[] = new byte[4];
		int i2 = datainputstream.readInt();
		l += 4;
		int j = datainputstream.readInt();
		l += 4;
		datainputstream.read(abyte0, 0, 4);
		l += 4;
		String s = new String(abyte0);
		if(s.startsWith("AIFC")) aifc = true;
		do {
			int ckID = datainputstream.readInt();
			abyte0[0] = (byte)(ckID >> 24 & 0xff);
			abyte0[1] = (byte)(ckID >> 16 & 0xff);
			abyte0[2] = (byte)(ckID >> 8 & 0xff);
			abyte0[3] = (byte)(ckID & 0xff);
			String s1 = new String(abyte0);
			if(s1.startsWith("FVER")) {
				datainputstream.readLong();
				l += 8;
				continue;
			}
			if(s1.startsWith("COMM")) {
				int csize = datainputstream.readInt();
				l += 4;
				//if(csize != 18 && csize != 34)
				//	throw new UnsupportedAudioFileException("Invalid AIFF chunksize");
				channels = datainputstream.readShort();
				l += 2;
				datainputstream.readInt();
				l += 4;
				sampleSize = datainputstream.readShort();
				l += 2;
				sampleRate = (float)read_ieee_extended(datainputstream);
				l += 10;
				encoding = AudioFormat.Encoding.PCM_SIGNED;
				if(aifc) {
					datainputstream.read(abyte0, 0, 4);
					l += 4;
					s1 = new String(abyte0);
					if(s1.startsWith("NONE")) {
						encoding = AudioFormat.Encoding.PCM_SIGNED;
					} else if(s1.startsWith("ULAW"))
						encoding = AudioFormat.Encoding.ULAW;
					else if(s1.startsWith("ALAW"))
						encoding = AudioFormat.Encoding.ALAW;
					else
						throw new UnsupportedAudioFileException("Invalid AIFF encoding");
					
					/* skip the compression description string */
					int stringLength = datainputstream.readByte();
					l++;
					/* this string occupies together with it's */
					/* length byte an even number of bytes */
					if (stringLength %2 != 1) stringLength++;
					l += datainputstream.skipBytes(stringLength);
				}
				continue;
			}
			if(s1.startsWith("SSND")) {
				int j1 = datainputstream.readInt();
				l += 4;
				int l1 = datainputstream.readInt();
				l += 4;
				int k2 = datainputstream.readInt();
				l += 4;
				if(j1 < j) j = j1 - 8;
				else j -= l;
				break;
			}
			int k1 = datainputstream.readInt();
			l = (l += 4) + datainputstream.skipBytes(k1);
		} while(true);
		frameSize = (sampleSize / 8) * channels;
		frameRate = sampleRate;
		//if (sampleSize == 8) encoding = AudioFormat.Encoding.PCM_SIGNED;
		AudioFormat audioformat = new AudioFormat(
			encoding, sampleRate, sampleSize, channels, frameSize, frameRate, true);
		return new AiffFileFormat(
			AudioFileFormat.Type.AIFF, audiofileformat.getByteLength(), audioformat, j / frameSize);
	}

	private void write_ieee_extended(DataOutputStream dataoutputstream, double d)
		throws IOException
	{
		int i = 16398;
		double d1;
		for(d1 = d; d1 < 44000D;) {
			d1 *= 2D;
			i--;
		}

		dataoutputstream.writeShort(i);
		dataoutputstream.writeInt((int)d1 << 16);
		dataoutputstream.writeInt(0);
	}

	private double read_ieee_extended(DataInputStream datainputstream)
		throws IOException
	{
		double d = 0.0D;
		int i = 0;
		long l = 0L;
		long l1 = 0L;
		double d1 = 3.4028234663852886E+38D;
		i = datainputstream.readUnsignedShort();
		long l2 = datainputstream.readUnsignedShort();
		long l3 = datainputstream.readUnsignedShort();
		l = l2 << 16 | l3;
		l2 = datainputstream.readUnsignedShort();
		l3 = datainputstream.readUnsignedShort();
		l1 = l2 << 16 | l3;
		if(i == 0 && l == 0L && l1 == 0L)
			d = 0.0D;
		else
		if(i == 32767) {
			d = d1;
		} else {
			i -= 16383;
			i -= 31;
			d = (double)l * Math.pow(2D, i);
			i -= 32;
			d += (double)l1 * Math.pow(2D, i);
		}
		return d;
	}

	public static final javax.sound.sampled.AudioFileFormat.Type types[];

	static 
	{
		types = (new javax.sound.sampled.AudioFileFormat.Type[] {
			javax.sound.sampled.AudioFileFormat.Type.AIFF
		});
	}
}

package imc.epresenter.player.util;

public class TimeFormat {
	private char[] output;
	
	public TimeFormat() {
		output = new char[9];
		output[1] = ':';
		output[4] = ':';
		output[7] = ',';
	}
	
	public String format(int millis) {
		int hours = millis/(1000*3600);
		millis %= 1000*3600;
		int minutes = millis/(1000*60);
		millis %= 1000*60;
		int seconds = millis/1000;
		millis %= 1000;
		
		output[0] = (char)('0'+hours);
		output[2] = (char)('0'+minutes/10);
		output[3] = (char)('0'+minutes%10);
		output[5] = (char)('0'+seconds/10);
		output[6] = (char)('0'+seconds%10);
		output[8] = (char)('0'+millis/100);
		
		return new String(output);
	}

   public String shortFormat(int millis)
   {
      return format(millis).substring(0,7);
   }
	
	public int getMinutes(int millis) {
		return millis/(1000*60);
	}
	public int getSeconds(int millis) {
		return millis/1000;
	}
	
}

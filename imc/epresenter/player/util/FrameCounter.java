package imc.epresenter.player.util;

public class FrameCounter {
	private int insertPointer;
	private int elementCount;
	private int currentSum;
	private int[] time; 
	
	public FrameCounter() { this(30); }
	
	public FrameCounter(int max) {
		if (max < 1) throw new IllegalArgumentException(
			"Number of elements must be greater than 0 but is "+max+".");

		insertPointer = 0;
		elementCount = 0;
		currentSum = 0;
		time = new int[max];
	}
	
	public void add(int millis) {
		if (millis < 0) millis = 1;
		currentSum -= time[insertPointer];
		time[insertPointer] = millis;
		currentSum += millis;
		if (elementCount < time.length) elementCount++;
		insertPointer++;
		if (insertPointer == time.length) insertPointer = 0;
	}

	public float getCurrentFrames() {
		//System.out.println("sum="+currentSum+" count="+elementCount);
		if (elementCount != 0) return 1000/(currentSum/(float)elementCount); 
		else return 0.0f;
	}

	public String toString() {
		String result = "last values first: ";
		for (int n = 0; n<elementCount; n++) {
			result += (time[n]+" ");
		}
		return result;
	}
} // FrameCounter

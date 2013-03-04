package imc.epresenter.player.util;

import java.util.*;

public class RangeGroup {
	// the content itself
	private int currentCount;
	private int totalValues;
	private int[] rangeStart;
	private short[] rangeLength; // at least 1 for every position
	private boolean isSorted;
	private int[] sortedStart;
	private short[] sortedLength;
	
	// for iterating through the content
	private int iteratorPosition;
	private int currentValue;
	private int remainingLength;
	
   // Note: Initially this class was to represent integer
   // values with space saving: every value can stand for a whole range.
   // But nowadays (2006): it seems to be only used like a normal array:
   // The add() method below gets only called with length 1.
	
	public RangeGroup() {
		this(100);
	}
	
	public RangeGroup(int initialSize) {
		if (initialSize < 1)
			throw new IllegalArgumentException("Initial size must be > 0, but is "+initialSize+"!");
		
		rangeStart = new int[initialSize];
		rangeLength = new short[initialSize];
		sortedStart = rangeStart;
		sortedLength = rangeLength;
		currentCount = 0;
		totalValues = 0;
		isSorted = true;
	}
	
	public String summary() {
		return ((currentCount/(float)totalValues*100))+"";
	}

	
	//
	// content methods
	//
	
	public void add(int value, int length) {
		if (value < 0)
			throw new IllegalArgumentException("Value must be >= 0, but is "+value+"!");
		if (length < 1)
			throw new IllegalArgumentException("Length must be >= 1, but is "+length+"!");
		
		
		if (length > Short.MAX_VALUE) {
			// if length does not fit in a short: do split
			add(value, Short.MAX_VALUE);
			add(value+Short.MAX_VALUE, length-Short.MAX_VALUE);
		} else {
			// do real insert
			totalValues += length;
			
			if (currentCount == rangeStart.length) changeSize(rangeStart.length*2);
			
			rangeStart[currentCount] = value;
			rangeLength[currentCount] = (short)length;
			
			if (isSorted && currentCount > 0 && rangeStart[currentCount] < rangeStart[currentCount-1]) {
				isSorted = false;
				sortedStart = null;
				sortedLength = null;
			}
			
			currentCount++;
		}
	}
	
	public void trim() {
		// if arrays are filled fewer than 90%: shrink them
		// but to a size of at least 4 entries
		if (currentCount < (int)(0.9*rangeStart.length))
			changeSize(Math.max(currentCount, 4));
	}
	
	public int size() {
		return currentCount;
	}
	
	private void changeSize(int newSize) {
		int[] rS = new int[newSize];
		short[] rL = new short[newSize];
		
		if (currentCount > 0) {
			System.arraycopy(rangeStart, 0, rS, 0, currentCount);
			System.arraycopy(rangeLength, 0, rL, 0, currentCount);
		}
		
		rangeStart = rS;
		rangeLength = rL;
		if (isSorted) {
			sortedStart = rS;
			sortedLength = rL;
		}
	}
	
	
	
	//
	// iterator methods
	//
	public void initIterator() {
		if (currentCount > 0) {
			currentValue = rangeStart[0];
			iteratorPosition = 0;
			remainingLength = rangeLength[0]-1;
		} else {
			currentValue = -1;
		}
	}
	
	public boolean hasNextValue() {
		return currentValue != -1;
	}
	
	public int nextValue() {
		int value = currentValue;
		findNextValue();
		return value;
	}
   
   public int firstValue() {
      if (currentCount > 0)
			return rangeStart[0];
      else
			return -1;
   }
	
	private void findNextValue() {
		if (remainingLength > 0) {
			currentValue++;
			remainingLength--;
		} else if (iteratorPosition < currentCount-1) {
			iteratorPosition++;
			currentValue = rangeStart[iteratorPosition];
			remainingLength = rangeLength[iteratorPosition]-1;
		} else {
			currentValue = -1;
		}
	}
	
	
	//
	// comparator methods
	//
	public RangeGroup diff(RangeGroup that) {
		if (this.size() == 0)
			if (that.size() == 0) return new RangeGroup(4);
			else return (RangeGroup) that.clone();
		else if (that.size() == 0) return (RangeGroup) this.clone();
		
		// eigentlich (ab hier) Verwendug von stopValues() und merge
		
		
		if (!this.isSorted && this.sortedStart == null) this.sort();
		if (!that.isSorted && that.sortedStart == null) that.sort();
		
		int[] stops = new int[this.size()*2+that.size()*2];
		for (int i=0; i<this.size(); i++) {
			stops[2*i] = this.sortedStart[i];
			stops[2*i+1] = this.sortedStart[i]+this.sortedLength[i];
			//System.out.print(this.sortedStart[i]+" "+(this.sortedStart[i]+this.sortedLength[i])+" ");
		}
		//System.out.println();
		for (int i=0; i<that.size(); i++) {
			stops[this.size()*2+2*i] = that.sortedStart[i];
			stops[this.size()*2+2*i+1] = that.sortedStart[i]+that.sortedLength[i];
			//System.out.print(that.sortedStart[i]+" "+(that.sortedStart[i]+that.sortedLength[i])+" ");
		}
		//System.out.println();
		//for (int i=0; i<stops.length; i++) System.out.print(stops[i]+" ");
		//System.out.println();
		
		Arrays.sort(stops);
		
		//for (int i=0; i<stops.length; i++) System.out.print(stops[i]+" ");
		//System.out.println();
		
		RangeGroup differences = new RangeGroup();
		
		for (int i=0; i<stops.length; i+=2) {
			if (stops[i] != stops[i+1])
				differences.add(stops[i], stops[i+1]-stops[i]);
		}
		
		return differences;
	}
	
	 int[] stopValues() {
		int[] stops = new int[currentCount*2];
		if (!isSorted && sortedStart == null) sort();
		
		for (int i=0; i<currentCount; i++) {
			stops[2*i] = sortedStart[i];
			stops[2*i+1] = sortedStart[i]+sortedLength[i];
		}
		return stops;
	}
	
	 int[] merge(int[] a, int[] b) {
		int[] c = new int[a.length+b.length];
		
		int i=0,j=0,k=0;
		while(i<a.length && j<b.length) {
			if (a[i] <= b[j]) c[k++] = a[i++];
			else c[k++] = b[j++];
		}
		while(i<a.length) c[k++] = a[i++];
		while(j<b.length) c[k++] = b[j++];
		
		return c;
	}
	
	private void sort() {
		IntHash oldPos = new IntHash(currentCount*2);
		for (int p=0; p<currentCount; p++) oldPos.put(rangeStart[p], p);
		
		int[] sS = new int[currentCount];
		short[] sL = new short[currentCount];
		
		System.arraycopy(rangeStart, 0 , sS, 0, currentCount);
		Arrays.sort(sS);
		for (int p=0; p<currentCount; p++) {
			int newP = p;
			int oldP = oldPos.get(sS[newP]);
			sL[newP] = rangeLength[oldP];
		}
		
		sortedStart = sS;
		sortedLength = sL;
	}
	
	// not that fast
	protected Object clone() {
		RangeGroup copy = new RangeGroup(currentCount);
		for (int i=0; i<currentCount; i++) copy.add(rangeStart[i], rangeLength[i]);
		if (isSorted && sortedStart != rangeStart) copy.sort();
		return copy;
	}


	public String toString()
	{
		// return at most first 10 elements
      
      StringBuffer buffer = new StringBuffer("["); 
		initIterator();
		int iCounter = 0;
      while(hasNextValue() && iCounter++ < 10)
		{
			buffer.append(nextValue()+", ");
		}
		buffer.append("]");
		return buffer.toString();
	}
	
	
	public static void main(String[] args) {
		RangeGroup rg = new RangeGroup();
		/*
		rg.add(1,7);
		rg.add(9,1);
		rg.add(11,1);
		rg.add(13,1);
		rg.add(15,1);
		*/
		
		RangeGroup rg2 = new RangeGroup();
		/*
		rg2.add(1,9);
		rg2.add(11,2);
		rg2.add(17,4);
		*/

		System.out.println(rg);
		System.out.println(rg2);
		System.out.println(rg.diff(rg2));
		
		/*
		
		rg = rg.diff(rg2);
		
		rg.initIterator();
		while(rg.hasNextValue()) System.out.print(rg.nextValue()+" ");
		System.out.println();
		*/
		
		/*
		int[] s1 = rg.stopValues();
		int[] s2 = rg2.stopValues();
		s1 = rg.merge(s1, s2);
		for (int i=0; i<s1.length; i++) System.out.print(s1[i]+" ");
		System.out.println();
		*/
	}
}

package imc.epresenter.player.util;

import java.util.Arrays;

public class IntHash {
	private long[] keys;
	private int[] values;
   private int numElements;
	
	
	public IntHash() {
		this(512);
	}
	
	// the size is fixed ! (no growing and rehash supported)
	// make sure that the hash is not fuller than the half of the specified size
	public IntHash(int size) {
		keys = new long[size];
		Arrays.fill(keys, Long.MIN_VALUE);
		values = new int[size];
	}
	
	
	
	public void put(int key, int value)
   {
		int index = findIndex(key);
      if (keys[index] == Long.MIN_VALUE)
         numElements++;
      if (numElements >= keys.length)
         throw new IllegalStateException("No more room to store additional values.");
		keys[index] = key;
		values[index] = value;
	}
	
	public int get(int key)
   {
		int index = findIndex(key);
      
		return values[index];
	}

   public int size()
   {
      return numElements;
   }

   public boolean containsKey(int key)
   {
      int index = findIndex(key);
      return keys[index] != Long.MIN_VALUE;
   }

   private int findIndex(int key)
   {
      int index = key%keys.length;
      do
      {
         try{
         if (keys[index] == Long.MIN_VALUE || keys[index] == key)
            break;
         } catch (Exception e)
         {
            System.out.println(index);
            break;
         }
         index++;
         if (index == keys.length)
            index = 0;
      } while (true);
   
      return index;
   }
}
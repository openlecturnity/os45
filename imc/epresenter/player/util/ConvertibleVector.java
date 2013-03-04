package imc.epresenter.player.util;

import java.lang.reflect.Array;
import java.util.Vector;

/**
 * @author Dennis M. Sosnoski
 */
public class ConvertibleVector extends Vector {
	public ConvertibleVector(int i1, int i2) { super(i1, i2); }
	public Object buildArray(Class type) {
		Object copy = Array.newInstance(type, elementCount);
		System.arraycopy(elementData, 0, copy, 0, elementCount);
		return copy;
	}
	public Object buildArray() {
		if (elementCount > 0 && elementData[0] != null) {
			return buildArray(elementData[0].getClass());
		} else {
			throw new IllegalStateException("first element null or no elements present");
		}
	}
}

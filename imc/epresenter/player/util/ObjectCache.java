package imc.epresenter.player.util;

import java.util.*;

/**
 * Verwaltet eine Menge von Elementen.
 * Zu dieser Verwaltung gehoert auch die Ueberwachung des 
 * Alterungsprozesses und der Element-Obergrenze, sofern dies
 * gewünscht ist. Standardmaessig duerfen beliebig viele 
 * Elemente abgespeichert werden, alle aber hoechstens eine Stunde lang.
 *
 * Trotz der Verwendung von einer (synchronisierten) Hashtable
 * muß der Zugriff synchronisiert werden -> es koennte auch eine Hashmap
 * verwendet werden.
 */ 
public class ObjectCache {
	private int containedElements = 0;
	private int maxElements;
	private int maxStoreMillis;
	private Hashtable elementStore;
	
	public ObjectCache() {
		this(-1, 60*60);
	}
	public ObjectCache(int mE, int mS) {
		maxElements = mE;
		maxStoreMillis = mS*1000;
		
		elementStore = new Hashtable();
	}
	
	public synchronized boolean contains(Object key) {
		if (key == null) return false;
		return elementStore.containsKey(key);
	}
	
	/**
	 * Fügt ein Element neu hinzu. Jedem Element wird zufaellig
	 * ein lesbarere Name zugeordnet ueber den das Element spaeter
	 * abgerufen werden kann.
	 * Falls noetig muss zur Speicherung aber zuerst das aelteste
	 * schon enthaltene Element rausgeworfen werden.
	 * 
	 * @returns einen eindeutigen Schluessel zum Wiederabruf der Ergebnisse
	 */
	public synchronized String put(Object o) {
		cleanup();
		
		int keyLength = 6;
		String key = randomName(keyLength); //Integer.toHexString((int)System.currentTimeMillis());
		while (elementStore.containsKey(key)) {
			key = randomName(keyLength);
			if (Math.random() < 0.3) keyLength++;
		}
		elementStore.put(key, new Element(o));
		containedElements++;
		
		return key;
	}
	
	public synchronized Object get(Object key) {
		Element sr = (Element)elementStore.get(key);
		if (sr != null) return sr.content;
		else return null;
	}
	
	/**
	 * Diese Methode sorgt dafuer, dass die gespeicherten Element nicht zu alt sind
	 * (maxStoreMillis) und nicht zu viele Elemente enthalten sind (maxElements).
	 * Beide Werte koennen allerdings 0 sein, dann macht diese Methode nichts. Ansonsten
	 * wird ueberprueft, ob zu alte Elemente geloescht werden muessen und wenn dies
	 * nicht der Fall ist, ob das aelteste Element geloescht werden muss 
	 * (um Platz zu schaffen).
	 */
	private void cleanup() {
		if (elementStore.size() > 0) {
			boolean removeOldest = false;
			if (maxElements > 0 && containedElements >= maxElements) removeOldest = true;
			Object oldestKey = null;
			long oldestDate = Long.MAX_VALUE;
			long currentDate = System.currentTimeMillis();
			
			Enumeration keys = elementStore.keys();
			while (keys.hasMoreElements()) {
				Object currentKey = keys.nextElement();
				Element sr = (Element)elementStore.get(currentKey);
				if (maxStoreMillis > 0 && currentDate-sr.createDate > maxStoreMillis) {
					elementStore.remove(currentKey);
					containedElements--;
					removeOldest = false;
				}
				if (removeOldest && sr.createDate < oldestDate) {
					oldestKey = currentKey;
					oldestDate = sr.createDate;
				}
			}
			
			if (removeOldest) elementStore.remove(oldestKey);
		}
	}
	
	private char[] konsonanten = {'b','c','d','f','g','h',
		'j','k','l','m','n','p','r','s','t','v','w','x','z'};
	private char[] vokale = {'a','e','i','o','u','y'};
	
	private String randomName(int length) {
		char[] name = new char[length];
		boolean konsonant = Math.random() < 0.5;
		for (int i=0; i<name.length; i++) {
			if (konsonant) name[i] = konsonanten[(int)(Math.random()*konsonanten.length)];
			else name[i] = vokale[(int)(Math.random()*vokale.length)];
			konsonant = !konsonant;
		}
		name[0] -= 32;
		return new String(name);
	}
	
	private class Element {
		long createDate;
		Object content;
		
		Element(Object o) {
			content = o;
			createDate = System.currentTimeMillis();
		}
	}
}

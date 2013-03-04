package imc.epresenter.player.util;

public class WaitingQueue {
	private class Element {
		Element next;
		Object content;
		Element(Object o) {
			content = o;
		}
	}
	private Element head;
	private Element tail;
	private int numElements;
	
	public void enqueue(Object o) {
		if (head == null) {
			head = new Element(o);
			tail = head;
		} else {
			tail.next = new Element(o);
			tail = tail.next;
		}
		numElements++;
	}
	
	public Object dequeue() {
		if (head != null) {
			Object data = head.content;
			head = head.next;
			numElements--;
			return data;
		} else {
			return null;
		}
	}
	
	public boolean contains(Object o) {
		Element n = head;
		while(n != null) {
			if (n == o) return true;
			else n = n.next;
		}
		return false;
	}
	
	public int size() {
		return numElements;
	}
}

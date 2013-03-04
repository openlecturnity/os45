package imc.epresenter.player.util;

import java.io.*;

public class AVLTree {
	public static void main(String[] args) throws IOException {
		int iter = 20000;
		if (args.length > 0) {
			try {
				iter = Integer.parseInt(args[0]);
			} catch (NumberFormatException e) {}
		}
		
		System.out.println("Doing "+iter+" trees:");
		
		for (int i=0; i<iter; i++) {
			int count = 10000+(int)(Math.random()*40000);
			Comparable[] keys = new Comparable[count];
			for (int j=0; j<count; j++) keys[j] = new Float((float)Math.random());
			
			System.out.print((i)+" ");
			
			AVLTree t = new AVLTree();
			boolean failure = false;
			for (int j=0; j<count; j++) {
				try {
					t.put(keys[j], null);
				} catch (NullPointerException e) {
					System.err.println("\nFound error in tree "+i+
						" at element "+j+".");
					System.err.println("Dumping keys to a file (error.tree)"
						+" of floats...");
					
					BufferedOutputStream bos = new BufferedOutputStream(
						new FileOutputStream("error.tree"));
					DataOutputStream dos = new DataOutputStream(bos);
					dos.writeInt(count);
					dos.writeInt(j);
					for (int k=0; k<count; k++)
						dos.writeFloat(((Float)keys[k]).floatValue());
					dos.close();
					
					System.err.println(" done.");
					
					failure = true;
					break;
				}
			}
			if (failure) break;
		}
	}
	
	private Entry root;
	private Entry minValue;
	private Entry maxValue;
	
	public void clear() {
		root = null;
	}
	
	public void put(Comparable key, Object content) {
		if (isEmpty()) {
			root = new Entry(key, content, null);
			minValue = root;
			maxValue = root;
		} else {
			Entry e = root.insert(key, content);
			root = root.rootElement();
			if (e.key.compareTo(minValue.key) == -1) minValue = e;
			if (e.key.compareTo(maxValue.key) == +1) maxValue = e;
		}
	}
	
	public Object get(Comparable key) {
		return get(key, false);
	}
	
	public Object get(Comparable key, boolean returnNearestMinor) {
		if (!isEmpty()) {
			if (key.compareTo(minValue.key) == -1 && returnNearestMinor)
				return minValue.getContent();
			if (key.compareTo(maxValue.key) == +1 && returnNearestMinor)
				return maxValue.getContent();
			
			Entry result = root.search(key, returnNearestMinor);
			// moeglicherweise ist dieses Element jetzt aber groesser:
			if (returnNearestMinor && result.key.compareTo(key) == 1) 
				result = result.previousElement();
			return result != null ? result.getContent() : null;
		} else {
			return null;
		}
	}
	
	public int depth() {
		if (!isEmpty()) return root.treeDepth();
		else return 0;
	}
	
	public int size() {
		if (!isEmpty()) return root.treeSize();
		else return 0;
	}
	
	public boolean isEmpty() {
		return root == null;
	}
	
	private class Entry {
		private Comparable key;
		private Object content;
		private Entry parent;
		private Entry left;
		private Entry right;
		private byte balance;
		
		Entry(Comparable k, Object c, Entry p) {
			key = k;
			content = c;
			parent = p;
		}
		
		/**
		 * @returns the new entry
		 */
		Entry insert(Comparable k, Object c) {
			int equal = key.compareTo(k);
			if (equal == -1) { // k groesser
				if (right != null) {
					return right.insert(k,c);
				} else {
					Entry r = new Entry(k,c,this);
					right = r;
					balance += 1;
					if (balance == 1) fixBalance();
					return r;
				}
			} else if (equal == 1) { // k kleiner
				if (left != null) {
					return left.insert(k,c);
				} else {
					Entry l = new Entry(k,c,this);
					left = l;
					balance -= 1;
					if (balance == -1) fixBalance();
					return l;
				}
			} else { // k gleich
				// eigentlich nicht unterstuetzt, aber
				content = c;
				return this;
			}
		} // insert
		
		Entry search(Comparable k, boolean returnLast) {
			int equal = key.compareTo(k);
			if (equal == -1) { // k groesser
				if (right != null) return right.search(k,returnLast);
				else if (returnLast) return this;
				else return null;
			} else if (equal == 1) { // k kleiner
				if (left != null) return left.search(k,returnLast);
				else if (returnLast) return this;
				else return null;
			} else { // k gleich
				return this;
			}
		} // search
		
		int treeDepth() {
			int leftD = left != null ? left.treeDepth() : 0;
			int rightD = right != null ? right.treeDepth() : 0;
			return Math.max(leftD, rightD)+1;
		}
		
		int elementDepth() {
			if (isRoot()) return 0;
			else return parent.elementDepth()+1;
		}
		
		int treeSize() {
			int leftSize = left != null ? left.treeSize() : 0;
			int rightSize = right != null ? right.treeSize() : 0;
			return leftSize+rightSize+1;
		}
		
		Object getContent() {
			return content;
		}
		
		Entry rootElement() {
			if (isRoot()) return this;
			else return parent.rootElement();
		}
		
		// das ist im allgemeinen nicht korrekt, eigentlich doch
		Entry previousElement() {
			if (left != null) {
				Entry e = left;
				while (e.right != null) e = e.right;
				return e;
			} else {
				if (isRoot()) return null;
				Entry e = this;
				while(e.isLeftSon()) e = e.parent;
				return e.parent;
			}
		}
		
		Entry nextElement() {
			if (right != null)  {
				Entry e = right;
				while(e.left != null) e = e.left;
				return e;
			} else  {
				if (isRoot()) return null;
				Entry e = this;
				while(!e.isLeftSon()) e = e.parent;
				return e.parent;
			}
		}
		
		// was ist, wenn parent == null
		boolean isLeftSon() {
			return parent.left == this;
		}
		
		boolean isRoot() {
			return parent == null;
		}
		
		private void fixBalance() {
			if (isRoot()) return; // isTop
			if (isLeftSon()) { // isLeftSon
				if (parent.balance == 1) {
					parent.balance = 0;
				} else if (parent.balance == 0) {
					parent.balance = -1;
					parent.fixBalance();
				} else {
					if (balance == -1) {
						parent.balance = 0;
						parent.rotateRight();
						balance = 0;
					} else {
						Entry p = parent;
						rotateLeft();
						p.rotateRight();
						if (parent.balance == 0) {
							p.balance = 1;
							balance = -1;
						} else if (parent.balance == -1)  {
							balance = 0;
							p.balance = 1;
						} else {
							balance = -1;
							p.balance = 0;
						}
						parent.balance = 0;
					}
				}
			} else { // isRightSon
				if (parent.balance == -1) {
					parent.balance = 0;
				} else if (parent.balance == 0) {
					parent.balance = 1;
					parent.fixBalance();
				} else {
					if (balance == 1) {
						parent.balance = 0;
						parent.rotateLeft();
						balance = 0;
					} else {
						Entry p = parent;
						rotateRight();
						p.rotateLeft();
						if (parent.balance == 0) {
							p.balance = -1;
							balance = 1;
						} else if (parent.balance == -1)  {
							balance = 1;
							p.balance = 0;
						} else {
							balance = 0;
							p.balance = -1;
						}
						parent.balance = 0;
					}
				}
			}
		} // fixBalance
		
		int dirtyCounter = 0;
		private void rotateLeft() {
			// dirty fix for sun jvm bug (?)
			// giving a null pointer exception after the
			// if (right != null)
			if (right != null && right.left != null) {
				try {
					right.left.replaceParent(this);
					dirtyCounter = 0;
				} catch (NullPointerException e) {
					if (dirtyCounter++ > 5) throw new InternalError("Buggy bug.");
					System.out.print("!");
					rotateLeft();
				}
			}
			Entry r = right;
			if (parent != null) parent.replaceChild(this, right);
			right.replaceParent(parent);
			this.replaceRightChild(right.left);
			/*
			if (right != null) {
				right.replaceParent(this);
			}*/
			r.replaceLeftChild(this);
			this.replaceParent(r);
		}
		
		private void rotateRight() {
			Entry l = left;
			if (parent != null) parent.replaceChild(this, left);
			left.replaceParent(parent);
			this.replaceLeftChild(left.right);
			if (left != null) left.replaceParent(this);
			l.replaceRightChild(this);
			this.replaceParent(l);
		}
		
		private void replaceChild(Entry from, Entry to) {
			if (from == left) replaceLeftChild(to);
			else replaceRightChild(to);
		}
		
		private void replaceLeftChild(Entry to) {
			left = to;
		}
		
		private void replaceRightChild(Entry to) {
			right = to;
		}
		
		private void replaceParent(Entry to) {
			parent = to;
		}
	} // Entry
} // AVLTree

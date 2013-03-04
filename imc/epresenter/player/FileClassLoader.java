package imc.epresenter.player;

import java.io.*;
import java.util.*;
import java.util.zip.*;

public class FileClassLoader extends ClassLoader {
	private File[] loadBase;
	
	public FileClassLoader(String classPath) {
		ArrayList bases = new ArrayList(5);
		StringTokenizer st = new StringTokenizer(classPath, File.pathSeparator);
		
		while(st.hasMoreTokens()) {
			File f = new File(st.nextToken());
			if (f.exists()) bases.add(f);
		}
		
		if (bases.size() == 0) throw new IllegalArgumentException(
			"Cannot find any of the ressources in "+classPath+".");
		
		loadBase = new File[bases.size()];
		bases.toArray(loadBase);
	}
	
	
	public Class findClass(String name) {
		// See if type as already been loaded by
        // this class loader:
        Class result = findLoadedClass(name);
        if (result != null)  return result;
        
        // Check with the primordial class loader:
        try {
            result = super.findSystemClass(name);
            return result;
        } catch (ClassNotFoundException e) { }
		
		
		// Load it from the dirs (or files) loadBase:
		try {
			InputStream classStream = null;
			int classLength = 0;
			
			for (int i=0; i<loadBase.length; i++) {
				File base = loadBase[i];
				if (base.isDirectory()) {
					File f = new File(base, name.replace('.', File.separatorChar)+".class");
					if (f.exists()) {
						classStream = new FileInputStream(f);
						classLength = (int)f.length();
						break;
					}
				} else {
					ZipFile f = new ZipFile(base);
					ZipEntry e = f.getEntry(name.replace('.', File.separatorChar)+".class");
					if (e != null) { // e exists
						classStream = f.getInputStream(e);
						classLength = (int)e.getSize();
						break;
					}
				}
			}
			
			if (classLength < 150) 
				System.err.println("Ooops! Class size is low: "+classLength);
						
			
			byte[] input = new byte[classLength];
			int read = 0;
			while(read < classLength)
				read += classStream.read(input, read, input.length-read);
			
         classStream.close();

			return defineClass(name, input, 0, classLength);
		} catch (Exception e) {
			throw new RuntimeException("Unable to load class "+name+" ("+e+").");
		}
	}
	/*
	public boolean isValidPlugin(String name) {
		Class p = Class.forName("SRStreamPlayer");
		try { findPlugin(name); } catch (Exception e) { return false; }
		return true;
	}
	*/
	
	public SRStreamPlayer findPlugin(String name) {
		Class c = findClass(name);
		Object o = null;
		try {
			o = c.newInstance();
		} catch (Exception e) {
			throw new RuntimeException(name+" is not instatiatable ("+e+").");
		}
		if (!(o instanceof SRStreamPlayer)) throw new RuntimeException(
			""+name+" is not valid implementation of SRStreamPlayer.");
		
		return (SRStreamPlayer)o;
	}
}

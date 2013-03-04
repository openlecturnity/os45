package imc.epresenter.player.whiteboard;

import java.awt.*;
import java.io.*;

import imc.epresenter.player.util.SGMLParser;
import imc.epresenter.filesdk.FileResources;

class Character extends VisualComponent {
	private Color color;
	private String character;
	private Font font;
	
	Character(String line, BufferedReader in, FileResources resources) {
		SGMLParser parser = new SGMLParser(line);
		
		character = extractString(parser, "char");
		if (character.equals("")) character = " ";
		String fontName = extractString(parser, "font");
		int size = (int)extractFloat(parser, "size");
		font = matchFont(fontName, null, "regular", "normal", resources);
		
		clip.width = size;
		clip.height = size+size/2;
		clip.x = extractShort(parser, "x");
		clip.y = extractShort(parser, "y")-clip.height;
		
		color = createColor(extractShort(parser, "fcolor"),
							extractString(parser, "rgb"));
		
	}
	
	void paint(Graphics g, int width, int height, double scale) {
		g.setColor(color);
		if (scale > 1.01 || scale < 0.99)
			g.setFont(font.deriveFont((float)(scale*font.getSize())));
		else g.setFont(font);
		g.drawString(character, 
			(int)(scale*clip.x), (int)(scale*(clip.y+clip.height)));
	}
}
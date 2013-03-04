package imc.lecturnity.util.ui;

import javax.swing.JTextArea;
import java.awt.Dimension;
import java.awt.Point;
import java.awt.Font;

import imc.lecturnity.util.wizards.Wizard;

public class LazyTextArea extends JTextArea
{
   private static boolean USE_CUSTOM_COLORS = false;
   
   public LazyTextArea(String content, Point location, 
                         Dimension size, Font font)
   {
      super(content);
      //if (imc.lecturnity.converter.ConverterWizard.USE_CUSTOM_COLORS)
      if (USE_CUSTOM_COLORS)
         setBackground(Wizard.BG_COLOR);
      else
         setOpaque(false);
      setLocation(location);
      setSize(size);
      setLineWrap(true);
      setWrapStyleWord(true);
      setFont(font);
      setEditable(false);
   }
}


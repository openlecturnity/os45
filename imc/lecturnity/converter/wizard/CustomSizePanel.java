package imc.lecturnity.converter.wizard;

import java.awt.Dimension;
import java.awt.Font;
import java.awt.BorderLayout;
import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;
import java.awt.event.FocusListener;
import java.awt.event.FocusEvent;

import javax.swing.*;

// From the Java (TM) tutorial for the subclass 'WholeNumberField'
import javax.swing.text.*; 
import javax.swing.event.*;
import java.awt.Toolkit;
//import java.text.NumberFormat;
import java.text.ParseException;
import java.util.Locale;

import imc.epresenter.filesdk.util.Localizer;

/**
 * The CustomSizePanel includes two TextFields (for the input of width and height) 
 * and a CheckBox to set a flag, if the aspect ratio should be considered. 
 * There are different constructors to create a CustomSizePanel: 
 * - CustomSizePanel(), which creates a 300x22 sized panel with initial values for width (0) and height (0). 
 * - CustomSizePanel(int, int), which creates a custom sized panel with initial values for width (0) and height (0). 
 * - CustomSizePanel(int, int, Dimension), which creates a custom sized panel with given values for width and height. 
 * - CustomSizePanel(int, int, Dimension, char), which creates a custom sized panel with given values for width and height 
 * and a mnemonic character for the CheckBox. 
 * The TextFields included work similar to a SpinBox: 
 * they accept only numbers in a given range that should not exceed 4 digits ==> [0..9999].
 */
class CustomSizePanel extends JPanel
{
   private static String ASPECT_RATIO = "[!]";
   private static char   MNEM_ASPECT = '?';

   static
   {
      try
      {
         Localizer l = new Localizer
            ("/imc/lecturnity/converter/wizard/CustomSizePanel_",
             "en");
         ASPECT_RATIO = l.getLocalized("ASPECT_RATIO");
         MNEM_ASPECT = l.getLocalized("MNEM_ASPECT").charAt(0);
      }
      catch (Exception e)
      {
         JOptionPane.showMessageDialog(null, "Could not open CustomSizePanel locale database!\n"+
                                       e.toString(),
                                       "Error", JOptionPane.ERROR_MESSAGE);
         System.exit(1);
      }
   }

   private Dimension actualSize_;
   private Dimension minimumSize_;
   private Dimension maximumSize_;

   private char mnem_;
   private double aspectRatio_;
   private boolean keepAspectRatio_;
   private boolean aspectRatioIsEnabled_;
   private boolean lastChangedIsWidthField_;
   private boolean throwChangeEvent_;

   private JLabel xLabel_;
   private WholeNumberField customWidthTextField_;
   private WholeNumberField customHeightTextField_;
   private JCheckBox useAspectRatioBox_;

   private MyDocumentListener mdl_;
   private ChangeListener chl_;
   

   public CustomSizePanel()
   {
      this(300, 22);
   }
   
   public CustomSizePanel(int panelWidth, int panelHeight)
   {
      this(panelWidth, panelHeight, new Dimension(0, 0));
   }
   
   public CustomSizePanel(int panelWidth, int panelHeight, Dimension actualSize)
   {
      this(panelWidth, panelHeight, actualSize, MNEM_ASPECT);
   }
   
   public CustomSizePanel(int panelWidth, int panelHeight, Dimension actualSize, char mnem)
   {
      this(panelWidth, panelHeight, actualSize, MNEM_ASPECT, false);
   }

   public CustomSizePanel(int panelWidth, int panelHeight, Dimension actualSize, char mnem, boolean bThrowChangeEvent)
   {
      super();

      if (panelWidth < 300)
         panelWidth = 300;

      super.setSize(panelWidth, panelHeight);
      
      mnem_ = mnem;
      actualSize_ = actualSize;
      minimumSize_ = new Dimension(0, 0);
      maximumSize_ = new Dimension(9999, 9999);
      
      keepAspectRatio_ = true;
      aspectRatioIsEnabled_ = true;
      lastChangedIsWidthField_ = false;
      throwChangeEvent_ = bThrowChangeEvent;

      if (actualSize_.height != 0)
         aspectRatio_ = (double)actualSize_.width / (double)actualSize_.height;
      else
         aspectRatio_ = 1.0;

      initGui();
   }

   public void addChangeListener(ChangeListener cl)
   {
      chl_ = cl;
   }

   private CustomSizePanel getThisPanel()
   {
      return this;
   }

   private void initGui()
   {
      int x1 = 0;
      int y  = 0;
      int s1 = 40;
      int s2 = 200;

      this.setLayout(new BorderLayout());
      JPanel panel = new JPanel();
      panel.setLayout(null);
      this.add(panel, BorderLayout.CENTER);

      Font descFont = imc.lecturnity.converter.ConverterWizard.createDefaultFont(Font.PLAIN, 12);
      CheckBoxListener cbl  = new CheckBoxListener();
      TextFieldListener tfl = new TextFieldListener();
      mdl_ = new MyDocumentListener();

      customWidthTextField_ = new WholeNumberField(actualSize_.width, 4);
      customWidthTextField_.setFont(descFont);
      customWidthTextField_.setHorizontalAlignment(JTextField.RIGHT);
      customWidthTextField_.setSize(s1, 20);
      customWidthTextField_.setLocation(x1, y);
      customWidthTextField_.addFocusListener(tfl);
      customWidthTextField_.getDocument().addDocumentListener(mdl_);
      customWidthTextField_.getDocument().putProperty("name", "Width Field");
      panel.add(customWidthTextField_);

      xLabel_ = new JLabel("x");
      xLabel_.setLocation(x1+s1+5, y-1);
      xLabel_.setSize(8, 22);
      xLabel_.setFont(descFont);
      panel.add(xLabel_);

      customHeightTextField_ = new WholeNumberField(actualSize_.height, 4);
      customHeightTextField_.setFont(descFont);
      customHeightTextField_.setHorizontalAlignment(JTextField.RIGHT);
      customHeightTextField_.setSize(s1, 20);
      customHeightTextField_.setLocation(x1+s1+15, y);
      customHeightTextField_.addFocusListener(tfl);
      customHeightTextField_.getDocument().addDocumentListener(mdl_);
      customHeightTextField_.getDocument().putProperty("name", "Height Field");
      panel.add(customHeightTextField_);

      useAspectRatioBox_ = new JCheckBox(ASPECT_RATIO, keepAspectRatio_);
      useAspectRatioBox_.setFont(descFont);
      useAspectRatioBox_.setSize(s2, 22);
      useAspectRatioBox_.setLocation(this.getSize().width-s2, y);
      useAspectRatioBox_.setMnemonic(mnem_);
      useAspectRatioBox_.addActionListener(cbl);
      panel.add(useAspectRatioBox_);
      
   }
      
   private void updateWidthField()
   {
      int h = customHeightTextField_.getValue();
      int w = (int)((aspectRatio_ * (double)h) + 0.5);
      customWidthTextField_.setValueWithoutListener(w, mdl_);
   }
   
   private void updateHeightField()
   {
      int w = customWidthTextField_.getValue();
      int h = (int)((aspectRatio_!=0.0) ? (((double)w / aspectRatio_) + 0.5) : 0);
      customHeightTextField_.setValueWithoutListener(h, mdl_);
   }
   
   public void setEnabled(boolean isEnabled)
   {
      customWidthTextField_.setEnabled(isEnabled);
      customHeightTextField_.setEnabled(isEnabled);
      xLabel_.setEnabled(isEnabled);
      useAspectRatioBox_.setEnabled(isEnabled && aspectRatioIsEnabled_);
   }
   
   public void setEnabledAspectRatioBox(boolean isEnabled)
   {
      aspectRatioIsEnabled_ = isEnabled;
   }
   
   public void configureAspectRatioBoxVisibility(boolean bEnabled, boolean bVisible)
   {
      useAspectRatioBox_.setEnabled(bEnabled);
      useAspectRatioBox_.setVisible(bVisible);
   }
   
   public void setSize(int width, int height)
   {
      // The Panel with the two JTextFields and the JCheckBox need at least 
      // a minimum size of 300x22
      if (width < 300)
         width = 300;
      if (height < 22)
         height = 22;

      super.setSize(width, height);

      // Align the JCheckBox to the right side
      int y  = 0;
      int s2 = 200;
      useAspectRatioBox_.setLocation(this.getSize().width-s2, y);
   }
   
   public void setUseAspectRatio(boolean isSelected)
   {
      useAspectRatioBox_.setSelected(isSelected);
   }
   
   public boolean getUseAspectRatio()
   {
      return useAspectRatioBox_.isSelected();
   }
   
   public void setMinimumSize(Dimension minimumSize)
   {
      minimumSize_ = minimumSize;
      customWidthTextField_.setRangeMin(minimumSize_.width);
      customHeightTextField_.setRangeMin(minimumSize_.height);
   }
   
   public Dimension getMinimumSize()
   {
      return minimumSize_;
   }
   
   public void setMaximumSize(Dimension maximumSize)
   {
      maximumSize_ = maximumSize;
      customWidthTextField_.setRangeMax(maximumSize_.width);
      customHeightTextField_.setRangeMax(maximumSize_.height);
   }
   
   public Dimension getMaximumSize()
   {
      return maximumSize_;
   }
   
   public void setActualSize(int width, int height)
   {
      setActualSize(new Dimension(width, height));
   }
   
   public void setActualSize(Dimension actualSize)
   {
      actualSize_ = actualSize;
      customWidthTextField_.setValueWithoutListener(actualSize_.width, mdl_);
      customHeightTextField_.setValueWithoutListener(actualSize_.height, mdl_);
   }
   
   public Dimension getActualSize()
   {
      actualSize_.width = customWidthTextField_.getValue();
      actualSize_.height = customHeightTextField_.getValue();
      return actualSize_;
   }
   
   public void setAspectRatio(double aspectRatio)
   {
      aspectRatio_ = aspectRatio;
   }
   
   public double getAspectRatio()
   {
      return aspectRatio_;
   }
   

   private class CheckBoxListener implements ActionListener
   {
      public void actionPerformed(ActionEvent e)
      {
         if (lastChangedIsWidthField_)
         {
            updateHeightField();
         }
         else
         {
            updateWidthField();
         }
      }
   }

   private class TextFieldListener implements FocusListener
   {
      public void focusGained(FocusEvent e)
      {
         // do nothing special
      }

      public void focusLost(FocusEvent e)
      {
         // Check for valid values in the TextFields,  
         // especially avoid something like ""
         int w = customWidthTextField_.getValue();
         if (w < customWidthTextField_.getRangeMin())
         {
            w = customWidthTextField_.getRangeMin();
            customWidthTextField_.setValue(w);
         }
         if (w > customWidthTextField_.getRangeMax())
         {
            w = customWidthTextField_.getRangeMax();
            customWidthTextField_.setValue(w);
         }

         int h = customHeightTextField_.getValue();
         if (h < customHeightTextField_.getRangeMin())
         {
            h = customHeightTextField_.getRangeMin();
            customHeightTextField_.setValue(h);
         }
         if (h > customHeightTextField_.getRangeMax())
         {
            h = customHeightTextField_.getRangeMax();
            customHeightTextField_.setValue(h);
         }
      }
   }


   // Copied and modified from the Java (TM) tutorial
   private class MyDocumentListener implements DocumentListener 
   {
      public void insertUpdate(DocumentEvent e) 
      {
         calculateValue(e);
         if (throwChangeEvent_)
            chl_.stateChanged(new ChangeEvent(getThisPanel()));
      }

      public void removeUpdate(DocumentEvent e) 
      {
         calculateValue(e);
         if (throwChangeEvent_)
            chl_.stateChanged(new ChangeEvent(getThisPanel()));
      }
      
      public void changedUpdate(DocumentEvent e) 
      {
         // we won't ever get this with a PlainDocument
         if (throwChangeEvent_)
            chl_.stateChanged(new ChangeEvent(getThisPanel()));
      }
      
      private void calculateValue(DocumentEvent e) 
      {
         Document whatsup = e.getDocument();
 
         if (whatsup.getProperty("name").equals("Width Field"))
         {
            lastChangedIsWidthField_ = true;

            if (useAspectRatioBox_.isSelected())
               updateHeightField();
         }

         if (whatsup.getProperty("name").equals("Height Field"))
         {
            lastChangedIsWidthField_ = false;

            if (useAspectRatioBox_.isSelected())
               updateWidthField();
         }
      }
   }    


   // Copied and modified from the Java (TM) tutorial
   public class WholeNumberField extends JTextField 
   {
      private Toolkit toolkit;
      // We don't need 'integerFormatter' here, because there is 
      // no reason to format '1600' as '1,600' (en) or '1.600' (ge)
      //private NumberFormat integerFormatter; 
      // New: Range of possible values
      private int minValue_;
      private int maxValue_;
      private int columns_;
       
      public WholeNumberField(int value, int columns) 
      {
         super(columns);
         // Set default values for range
         columns_ = columns;
         minValue_ = 0;
         maxValue_ = (int)Math.pow(10, columns_) - 1;

         toolkit = Toolkit.getDefaultToolkit();
         //integerFormatter = NumberFormat.getNumberInstance(Locale.US);
         //integerFormatter.setParseIntegerOnly(true);
         setValue(value);
      }

      public int getValue() 
      {
         int retVal = 0;
         //try 
         //{
         //   retVal = integerFormatter.parse(getText()).intValue();
         //} 
         //catch (ParseException e) 
         //{
         //   // This should never happen because insertString allows
         //   // only properly formatted data to get in the field.
         //   toolkit.beep();
         //}
         if (getText().length() > 0)
            retVal = Integer.parseInt(getText());
         else
            retVal = 0;
         
         return retVal;
      }

      public void setValueWithoutListener(int value, MyDocumentListener mdl)
      {
         // Deactivate Document Listener temporarily while changing the value
         this.getDocument().removeDocumentListener(mdl);
         this.setValue(value);
         this.getDocument().addDocumentListener(mdl);
      }
      
      public void setValue(int value) 
      {
         // Check the range
         if (value < minValue_)
         {
            toolkit.beep();
            System.err.println("Input is lower than valid range");
            value = minValue_;
         }
         if (value > maxValue_)
         {
            toolkit.beep();
            System.err.println("Input is higher than valid range");
            value = minValue_;
         }
         //setText(integerFormatter.format(value));
         setText(Integer.toString(value));
      }

      public void setRangeMin(int value)
      {
         minValue_ = value;
      }

      public void setRangeMax(int value)
      {
         maxValue_ = value;
      }

      public int getRangeMin()
      {
         return minValue_;
      }
      
      public int getRangeMax()
      {
         return maxValue_;
      }
      
      protected Document createDefaultModel() 
      {
         return new WholeNumberDocument();
      }


      protected class WholeNumberDocument extends PlainDocument 
      {
         public void insertString(int offs, 
                                  String str,
                                  AttributeSet a) 
                 throws BadLocationException 
         {
            // Check, if number of columns is exceeded
            int sLength = super.getLength() + str.length();
            if (sLength > columns_)
            {
               toolkit.beep();
               return;
            }
            char[] source = str.toCharArray();
            char[] result = new char[source.length];
            int j = 0;
            for (int i = 0; i < result.length; i++) 
            {
               if (Character.isDigit(source[i]))
                  result[j++] = source[i];
               else 
               {
                  toolkit.beep();
                  //System.err.println("insertString: " + source[i]);
               }
            }
            super.insertString(offs, new String(result, 0, j), a);
            
            checkDocumentForValidRange();
         }
         
         public void remove(int offs, 
                            int len) 
                 throws BadLocationException 
         {
            // do nothing special
            super.remove(offs, len);
         }
         
         private void checkDocumentForValidRange()
         {
            int value;
            int strLength = super.getLength();
            String strValue = "";
            try
            {
               strValue = super.getText(0, strLength);
            }
            catch (BadLocationException e)
            {
               System.err.println("Unable to get Text from Document!");
            }
               
            boolean isOutOfRange = false;

            if (strLength > 0)
            {
               value = Integer.parseInt(strValue);
            }
            else
            {
               //System.err.println("Null String");
               value = minValue_;
               isOutOfRange = true;
            }
            
            if (value < minValue_)
            {
               value = minValue_;
               isOutOfRange = true;
            }
            if (value > maxValue_)
            {
               value = maxValue_;
               isOutOfRange = true;
            }

            if (isOutOfRange)
            {
               //System.err.println("isOutOfRange");
               toolkit.beep();
               try
               {
                  super.remove(0, strLength);
               }
               catch (BadLocationException e)
               {
                  System.err.println("Unable to remove Text from Document!");
               }

               strValue = Integer.toString(value);
               try
               {
                  super.insertString(0, strValue, null);
               }
               catch (BadLocationException e)
               {
                  System.err.println("Unable to insert Text into Document!");
               }
            }
         }
      }
   }
}
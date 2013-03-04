package imc.epresenter.player;

import java.awt.*;
import java.awt.event.*;
import java.awt.image.*;
import java.io.*;
import java.net.*;
import java.util.HashMap;
import javax.swing.*;
import javax.swing.border.Border;

import imc.epresenter.filesdk.*;
import imc.epresenter.player.util.MouseAwarePanel;
import imc.epresenter.player.util.TimeFormat;

public class ThumbnailHelper implements SRStreamPlayer, ActionListener, MouseListener
{
   private Coordinator coordinator_;
   private FileResources resources_;
   private JComponent visualComponent_;
   private JScrollPane scrollPane_;
   private JViewport viewPort_;
   private JScrollBar scrollBar_;
   private Color backColor_;
   private Color overColor_;
   private Color selectColor_;
   private Metadata metadata_;
   private boolean metadataAvailable_;
   private HashMap buttonHash_;
   private JButton[] buttonArray_;
   private JButton lastHighlightedButton_;
   private int[] startPageTimes_;

   
   public void setStartOffset(int millis)
   {
   }

	public void init(FileResources resources, String[] args, Coordinator c)
   {
      
      coordinator_ = c;
      resources_ = resources;

      try
      {
         metadata_ = resources_.getMetadata(args[0]);
         metadataAvailable_ = true;
      }
      catch (IOException ioe)
      {
         Manager.showError(Manager.getLocalized("metaNotFound")+" "+args[0], Manager.WARNING, ioe);
         metadataAvailable_ = false;
      }

      //backColor_ = new JButton().getBackground();//new Color(0xd5d5d5);
      backColor_ = Color.white;
      //overColor_ = new Color(0xf7c9a7); // light orange
      overColor_ = new Color(0xA6C7F5); // light blue
      //selectColor_ = new Color(0xf88531); // darker orange
      selectColor_ = new Color(0x3084F7); // darker blue


      
      if (metadataAvailable_)
      {
         //BufferedImage[] thumbs = new BufferedImage[5];
         ThumbnailData[] thumbs = metadata_.thumbnails;
         buttonArray_ = new JButton[thumbs.length];
         startPageTimes_ = new int[thumbs.length];
         buttonHash_ = new HashMap();

         TimeFormat formatter = new TimeFormat();

         JPanel container = new JPanel();//new GridLayout(0,1));
         BoxLayout lay = new BoxLayout(container, BoxLayout.Y_AXIS);
         container.setLayout(lay);
         container.setOpaque(false);
         //container.setBorder(BorderFactory.createEmptyBorder(0,6,0,6));
         //container.setBorder(BorderFactory.createLineBorder(Color.black, 1));
         
         int maximumChapterLevel = 0;
         for (int i=0; i<thumbs.length; i++)
            maximumChapterLevel = Math.max(thumbs[i].chapterLevel, maximumChapterLevel);
         int spacerStep = 20;
         Border buttonBorder = BorderFactory.createEmptyBorder(0,maximumChapterLevel*spacerStep,0,0);
         //buttonBorder = BorderFactory.createCompoundBorder(
         //   buttonBorder, BorderFactory.createEtchedBorder());

         int[] thumbnailSizes = resources_.getThumbnailSizes();

         for (int i=0; i<thumbs.length; i++)
         {
            String fileName = thumbs[i].thumbnailResource;
            if (thumbnailSizes != null && thumbnailSizes.length > 0)
               fileName = thumbnailSizes[0]+"_"+fileName;
            Icon icon = createIcon(fileName);
            
            // BUGFIX 5505: handle missing "_internal_thumbinfo"
            if ( i==0 && icon == null && thumbnailSizes == null) {
               // no icons and "_internal_thumbinfo" not found: try with default values
               thumbnailSizes = new int[3];
               thumbnailSizes[0] = 100;
               thumbnailSizes[1] = 150;
               thumbnailSizes[2] = 200;
               // try to create icon
               fileName = thumbnailSizes[0]+"_"+fileName;
               icon = createIcon(fileName);
               if ( icon == null )
                  // still not found: reset
                  thumbnailSizes = null;
            }

            JButton b = new JButton(icon);
            b.setDisabledIcon(icon);
            b.setOpaque(false);
            b.addActionListener(this);
            b.addMouseListener(this);
            b.setFocusPainted(false);
            int minutes = formatter.getMinutes((int)thumbs[i].beginTimestamp);
            int seconds = formatter.getSeconds((int)thumbs[i].beginTimestamp);
            String secondString = ""+(seconds-minutes*60);
            if (secondString.length() == 1)
               secondString = "0"+secondString;
            String buttonText = " "+minutes+":"+secondString;
            b.setText(buttonText);
            // it is by default to the right of the icon, so only vertical pos must be corrected
            b.setVerticalTextPosition(JButton.BOTTOM); 
            b.setBorder(buttonBorder);
            b.setContentAreaFilled(false);
            
            buttonHash_.put(b, new Integer((int)thumbs[i].beginTimestamp));
            buttonArray_[i] = b;
            startPageTimes_[i] = (int)thumbs[i].beginTimestamp;
            
            
            JLabel l = new JLabel(thumbs[i].title);
            String s = thumbs[i].title;
                        
            Dimension d232 = l.getPreferredSize();
            if (d232.width > 220)
               d232.width = 220;
            l.setPreferredSize(d232);
            l.setToolTipText(thumbs[i].title);
            l.setOpaque(false);
            l.setBorder(buttonBorder);
            //l.setAlignmentX(0.5f);
            

            //JPanel p = new MouseAwarePanel(backColor_, overColor_, selectColor_);
            JPanel p = new JPanel();
            BoxLayout layout = new BoxLayout(p, BoxLayout.Y_AXIS);
            p.setLayout(layout);
            if (thumbs[i].displayTitle)
            {
               int offset = thumbs[i].titleOffset;
               if (offset < 0) offset = 0;
               int level = thumbs[i].chapterLevel;
               String[] titles = thumbs[i].chapterTitles;
               for (int t=offset; t<titles.length; t++)
               {
                  JLabel l2 = new JLabel(titles[t]);
                  Dimension d233 = l2.getPreferredSize();
                  if (d233.width > 220)
                     d233.width = 220;
                  l2.setPreferredSize(d233);
                  l2.setToolTipText(titles[t]);
                  l2.setOpaque(false);
                  l2.setFont(l2.getFont().deriveFont(Font.BOLD));
                  l2.setBorder(BorderFactory.createEmptyBorder(0, t*spacerStep, 0, 0)); 
                  p.add(l2);
               }
            }
            p.add(b);
            p.add(l);
            
            p.setBorder(BorderFactory.createEmptyBorder(6,6,6,6));

            // so that the BoxLayout can assign additional width to this panel
            p.setMaximumSize(new Dimension(Short.MAX_VALUE,
                                           Short.MAX_VALUE));//p.getMaximumSize().height));

            p.setBackground(backColor_);
            container.add(p);
         }

         JPanel containerOuter = new JPanel(new BorderLayout());
         containerOuter.add("North", container);
         containerOuter.setOpaque(true);
         containerOuter.setBackground(backColor_);
         
         scrollPane_ = new JScrollPane(containerOuter,
                                       JScrollPane.VERTICAL_SCROLLBAR_AS_NEEDED,
                                       JScrollPane.HORIZONTAL_SCROLLBAR_NEVER);
         scrollBar_ = scrollPane_.getVerticalScrollBar();
         //scrollBar_.setBorder(BorderFactory.createLineBorder(Color.black, 1));
         viewPort_ = scrollPane_.getViewport();
         viewPort_.setScrollMode(JViewport.SIMPLE_SCROLL_MODE);

         correctSizes(scrollPane_);

         JPanel comboPanel = null;
         if (thumbnailSizes != null && thumbnailSizes.length > 1)
         {
            JComboBox cb = null;
            String[] elements = new String[thumbnailSizes.length];
            for (int e=0; e<elements.length; e++)
               elements[e] = ""+thumbnailSizes[e];
            cb = new JComboBox(elements);
            cb.addItemListener(new ItemListener() {
               public void itemStateChanged(ItemEvent e)
               {
                  if (e.getStateChange() == ItemEvent.SELECTED)
                  {
                     int newSize = Integer.parseInt((String)e.getItem());
                     changeIconSize(newSize);
                  }
               }
            });
            
            comboPanel = new JPanel();
            comboPanel.setOpaque(false);
            JLabel l7 = new JLabel(Manager.getLocalized("size")+":");
            comboPanel.add(l7);
            comboPanel.add(cb);
            JLabel l8 = new JLabel("px");
            comboPanel.add(l8);
         }
         
         
         visualComponent_ = new JPanel(new BorderLayout());
         //visualComponent_.setBackground(backColor_);
         visualComponent_.add("Center", scrollPane_);
         if (comboPanel != null)
            visualComponent_.add("North", comboPanel);
         
         Dimension d34 = visualComponent_.getPreferredSize();
         if (d34.height > 120)
            d34.height = 120;
         visualComponent_.setPreferredSize(d34);


         setMediaTime(0, null);
      }
      else // metadata not available
      {
         visualComponent_ = new JPanel();
         //visualComponent_.setBackground(backColor_);
      }
   }
   
   private void changeFontBatang(JLabel label)
   {
      String text = label.getText();
      
      boolean bContainsJapanese = false;
      for (int h=0; h<text.length(); ++h)
      {
         int iCharValue = (int)text.charAt(h);
         if (iCharValue > 0x3030) // rather arbitrary value but comes close
            bContainsJapanese = true;
      }
                  
      Font fontParent = label.getFont();
      Font fontJapanese = new Font("Batang", Font.BOLD, 16);//(int)fontParent.getSize());
      if (bContainsJapanese)
         label.setFont(fontJapanese);
                  
   }

	public void enableVisualComponents(boolean bEnable)
   {
      for (int i=0; i<metadata_.thumbnails.length; i++)
         buttonArray_[i].setEnabled(bEnable);
   }

	public Component[] getVisualComponents()
   {
      return new Component[] { visualComponent_ };
   }

	public String getTitle()
   {
      return "ThumbnailHelper";
   }

	public String getDescription()
   {
      return "Display of slide thumbnails.";
   }

	public void setMediaTime(int millis, EventInfo info)
   {
      if (metadataAvailable_ && startPageTimes_.length > 0)
      {
         JViewport vp = scrollPane_.getViewport();

         int buttonIdx = 0;
         if (millis > startPageTimes_[0])
         {
            for (int i=startPageTimes_.length-1; i>=0; i--)
            {
               if (millis >= startPageTimes_[i])
               {
                  buttonIdx = i;
                  break;
               }
            }
         }

         
         JButton b = buttonArray_[buttonIdx];
         if (b != lastHighlightedButton_)
         {
            Container cOld = SwingUtilities.getAncestorOfClass(JPanel.class, lastHighlightedButton_);
            Container cNew = SwingUtilities.getAncestorOfClass(JPanel.class, b);
            if (cOld != null)
               cOld.setBackground(backColor_);
            if (cNew != null)
               cNew.setBackground(overColor_);

            Dimension d = b.getSize();
            Point p = new Point(d.width/2, d.height/2);
            p = SwingUtilities.convertPoint(b, p, scrollPane_);
            p.y += vp.getViewPosition().y;
         
            d = vp.getExtentSize();
            p.x = 0;
            p.y -= d.height/2;
            if (p.y < 0)
               p.y = 0;
            else if (p.y > vp.getViewSize().height-vp.getExtentSize().height)
               p.y = vp.getViewSize().height-vp.getExtentSize().height;

            vp.setViewPosition(p);

            lastHighlightedButton_ = b;
         }
         
      }
   }

	public void start(EventInfo info)
   {
      //scrollBar_.setEnabled(false);
   }

	public void stop(EventInfo info)
   {
      //scrollBar_.setEnabled(true);
   }

	public void pause(EventInfo info){}

	public int getMediaTime(){ return 0; }

	public int getDuration(){ return 0; }

   public void setDuration(int millis){}

	public void setInfo(String key, String value){}

   
   public void close(EventInfo info) 
   {
      coordinator_ = null;
   }
 

   public void actionPerformed(ActionEvent e)
   {
      int millis = ((Integer)buttonHash_.get(e.getSource())).intValue();
      coordinator_.requestTime(millis, this);
   }

   public void mouseEntered(MouseEvent e)
   {
      e.getComponent().setCursor(Cursor.getPredefinedCursor(Cursor.HAND_CURSOR));
   }

   public void mouseExited(MouseEvent e)
   {
      e.getComponent().setCursor(Cursor.getPredefinedCursor(Cursor.DEFAULT_CURSOR));
   }
   
   public void mouseClicked(MouseEvent e){}
   public void mousePressed(MouseEvent e){}
   public void mouseReleased(MouseEvent e){}


   private Icon createIcon(String fileName)
   {
      ImageIcon icon = null;
      try
      {
         int fileSize = (int)resources_.getFileSize(fileName);
         if (fileSize > 0)
         {
            byte[] imageData = new byte[fileSize];
            InputStream inputStream = resources_.createInputStream(fileName);
            int read = 0;
            while (read < fileSize)
               read += inputStream.read(imageData, read, fileSize-read);

            inputStream.close();

            icon = new ImageIcon(imageData);
         }
         else 
         {
            /*
            BufferedImage wimmy = new BufferedImage(100, 80, BufferedImage.TYPE_INT_ARGB);
            Graphics g = wimmy.createGraphics();
            g.setColor(Color.black);
            g.fillRect(0,0,100,80);
            
            icon = new ImageIcon(wimmy);
            */

         }
      }
      catch (IOException ioe)
      {
         ioe.printStackTrace();
      }

      return icon;
   }

   private void changeIconSize(int size)
   {
      ThumbnailData[] thumbs = metadata_.thumbnails;
      for (int i=0; i<thumbs.length; i++)
      {
         String fileName = size+"_"+thumbs[i].thumbnailResource;
         buttonArray_[i].setIcon(createIcon(fileName));
         buttonArray_[i].setDisabledIcon(createIcon(fileName));
      }

      correctSizes(scrollPane_);

      JFrame f = (JFrame)SwingUtilities.windowForComponent(visualComponent_);
      ((JPanel)f.getContentPane()).validate();
   }

  
   private void correctSizes(JScrollPane sp)
   {
      JScrollBar scrollBar = sp.getVerticalScrollBar();
      JViewport viewPort = sp.getViewport();
      
      Dimension vdMin = viewPort.getMinimumSize();
      vdMin.height = 100;
      viewPort.setMinimumSize(vdMin);
      Dimension vdPref = viewPort.getPreferredSize();
      vdPref.height = 100;
      vdPref.width += scrollBar.getPreferredSize().width;
      sp.setPreferredSize(vdPref);
   }
}
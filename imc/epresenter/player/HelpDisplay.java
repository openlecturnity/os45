package imc.epresenter.player;

import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.File;
import java.io.IOException;
import java.io.FileNotFoundException;
import java.net.URL;
import java.net.MalformedURLException;
import java.text.*;
import java.util.*;
import javax.swing.*;
import javax.swing.border.Border;
import javax.swing.event.*;
import javax.swing.text.AttributeSet;
import javax.swing.text.Element;
import javax.swing.text.html.*;

import imc.lecturnity.util.NativeUtils;
import imc.lecturnity.util.ColorManager;
import imc.epresenter.player.util.VariablePicturedPanel;


public class HelpDisplay extends JFrame implements HyperlinkListener, ActionListener
{
   private GlobalMasterAndUI master_;
   private JEditorPane helpPane_;
   private JLabel statusL_;
   private JTextField locationF_;
   private JButton backwardB_;
   private JButton forwardB_;
   private JScrollPane scrollPane_;
   private LinkedList recentDocuments_;
   private LinkedList futureDocuments_;
   private URL currentURL_;

   public HelpDisplay(String helpPath, GlobalMasterAndUI master, String title)
   {
      master_ = master;

      if (title != null)
         setTitle(title);
      //else
      //   setTitle(Manager.getLocalized("help"));

      recentDocuments_ = new LinkedList();
      futureDocuments_ = new LinkedList();

      helpPane_ = new JEditorPane();
      helpPane_.setEditable(false);
      try
      {
         URL helpURL = new File(helpPath).toURL();
         loadPage(helpURL);
         
         HTMLDocument d = (HTMLDocument)helpPane_.getDocument();
         /*
         Element e = d.getElement(d.getDefaultRootElement(), HTML.Tag.TITLE, null);
         System.out.println(e);
         */
         
         //goDownElements(d.getDefaultRootElement(), "");
         
         /*
         HTMLDocument.Iterator iter = d.getIterator(HTML.Tag.TITLE);
         while(iter != null && iter.isValid())
         {
            AttributeSet as = iter.getAttributes();
            Enumeration keys = as.getAttributeNames();
            while(keys.hasMoreElements())
            {
               Object key = keys.nextElement();
               System.out.println(key+" "+as.getAttribute(key));
            }
            iter.next();
         }
         */
         /*
         Dictionary d = ((AbstractDocument)helpPane_.getDocument()).getDocumentProperties();
         Enumeration keys = d.keys();
         while(keys.hasMoreElements())
         {
            Object key = keys.nextElement();
            System.out.println(key+" "+d.get(key));
         }
         */
      }
      catch (IOException ioe)
      {
         ioe.printStackTrace();
      }

      helpPane_.addHyperlinkListener(this);

      Border border = BorderFactory.createEmptyBorder(1,2,1,2);

      statusL_ = new JLabel();
      backwardB_ = new JButton();
      backwardB_.setOpaque(false);
      //backwardB_.setBackground(Color.white);
      setButtonImages(backwardB_, "button_zurueck_w");
      backwardB_.setBorder(border);
      backwardB_.setFocusPainted(false);
      backwardB_.addActionListener(this);
      backwardB_.setToolTipText(Manager.getLocalized("pageBackward"));
      forwardB_ = new JButton();
      forwardB_.setOpaque(false);
      //forwardB_.setBackground(Color.white);
      setButtonImages(forwardB_, "button_vor_w");
      forwardB_.setBorder(border);
      forwardB_.setFocusPainted(false);
      forwardB_.addActionListener(this);
      forwardB_.setToolTipText(Manager.getLocalized("pageForward"));
      locationF_ = new JTextField(helpPath);
      locationF_.addActionListener(this);
      locationF_.setPreferredSize(
         new Dimension(24, locationF_.getPreferredSize().height));
      
      //locationF_.setBorder(BorderFactory.createEmptyBorder(5,3,5,5));
      

      //JPanel buttonPanel = new VariablePicturedPanel(
      //   createIcon("/imc/epresenter/player/icons2/backchen_left.gif").getImage(),
      //   createIcon("/imc/epresenter/player/icons2/backchen_middle.gif").getImage(),
      //   createIcon("/imc/epresenter/player/icons2/backchen_right.gif").getImage());
      //buttonPanel.setLayout(new GridLayout(1,0));
      JPanel buttonPanel = new JPanel(new GridLayout(1,0));
      buttonPanel.setBackground(ColorManager.greyResource);
      buttonPanel.setOpaque(true);
      buttonPanel.add(backwardB_);
      buttonPanel.add(forwardB_);
      //buttonPanel.setBorder(BorderFactory.createEmptyBorder(5,5,5,2));



      
      JPanel controlPanel = new JPanel(new BorderLayout());
      controlPanel.add("West", buttonPanel);
      controlPanel.add("Center", locationF_);
      


      scrollPane_ = new JScrollPane(helpPane_);
      Dimension prefSize = new Dimension(600,680);
      if (helpPath.length() > 12)
      {
         int idx = helpPath.lastIndexOf(".");
         if (idx > 7)
         {
            String size = helpPath.substring(idx-7, idx);
            idx = size.indexOf("x");
            if (idx == 3)
            {
               int w = 600, h = 680;
               try
               {
                  w = Integer.parseInt(size.substring(0,3));
                  h = Integer.parseInt(size.substring(4,7));
               }
               catch (NumberFormatException exc)
               {
               }
               prefSize = new Dimension(w, h);

            }

         }
      }
      scrollPane_.setPreferredSize(prefSize);

      JPanel statusPanel = new JPanel(new BorderLayout());
      statusPanel.add("West", new JLabel("Status: "));
      statusPanel.add("Center", statusL_);


      JPanel browserPanel = new JPanel(new BorderLayout());
      browserPanel.add("North", controlPanel);
      browserPanel.add("Center", scrollPane_);
      browserPanel.add("South", statusPanel);
      

      setContentPane(browserPanel);

      pack();
      centerFrame(this);
      if (title.toLowerCase().indexOf("tutorial") != -1)
         setLocation(getLocation().x-50, getLocation().y);
   }
     
   public void hyperlinkUpdate(HyperlinkEvent he)
   {
      if (he.getURL() != null)
      {
         String urlString = he.getURL().toString();
         String urlParameters = null;

         if (urlString.indexOf("?") > -1)
         {
            urlParameters = urlString.substring(urlString.indexOf("?")+1);
            urlString = urlString.substring(0, urlString.indexOf("?"));
         }


         if (he.getEventType() == HyperlinkEvent.EventType.ENTERED)
         {
            statusL_.setText(urlString);
            //System.out.print("e");
         }
         else if (he.getEventType() == HyperlinkEvent.EventType.EXITED)
         {
            statusL_.setText("");
            //System.out.print("x");
         }
         else if (he.getEventType() == HyperlinkEvent.EventType.ACTIVATED)
         {
            //System.out.print("a");
            try {
               this.setCursor(Cursor.getPredefinedCursor(Cursor.WAIT_CURSOR));
               helpPane_.setCursor(Cursor.getPredefinedCursor(Cursor.WAIT_CURSOR));
               statusL_.setText(Manager.getLocalized("loading")+" "+urlString);
               //System.out.print(Manager.getLocalized("loading")+" "+urlString);

               if (urlString.endsWith(".lpd") && !"startExternal=true".equals(urlParameters))
               {
                  if (Manager.isWindows())
                     urlString = urlString.replace('/', '\\');
                     
                  Manager.putLoadWindow(this);

                  // for "put html frame to foreground" if loaded from a web page
                  // after replay finish
                  Manager.mapLocationToTitle(urlString.substring(6), getTitle());
                  
                  // invoked with this as ActionListener
                  // actionPerformed is then called as soon as the document
                  // loading is finished
                  master_.loadDocument(
                     urlString.substring(6), 0, false, this, urlParameters);
                  
               }
               else  if ((urlString.endsWith(".html") || urlString.endsWith(".htm")) && !"startExternal=true".equals(urlParameters))
               {

                  recentDocuments_.addLast(currentURL_);
                  futureDocuments_.clear();

                  loadPage(he.getURL());

               }
               else
               {
                  if (NativeUtils.isLibraryLoaded() && Manager.isWindows())
                  {
                     if (urlString.startsWith("file:/"))
                        urlString = urlString.substring(6);
                     if (Manager.isWindows())
                        urlString = urlString.replace('/', '\\');
                     if (!NativeUtils.startFile(urlString))
                        Manager.showError(Manager.getLocalized("fileFail1")+" "+
                                          urlString+" "+Manager.getLocalized("fileFail2"),
                                          Manager.WARNING, null);
                     else
                        try { Thread.sleep(3500); } catch (InterruptedException e) {}
                        // display loading message a bit
                  }
               }

               // when lpd keep WAIT_CURSOR until document has been loaded
               // -> actionPerformed()
               if (!urlString.endsWith(".lpd"))
               {
                  this.setCursor(Cursor.getPredefinedCursor(Cursor.DEFAULT_CURSOR));
                  helpPane_.setCursor(Cursor.getPredefinedCursor(Cursor.DEFAULT_CURSOR));
                  statusL_.setText("");
               }

            } catch (IOException ioe) {
               if (ioe instanceof FileNotFoundException)
                  Manager.showError(this, Manager.getLocalized("fileNotFound1")
                                    +" "+Manager.getLocalized("fileNotFound2"),
                                    Manager.WARNING, ioe);
               else
                  ioe.printStackTrace();
            }
         }
      }
   }

   public void actionPerformed(ActionEvent e)
   {
      if (e.getSource() == backwardB_)
      {
         if (recentDocuments_.size() > 0)
         {
            URL toLoad = (URL)recentDocuments_.removeLast();
            statusL_.setText(Manager.getLocalized("loading")+" "+toLoad.toString());
            futureDocuments_.addFirst(currentURL_);
            try
            {
               loadPage(toLoad);
            }
            catch (IOException ex)
            {
               ex.printStackTrace();
            }
            statusL_.setText("");
         }
      }
      else if (e.getSource() == forwardB_)
      {
         if (futureDocuments_.size() > 0)
         {
            URL toLoad = (URL)futureDocuments_.removeFirst();
            statusL_.setText(Manager.getLocalized("loading")+" "+toLoad.toString());
            recentDocuments_.addLast(currentURL_);
            try
            {
               loadPage(toLoad);
            }
            catch (IOException ex)
            {
               ex.printStackTrace();
            }
            statusL_.setText("");
         }
      }
      else if (e.getSource() == locationF_)
      {
         recentDocuments_.addLast(currentURL_);
         try
         {
            String locationT = locationF_.getText();

            URL toLoad = null;
            if (new File(locationT).exists())
               toLoad = new File(locationT).toURL();
            else if (locationT.indexOf(":/") == -1)
               toLoad = new URL("http://"+locationT);
            else
               toLoad = new URL(locationT);

            statusL_.setText(Manager.getLocalized("loading")+" "+locationT);
            loadPage(toLoad);
            statusL_.setText("");
         
         }
         catch (IOException ex)
         {
            ex.printStackTrace();
         }
         /*
         catch (MalformedURLException ue)
         {
         ue.printStackTrace();
         }
         */
      }
      else if (e.getSource() == master_)
      {
         // document loading has finished (or failed)
         this.setCursor(Cursor.getPredefinedCursor(Cursor.DEFAULT_CURSOR));
         helpPane_.setCursor(Cursor.getPredefinedCursor(Cursor.DEFAULT_CURSOR));
         statusL_.setText("");
      }

   }


   public void loadPage(URL address) throws IOException
   {
      helpPane_.setPage(address);
      currentURL_ = address;
      if (locationF_ != null)
         locationF_.setText(address.toString());
   }

   private void centerFrame(JFrame f)
   {
      Dimension d = f.getSize();
      Dimension screen = Toolkit.getDefaultToolkit().getScreenSize();
      int diffX = screen.width-d.width;
      int diffY = screen.height-d.height;
      f.setLocation(diffX/2, diffY/2);
   }

   private ImageIcon createIcon(String location)
   {
		URL loc = getClass().getResource(location);
      if (loc != null)
         return new ImageIcon(loc);
      else
         return null;
	}
   
   // see also LayoutDecember2001; double code MasterPlayer
   private void setButtonImages(JButton button, String name)
   {
      String prefix = "/imc/epresenter/player/icons2/";
		ImageIcon icon1 = createIcon(prefix+name+"_inactive.gif");
      ImageIcon icon2 = createIcon(prefix+name+"_active.gif");
      ImageIcon icon3 = createIcon(prefix+name+"_over.gif");
      ImageIcon icon4 = createIcon(prefix+name+"_not_available.gif");

      if (icon1 == null) throw new IllegalArgumentException(
         "Did not find resource "+prefix+name+"_inactive.gif for icon loading.");
		button.setIcon(icon1);
      if (icon2 != null)
         button.setPressedIcon(icon2);
      if (icon3 != null)
      {
         if (!button.isRolloverEnabled())
            button.setRolloverEnabled(true);
         button.setRolloverIcon(icon3);
      }
      if (icon4 != null)
      {
         button.setDisabledIcon(icon4);
      }
   
   }
   

   private void goDownElements(Element e, String space)
   {
      System.out.println(space+e);
      AttributeSet as = e.getAttributes();
      Enumeration keys = as.getAttributeNames();
      while(keys.hasMoreElements())
      {
         Object key = keys.nextElement();
         Object a = as.getAttribute(key);
         System.out.println(space+"   "+key+"="+a);
      }

      if (!e.isLeaf())
      {
         int c = e.getElementCount();
         for (int i=0; i<c; i++)
         {
            Element e2 = e.getElement(c);
            if (e2 != null)
               goDownElements(e2, space+" ");
         }
      }

   }
}
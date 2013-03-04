package imc.epresenter.player;

import java.awt.*;
import java.awt.event.*;
import java.io.*;
import java.lang.ref.WeakReference;
import java.net.URL;
import java.text.*;
import java.util.*;
import javax.swing.*;
import javax.swing.border.*;
import javax.swing.event.*;

import imc.epresenter.player.Document;
import imc.epresenter.player.master.MasterPlayer;
import imc.epresenter.player.util.ClockOverlay;
import imc.epresenter.player.util.TimeFormat;
import imc.epresenter.player.util.FloatNativeComponent;
import imc.epresenter.player.whiteboard.WhiteBoardPlayer;


public class LayoutNov2006 
implements ActionListener, MouseListener, MouseMotionListener
{
   static
   {
      JPopupMenu.setDefaultLightWeightPopupEnabled(false);
   }


   private static HashMap windowCollector_ = new HashMap();
   static boolean fullScreenActiveStatic_ = false;

   private Document document_;

   private Component control_;
   private Component slider_;
   private Component board_;
   private Component video1_;
   private Component video2_;
   private Component thumbs_;
   private Component search_;
   private Component metaData_;

   // denotes the size needed for the video panel to display the video
   // in a desired size; it is not necessarily the video's source size
   private Dimension         origVideoSize_;
   // the video component as given by the JMFVideoPlayer
   // should be JComponent in order to able to use setPreferredSize()
   //private VideoPanel        videoItself_;
   //private JComponent        boardItself_;
   private JButton           videoB_;
   private JButton           thumbsB_;
   private JButton           boardB_;
   private JPanel            mainPanel_;
   private JPanel            sidePanel_;
   private JPanel            buttonPanel_;
   private JPanel            clockOverlay_;
   private Border            sideBorder_;
   private boolean           videoDisplayed_;
   private boolean           thumbsDisplayed_;
   private boolean           boardDisplayed_;
   private boolean           labelsDisplayed_;
   private Point             lastMousePosition_;
   private int               mousePositionValue_;
   private JWindow           fullScreenWindow_;
   private MouseListener     fullMouseListener_;
   private MouseMotionListener fullMouseMotionListener_;
   private JPopupMenu        popupMenu_;
   private boolean           isStandAloneMode_; // this means nothing besides one video
   private JComponent        boardPanel_;
   private JComponent        thumbsPanel_;
   private JComponent        videoPanel_;
   // 8 'Standard Navigation' states: 
   // --> see 'Document.java' for more details
   private int[]             anNavigationStates_;
   private WhiteBoardPlayer  whiteboardPlayer_;

   
   public LayoutNov2006(Document document,
                        int[] anNavigationStates,
                        SRStreamPlayer masterP,
                        SRStreamPlayer boardP,
                        SRStreamPlayer videoP,
                        SRStreamPlayer clipsP,
                        SRStreamPlayer appP,
                        SRStreamPlayer thumbsP,
                        SRStreamPlayer searchP)
   {
      document_ = document;
      
      // 'Standard Navigation' states
      anNavigationStates_ = anNavigationStates;

      // these components must be present (master must be non-null)
      control_ = masterP.getVisualComponents()[0];
      slider_ = masterP.getVisualComponents()[1];
      clockOverlay_ = (JPanel)masterP.getVisualComponents()[2];
       
      // look for additional components
      if (boardP != null)
      {
         board_ = boardP.getVisualComponents()[0];
         
         whiteboardPlayer_ = (WhiteBoardPlayer)boardP;
      }
      if (videoP != null && ((JMFVideoPlayer)videoP).isDisplayingSomething()) 
      {
         video1_ = videoP.getVisualComponents()[0];
         
         // old style: one player handling all video
         //if (videoP.getVisualComponents().length > 1)
         //   video2_ = videoP.getVisualComponents()[1];
         
      }
    
      // new style: video and clips separated
      if (clipsP != null)
      {
         video2_ = clipsP.getVisualComponents()[0];
      }
      
      if (thumbsP != null)
         thumbs_ = thumbsP.getVisualComponents()[0];
      if (searchP != null)
         search_ = searchP.getVisualComponents()[0];
      

      isStandAloneMode_ = board_ == null;


      // enclosing panels for borders and latter (fullscreen) layout operations
      //
      if (board_ != null)
      {  
         if (video2_ == null) // old style
         {
            boardPanel_ = new JPanel(new GridLayout(1,1));
            boardPanel_.add(board_);
         }
         else // new style: clips on slides
         {
            JLayeredPane lp = new MyLayeredPane();

            lp.add(board_, JLayeredPane.DEFAULT_LAYER);
            lp.add(video2_, JLayeredPane.PALETTE_LAYER);

            boardPanel_ = lp;
         }
      }

      JComponent video = null;
      if (video1_ != null)
         video = (JComponent)video1_;
      else
         video = (JComponent)video2_;


      if ((video1_ != null || isStandAloneMode_) && video != null)
      {
         videoPanel_ = new JPanel(new GridLayout(1,1));
         videoPanel_.add(video);
         videoPanel_.setOpaque(true);
         videoPanel_.setBackground(Color.black);
      
      }

      if (isStandAloneMode_ && video != null)
      {
         // in order to have the video component occupy the size of the 
         // enclosed (standalone clip) video
         video.setPreferredSize(video.getMaximumSize());
         
      }
     
      if (videoPanel_ != null)
      {
         origVideoSize_ = videoPanel_.getPreferredSize();
      }


      if (thumbs_ != null)
      {
         if (search_ != null)
         {
            JTabbedPane tp = new JTabbedPane() {
               public boolean isFocusTraversable()
               {
                  return false;
               }
            };
            
            // 'document structure' visible?
            if (anNavigationStates_[Document.NAV_DOCUMENTSTRUCTURE] != Document.NAV_STATE_HIDDEN)
               tp.add(Manager.getLocalized("thumbnails"), thumbs_);
            // 'Player search field' visible?
            if (anNavigationStates_[Document.NAV_PLAYERSEARCHFIELD] != Document.NAV_STATE_HIDDEN)
               tp.add(Manager.getLocalized("search"), search_);
            // If both 'document structure' and 'search field' are hidden:
            // --> minimize panel size
            if ((anNavigationStates_[Document.NAV_DOCUMENTSTRUCTURE] == Document.NAV_STATE_HIDDEN) 
               && (anNavigationStates_[Document.NAV_PLAYERSEARCHFIELD] == Document.NAV_STATE_HIDDEN))
               tp.setPreferredSize(new Dimension(0, 0));

            // 'document structure' disabled?
            if (anNavigationStates_[Document.NAV_DOCUMENTSTRUCTURE] == Document.NAV_STATE_DISABLED)
               thumbsP.enableVisualComponents(false);
            // 'Player search field' disabled
            if (anNavigationStates_[Document.NAV_PLAYERSEARCHFIELD] == Document.NAV_STATE_DISABLED)
               searchP.enableVisualComponents(false);

            thumbsPanel_ = tp;
         }
         else
         {
            thumbsPanel_ = new JPanel(new GridLayout(1,1));
            thumbsPanel_.add(thumbs_);
         }
     }
  

      // create and enable/disable the layout buttons
      //
      Border emptyBorder = BorderFactory.createEmptyBorder();
      // 'Player config buttons' visible?
      if (anNavigationStates_[Document.NAV_PLAYERCONFIGBUTTONS] != Document.NAV_STATE_HIDDEN)
      {
         boardB_ = createButton("icon_fullscreen_", 
                                Manager.getLocalized("fullscreenToogle"), 0, 0, emptyBorder);
         videoB_ = createButton("icon_video_",
                                Manager.getLocalized("videoToogle"), 0, 0, emptyBorder);
         thumbsB_ = createButton("icon_thumbs_", 
                                 Manager.getLocalized("thumbsToogle"), 0, 0, emptyBorder);
      }
      else
      {
         boardB_ = createButton("trans_", "", 0, 0, emptyBorder);
         videoB_ = createButton("trans_", "", 0, 0, emptyBorder);
         thumbsB_ = createButton("trans_", "", 0, 0, emptyBorder);
      }
      // the labels are set (and removed) upon resize (ComponentListener)

      
      boardB_.setEnabled(false);
      thumbsB_.setEnabled(false);
      videoB_.setEnabled(false);

      // 'Player config burrons' enabled?
      if ((!isStandAloneMode_)
         && (anNavigationStates_[Document.NAV_PLAYERCONFIGBUTTONS] == Document.NAV_STATE_ENABLED))
      {
         if (boardPanel_ != null)
            boardB_.setEnabled(true);
         if (videoPanel_ != null)
            videoB_.setEnabled(true);
         if (thumbsPanel_ != null) 
            thumbsB_.setEnabled(true);
      }


      // create fullscreen popup menu
      //
      popupMenu_ = createPopupMenu();


      // make it popup during right mouse click (popup trigger)
      // in fullscreen mode
      fullMouseListener_ = new MouseAdapter() {
         public void mouseClicked(MouseEvent evt)
         {
            if (evt.isPopupTrigger())
            {
               popupMenu_.show((Component)evt.getSource(), evt.getX(), evt.getY());
            }
            else
               propagateEvent(evt);
         }

         public void mousePressed(MouseEvent evt)
         {
            if (evt.isPopupTrigger())
            {
               popupMenu_.show((Component)evt.getSource(), evt.getX(), evt.getY());
            }
            else
               propagateEvent(evt);
         }

         public void mouseReleased(MouseEvent evt)
         {
            if (evt.isPopupTrigger())
            {
               popupMenu_.show((Component)evt.getSource(), evt.getX(), evt.getY());
            }
            else
               propagateEvent(evt);

            // return (keyboard) focus to main window
            fullScreenWindow_.getOwner().requestFocus();
         }
         
         private void propagateEvent(MouseEvent evt)
         {
            Component comp = (Component)evt.getSource();
            if (comp != board_ && whiteboardPlayer_ != null)
            {
               if (evt.getID() == MouseEvent.MOUSE_CLICKED)
                  whiteboardPlayer_.FullScreenMouseClicked(evt);
               else if (evt.getID() == MouseEvent.MOUSE_PRESSED)
                  whiteboardPlayer_.FullScreenMousePressed(evt);
               else if (evt.getID() == MouseEvent.MOUSE_RELEASED)
                  whiteboardPlayer_.FullScreenMouseReleased(evt);
               
            }
         }
      };
      
      fullMouseMotionListener_ = new MouseMotionAdapter()
      {
         public void mouseMoved(MouseEvent evt)
         {
            propagateEvent(evt);
         }
         
         public void mouseDragged(MouseEvent evt)
         {
            mouseMoved(evt);
         }
         
         private void propagateEvent(MouseEvent evt)
         {
            Component comp = (Component)evt.getSource();
            if (comp != board_ && whiteboardPlayer_ != null)
            {
               if (evt.getID() == MouseEvent.MOUSE_MOVED)
                  whiteboardPlayer_.FullScreenMouseMoved(evt);
               else if (evt.getID() == MouseEvent.MOUSE_DRAGGED)
                  whiteboardPlayer_.FullScreenMouseDragged(evt);
            }
         }
      
      };

	}

   public JPanel doLayout()
   {
      mainPanel_ = new JPanel(new BorderLayout());
      mainPanel_.setOpaque(true);
  
      // create control bar (whole bottom area)
      // 
      GridBagLayout gridBagLayout = new GridBagLayout();
      GridBagConstraints constraints = new GridBagConstraints();

      gridBagLayout.setConstraints(control_, constraints);
      constraints.fill = GridBagConstraints.HORIZONTAL;
      constraints.weightx = 1.0;
      gridBagLayout.setConstraints(slider_, constraints);


      buttonPanel_ = new JPanel(new GridLayout(1,0));
      buttonPanel_.setOpaque(false);
      buttonPanel_.setLocation(10,48);
      
      JPanel p1 = new JPanel(new FlowLayout(FlowLayout.CENTER, 0,0));
      p1.setOpaque(false);
      p1.add(videoB_);
      JPanel p2 = new JPanel(new FlowLayout(FlowLayout.CENTER, 0,0));
      p2.setOpaque(false);
      p2.add(thumbsB_);
      JPanel p3 = new JPanel(new FlowLayout(FlowLayout.CENTER, 0,0));
         p3.setOpaque(false);
      p3.add(boardB_);

      buttonPanel_.add(p3);
      buttonPanel_.add(p1);
      buttonPanel_.add(p2);
      

      ((JPanel)slider_).add(buttonPanel_);

      // is gridbag layout, so the following is useless:
      //((JPanel)slider_).setMinimumSize(new Dimension(160, 60));
  

      JPanel bottom = new JPanel(gridBagLayout);
      
      bottom.setOpaque(false);
      bottom.add(control_);
      bottom.add(slider_);

      Dimension d1 = control_.getPreferredSize();
      Dimension d2 = new Dimension(d1.width+180, d1.height);
      Dimension d3 = new Dimension(d2.width, 0);
      // 'control bar' visible?
      if (anNavigationStates_[Document.NAV_CONTROLBAR] != Document.NAV_STATE_HIDDEN)
         bottom.setPreferredSize(d2);
      else
         bottom.setPreferredSize(d3);
      
      // this is a (semi-) static layout with LayoutManager null
      slider_.addComponentListener(new ComponentAdapter() {
         public void componentResized(ComponentEvent e)
         {
            buttonPanel_.setSize(slider_.getWidth()-10, buttonPanel_.getPreferredSize().height);

            if (slider_.getWidth()-10 > 300 && !labelsDisplayed_)
            {
               // 'Player config buttons' visible?
               if (anNavigationStates_[Document.NAV_PLAYERCONFIGBUTTONS] != Document.NAV_STATE_HIDDEN)
               {
                  thumbsB_.setText(Manager.getLocalized("thumbnail"));
                  boardB_.setText(Manager.getLocalized("fullScreenMode"));//Manager.getLocalized("content"));
                  videoB_.setText(Manager.getLocalized("video"));
               }
               else
               {
                  thumbsB_.setText("");
                  boardB_.setText("");
                  videoB_.setText("");
               }

               labelsDisplayed_ = true;
            }
            else if (slider_.getWidth()-10 <= 300 && labelsDisplayed_)
            {
               thumbsB_.setText("");
               boardB_.setText("");
               videoB_.setText("");

               labelsDisplayed_ = false;
            }
            
            
            mainPanel_.validate();
         }
      });
      
      
     
      // create sidePanel with video, structure, search
      // but all only if present 
      //
      sidePanel_ = new JPanel(new BorderLayout(0,0));
      sidePanel_.setOpaque(false);


      if (thumbsPanel_ != null)
      {
         sidePanel_.add("Center", thumbsPanel_);
         
         thumbsDisplayed_ = true;
      }

      if (videoPanel_ != null)
      {
         sidePanel_.add("South", videoPanel_);
         
         videoDisplayed_ = true;


         if (!isStandAloneMode_)
         {
            // dynamic layout: rescale video (and other) component
            // according to mouse movements
            //
            videoPanel_.addMouseListener(this);
            videoPanel_.addMouseMotionListener(this);

            // most likely non-transparent and therefor needs the listeners
            // on its own
            // TODO: hm: not video1_ ???
            JComponent videoItself = (JComponent)videoPanel_.getComponent(0);
            videoItself.addMouseListener(this);
            videoItself.addMouseMotionListener(this);

            // TODO: ???
            videoPanel_.setCursor(Cursor.getPredefinedCursor(Cursor.MOVE_CURSOR));
            videoItself.setCursor(Cursor.getPredefinedCursor(Cursor.MOVE_CURSOR));
         }
      }

      if (boardPanel_ != null) // this is a bit pointless as the button is deactivated otherwise
         boardDisplayed_ = true;

      // set borders to the enclosing panels (if existant)
      //
      Border b1 = BorderFactory.createBevelBorder(
         BevelBorder.LOWERED, Color.gray.brighter(), Color.gray.darker());
      Border b2 = BorderFactory.createLineBorder(mainPanel_.getBackground(), 2);

      sideBorder_ = BorderFactory.createCompoundBorder(b2,b1);

      if (boardPanel_ != null)
         boardPanel_.setBorder(sideBorder_);
      if (videoPanel_ != null)
         videoPanel_.setBorder(sideBorder_);
      if (thumbsPanel_ != null)
         thumbsPanel_.setBorder(sideBorder_);
     


      if (!isStandAloneMode_)
      {
         mainPanel_.add("Center", boardPanel_);
         if (sidePanel_ != null)
            mainPanel_.add("West", sidePanel_);
         
      }
      else
      {
         if (videoPanel_ != null)
            mainPanel_.add("Center", videoPanel_);
         else
            mainPanel_.add("Center", new JLabel(Manager.getLocalized("videoFail1")));
      }
      
      mainPanel_.add("South", bottom);
     
      return mainPanel_;
   }

   public void restoreVideoSize()
   {
      if (videoPanel_ != null)
      {
         videoPanel_.setPreferredSize(origVideoSize_);
         videoPanel_.revalidate();
      }
   }


   /**
    * The layout during full screen mode is as follows: 
    * - The whiteboard if present is the DEFAULT_LAYER in the JLayeredPane.
    * - Above if present is the transparent video component as the PALETTE_LAYER.
    * Setting the video component as glass pane didn't really work.
    * - Above that there was (until version 2.0) the clock overlay as glass pane.
    * If a glass pane has mouse or mouse motion listeners it will consume all
    * the events. The same goes for the video component.
    * So the fullMouseListener_ and fullMouseMotionListener_ must be present
    * on the whiteboard and/or the video component.
    */
   public int switchToFullScreen(
      boolean escapePressed, boolean videoEnabled, final JMFVideoPlayer videoPlayer)
   {
      if (escapePressed && (fullScreenWindow_ == null || !fullScreenWindow_.isVisible()))
         return 0; // nothing do be done

      if (fullScreenWindow_ == null)
      {
         Window parent = SwingUtilities.windowForComponent(mainPanel_);

         fullScreenWindow_ = (JWindow)windowCollector_.get(parent);

         if (fullScreenWindow_ == null)
         {
            fullScreenWindow_ = new JWindow(parent);
            windowCollector_.put(parent, fullScreenWindow_);
         }
         
         Dimension dimScreen = Toolkit.getDefaultToolkit().getScreenSize();
         
         fullScreenWindow_.setBounds(0, 0, dimScreen.width, dimScreen.height);
         
         fullScreenWindow_.setGlassPane(clockOverlay_);
         fullScreenWindow_.getGlassPane().setVisible(true);
         
         fullScreenWindow_.getLayeredPane().add(popupMenu_);
       
      }


      if (!fullScreenWindow_.isVisible()) // --> activate
      {
         fullScreenWindow_.setBackground(Color.white);

         if (board_ != null)
         {
            try
            {
               fullScreenWindow_.getLayeredPane().add(
                  board_,  JLayeredPane.DEFAULT_LAYER);
               // sometimes this generates a silly ArrayIndexOutOfBoundsExc
               // in an attempt to check if the component is already present
            }
            catch (ArrayIndexOutOfBoundsException exc)
            {
               exc.printStackTrace();
               fullScreenWindow_.getLayeredPane().add(
                  board_,  JLayeredPane.DEFAULT_LAYER);
            }

            board_.setBounds(fullScreenWindow_.getBounds());

            board_.addMouseListener(fullMouseListener_);
            board_.addMouseMotionListener(fullMouseMotionListener_);
            
         }
         else
         {
            fullScreenWindow_.setBackground(Color.black);
            fullScreenWindow_.getContentPane().setBackground(Color.black);

            fullScreenWindow_.getContentPane().addMouseListener(fullMouseListener_);
         
         }

         

         if (videoPlayer != null && (video1_ != null || video2_ != null) && videoEnabled)
         {
            videoPlayer.activateFullScreen();
         }

         if (video2_ == null) // old mode
         {
            if (video1_ != null && videoEnabled)
            {
               fullScreenWindow_.getLayeredPane().add(
                  video1_, JLayeredPane.PALETTE_LAYER);
               video1_.setBounds(fullScreenWindow_.getBounds());

               // TODO: review: listeners: where and when? standalone?
               video1_.removeMouseListener(this);
               video1_.removeMouseMotionListener(this);
               video1_.addMouseListener(fullMouseListener_);
               video1_.addMouseMotionListener(fullMouseMotionListener_);
               video1_.setCursor(Cursor.getPredefinedCursor(Cursor.DEFAULT_CURSOR));
         
            }
         }
         else // video2_ != null, then only that needs to be added
         {
            //TODO: review: videoEnabled?
         
            if (videoEnabled)
            {
               fullScreenWindow_.getLayeredPane().add(
                  video2_, JLayeredPane.PALETTE_LAYER);
               video2_.setBounds(fullScreenWindow_.getBounds());

               video2_.addMouseListener(fullMouseListener_);
               video2_.addMouseMotionListener(fullMouseMotionListener_);
            }
         }
         
         fullScreenWindow_.setVisible(true);
         fullScreenActiveStatic_ = true;
      }
      else // fullScreenWindow_.isVisible() --> deactivate
      {
         
         if (board_ != null)
         {
            board_.removeMouseListener(fullMouseListener_);
            board_.removeMouseMotionListener(fullMouseMotionListener_);
            
            boardPanel_.remove(board_); // make sure it is not already there
            if (boardPanel_ instanceof JLayeredPane)
               ((JLayeredPane)boardPanel_).add(board_, JLayeredPane.DEFAULT_LAYER);
            else
               boardPanel_.add(board_);
           board_.invalidate();

         }
         else
         {
            fullScreenWindow_.getContentPane().removeMouseListener(fullMouseListener_);
         }
      

         if (video2_ == null) // old style
         {
            if (video1_ != null && videoEnabled)
            {
               videoPanel_.add(video1_);
               
               video1_.addMouseListener(this);
               video1_.addMouseMotionListener(this);
               video1_.removeMouseListener(fullMouseListener_);
               video1_.removeMouseMotionListener(fullMouseMotionListener_);
               video1_.setCursor(Cursor.getPredefinedCursor(Cursor.MOVE_CURSOR));
        
               video1_.invalidate();
            }
         }
         else
         {
            video2_.removeMouseListener(fullMouseListener_);
            video2_.removeMouseMotionListener(fullMouseMotionListener_);

            if (boardPanel_ != null)
               ((JLayeredPane)boardPanel_).add(video2_, JLayeredPane.PALETTE_LAYER);
            else // this should be standalone mode
               videoPanel_.add(video2_);

            video2_.invalidate();
         }

         if (videoPlayer != null && (video1_ != null || video2_ != null) && videoEnabled)
         {
            videoPlayer.deactivateFullScreen();
         }



         fullScreenWindow_.setVisible(false);
         fullScreenActiveStatic_ = false;


         // it seems to not matter what gets validated...
         mainPanel_.validate();
         //((JFrame)SwingUtilities.windowForComponent(mainPanel_)).getRootPane().validate();
         //SwingUtilities.windowForComponent(mainPanel_).validate();

      }

      return 0;
   }

   public void actionPerformed(ActionEvent e)
   {
      if (e.getSource() instanceof JMenuItem)
      {
         // source is the fullscreen popup menu

         if (e.getActionCommand().equals(Manager.getLocalized("closeFullScreen")))
            document_.switchToFullScreen(false);
         else if (e.getActionCommand().equals(Manager.getLocalized("startStopReplay")))
         {
            Dimension d = ((JMenuItem)e.getSource()).getSize();
            
            if (d.width > 0)
               document_.doStartOrStop();
            
            // Note "d.width > 0":
            // These accelerators are not always executed; especially in the case
            // of SPACE and F10. If they are then the respective action
            // is executed twice: again by the global key event handler
            // in LayoutNov2006.
            // This is prohibited with checking if the accelerator was executed 
            // (d.width < 0) or if the entry was clicked with the mouse.
            //
            // Bugfix #4025
         }
         else if (e.getActionCommand().equals(Manager.getLocalized("nextSlide")))
            document_.requestNextSlide(null);
         else if (e.getActionCommand().equals(Manager.getLocalized("prevSlide")))
            document_.requestPreviousSlide(false, null);
         else if (e.getActionCommand().equals(Manager.getLocalized("toBeginning")))
            document_.jumpForward(-1);
         else if (e.getActionCommand().equals(Manager.getLocalized("toEnd")))
            document_.jumpBackward(-1);
         else if (e.getActionCommand().equals(Manager.getLocalized("secondsForward")))
            document_.jumpForward(10000);
         else if (e.getActionCommand().equals(Manager.getLocalized("secondsBackward")))
            document_.jumpBackward(10000);
         else if (e.getActionCommand().equals(Manager.getLocalized("increaseVolume")))
         {
            Dimension d = ((JMenuItem)e.getSource()).getSize();
            
            if (d.width > 0)
               document_.increaseVolume();
            
            // Note "d.width > 0": see above
         }
         else if (e.getActionCommand().equals(Manager.getLocalized("decreaseVolume")))
            document_.decreaseVolume();
         else if (e.getActionCommand().equals(Manager.getLocalized("muteOnOff")))
            document_.muteOnOff();
         else if (e.getActionCommand().equals(Manager.getLocalized("closeDocument")))
            document_.closeIndirect();
         else if (e.getActionCommand().equals(Manager.getLocalized("closeProgram")))
         {
            document_.close();
            System.exit(0);
         }
         
         
         // seems to be necessary for JPopupMenu (and not for PopupMenu)
         fullScreenWindow_.getOwner().requestFocus();

      }
      else
      {
         // source is one of the layout specific buttons
         // show or hide a component

         boolean somethingChanged = false;
         if (e.getSource() == videoB_ && videoPanel_ != null)
         {
            if (videoDisplayed_)
            {
               sidePanel_.remove(videoPanel_);
               document_.deactivateVideo();
            }
            else
            {
               sidePanel_.add("South", videoPanel_);
               document_.activateVideo();
            }
            videoDisplayed_ = !videoDisplayed_;
            somethingChanged = true;

         }
         else if (e.getSource() == thumbsB_ && thumbsPanel_ != null)
         {
            if (thumbsDisplayed_)
               sidePanel_.remove(thumbsPanel_);
            else
               sidePanel_.add("Center", thumbsPanel_);
            thumbsDisplayed_ = !thumbsDisplayed_;
            somethingChanged = true;

         }
         else if (e.getSource() == boardB_)// && boardPanel_ != null)
         {
            /*
            if (boardDisplayed_)
               mainPanel_.remove(board_);
            else
               mainPanel_.add("Center", board_);
            boardDisplayed_ = !boardDisplayed_;
            somethingChanged = true;
            */

            document_.switchToFullScreen(false);
         }

         if (somethingChanged)
         {
            mainPanel_.validate();
            mainPanel_.repaint();
      
            
            // in order to "request" the update of a JMFVideoPlayer (bugaround)
            document_.layoutDrivenSetTimeAgain();
         }
      } // ! MenuItem
   }


   public void shutDown()
   {
      if (fullScreenWindow_ != null && fullScreenWindow_.isVisible())
      {
         document_.switchToFullScreen(true, true);
      }

      // do some cleanup; due to some video parts (JMF) are not deleted
      if (videoPanel_ != null)
      {
         if (!isStandAloneMode_)
            sidePanel_.remove(videoPanel_);
         else
            mainPanel_.remove(videoPanel_);
         videoPanel_.setBorder(null);
      }
      document_ = null;
   }
     

   //
   // layout/creation helper methods
   // 

   // see also MasterPlayer
   private JButton createButton(String name, String toolTip, int x, int y, Border border)
   {
      JButton but = new JButton();
      setButtonImages(but, name);
		but.setContentAreaFilled(false);
      but.setBorder(border);
      but.setBounds(new Rectangle(x, y, but.getPreferredSize().width, but.getPreferredSize().height));
      but.setToolTipText(toolTip);
      but.setFocusPainted(false); // do not display blue (half) border after clicked
      but.addActionListener(this);

		return but;
	}

   // see also MasterPlayer
   private void setButtonImages(JButton button, String name)
   {
      String prefix = "/imc/epresenter/player/icons2/";
		ImageIcon icon1 = createIcon(prefix+name+"inactive.gif");
      ImageIcon icon2 = createIcon(prefix+name+"active.gif");
      ImageIcon icon3 = createIcon(prefix+name+"over.gif");
      ImageIcon icon4 = createIcon(prefix+name+"not_available.gif");

      if (icon1 == null) throw new IllegalArgumentException(
         "Did not find resource "+prefix+name+"inactive.gif for icon loading.");
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

   private ImageIcon createIcon(String location)
   {
		URL loc = getClass().getResource(location);
      if (loc != null)
         return new ImageIcon(loc);
      else
         return null;
	}


   private JLabel createLabel(String text)
   {
      return createLabel(text, Font.PLAIN);
   }

   private JLabel createLabel(String text, int style)
   {
      JLabel l = new JLabel(text, JLabel.LEFT);
      if (style != Font.PLAIN)
         l.setFont(l.getFont().deriveFont(Font.BOLD));
      return l;
   }


   private JPopupMenu createPopupMenu()
   {
      JPopupMenu popupMenu = new JPopupMenu();

      JMenuItem closeFullItem = new JMenuItem(Manager.getLocalized("closeFullScreen"));
      closeFullItem.addActionListener(this);
      closeFullItem.setAccelerator(
          KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_ESCAPE, 0));
      popupMenu.add(closeFullItem);
      

      //JMenuItem separatorItem1 = new JMenuItem("-");
      //popupMenu.add(separatorItem1);

      popupMenu.addSeparator();


      JMenuItem startStopItem = new JMenuItem(Manager.getLocalized("startStopReplay"));
      // 'standard buttons' enabled?
      if (anNavigationStates_[Document.NAV_STANDARDBUTTONS] == Document.NAV_STATE_ENABLED)
      {
         startStopItem.addActionListener(this);
         startStopItem.setAccelerator(
            KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_SPACE, 0));
      }
      else
         startStopItem.setEnabled(false);
      // 'standard buttons' visible?
      if (anNavigationStates_[Document.NAV_STANDARDBUTTONS] != Document.NAV_STATE_HIDDEN)
         popupMenu.add(startStopItem);

      //JMenuItem separatorItem2 = new JMenuItem("-");
      //popupMenu.add(separatorItem2);

      // 'standard buttons' visible?
      if (anNavigationStates_[Document.NAV_STANDARDBUTTONS] != Document.NAV_STATE_HIDDEN)
         popupMenu.addSeparator();


      JMenuItem nextSlideItem = new JMenuItem(Manager.getLocalized("nextSlide"));
      // 'standard buttons' enabled?
      if (anNavigationStates_[Document.NAV_STANDARDBUTTONS] == Document.NAV_STATE_ENABLED)
      {
         nextSlideItem.addActionListener(this);
         nextSlideItem.setAccelerator(
            KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_PAGE_DOWN, 0));
      }
      else
         nextSlideItem.setEnabled(false);
      // 'standard buttons' visible?
      if (anNavigationStates_[Document.NAV_STANDARDBUTTONS] != Document.NAV_STATE_HIDDEN)
         popupMenu.add(nextSlideItem);
       

      JMenuItem prevSlideItem = new JMenuItem(Manager.getLocalized("prevSlide"));
      // 'standard buttons' enabled?
      if (anNavigationStates_[Document.NAV_STANDARDBUTTONS] == Document.NAV_STATE_ENABLED)
      {
         prevSlideItem.addActionListener(this);
         prevSlideItem.setAccelerator(
            KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_PAGE_UP, 0));
      }
      else
         prevSlideItem.setEnabled(false);
      // 'standard buttons' visible?
      if (anNavigationStates_[Document.NAV_STANDARDBUTTONS] != Document.NAV_STATE_HIDDEN)
         popupMenu.add(prevSlideItem);
       

      JMenuItem forwardItem = new JMenuItem(Manager.getLocalized("secondsForward"));
      // 'standard buttons' enabled?
      if (anNavigationStates_[Document.NAV_STANDARDBUTTONS] == Document.NAV_STATE_ENABLED)
      {
         forwardItem.addActionListener(this);
         forwardItem.setAccelerator(
            KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_RIGHT, 0));
      }
      else
         forwardItem.setEnabled(false);
      // 'standard buttons' visible?
      if (anNavigationStates_[Document.NAV_STANDARDBUTTONS] != Document.NAV_STATE_HIDDEN)
         popupMenu.add(forwardItem);
       

      JMenuItem backwardItem = new JMenuItem(Manager.getLocalized("secondsBackward"));
      // 'standard buttons' enabled?
      if (anNavigationStates_[Document.NAV_STANDARDBUTTONS] == Document.NAV_STATE_ENABLED)
      {
         backwardItem.addActionListener(this);
         backwardItem.setAccelerator(
            KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_LEFT, 0));
      }
      else
         backwardItem.setEnabled(false);
      // 'standard buttons' visible?
      if (anNavigationStates_[Document.NAV_STANDARDBUTTONS] != Document.NAV_STATE_HIDDEN)
         popupMenu.add(backwardItem);
       

      JMenuItem toBeginningItem = new JMenuItem(Manager.getLocalized("toBeginning"));
      // 'standard buttons' enabled?
      if (anNavigationStates_[Document.NAV_STANDARDBUTTONS] == Document.NAV_STATE_ENABLED)
      {
         toBeginningItem.addActionListener(this);
         toBeginningItem.setAccelerator(
            KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_HOME, 0));
      }
      else
         toBeginningItem.setEnabled(false);
      // 'standard buttons' visible?
      if (anNavigationStates_[Document.NAV_STANDARDBUTTONS] != Document.NAV_STATE_HIDDEN)
         popupMenu.add(toBeginningItem);
       

      JMenuItem toEndItem = new JMenuItem(Manager.getLocalized("toEnd"));
      // 'standard buttons' enabled?
      if (anNavigationStates_[Document.NAV_STANDARDBUTTONS] == Document.NAV_STATE_ENABLED)
      {
         toEndItem.addActionListener(this);
         toEndItem.setAccelerator(
            KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_END, 0));
      }
      else
         toEndItem.setEnabled(false);
      // 'standard buttons' visible?
      if (anNavigationStates_[Document.NAV_STANDARDBUTTONS] != Document.NAV_STATE_HIDDEN)
         popupMenu.add(toEndItem);
       
      //JMenuItem separatorItem3 = new JMenuItem("-");
      //popupMenu.add(separatorItem3);

      // 'standard buttons' visible?
      if (anNavigationStates_[Document.NAV_STANDARDBUTTONS] != Document.NAV_STATE_HIDDEN)
         popupMenu.addSeparator();

      JMenuItem increaseVolumeItem = new JMenuItem(Manager.getLocalized("increaseVolume"));
      increaseVolumeItem.addActionListener(this);
      increaseVolumeItem.setAccelerator(
          KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_F10, 0));
      popupMenu.add(increaseVolumeItem);
       

      JMenuItem decreaseVolumeItem = new JMenuItem(Manager.getLocalized("decreaseVolume"));
      decreaseVolumeItem.addActionListener(this);
      decreaseVolumeItem.setAccelerator(
          KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_F9, 0));
      popupMenu.add(decreaseVolumeItem);
      

      JMenuItem muteOnOffItem = new JMenuItem(Manager.getLocalized("muteOnOff"));
      muteOnOffItem.addActionListener(this);
      muteOnOffItem.setAccelerator(
          KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_M, 
                                 java.awt.event.InputEvent.CTRL_MASK));
      popupMenu.add(muteOnOffItem);
       


      //JMenuItem separatorItem4 = new JMenuItem("-");
      //popupMenu.add(separatorItem4);
      
      popupMenu.addSeparator();


      JMenuItem closeDocumentItem = new JMenuItem(Manager.getLocalized("closeDocument"));
      closeDocumentItem.addActionListener(this);
      /* geht nicht, wahrscheinlich, weil die Komponente mit dem PopupMenu nicht den Focus hat
      closeDocumentItem.setAccelerator(
          KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_F4, 
                                 java.awt.event.InputEvent.CTRL_MASK));
                                 */
      popupMenu.add(closeDocumentItem);
      
      
      popupMenu.addSeparator();


      JMenuItem closeProgramItem = new JMenuItem(Manager.getLocalized("closeProgram"));
      closeProgramItem.addActionListener(this);
      closeProgramItem.setAccelerator(
          KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_F4, 
                                 java.awt.event.InputEvent.ALT_MASK));
      popupMenu.add(closeProgramItem);
      
      

      return popupMenu;
   }






   //
   // methods for handling of mouse events on the video component
   // * for resizing the video component
   // * hiding the video during resizing
   // * switch to fullscreen
   //

   public void mouseClicked(MouseEvent evt)
   {
      if (evt.getClickCount() >= 2)
         document_.switchToFullScreen(false);
   }


   public void mousePressed(MouseEvent evt)
   {
      lastMousePosition_ = evt.getPoint();
      mousePositionValue_ = determinePosition(
         lastMousePosition_, (Component)evt.getSource());
      SwingUtilities.convertPointToScreen(
         lastMousePosition_, (Component)evt.getSource());

   }

   public void mouseReleased(MouseEvent evt)
   {
      if (video1_ != null && !isStandAloneMode_)
         ((VideoPanel)video1_).showVideo();
   }
   
   public void mouseDragged(MouseEvent evt)
   {
               
      long seconds = System.currentTimeMillis()/1000;
      seconds %= 60;

      //System.out.println(seconds+": on "+evt.getSource());

      Point currentMousePosition = evt.getPoint();

      //
      // video resize is only meaningfull if the mode is not denver
      // (sg stand alone mode)
      //
      if (!isStandAloneMode_)
      {
         SwingUtilities.convertPointToScreen(
            currentMousePosition, (Component)evt.getSource());

         //System.out.println("pos="+currentMousePosition);
         //System.out.println(evt.getSource().getClass());


         if (video1_ != null)
         {
            int diffX = currentMousePosition.x-lastMousePosition_.x;
            int diffY = currentMousePosition.y-lastMousePosition_.y;

                  
            if (mousePositionValue_ == 6 || mousePositionValue_ == 3)
               diffY = 0;
            else if (mousePositionValue_ == 7 || mousePositionValue_ == 8)
               diffX = 0;

            boolean boardTooSmall = board_ != null && 
               (board_.getSize().width < 100 || board_.getSize().height < 100);
            boolean thumbsTooSmall = thumbs_ != null &&
               (thumbs_.getSize().width < 100 || thumbs_.getSize().height < 120);
            //boolean makeBigger = diffX > 0 || diffY < 0;

            if (boardTooSmall && diffX > 0)
               diffX = 0;
            if (thumbsTooSmall && diffY < 0)
               diffY = 0;

            //System.out.println(makeBigger+"("+diffX+","+diffY+") ");
                  
            //System.out.println("makeBigger="+makeBigger+" boardTooSmall="
            //                   +boardTooSmall+" thumbsTooSmall="
            //                   +thumbsTooSmall);

            if (diffX != 0 || diffY != 0)
            {
               Dimension d = videoPanel_.getPreferredSize();
               Dimension nd = new Dimension(d.width+diffX, d.height-diffY);
               //float ratio = d.width/(float)d.height;
               //nd.width = Math.min(nd.width, Math.round(nd.height*(ratio)));
               //nd.height = Math.min(nd.height, Math.round(nd.width*(1/ratio)));
               if (nd.width > 99 && nd.height > 99)
                  videoPanel_.setPreferredSize(nd);

               //System.out.println("new size = "+nd.width+"x"+nd.height);

               if (((VideoPanel)video1_).isVideoVisible())
                  ((VideoPanel)video1_).hideVideo();
            
               videoPanel_.invalidate();

               mainPanel_.validate();
            }
         }
      }


      lastMousePosition_ = currentMousePosition;
   }

   public void mouseMoved(MouseEvent evt)
   {
      //System.out.println(  evt.getSource() instanceof JComponent);
               
      if (!isStandAloneMode_ && video1_ != null)// && evt.getSource() instanceof JComponent)
      {
         int value = determinePosition(
            evt.getPoint(), (Component)evt.getSource());

         if (value == 6 || value == 3)
            videoPanel_.setCursor(
               Cursor.getPredefinedCursor(Cursor.E_RESIZE_CURSOR));
         else if (value == 9)
            videoPanel_.setCursor(
               Cursor.getPredefinedCursor(Cursor.NE_RESIZE_CURSOR));
         else if (value == 7 || value == 8)
            videoPanel_.setCursor(
               Cursor.getPredefinedCursor(Cursor.N_RESIZE_CURSOR));
          /*
         else
            video1_.setCursor(
               Cursor.getPredefinedCursor(Cursor.MOVE_CURSOR));
               */
                     
      }
   }

   public void mouseEntered(MouseEvent evt) {}
   public void mouseExited(MouseEvent evt) {}




   /**
    * @returns the location of the given point with respect to the
    *          given component and especially its border
    *          5 = truly inside (not on the border)
    *          6 = on the eastern border (but not the northern or southern)
    *          9 = on the north-eastern border
    *          8 = on the northern border (but not the western or eastern)
    *          7 = on the north-western border
    *          4 = on the western border (but not the northern or southern)
    *          1 = on the south-western border
    *          2 = on the southern border (but not the western or eastern)
    *          3 = on the south-eastern border
    */

   private int determinePosition(Point pos, Component comp)
   {
      int value = 5;

      if (comp instanceof JComponent)
      {
         JComponent jcomp = (JComponent)comp;

         if (jcomp.getBorder() != null && 
             jcomp.getBorder().getBorderInsets(comp).top > 0)
         {
            Insets insets = jcomp.getBorder().getBorderInsets(jcomp);
            Dimension dim = jcomp.getSize();

            Rectangle inner = new Rectangle(insets.left, insets.top,
                                            dim.width-insets.left-insets.right,
                                            dim.height-insets.top-insets.bottom);
            if (!inner.contains(pos))
            {
               // the mouse cursor is on the border
               // do some magic math resulting in unique values for value
               // denoting the position
              
               if (pos.x >= inner.width)
                  value += 1;
               if (pos.x < inner.x)
                  value -= 1;
               if (pos.y < inner.y)
                  value += 3;
               if (pos.y >= inner.height)
                  value -= 3;
            }
         }
      }
      
      return value;
   }


   private class MyLayeredPane extends JLayeredPane
   {
      public void setBounds(int x, int y, int w, int h)
      {
         super.setBounds(x, y, w, h);

         Insets insets = getInsets() != null ? getInsets() : new Insets(0,0,0,0);

         // maximize the contained components
         // otherwise in a normal LayeredPane they will have no size at all
         // until specified from the outside
         Component[] cs = this.getComponents();
         for (int i=0; i<cs.length; ++i)
            cs[i].setBounds(insets.left, insets.top, 
                            w-(insets.left+insets.right), h-(insets.top+insets.bottom));
      }

      public Dimension getPreferredSize()
      {
         Dimension prefSize = new Dimension(0,0);
         Component[] cs = this.getComponents();
         for (int i=0; i<cs.length; ++i)
         {
            Dimension compSize = cs[i].getPreferredSize();
            if (compSize != null && compSize.width > prefSize.width)
               prefSize.width = compSize.width;
            if (compSize != null && compSize.height > prefSize.height)
               prefSize.height = compSize.height;
         }
         
         // PZI: add insets to avoid bad quality of structured clips caused by down-scaling a few pixel
         Insets insets = getInsets() != null ? getInsets() : new Insets(0,0,0,0);
         prefSize.width += insets.left + insets.right;
         prefSize.height += insets.top + insets.bottom;

         return prefSize;
      }

      protected void addImpl(Component comp, Object constraints, int index) 
      {
         super.addImpl(comp, constraints, index);

         Rectangle r = this.getBounds();
         Insets insets = getInsets() != null ? getInsets() : new Insets(0,0,0,0);

         // maximize the contained components
         // otherwise in a normal LayeredPane they will have no size at all
         // until specified from the outside
         if (r != null)
            setBounds(insets.left, insets.top, 
                      r.width-(insets.left+insets.right), 
                      r.height-(insets.top+insets.bottom));
      }
   }

}
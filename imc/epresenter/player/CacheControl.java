package imc.epresenter.player;



import java.awt.*;

import java.awt.event.ActionListener;

import java.awt.event.ActionEvent;

import java.awt.event.WindowAdapter;

import java.awt.event.WindowEvent;

import java.io.*;

import java.util.ArrayList;

import javax.swing.*;

import javax.swing.border.Border;

import javax.swing.event.TableModelEvent;

import javax.swing.event.TableModelListener;

import javax.swing.table.*;



import imc.epresenter.filesdk.FileResources;

import imc.epresenter.filesdk.Metadata;

import imc.epresenter.player.util.XmlTag;

import imc.lecturnity.util.NativeUtils;

import imc.lecturnity.util.SpecialTable;

import imc.lecturnity.util.ui.LayoutUtils;

import imc.lecturnity.util.ui.DirectoryChooser;




public class CacheControl extends JPanel implements ActionListener

{

   

   private Border emptyBorder6_ = BorderFactory.createEmptyBorder(6,6,6,6);

   private Border emptyBorder2_ = BorderFactory.createEmptyBorder(2,0,2,3);

      

   

   private CacheBackend backend_;

   private String location_;



   private JTabbedPane tabbedPane_;

 

   // global

   private JButton okB_;

   private JButton cancelB_;

   // one

   private CacheDialogs.CopyStub copyStub_; // is JComponent

   private JLabel copyStatusL_;

   // two

   private JRadioButton cacheAlwaysB_;

   private JRadioButton cacheSelectiveB_;

   private JRadioButton cacheNeverB_;

   private JRadioButton deleteRarestB_;

   private JRadioButton deleteOldestB_;

   private JRadioButton deleteAskB_;

   private JTable pathTable_;

   // three

   private JRadioButton tempDirB_;

   private JRadioButton pathDirB_;

   private JButton chooseDirB_;

   private JTextField chooseDirF_;

   private JButton deleteB_;

   private JTable entriesTable_;

   private ArrayList entriesLocations_; // not listed in the table

   private JLabel totalSizeL_;

   private JLabel freeInCacheL_;

   private JLabel freeOnDeviceL_;

   private CacheDialogs.SizeStub sizeStub_;

   private String initialCacheLocation_;

   private boolean cacheLocationChanged_;

    



   public CacheControl(CacheBackend backend, String location)

   {

      backend_ = backend;

      location_ = location;

      

      tabbedPane_ = new JTabbedPane();

      tabbedPane_.setOpaque(true);

      tabbedPane_.addTab(Manager.getLocalized("tabDocument"), createPanelOne());

      tabbedPane_.addTab(Manager.getLocalized("tabPolicies"), createPanelTwo());

      tabbedPane_.addTab(Manager.getLocalized("tabExtended"), createPanelThree());



      tabbedPane_.setPreferredSize(new Dimension(480, tabbedPane_.getPreferredSize().height));



      setLayout(new BorderLayout());

      add(tabbedPane_, BorderLayout.CENTER);



      JPanel lower = new JPanel(new FlowLayout(FlowLayout.RIGHT));

      okB_ = new JButton("OK");

      okB_.addActionListener(this);

      cancelB_ = new JButton(Manager.getLocalized("cancel"));

      cancelB_.addActionListener(this);

      LayoutUtils.evenSizes(new JComponent[] { okB_, cancelB_ });

      lower.add(okB_);

      lower.add(cancelB_);

      LayoutUtils.addBorder(lower, emptyBorder6_);



      add("South", lower);

      

      /*

      

      ButtonPanel bp = new ButtonPanel();

      bp.addRightButton(new WizardButton(WizardPanel.OK));

      bp.addRightButton(Box.createHorizontalStrut(10));

      bp.addRightButton(new WizardButton(WizardPanel.CANCEL));

      add(bp, BorderLayout.SOUTH);

         */

      /* may cause memory leaks?

      Runtime.getRuntime().addShutdownHook(new Thread() {

         public void run()

         {

            if (cacheLocationChanged_)

               backend_.getSettings().setProperty("cacheLocation", initialCacheLocation_);

         }

      });

      */

   }



   public void showYourself()

   {

      openUpFrame(false);



   }



   public void showSettingsOnly()

   {

      tabbedPane_.setEnabledAt(0, false);

      tabbedPane_.setEnabledAt(1, false);

      tabbedPane_.setSelectedIndex(2);

      

      openUpFrame(true);

   }



   public void showSettingsAndPoliciesOnly()

   {

      tabbedPane_.setEnabledAt(0, false);

      tabbedPane_.setSelectedIndex(2);

      

      openUpFrame(true);

   }



   public void waitForFinish()

   {

      synchronized(this) { try { wait(); } catch (InterruptedException e) {} }

   }



   private void openUpFrame(final boolean isModal)

   {

      Window window = null;

      if (isModal)

      {

         Window parent = Manager.getLoadWindow();

      



         window = Manager.requestDialog((JFrame)parent, "", true);

      }

      else

         window = Manager.requestFrame();



      if (window instanceof JFrame)

         ((JFrame)window).setTitle(Manager.getLocalized("cacheManager"));

      else

         ((JDialog)window).setTitle(Manager.getLocalized("cacheManager"));

           

      window.addWindowListener(new WindowAdapter() {

         public void windowClosing(WindowEvent evt)

         {

            // only on window close button (X) invoked

            //System.out.println("Window closing");

            if (cacheLocationChanged_)

               backend_.getSettings().setProperty("cacheLocation", initialCacheLocation_);



            //synchronized(this) { notify(); }

         }



         public void windowClosed(WindowEvent evt)

         {

            //synchronized(this) { notify(); }

         }

      });

         



      if (window instanceof JFrame)

         ((JFrame)window).setContentPane(CacheControl.this);

      else

         ((JDialog)window).setContentPane(CacheControl.this);

      window.pack();



      triggerResizeForTableInPanelThree();

   



      if (window.isVisible())

         window.toFront();

      else

      {

         centerFrame(window);

         window.setVisible(true); // if modal (dialog) this is blocking

      }

}



   private void triggerResizeForTableInPanelThree()

   {

      ((SpecialTable)entriesTable_).triggerResizeAndRepaint();

   }



   private JComponent createPanelOne()

   {

      Border emptyBorder3 = BorderFactory.createEmptyBorder(0,0,3,3);



      String location = location_;

      File f = new File(location);

      int idx = location.indexOf(File.separator);

      String dir = f.getParent();

      String name = f.getName();

      

      JLabel nameL1 = new JLabel(Manager.getLocalized("name")+": ");

      nameL1.setBorder(emptyBorder3);

      JLabel locationL1 = new JLabel(Manager.getLocalized("location")+": ");

      locationL1.setBorder(emptyBorder3);

      JLabel sizeL1 = new JLabel(Manager.getLocalized("size")+": ");

      sizeL1.setBorder(emptyBorder3);

      JLabel statusL1 = new JLabel(Manager.getLocalized("status")+": ");

      statusL1.setBorder(emptyBorder3);

      JLabel nameL2 = new JLabel(name);

      nameL2.setToolTipText(name);

      JLabel locationL2 = new JLabel(dir);

      locationL2.setToolTipText(dir);

      JLabel sizeL2 = new JLabel(sizeString(f.length()));

      copyStatusL_ = new JLabel();

      copyStatusL_.setPreferredSize(

         new Dimension(505, copyStatusL_.getPreferredSize().height));

      copyStub_ = new CacheDialogs.CopyStub(backend_, location_, this, false);

      updateStatusLabelAndButton();



      LayoutUtils.evenSizes(new JComponent[] { nameL1, locationL1, sizeL1, statusL1 });



      JLabel questionL = new JLabel(Manager.getLocalized("copyManually"));

      

      JPanel inner0 = new JPanel(new BorderLayout());

      inner0.add("West", nameL1);

      inner0.add("Center", nameL2);





      JPanel inner1 = new JPanel(new BorderLayout());

      inner1.add("West", locationL1);

      inner1.add("Center", locationL2);



      JPanel inner2 = new JPanel(new BorderLayout());

      inner2.add("West", sizeL1);

      inner2.add("Center", sizeL2);



      JPanel inner3 = new JPanel(new BorderLayout());

      inner3.add("West", statusL1);

      inner3.add("Center", copyStatusL_);



      JPanel outer1 = new JPanel(new GridLayout(0,1));

      outer1.add(inner0);

      outer1.add(inner1);

      outer1.add(inner2);

      outer1.add(inner3);

      outer1.setBorder(BorderFactory.createCompoundBorder(

         BorderFactory.createTitledBorder(

            " "+Manager.getLocalized("currentDocument")+" "), emptyBorder6_));



      JPanel outer2 = new JPanel(new GridLayout(0,1));

      outer2.add(questionL);

      outer2.add(copyStub_);

      outer2.setBorder(emptyBorder6_);







      JPanel totalOne = LayoutUtils.createBoxLayoutPanel(true);

      

      totalOne.add(LayoutUtils.makeBoxable(outer1, true));

      totalOne.add(Box.createVerticalStrut(10));

      totalOne.add(LayoutUtils.makeBoxable(outer2, true));



      totalOne.setBorder(emptyBorder6_);





      return totalOne;

   }



   private JComponent createPanelTwo()

   {

      CacheSettings settings = backend_.getSettings();

      

      /*

      settings.setProperties("selectiveCachePaths", new String[] {

         "c:\\Lecturnity-Dokumente\\", "d:\\Vorlesung\\Vorträge\\"

      });

      settings.setProperties("selectiveCacheValues", new String[] {

         "Yes", "No"

      });

      */





      String cacheMode = settings.getProperty("cacheMode");





      cacheAlwaysB_ = new JRadioButton(Manager.getLocalized("always"));

      cacheSelectiveB_ = new JRadioButton(

         Manager.getLocalized("followingPaths")+"...", true);

      cacheNeverB_ = new JRadioButton(Manager.getLocalized("never"));



      ButtonGroup g1 = new ButtonGroup();

      g1.add(cacheAlwaysB_);

      g1.add(cacheSelectiveB_);

      g1.add(cacheNeverB_);

      

      if (cacheMode.equals("Always"))

         cacheAlwaysB_.setSelected(true);

      else if (cacheMode.equals("Never"))

         cacheNeverB_.setSelected(true);

      

      String[] paths = settings.getProperties("selectiveCachePaths");

      String[] values = settings.getProperties("selectiveCacheValues");



      Object[][] tableData = new Object[0][3];

      if (paths != null)

      {

         tableData = new Object[paths.length][3];

         for (int i=0; i<paths.length; i++)

         {

            boolean yes = values[i].equals("Yes");

            tableData[i][0] = paths[i];

            tableData[i][1] = new Boolean(yes);

            tableData[i][2] = new Boolean(!yes);

         }

      }





      String[] tableHeader = new String[] { 

         Manager.getLocalized("path"), " "+Manager.getLocalized("yes")+" ", 

            Manager.getLocalized("no") 

      };

      final TableModel tm = new MyTableModel(tableData, tableHeader);

      pathTable_ = new SpecialTable(tm, new boolean[] { false, true, true });

      pathTable_.getTableHeader().setReorderingAllowed(false);

      pathTable_.getModel().addTableModelListener(new TableModelListener() {

         public void tableChanged(TableModelEvent e)

         {

            int row = e.getFirstRow();

            int col = e.getColumn();

            int otherColumn = col == 1 ? 2 : 1;

            Boolean value = (Boolean)tm.getValueAt(row, col);

            Boolean otherValue = (Boolean)tm.getValueAt(row, otherColumn);

            if (value.equals(otherValue))

               tm.setValueAt(new Boolean(!otherValue.booleanValue()), row, otherColumn);

            // else the event is caused by us

         }

      });

      pathTable_.setDefaultRenderer(Boolean.class, new BooleanCellRenderer());

      pathTable_.setDefaultRenderer(String.class, new StringCellRenderer());

      pathTable_.setDefaultEditor(Boolean.class, new BooleanCellEditor());

      JScrollPane pane17 = new JScrollPane(pathTable_, JScrollPane.VERTICAL_SCROLLBAR_AS_NEEDED,

                                           JScrollPane.HORIZONTAL_SCROLLBAR_NEVER);

      pane17.setPreferredSize(new Dimension(pathTable_.getPreferredSize().width+20, 

                                            pathTable_.getPreferredSize().height+80));





      JPanel inner10 = new JPanel(new BorderLayout());

      inner10.add("North", cacheSelectiveB_);

      inner10.add("Center", pane17);

      inner10.add("West", Box.createHorizontalStrut(20));



      JPanel cachingWhen = new JPanel();

      BoxLayout layout2 = new BoxLayout(cachingWhen, BoxLayout.Y_AXIS);

      cachingWhen.setLayout(layout2);



      cachingWhen.add(LayoutUtils.makeBoxable(cacheAlwaysB_, true));

      cachingWhen.add(LayoutUtils.makeBoxable(inner10, true));

      cachingWhen.add(LayoutUtils.makeBoxable(cacheNeverB_, true));



      cachingWhen.setBorder(BorderFactory.createCompoundBorder(

         BorderFactory.createTitledBorder(

            Manager.getLocalized("cacheWhen")), emptyBorder6_));



      String deleteMode = settings.getProperty("deleteMode");



      deleteRarestB_ = new JRadioButton(Manager.getLocalized("rarestFirst"));

      deleteOldestB_ = new JRadioButton(Manager.getLocalized("oldestFirst"));

      deleteAskB_ = new JRadioButton(Manager.getLocalized("askFirst"), true);

      

      ButtonGroup g2 = new ButtonGroup();

      g2.add(deleteRarestB_);

      g2.add(deleteOldestB_);

      g2.add(deleteAskB_);



      if (deleteMode.equals("Rarest"))

         deleteRarestB_.setSelected(true);

      else if (deleteMode.equals("Oldest"))

         deleteOldestB_.setSelected(true);

      



      JPanel cacheFull = new JPanel();

      BoxLayout layout4 = new BoxLayout(cacheFull, BoxLayout.Y_AXIS);

      cacheFull.setLayout(layout4);



      cacheFull.setBorder(BorderFactory.createCompoundBorder(

         BorderFactory.createTitledBorder(

            " "+Manager.getLocalized("howtoDelete")+" "), emptyBorder6_));





      cacheFull.add(LayoutUtils.makeBoxable(deleteRarestB_, true));

      cacheFull.add(LayoutUtils.makeBoxable(deleteOldestB_, true));

      cacheFull.add(LayoutUtils.makeBoxable(deleteAskB_, true));













      JPanel totalTwo = LayoutUtils.createBoxLayoutPanel(true);

      



      totalTwo.add(cachingWhen);

      //totalTwo.add(Box.createVerticalStrut(10));

      //totalTwo.add(cacheFull);



      totalTwo.setBorder(emptyBorder6_);



      return totalTwo;

   }



   private JComponent createPanelThree()

   {

      CacheSettings settings = backend_.getSettings();

      

      initialCacheLocation_ = settings.getProperty("cacheLocation");





      tempDirB_ = new JRadioButton(Manager.getLocalized("tmpDir"), true);

      tempDirB_.addActionListener(this);

      pathDirB_ = new JRadioButton(Manager.getLocalized("name")+": ");

      pathDirB_.addActionListener(this);



      ButtonGroup g1 = new ButtonGroup();

      g1.add(tempDirB_);

      g1.add(pathDirB_);



      if (!initialCacheLocation_.equals("TEMP"))

         pathDirB_.setSelected(true);



      chooseDirF_ = new JTextField();

      chooseDirF_.addActionListener(this);

      if (!initialCacheLocation_.equals("TEMP"))

         chooseDirF_.setText(initialCacheLocation_);



      chooseDirB_ = new JButton(Manager.getLocalized("choose")+"...");

      chooseDirB_.addActionListener(this);





      JPanel helper8 = new JPanel(new GridLayout(1,1)); // different background colors

      helper8.add(chooseDirF_);

      helper8.setBorder(emptyBorder2_);

      

      

      



      JPanel inner102 = new JPanel(new BorderLayout());

      inner102.add("West", pathDirB_);

      inner102.add("Center", helper8);

      inner102.add("East", chooseDirB_);



      JPanel outer398 = new JPanel(new BorderLayout());

      outer398.add("North", tempDirB_);

      outer398.add("South", inner102);



      outer398.setBorder(BorderFactory.createCompoundBorder(

         BorderFactory.createTitledBorder(

            " "+Manager.getLocalized("cacheLocation")+" "), emptyBorder6_));





      sizeStub_ = new CacheDialogs.SizeStub(this, settings.getMaxSize());

     

      sizeStub_.setBorder(emptyBorder6_);

      LayoutUtils.addBorder(sizeStub_, 

         BorderFactory.createTitledBorder(

            " "+Manager.getLocalized("cacheSize")+" "));



      Object[][] tableData2 = new Object[0][4];

      String[] tableHeader2 = new String[] { " ", 

         Manager.getLocalized("title"), Manager.getLocalized("author"), Manager.getLocalized("size") };

      final TableModel tm2 = new MyTableModel(tableData2, tableHeader2);

      entriesTable_ = new SpecialTable(tm2, new boolean[] { true, false, false, true });

      entriesTable_.getTableHeader().setReorderingAllowed(false);

      entriesTable_.getModel().addTableModelListener(new TableModelListener() {

         public void tableChanged(TableModelEvent e)

         {

            //System.out.println(tm2.getValueAt(e.getFirstRow(), e.getColumn()));

         }

      });

      entriesTable_.setDefaultRenderer(String.class, new StringCellRenderer());

      entriesTable_.setDefaultRenderer(Boolean.class, new BooleanCellRenderer());

      JScrollPane pane9 = new JScrollPane(entriesTable_, JScrollPane.VERTICAL_SCROLLBAR_AS_NEEDED,

                                          JScrollPane.HORIZONTAL_SCROLLBAR_NEVER);

      pane9.setPreferredSize(new Dimension(entriesTable_.getPreferredSize().width+20, 

                                           entriesTable_.getPreferredSize().height+80));



      Border eb = BorderFactory.createEmptyBorder(0,3,0,3);

      Border lb = BorderFactory.createLoweredBevelBorder();

      totalSizeL_ = new JLabel();

      totalSizeL_.setBorder(eb);

      LayoutUtils.addBorder(totalSizeL_, lb);

      freeInCacheL_ = new JLabel();

      freeInCacheL_.setBorder(eb);

      LayoutUtils.addBorder(freeInCacheL_, lb);

      freeOnDeviceL_ = new JLabel();

      freeOnDeviceL_.setBorder(eb);

      LayoutUtils.addBorder(freeOnDeviceL_, lb);

      

      updateEntriesTableAndAlike();

      





      

      

      deleteB_ = new JButton(Manager.getLocalized("deleteSelected"));

      deleteB_.addActionListener(this);



      JPanel inner80 = new JPanel(new BorderLayout());

      inner80.add("East", deleteB_);

      inner80.add("Center", Box.createHorizontalGlue());



      JPanel inner81 = new JPanel(new GridLayout(2,0));

      inner81.setBorder(BorderFactory.createEmptyBorder(3,0,6,0));

      inner81.add(totalSizeL_);

      inner81.add(freeInCacheL_);

      inner81.add(new JLabel());

      inner81.add(freeOnDeviceL_);



      JPanel inner34 = new JPanel(new BorderLayout());

      inner34.add("North", pane9);

      inner34.add("Center", inner81);

      inner34.add("South", inner80);



      inner34.setBorder(BorderFactory.createCompoundBorder(

         BorderFactory.createTitledBorder(

            " "+Manager.getLocalized("filesInCache")+" "), emptyBorder6_));







      JPanel totalThree = LayoutUtils.createBoxLayoutPanel(true);

      



      totalThree.add(LayoutUtils.makeBoxable(outer398, true));

      totalThree.add(Box.createVerticalStrut(10));

      totalThree.add(LayoutUtils.makeBoxable(sizeStub_, true));

      totalThree.add(Box.createVerticalStrut(10));

      totalThree.add(LayoutUtils.makeBoxable(inner34, true));



      totalThree.setBorder(emptyBorder6_);





      return totalThree;

   }





   public void actionPerformed(ActionEvent evt)

   {

      if (evt.getSource() == okB_)

      {

         if (copyStub_.isActive())

            return;





         putOutSettings();

         backend_.getSettings().sync();

         cacheLocationChanged_ = false;





         SwingUtilities.windowForComponent(this).dispose();

         

     

      }

      else if (evt.getSource() == cancelB_)

      {

         if (copyStub_.isActive())

            return;





         // some settings maybe need to be reset

         if (cacheLocationChanged_)

            backend_.getSettings().setProperty("cacheLocation", initialCacheLocation_);

         backend_.getSettings().sync();

         

         SwingUtilities.windowForComponent(this).dispose();

      

      }

      else if (evt.getSource() == copyStub_) // call back from copy process

      {

         if (backend_.contains(location_))

         {

            updateEntriesTableAndAlike();



            Thread t = new Thread()

            {

               public void run()

               {

                  

                  backend_.loadFile(location_);//backend_.getFileInCache(location_)+"");

                  // Bugfix für Bug #1042: Bei location ist noch bekannt,

                  // ob es freigeschaltet ist. Dennoch sollte 

                  // da aber die Version aus

                  // dem Cache geladen werden.



               }

            };

            t.start();

         }



         updateStatusLabelAndButton();





      }

      else if (evt.getSource() == sizeStub_)

      {

         //System.out.println("event from size");



         updateSizeLabels();

         updateStatusLabelAndButton();

            

      }

      else if (evt.getSource() == chooseDirB_)

      {

         Window w = SwingUtilities.windowForComponent(this);

         DirectoryChooser dc = null;

         if (w instanceof JDialog)

            dc = new DirectoryChooser((JDialog)w);

         else

            dc = new DirectoryChooser((JFrame)w);

         String currentPath = chooseDirF_.getText();

         currentPath = findValidPath(currentPath);

         if (currentPath == null)

            currentPath = initialCacheLocation_;

         if (new File(currentPath).exists())

            dc.setSelectedDir(new File(currentPath));

         else

            dc.setSelectedDir(new File(System.getProperty("user.home")));

         

         int result = dc.showDialog();



         if (result == DirectoryChooser.OPTION_APPROVE)

         {

            File choosen = dc.getSelectedDir();

            chooseDirF_.setText(choosen+"");

            pathDirB_.setSelected(true);



            try

            {

               if (backend_.getSettings().setLocation(chooseDirF_.getText()))

               {

                  cacheLocationChanged_ = true;

                  updateEntriesTableAndAlike();

               }

            }

            catch (IOException e)

            {

               Manager.showError(SwingUtilities.windowForComponent(this),

                                 Manager.getLocalized("cacheDirFail"),

                                 Manager.WARNING, e);

            }

         }

      }

      else if (evt.getSource() == pathDirB_ || evt.getSource() == chooseDirF_)

      {

         String currentPath = chooseDirF_.getText();

         if (currentPath.length() > 0)

         {

            currentPath = findOrCreateDirectory(currentPath);



            if (new File(currentPath).exists())

            {

               pathDirB_.setSelected(true);



               try

               {

                  if (backend_.getSettings().setLocation(currentPath))

                  {

                     cacheLocationChanged_ = true;

                     updateEntriesTableAndAlike();

                  }

               }

               catch (IOException e)

               {

                  

                  Manager.showError(SwingUtilities.windowForComponent(this),

                                    Manager.getLocalized("cacheDirFail"),

                                    Manager.WARNING, e);

               }

            }

         }

      }

      else if (evt.getSource() == tempDirB_)

      {

         try

         {

            if (backend_.getSettings().setLocation("TEMP"))

            {

               cacheLocationChanged_ = true;

               updateEntriesTableAndAlike();

            }

         }

         catch (IOException e)

         {

            Manager.showError(SwingUtilities.windowForComponent(this),

                              Manager.getLocalized("cacheDirFail"),

                              Manager.WARNING, e);

         }

      }

      else if (evt.getSource() == deleteB_)

      {

         int[] sel = getDeleteSelection();



         deleteInCache(sel);



        

         updateEntriesTableAndAlike();

         

         updateStatusLabelAndButton();

         if (!backend_.contains(location_))

         {

            copyStub_.reinit();

         }

            

      }



   }



   private void putOutSettings()

   {

      CacheSettings settings = backend_.getSettings();



      String cacheMode = "Selective";

      if (cacheAlwaysB_.isSelected())

         cacheMode = "Always";

      else if (cacheNeverB_.isSelected())

         cacheMode = "Never";

      settings.setProperty("cacheMode", cacheMode);



      TableModel tm = pathTable_.getModel();

      int count = tm.getRowCount();

      if (count > 0)

      {

         String paths[] = new String[count];

         String values[] = new String[count];

         for (int i=0; i<count; i++)

         {

            paths[i] = (String)tm.getValueAt(i, 0);

            boolean yes = ((Boolean)tm.getValueAt(i, 1)).booleanValue();

            values[i] = yes ? "Yes" : "No";

         }

         settings.setProperties("selectiveCachePaths", paths);

         settings.setProperties("selectiveCacheValues", values);

      }





      String deleteMode = "Ask";

      if (deleteRarestB_.isSelected())

         deleteMode = "Rarest";

      else if (deleteOldestB_.isSelected())

         deleteMode = "Oldest";

      settings.setProperty("deleteMode", deleteMode);



      String cacheLocation = "TEMP";

      if (!tempDirB_.isSelected() && chooseDirF_.getText().length() > 0)

         cacheLocation = findOrCreateDirectory(chooseDirF_.getText());

      settings.setProperty("cacheLocation", cacheLocation);



      settings.setProperty("maxCacheSize", sizeStub_.getValue()+"");



      settings.sync();

      //System.out.println(sizeStub_.getValue());

   }



   private void updateEntriesTableAndAlike()

   {

      File[] list = backend_.listFiles();



      Object[][] tableData = new Object[list.length][4];

      entriesLocations_ = new ArrayList(list.length);

      for (int i=0; i<list.length; i++)

      { 

         entriesLocations_.add(list[i]+"");



         String title = list[i].getParent(); // fall back defaults

         String author = list[i].getName();



         try // to find Title and Author in the metadata (open the file)

         {

            FileResources resources = FileResources.createFileResources(list[i]+"");

            InputStream configStream = resources.createConfigFileInputStream();

            XmlTag configTag = (XmlTag.parse(configStream))[0];

            configStream.close();

            Object[] metaA = configTag.getValues("METADATA");

            if (metaA != null && metaA.length > 0)

            {

               String metaFile = (String)metaA[0];



               Metadata metadata = resources.getMetadata((String)metaA[0]);



               if (metadata != null)

               {

                  if (metadata.title != null && metadata.title.length() > 0)

                     title = metadata.title;

                  if (metadata.author != null && metadata.author.length() > 0)

                     author = metadata.author;

               }

            }

         }

         catch (IOException ioe)

         {

            // if it is corrupt (the lpd file) the above defaults are used 

         }



         try { list[i] = list[i].getCanonicalFile(); } catch (Exception schmonzus) {}

         File locationFile = new File(location_);

         try { locationFile = locationFile.getCanonicalFile(); } catch (Exception e) {}





         if (!list[i].equals(locationFile))

         {

            //System.out.println(list[i]+"!="+location_);

            tableData[i][0] = new Boolean(false);

         }

         else

            tableData[i][0] = null; // currently loaded document may not be deleted

         tableData[i][1] = title;

         tableData[i][2] = author;

         tableData[i][3] = sizeString(list[i].length());

      }



      MyTableModel tm = (MyTableModel)entriesTable_.getModel();

      tm.exchangeData(tableData);



      updateSizeLabels();

   }



   private void updateSizeLabels()

   {

      File[] list = backend_.listFiles();

      long totalSize = 0;

      for (int i=0; i<list.length; i++)

         totalSize += list[i].length();

      

      CacheSettings settings = backend_.getSettings();

      long maxSize = sizeStub_.getValue()*1024L*1024;

      long freeSize = maxSize-totalSize;

      long deviceSize = NativeUtils.getSpaceLeftOnDevice(

         backend_.getSettings().getLocation());



      if (list.length > 0)

         totalSizeL_.setText(

            list.length+" "

            +Manager.getLocalized(list.length == 1 ? "document" : "documents")

            +" "+Manager.getLocalized("with")+" "+sizeString(totalSize));

      else

         totalSizeL_.setText("0 "+Manager.getLocalized("documents"));

      freeInCacheL_.setText(

         Manager.getLocalized("free")+" ("

         +Manager.getLocalized("cache")+"): "+sizeString(freeSize));

      freeOnDeviceL_.setText(

         Manager.getLocalized("free")+" ("

         +Manager.getLocalized("device")+"): "+sizeString(deviceSize));

   }



   private void updateStatusLabelAndButton()

   {

      long maxCacheSize = -1L; // value yet unknown

      if (sizeStub_ != null)

         maxCacheSize = 1024L*1024L*sizeStub_.getValue();



      String location = location_;

      String status = Manager.getLocalized("statusCan");

      if (!(location.endsWith("lpd")))

         status = Manager.getLocalized("statusWrong");

      else if (backend_.contains(location))

         status = Manager.getLocalized("statusHas");

      else if (!backend_.cacheAndDeviceSufficient(location, maxCacheSize))

         status = Manager.getLocalized("statusInsuf");

      else if (!backend_.canTake(location, maxCacheSize))

         status = Manager.getLocalized("statusFull");

      copyStatusL_.setText(status);

      if (!status.equals(Manager.getLocalized("statusCan")))

         copyStub_.setEnabled(false);

      else

         copyStub_.setEnabled(true);

   }





   private int[] getDeleteSelection()

   {

      // double pass



      int wishCounter = 0;

      MyTableModel tm = (MyTableModel)entriesTable_.getModel();

      for (int r=0; r<tm.getRowCount(); r++)

      {

         if (tm.getValueAt(r,0) == null || !(tm.getValueAt(r,0) instanceof Boolean))

            continue;

         if (((Boolean)tm.getValueAt(r, 0)).booleanValue())

            wishCounter++;

      }

      int[] selection = new int[wishCounter];

      int i = 0;

      for (int r=0; r<tm.getRowCount(); r++)

      {

         if (tm.getValueAt(r,0) == null)

            continue;

         if (((Boolean)tm.getValueAt(r, 0)).booleanValue())

            selection[i++] = r;

      }

      

      return selection;

   }



   private void deleteInCache(int[] sel)

   {

      for (int i=0; i<sel.length; i++)

      {

         String location = (String)entriesLocations_.get(sel[i]);



         

         try

         {

            backend_.deleteInCache(location);

            if (backend_.contains(location))

               Manager.showError(SwingUtilities.windowForComponent(this),

                                 Manager.getLocalized("fileDeleteFail")

                                 +": "+location, Manager.WARNING, null);

            

         }

         catch (IOException e)

         {

            Manager.showError(SwingUtilities.windowForComponent(this),

                              Manager.getLocalized("fileDeleteFail")

                              +": "+location, Manager.WARNING, e);

            break;

         }



      }

   }



   /* // was replaced with a brute force "rescan directory"

   private void deleteFromDataStructures(int[] del)

   {

      boolean stillPresent = false;

      int presentIndex = -1;

      for (int i=del.length-1; i>=0; i--) 

      {



         if (!backend_.contains((String)entriesLocations_.get(del[i])))

         {

            entriesLocations_.remove(del[i]);



         }

         else

         {

            stillPresent = true;

            presentIndex = i;

         }

      }



      if (stillPresent) // some may be deleted, some may not

      {

         int[] newDel = new int[presentIndex];

         System.arraycopy(del, 0, newDel, 0, newDel.length);

         del = newDel;

      }



      if (del.length > 0)

      {

         MyTableModel tm = (MyTableModel)entriesTable_.getModel();

         tm.removeRows(del);



         updateSizeLabels();

      }

   }

   */



   private String findValidPath(String location)

   {

      if (location == null)

         return null;



      while(location != null && !new File(location).exists())

         location = new File(location).getParent();



      if (location != null && new File(location).exists())

         return location;

      else

         return null;



   }



   private String findOrCreateDirectory(String currentPath)

   {

      if (!new File(currentPath).exists())

      {

         if (findValidPath(currentPath) != null)

         {

            



            int result = JOptionPane.showConfirmDialog(

               SwingUtilities.windowForComponent(this),

               Manager.getLocalized("createDir"),

               Manager.getLocalized("makeDir"), JOptionPane.YES_NO_OPTION);



            if (result == JOptionPane.YES_OPTION)

            {

               boolean success = new File(currentPath).mkdir();

               if (success)

                  return currentPath;

               else

               {

                  Manager.showError(SwingUtilities.windowForComponent(this),

                                    Manager.getLocalized("cannotCreate"),

                                    Manager.WARNING, null);

                  return initialCacheLocation_;

               }

            }

         }

      }



      if (new File(currentPath).exists())

         return currentPath;

      else

         return initialCacheLocation_;

   }



  

   private String sizeString(long length)

   {

      long plength = Math.abs(length);



      if (plength > Math.pow(1024, 3))

         return (Math.round((length/Math.pow(1024, 3.0))*10)/10.0f)+" GB";

      else if (plength > Math.pow(1024, 2))

         return (Math.round((length/Math.pow(1024, 2.0))*10)/10.0f)+" MB";

      else if (plength > 1024)

         return (Math.round((length/1024.0)*10)/10.0f)+" KB";

      else 

         return length+" B";

   }



   private void centerFrame(Window target)

   {

      Dimension d = Toolkit.getDefaultToolkit().getScreenSize();

      centerFrame(target, new Rectangle(0,0, d.width, d.height));

   }



   private void centerFrame(Window target, Rectangle source)

   {

      Dimension d = target.getSize();

      int diffX = source.width-d.width;

      int diffY = source.height-d.height;

      target.setLocation(source.x+diffX/2, source.y+diffY/2);

   }







   static class BooleanCellEditor extends AbstractCellEditor implements TableCellEditor, ActionListener

   {

      private JCheckBox editor_;



      BooleanCellEditor()

      {

         editor_ = new JCheckBox();

         editor_.setHorizontalAlignment(JLabel.CENTER);

         editor_.setOpaque(false);

         editor_.addActionListener(this);

      }





      public Object getCellEditorValue()

      {

         return new Boolean(editor_.isSelected());

      }



      public Component getTableCellEditorComponent(JTable table, Object value,

                                                   boolean s, int row, int column)

      {

         if (value != null)

         {

            Boolean bool = (Boolean)value;

            editor_.setSelected(bool.booleanValue());

            editor_.setEnabled(true);

         }

         else

         {

            editor_.setSelected(false);

            editor_.setEnabled(false);

         }



         return editor_;

         

      }



      public void actionPerformed(ActionEvent evt)

      {

         stopCellEditing();

      }

   }



   static class BooleanCellRenderer extends JCheckBox implements TableCellRenderer

   {

      public BooleanCellRenderer()

      {

         setHorizontalAlignment(JLabel.CENTER);

         setOpaque(false);

      }



      public Component getTableCellRendererComponent(JTable table, Object value,

                                                     boolean s, boolean f,

                                                     int row, int column)

      {

         if (value != null)

         {

            setEnabled(true);

            setSelected(((Boolean)value).booleanValue());

         }

         else

         {

            setEnabled(false);

            setSelected(false);

         

         }



         return this;

      }



      //

      // overriden due to performance resons

      // 

      public void firePropertyChanged(String name, boolean oldV, boolean newV)

      {}



      public void firePropertyChanged(String name, Object oldV, Object newV)

      {}



      public void repaint(long tm, int x, int y, int w, int h)

      {}



      public void repaint(Rectangle r)

      {}



      public void validate()

      {}



      public void revalidate()

      {}



   }



   static class StringCellRenderer extends DefaultTableCellRenderer

   {

      public Component getTableCellRendererComponent(JTable table, Object value,

                                                     boolean s, boolean f,

                                                     int row, int column)

      {

         Component c = super.getTableCellRendererComponent(

            table, value, false, false, row, column);

         if (c instanceof JLabel)

         {

            String v = (String)value;

            if (v.length() > 0 && Character.isDigit(v.charAt(0)) && v.charAt(v.length()-1) == 'B')

               ((JLabel)c).setHorizontalAlignment(JLabel.RIGHT);

            else

               ((JLabel)c).setHorizontalAlignment(JLabel.LEFT);

         }

         return c;

      }

   }



}
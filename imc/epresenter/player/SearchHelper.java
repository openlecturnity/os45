package imc.epresenter.player;

import java.awt.*;
import java.awt.event.*;
import java.io.*;
import javax.swing.*;
import javax.swing.event.ListSelectionListener;
import javax.swing.event.ListSelectionEvent;
import javax.swing.table.*;
import javax.swing.border.Border;
import javax.swing.border.LineBorder;

import imc.epresenter.filesdk.*;
import imc.epresenter.player.whiteboard.ObjectQueue;
import imc.epresenter.player.whiteboard.EventQueue;
import imc.epresenter.player.util.TimeFormat;

public class SearchHelper extends AbstractTableModel
implements SRStreamPlayer, ActionListener, ListSelectionListener
{
   private FileResources resources_;
   private Coordinator coordinator_;

   private SearchEngine searchEngine_;
   private SearchResult[] searchResults_;

   private TimeFormat timeFormat_;

   private JTextField textField_;
   private JButton searchB_;
   private JRadioButton allRB_;
   private JRadioButton oneRB_;
   private JCheckBox titleCB_;
   private JCheckBox keywordsCB_;
   private JCheckBox fullCB_;
   private JTable hitsTable_;
   private JLabel hitsLabel_;
   private JLabel searchLabel_;

   private JPanel visualComponent_;

   private int lastTime_ = -2;

   public SearchHelper(Metadata metadata, ObjectQueue objectQueue, EventQueue eventQueue)
   {
      searchEngine_ = new SearchEngine(metadata, objectQueue, eventQueue, true);
      timeFormat_ = new TimeFormat();
   }
   
   
   public void setStartOffset(int millis)
   {
   }


   public void init(FileResources resources, String[] args, Coordinator c)
   {
      resources_ = resources;
      coordinator_ = c;

      //
      // construct and setup components
      //
      textField_ = new JTextField();
      textField_.addActionListener(this);
      JPanel p1 = new JPanel(new GridLayout(1,1));
      p1.add(textField_);
      p1.setBorder(BorderFactory.createEmptyBorder(2,0,2,6));
      searchB_ = new JButton(Manager.getLocalized("search"));
      searchB_.addActionListener(this);
      searchB_.setToolTipText(Manager.getLocalized("startSearch"));
      titleCB_ = new JCheckBox(Manager.getLocalized("titles"), true);
      titleCB_.addActionListener(this);
      titleCB_.setToolTipText(Manager.getLocalized("searchTitle"));
      keywordsCB_ = new JCheckBox(Manager.getLocalized("keywords"), true);
      keywordsCB_.addActionListener(this);
      keywordsCB_.setToolTipText(Manager.getLocalized("searchKeyword"));
      fullCB_ = new JCheckBox(Manager.getLocalized("fullText"), true);
      fullCB_.addActionListener(this);
      fullCB_.setToolTipText(Manager.getLocalized("searchFulltext"));
      allRB_ = new JRadioButton(Manager.getLocalized("allWords"));
      allRB_.addActionListener(this);
      oneRB_ = new JRadioButton(Manager.getLocalized("oneWord"), true);
      oneRB_.addActionListener(this);
      hitsTable_ = new JTable(this);
      hitsTable_.setBackground(Color.white);
      
      // PZI remove grid and spacing and draw everything by hand
      // hitsTable_.setIntercellSpacing(new Dimension(6, 3));      
      hitsTable_.setIntercellSpacing(new Dimension(0, 0));
      hitsTable_.setShowGrid(false);
      
      hitsTable_.getTableHeader().setReorderingAllowed(false);
      TableColumnModel cm = hitsTable_.getColumnModel();
      TableColumn column2 = cm.getColumn(2);
      TableCellRenderer headerRenderer = column2.getHeaderRenderer();
      if (headerRenderer == null)
         headerRenderer = hitsTable_.getTableHeader().getDefaultRenderer();
      //System.out.println(headerRenderer);
      //headerRenderer.
      JLabel widthsL = new JLabel();
      widthsL.setText(Manager.getLocalized("time"));
      cm.getColumn(0).setMinWidth(widthsL.getPreferredSize().width+16);
      cm.getColumn(0).setMaxWidth(widthsL.getPreferredSize().width+16);
      cm.getColumn(1).setPreferredWidth(80);
      widthsL.setText("_ _ _");
      cm.getColumn(2).setMinWidth(widthsL.getPreferredSize().width+16);
      cm.getColumn(2).setMaxWidth(widthsL.getPreferredSize().width+16);
      hitsTable_.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);

      // PZI: using a single renderer for everything (with custom painting)
      DefaultTableCellRenderer renderer = new DefaultTableCellRenderer() {
         // custom painting: setting background, borders and tooltips as required
         // Note: Java reuses the component (settings) for subsequent cells.
         // Hence, parameters must be set for each cell
         public Component getTableCellRendererComponent(JTable table, Object value,
               boolean isSelected, boolean hasFocus, int row, int column) {
            // determine row type: page info (header) or fulltext match
            boolean bIsHeader = false;
            int index = 0;
            int element = 0;
            int count = 0;
            for (index = 0; index < searchResults_.length; index++) {
               if (count == row) {
                  // row is header
                  bIsHeader = true;
                  break;
               } else if (row - count < searchResults_[index].getNumberOfMatches() + 1) {
                  // search result within this index
                  element = row - count - 1;
                  break;
               }
               // increase count by header and results of current index
               count += 1 + searchResults_[index].getNumberOfMatches();
            }

            // set cell text
            if (column == 0 && value instanceof Integer) {
               int i = ((Integer) value).intValue();
               setText(timeFormat_.shortFormat(i));
            } else {
               String text = value.toString();
               if (!bIsHeader && column == 1) {
                  // cut text to fit into cell and add "..." if required
                  // note: Java does this for normal text but not for HTML text, which is used here
                  int maxWidth = hitsTable_.getColumnModel().getColumn(1).getWidth() - 22;
                  JLabel label = new JLabel(text);
                  if (label.getPreferredSize().width > maxWidth) {
                     // narrow to correct size
                     while (label.getPreferredSize().width > maxWidth) {
                        text = text.substring(0, text.length() - 1
                              - Math.max(0, (label.getPreferredSize().width - maxWidth) / 5));
                        label.setText(text);
                     }
                     text += "...";
                  }
               }
               setText(text);
            }

            // set cell tool tip
            String toolTip = null;
            if (bIsHeader) {
               // set "Matches in: ..."
               String string = hitsTable_.getValueAt(row, 2).toString();
               if (string.length() == 5 && string.charAt(1) == ' ' && string.charAt(3) == ' ') {
                  toolTip = Manager.getLocalized("matchesIn") + ":";
                  // maybe t k f
                  if (string.charAt(0) == Manager.getLocalized("titlesShort").charAt(0))
                     toolTip += " " + Manager.getLocalized("title");
                  if (string.charAt(2) == Manager.getLocalized("keywordsShort").charAt(0))
                     toolTip += " " + Manager.getLocalized("keywords");
                  if (string.charAt(4) == Manager.getLocalized("fullTextShort").charAt(0))
                     toolTip += " " + Manager.getLocalized("fullText");
               }
            } else {
               // set full text match as tool tip
               toolTip = hitsTable_.getValueAt(row, 1).toString();
               if (toolTip.toUpperCase().startsWith("<HTML>"))
                  toolTip = toolTip.substring(0, 6) + Manager.getLocalized("fullText") + ": "
                        + toolTip.substring(6);
               else
                  toolTip = Manager.getLocalized("fullText") + ": " + toolTip;
            }
            // bugfix #4733: set tooltip for each cell
            setToolTipText(toolTip);

            // set alignment (RIGHT for timestamp, LEFT else)
            setHorizontalAlignment(column == 0 ? RIGHT : LEFT);

            // set borders
            if (bIsHeader) {
               // search result: page info (header)
               setBackground(Color.WHITE);
               int top = row == 0 ? 0 : 1;
               int bottom = searchResults_[index].getNumberOfMatches() == 0
                     && row != getRowCount() - 1 ? 0 : 1;
               int left = column == 1 ? 1 : 0;
               int right = column == 1 ? 1 : 0;
               setBorder(BorderFactory.createCompoundBorder(BorderFactory.createMatteBorder(top,
                     left, bottom, right, hitsTable_.getGridColor()), BorderFactory
                     .createEmptyBorder(1, 4, 1, 4)));

            } else {
               // search result: fulltext match
               setBackground(new Color(233, 233, 233));
               setBorder(BorderFactory.createCompoundBorder(BorderFactory.createMatteBorder(1, 0,
                     1, 0, Color.WHITE), BorderFactory.createEmptyBorder(1, 5, 1, 5)));
            }

            // mark selected row
            if (isSelected)
               setBackground(hitsTable_.getSelectionBackground());

            return this; // default renderer is a JLabel used for painting
         }
      };
      
      hitsTable_.setDefaultRenderer(Integer.class, renderer);
      hitsTable_.setDefaultRenderer(String.class, renderer);
      hitsTable_.getSelectionModel().addListSelectionListener(this);
      hitsLabel_ = new JLabel("", JLabel.RIGHT);
      
      //
      // make a layout
      //
      JPanel inner1 = new JPanel(new BorderLayout());
      inner1.add("Center", p1);
      inner1.add("East", searchB_);
      JPanel inner2 = new JPanel(new FlowLayout(FlowLayout.CENTER, 0, 0));//new GridLayout(1,3));
      titleCB_.setHorizontalAlignment(JCheckBox.LEFT);
      keywordsCB_.setHorizontalAlignment(JCheckBox.CENTER);
      fullCB_.setHorizontalAlignment(JCheckBox.RIGHT);
      inner2.add(titleCB_);
      inner2.add(keywordsCB_);
      inner2.add(fullCB_);
      JPanel inner3 = new JPanel(new FlowLayout(FlowLayout.CENTER, 0, 0));//new GridLayout(1,3));
      searchLabel_ = new JLabel(Manager.getLocalized("searchFor")+"  ");
      inner3.add(searchLabel_);
      inner3.add(allRB_);
      inner3.add(oneRB_);
      JPanel controlPanel = new JPanel(new GridLayout(0,1));
      controlPanel.add(inner1);
      controlPanel.add(inner2);
      controlPanel.add(inner3);
      JScrollPane scrollPane = new JScrollPane(hitsTable_,
                                               JScrollPane.VERTICAL_SCROLLBAR_AS_NEEDED,
                                               JScrollPane.HORIZONTAL_SCROLLBAR_NEVER);
      scrollPane.getViewport().setBackground(Color.white);
      /*
      Dimension d1 = scrollPane.getPreferredSize();
      if (d1.width > 290)
         d1.width = 290;
      //d1.height = 100;
      scrollPane.setPreferredSize(d1);
      Dimension d2 = scrollPane.getPreferredSize();
      if (d2.width > 280)
         d2.width = 280;
      hitsTable_.setPreferredSize(d2);
      */
      /*
      if (Manager.isWindows()) // very strange that the first works with Win L&F
         hitsLabel_.setBorder(scrollPane.getBorder());
      else // and it does not work with Metal L&F
         hitsLabel_.setBorder(BorderFactory.createEtchedBorder());
         */
      hitsLabel_.setBorder(BorderFactory.createEtchedBorder());
      

      visualComponent_ = new JPanel(new BorderLayout());
      visualComponent_.setBorder(BorderFactory.createEmptyBorder(6,6,6,6));
      visualComponent_.add("North", controlPanel);
      visualComponent_.add("Center", scrollPane);
      visualComponent_.add("South", hitsLabel_);
      
      Dimension d3 = visualComponent_.getPreferredSize();
      if (d3.width > 300)
         d3.width = 300;
      if (d3.height > 120)
         d3.height = 120;
      visualComponent_.setPreferredSize(d3);
   
   }
	
	public void enableVisualComponents(boolean bEnable)
   {
      textField_.setEnabled(bEnable);
      searchB_.setEnabled(bEnable);
      titleCB_.setEnabled(bEnable);
      keywordsCB_.setEnabled(bEnable);
      fullCB_.setEnabled(bEnable);
      allRB_.setEnabled(bEnable);
      oneRB_.setEnabled(bEnable);
      hitsTable_.setEnabled(bEnable);
      hitsLabel_.setEnabled(bEnable);
      searchLabel_.setEnabled(bEnable);
   }

	public Component[] getVisualComponents()
   {
      return new Component[] { visualComponent_ };
   }

   public void actionPerformed(ActionEvent e)
   {
      if (e.getSource() == textField_ || e.getSource() == searchB_)
      {
         int mode = SearchConstants.MODE_OR;
         if (allRB_.isSelected())
            mode = SearchConstants.MODE_AND;

         int where = 0;
         if (titleCB_.isSelected())
            where |= SearchConstants.TITLES;
         if (keywordsCB_.isSelected())
            where |= SearchConstants.KEYWORDS;
         if (fullCB_.isSelected())
            where |= SearchConstants.FULLTEXT;
         
         if (where != 0)
         {
            String searchText = textField_.getText();
            try 
            {
               // handles empty search as well - required to reset highlighting of search matches
               searchResults_ = searchEngine_.search(searchText, mode, where);
               // PZI: force repaint if search was performed
               if (coordinator_ != null)
                  coordinator_.requestRepaintOfWhiteboard();

               fireTableDataChanged();
            } 
            catch (SearchException se) 
            {
               se.printStackTrace();
            }
            int numHits = searchResults_ != null ? searchResults_.length : 0;
            hitsLabel_.setText(numHits+" "+Manager.getLocalized("hits")+" ");
         }
         else
         {
            Manager.showError(Manager.getLocalized("atLeastOne"), Manager.WARNING, null);
         }
      }
      else if (e.getSource() == allRB_)
      {
         oneRB_.setSelected(false);
         allRB_.setSelected(true);
      }
      else if (e.getSource() == oneRB_)
      {
         allRB_.setSelected(false);
         oneRB_.setSelected(true);
      }

   }

   public void valueChanged(ListSelectionEvent e)
   {
      // PZI: translate row -> index
      // event-based access (instead of page-based)
      if (!e.getValueIsAdjusting())
	  {
         int row = hitsTable_.getSelectedRow();
         
         if (row != -1) {
            int index = 0;
            int count = 0;
            for (index = 0; index < searchResults_.length; index++) {
               if (count == row) {
                  // row is header - access at page time
                  requestTime((int) searchResults_[index].timeStamp);
                  break;
               } else if (row - count < searchResults_[index].getNumberOfMatches() + 1) {
                  // search result within this index - acces at event time
                  int element = row - count - 1;
                  requestTime((int) searchResults_[index].getMatchTimestamp(element));
                  break;
               }
               // increase count by header and results of current index
               count += 1 + searchResults_[index].getNumberOfMatches();
            }
         }
      }
      
//      if (!e.getValueIsAdjusting())
//      {
//         int row = hitsTable_.getSelectedRow();        
//         if (row != -1)
//            requestTime((int)searchResults_[row].timeStamp);
//      }
   }

   public int getColumnCount()
   {
      return 3;
   }

   public int getRowCount()
   {
      // PZI: return number of pages plus number of matches of each page
      int count = 0;
      if (searchResults_ != null) 
          for (SearchResult searchResult : searchResults_) 
              count += 1 + searchResult.getNumberOfMatches();
      return count;
//      if (searchResults_ != null) 
//         return searchResults_.length;
//      else
//         return 0;
   }

   public String getColumnName(int col)
   {
      switch (col)
      {
         case 0:
            return Manager.getLocalized("time");
         case 1:
            return Manager.getLocalized("title");
         case 2:
            return Manager.getLocalized("type");
         default:
            return "Njet";
      }
   }
   
   public Class getColumnClass(int col)
   {
      return getValueAt(0, col).getClass();
   }
   
   public Object getValueAt(int row, int col)
   {
      boolean bIsHeader = false;
      int index = 0;
      int element = 0;
      int count = 0;
      // translate row -> index 
      for (index = 0; index < searchResults_.length; index++) {
          if(count == row) {
              // row is header
              bIsHeader = true;
              break;
          } else if(row - count < searchResults_[index].getNumberOfMatches()+1) {
              // search result within this index
              element = row - count - 1;
              break;
          }
          // increase count by header and results of current index
          count += 1 + searchResults_[index].getNumberOfMatches();
      }
      
      switch (col) {
      case 0: // timestamp
         if (bIsHeader)
            return new Integer((int) searchResults_[index].timeStamp);
         else {
            int iTimestamp = searchResults_[index].getMatchTimestamp(element);
            int iPrevTimestamp = element == 0 ? (int) searchResults_[index].timeStamp
                  : searchResults_[index].getMatchTimestamp(element - 1);
            if (iTimestamp > iPrevTimestamp + 1000)
               // show timestamp of text (event) that was added during recording
               return new Integer(iTimestamp);
            else
               return "";
         }
      case 1: // title or matching text
         if (bIsHeader)
            return searchResults_[index].title;
         else
            return searchResults_[index].getMatchText(element);
      case 2: // type
         if (bIsHeader) {
            String type = "";
            if ((searchResults_[index].where & SearchConstants.TITLES) != 0)
               type += Manager.getLocalized("titlesShort");
            else
               type += "_";
            type += " ";
            if ((searchResults_[index].where & SearchConstants.KEYWORDS) != 0)
               type += Manager.getLocalized("keywordsShort");
            else
               type += "_";
            type += " ";
            if ((searchResults_[index].where & SearchConstants.FULLTEXT) != 0)
               type += Manager.getLocalized("fullTextShort");
            else
               type += "_";
            return type;
         }
      default:
         return "";
      }
   }

   public void setMediaTime(int millis, EventInfo info)
   {
      lastTime_ = millis;
   }
	


	public String getTitle()
   {
      return "SearchHelper";
   }

	public String getDescription()
   {
      return "Searches for keywords in documents.";
   }
   
   public void close(EventInfo info) 
   {
      coordinator_ = null;
   }
												
	public void start(EventInfo info) {}
	public void stop(EventInfo info) {}
	public void pause(EventInfo info) {}
	
	public int getMediaTime() { return 0; }
	public int getDuration() { return 0; }
	public void setDuration(int millis) {}
   public void setInfo(String key, String value) {}
  

   private void requestTime(int time)
   {
      if (time != lastTime_)
      {
         coordinator_.requestTime(time, this);
         lastTime_ = time;
      }
      
   }

}
package imc.lecturnity.converter.wizard;

import imc.lecturnity.converter.DocumentData;
import imc.lecturnity.util.FileUtils;
import imc.lecturnity.util.wizards.WizardPanel;
import imc.lecturnity.util.wizards.Wizard;
import imc.lecturnity.util.wizards.WizardData;

import imc.epresenter.filesdk.util.Localizer;
import imc.epresenter.filesdk.VideoClipInfo;
import imc.lecturnity.util.NativeUtils;

import java.io.*;

import javax.swing.*;
import javax.swing.filechooser.FileFilter;
import javax.swing.border.EtchedBorder;
import javax.swing.border.TitledBorder;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.Container;

import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;

class VideoSelectWizardPanel extends WizardPanel
{
   private static String HEADER = "[!]";
   private static String SUBHEADER = "[!]";
   private static String ICON_RESOURCE = 
      "/imc/lecturnity/converter/images/lecturnity_logo.gif";
   private static String NO_VIDEO = "[!]";
   private static char   MNEM_NO_VIDEO = '?';
   private static String USE_VIDEO = "[!]";
   private static char   MNEM_USE_VIDEO = '?';
   private static String STILL_IMAGE = "[!]";
   private static char   MNEM_STILL_IMAGE = '?';
   private static String MULTI_VIDEO = "[!]";
   private static char   MNEM_MULTI_VIDEO = '?';
   private static String VIDEO_SETTINGS = "[!]";
   private static String FIND = "[!]";
   private static char   MNEM_FIND = '?';
   private static String SELECT_VIDEO = "[!]";
   private static String AVI_DESCRIPTION = "[!]";
   private static String CHOOSER_TITLE = "[!]";

   private static String ERROR = "[!]";
   private static String CONFIRM = "[!]";
   private static String INFORMATION = "[!]";
   private static String WARNING = "[!]";
   private static String FILE_NOT_FOUND = "[!]";
   private static String FILE_EXISTS = "[!]";
   private static String COPY_ERROR = "[!]";
   private static String BACKUP_LRD_SUCCESS = "[!]";
   private static String BACKUP_LRD_FAILED = "[!]";
   private static String ERR_AVISIZE = "[!]";

   private static String COPY_AVI_FILE = "[!]";

   private static String ADD_VIDEO_LRD_ERROR = "[!]";
   private static String REMOVE_VIDEO_LRD_ERROR = "[!]";
   private static String CHANGE_VIDEO_LRD_ERROR = "[!]";
   private static String DEL_TEMP_ERROR = "[!]";
   private static String PANEL_TITLE = "[!]";
   
   private static String IMAGE_DESCRIPTION = "[!]";
   private static String CHOOSER_IMAGE = "[!]";

   private static String EXT_ERROR_AVI = "[!]";
   private static String EXT_ERROR_IMG = "[!]";
   
   static
   {
      try
      {
         Localizer l = new Localizer
            ("/imc/lecturnity/converter/wizard/VideoSelectWizardPanel_",
             "en");
         
         HEADER = l.getLocalized("HEADER");
         SUBHEADER = l.getLocalized("SUBHEADER");
         NO_VIDEO = l.getLocalized("NO_VIDEO");
         MNEM_NO_VIDEO = l.getLocalized("MNEM_NO_VIDEO").charAt(0);
         USE_VIDEO = l.getLocalized("USE_VIDEO");
         MNEM_USE_VIDEO = l.getLocalized("MNEM_USE_VIDEO").charAt(0);
         STILL_IMAGE = l.getLocalized("STILL_IMAGE");
         MNEM_STILL_IMAGE = l.getLocalized("MNEM_STILL_IMAGE").charAt(0);
         MULTI_VIDEO = l.getLocalized("MULTI_VIDEO");
         MNEM_MULTI_VIDEO = l.getLocalized("MNEM_MULTI_VIDEO").charAt(0);
         VIDEO_SETTINGS = l.getLocalized("VIDEO_SETTINGS");
         FIND = l.getLocalized("FIND");
         MNEM_FIND = l.getLocalized("MNEM_FIND").charAt(0);
         SELECT_VIDEO = l.getLocalized("SELECT_VIDEO");
         AVI_DESCRIPTION = l.getLocalized("AVI_DESCRIPTION");
         CHOOSER_TITLE = l.getLocalized("CHOOSER_TITLE");
         IMAGE_DESCRIPTION = l.getLocalized("IMAGE_DESCRIPTION");
         CHOOSER_IMAGE = l.getLocalized("CHOOSER_IMAGE");

         ERROR = l.getLocalized("ERROR");
         CONFIRM = l.getLocalized("CONFIRM");
         INFORMATION = l.getLocalized("INFORMATION");
         WARNING = l.getLocalized("WARNING");
         FILE_NOT_FOUND = l.getLocalized("FILE_NOT_FOUND");
         FILE_EXISTS = l.getLocalized("FILE_EXISTS");

         COPY_AVI_FILE = l.getLocalized("COPY_AVI_FILE");
         COPY_ERROR = l.getLocalized("COPY_ERROR");

         BACKUP_LRD_SUCCESS = l.getLocalized("BACKUP_LRD_SUCCESS");
         BACKUP_LRD_FAILED = l.getLocalized("BACKUP_LRD_FAILED");
         ERR_AVISIZE = l.getLocalized("ERR_AVISIZE");

         ADD_VIDEO_LRD_ERROR = l.getLocalized("ADD_VIDEO_LRD_ERROR");
         REMOVE_VIDEO_LRD_ERROR = l.getLocalized("REMOVE_VIDEO_LRD_ERROR");
         CHANGE_VIDEO_LRD_ERROR = l.getLocalized("CHANGE_VIDEO_LRD_ERROR");
         DEL_TEMP_ERROR = l.getLocalized("DEL_TEMP_ERROR");

         EXT_ERROR_AVI = l.getLocalized("EXT_ERROR_AVI");
         EXT_ERROR_IMG = l.getLocalized("EXT_ERROR_IMG");
      }
      catch (IOException e)
      {
         JOptionPane.showMessageDialog(null, "Could not open Locale database: VideoSelectWizardPanel\n" +
                                       e.toString(),
                                       "Error", JOptionPane.ERROR_MESSAGE);
         System.exit(1);
      }
   }

   private WizardPanel videoSyncWizard_;
   private WizardPanel selectActionWizard_;

   private PublisherWizardData pwData_;

   private JLabel selectDesc_;
   private JTextField videoFileNameField_;
   private JButton findButton_;
   // private JCheckBox useVideoCheckbox_;
   private JPanel videoPanel_;

   private JRadioButton noVideoRadio_;
   private JRadioButton useVideoRadio_;
   private JRadioButton stillImageRadio_;
   private JCheckBox multiVideoBox_;

   private File lastDirectory_;
   
   private ExtendedVideoDialog parent_ = null;
   
   public VideoSelectWizardPanel(ExtendedVideoDialog parent, PublisherWizardData pwData)
   {
      super();

      parent_ = parent;

      pwData_ = pwData;

      setPreferredSize(new Dimension(500, 400)); //(500, 400)

//       addButtons(NEXT_BACK_CANCEL);
      
//       useHeaderPanel(HEADER, SUBHEADER,
//                      ICON_RESOURCE);
      buttonPanel_.showBreakLine(false);

      Container r = getContentPanel();
      r.setLayout(null);

      UseVideoListener useVideoListener = new UseVideoListener();

      boolean useVideo = (pwData_.HasNormalVideo());
      boolean stillImage = (pwData_.HasStillImage());

      noVideoRadio_ = new JRadioButton(NO_VIDEO, !(useVideo||stillImage));
      noVideoRadio_.setSize(410, 22);
      noVideoRadio_.setLocation(30, 30); //(40, 20);
      noVideoRadio_.setMnemonic(MNEM_NO_VIDEO);
      noVideoRadio_.addActionListener(useVideoListener);
      r.add(noVideoRadio_);

      useVideoRadio_ = new JRadioButton(USE_VIDEO, useVideo);
      useVideoRadio_.setSize(410, 22);
      useVideoRadio_.setLocation(30, 60); //(40, 50);
      useVideoRadio_.setMnemonic(MNEM_USE_VIDEO);
      useVideoRadio_.addActionListener(useVideoListener);
      r.add(useVideoRadio_);

      stillImageRadio_ = new JRadioButton(STILL_IMAGE, stillImage);
      stillImageRadio_.setSize(410, 22);
      stillImageRadio_.setLocation(30, 90); //(40, 80);
      stillImageRadio_.setMnemonic(MNEM_STILL_IMAGE);
      stillImageRadio_.addActionListener(useVideoListener);
      r.add(stillImageRadio_);

      multiVideoBox_ = new JCheckBox(MULTI_VIDEO, false);
      multiVideoBox_.setSize(410, 22);
      multiVideoBox_.setLocation(30, 120); // (40, 110);
      multiVideoBox_.setMnemonic(MNEM_MULTI_VIDEO);
      multiVideoBox_.addActionListener(useVideoListener);
      multiVideoBox_.setEnabled(false);
      r.add(multiVideoBox_);

      ButtonGroup buttonGroup = new ButtonGroup();
      buttonGroup.add(noVideoRadio_);
      buttonGroup.add(useVideoRadio_);
      buttonGroup.add(stillImageRadio_);
//       buttonGroup.add(multiVideoBox_);

      videoPanel_ = new JPanel();
//       videoPanel_.setBackground(Wizard.BG_COLOR);
      videoPanel_.setLayout(null);
      videoPanel_.setSize(440, 120); //(460, 120);
      videoPanel_.setLocation(10, 160); //(20, 150);
      videoPanel_.setBorder(new TitledBorder(" " + SELECT_VIDEO + " "));

//       useVideoCheckbox_ = new JCheckBox(USE_VIDEO, pwData_.HasNormalVideo());
//       useVideoCheckbox_.setSize(useVideoCheckbox_.getPreferredSize());
//       useVideoCheckbox_.setLocation(10, -3);
//       useVideoCheckbox_.addActionListener(new UseVideoListener());
//       videoPanel_.add(useVideoCheckbox_);

//       selectDesc_ = new JLabel(SELECT_VIDEO);
//       selectDesc_.setFont(imc.lecturnity.converter.ConverterWizard.createDefaultFont(Font.PLAIN, 12));
//       selectDesc_.setLocation(20, 80);
//       selectDesc_.setSize(420, 15);
//       videoPanel_.add(selectDesc_);

      String sVideoOrStillImageName = "";
      if (pwData_.HasNormalVideo())
         sVideoOrStillImageName = pwData_.GetDocumentData().GetVideoFileName();
      if (pwData_.HasStillImage())
         sVideoOrStillImageName = pwData_.GetDocumentData().GetStillImageName();
      videoFileNameField_ = new JTextField(sVideoOrStillImageName);
      videoFileNameField_.setFont(imc.lecturnity.converter.ConverterWizard.createDefaultFont(Font.PLAIN, 12));
      videoFileNameField_.setLocation(20, 35);
      videoFileNameField_.setSize(400, 22); //(420, 22);
      videoPanel_.add(videoFileNameField_);

      findButton_ = new JButton(FIND);
      if (imc.lecturnity.converter.ConverterWizard.USE_CUSTOM_COLORS)
         findButton_.setBackground(Wizard.BG_COLOR);
      findButton_.setSize(120, 30);
      findButton_.setLocation(20, 63);
      findButton_.setMnemonic(MNEM_FIND);
      findButton_.addActionListener(new FindButtonListener());
      videoPanel_.add(findButton_);

      videoPanel_.setEnabled(useVideo||stillImage);
      videoFileNameField_.setEnabled(useVideo||stillImage);
      findButton_.setEnabled(useVideo||stillImage);
      
      r.add(videoPanel_);

      refreshDataDisplay();
   }

   public String getDescription()
   {
      return "";
   }

   protected WizardPanel getNextWizardPanel()
   {
      return null;
   }

   public PublisherWizardData getWizardData()
   {
      if (useVideoRadio_.isSelected())
      {
         if (videoSyncWizard_ == null)
            videoSyncWizard_ = new VideoSyncWizardPanel(parent_, pwData_);
         
         pwData_.nextWizardPanel = videoSyncWizard_;

         pwData_.GetDocumentData().SetVideoFileName( (new File(videoFileNameField_.getText())).getName() );
      }
      else if (stillImageRadio_.isSelected())
      {
         if (selectActionWizard_ == null)
            selectActionWizard_ = new SelectActionWizardPanel(pwData_);
         
         pwData_.GetDocumentData().SetStillImageName( (new File(videoFileNameField_.getText())).getName() );

         pwData_.nextWizardPanel = selectActionWizard_;
      }
      else if (noVideoRadio_.isSelected())
      {
         if (selectActionWizard_ == null)
            selectActionWizard_ = new SelectActionWizardPanel(pwData_);
         
         pwData_.GetDocumentData().ClearVideoAndStillImage();

         pwData_.nextWizardPanel = selectActionWizard_;
      }
      
      return pwData_;
   }

   public void refreshDataDisplay()
   {
      DocumentData dd = pwData_.GetDocumentData();

      boolean useVideo = dd.HasNormalVideo();
      boolean stillImage = dd.HasStillImage();
      boolean noVideo = !(useVideo || stillImage);
      boolean multiVideo = dd.HasMultipleVideos();

      noVideoRadio_.setSelected(noVideo);
      useVideoRadio_.setSelected(useVideo);
      stillImageRadio_.setSelected(stillImage);

      boolean ok = ((noVideo && !useVideo && !stillImage) ||
                    (!noVideo && useVideo && !stillImage) ||
                    (!noVideo && !useVideo && stillImage));
      if (!ok)
         System.out.println("---> More than one of the options is \"true\"!");

      multiVideoBox_.setSelected(multiVideo);
      // create lable text for multi video check box
      String temp = MULTI_VIDEO;
      if (multiVideo)
      {
         temp += " (";
         for (int i = 0; i < dd.GetVideoClipInfos().length; ++i)
         {
            temp += dd.GetVideoClipInfos()[i].clipFileName;
            if (i < dd.GetVideoClipInfos().length - 1)
               temp += "; ";
         }
         temp += ")";
      }
      multiVideoBox_.setText(temp);

      checkEnabled();

      if (useVideo)
         videoFileNameField_.setText(dd.GetVideoFileName());
      else if (stillImage)
         videoFileNameField_.setText(dd.GetStillImageName());
      else
         videoFileNameField_.setText("");
            
      super.refreshDataDisplay();
   }

   private boolean copyVideoFile()
   {
      // Gather some useful information on the source directory
      File presFile = new File(pwData_.GetDocumentData().GetPresentationFileName());
      File sourceDirFile = presFile.getParentFile();
      String sourceDir = sourceDirFile.toString();

      // check if the new video file really exists
      File srcFile = new File(videoFileNameField_.getText());

      boolean fileIsOk = false;
      fileIsOk = srcFile.exists();

      if (!srcFile.exists())
      {
         // try to add the current directory, is it here already?
         srcFile = new File(sourceDir + File.separatorChar + videoFileNameField_.getText());
         
         // Does the file already exist
         if (srcFile.exists())
            return true;
      }

      if (!fileIsOk)
      {
         JOptionPane.showMessageDialog(this, FILE_NOT_FOUND, ERROR,
                                       JOptionPane.ERROR_MESSAGE);
         return false;
      }

      // good, now check in source directory...
      String srcFileName = srcFile.getName();
      
      String destFileName = sourceDir + File.separatorChar + srcFileName;
      File destFile = new File(destFileName);
      
      if (destFile.exists())
      {
         if (destFile.length() != srcFile.length())
         {
            // there's another file with the same name in source directory!
            int res = JOptionPane.showConfirmDialog
               (this, FILE_EXISTS, CONFIRM,
                JOptionPane.YES_NO_OPTION,
                JOptionPane.WARNING_MESSAGE);
            
            if (res != JOptionPane.YES_OPTION)
               return false;
            
            // copy file to source directory
            if (!copyFile(srcFile, destFile, COPY_AVI_FILE))
               return false;
         }
         // else: file sizes are equal, assume equal files
      }
      else
      {
         if (!copyFile(srcFile, destFile, COPY_AVI_FILE))
            return false;
      }
      
      return true;
   }

   public boolean verifyNext()
   {
      DocumentData dd = pwData_.GetDocumentData();
      boolean hasVideoOrStillImage = (dd.HasNormalVideo() || dd.HasStillImage());
      boolean hasClips = dd.HasMultipleVideos();

      if (useVideoRadio_.isSelected() ||
          stillImageRadio_.isSelected())
      {
         if (!verifyExtensions())
            return false;

         // a video may have been added in two cases:
         // (1) the presentation did not have any kind of video
         //     before,
         // (2) the presentation had SG clips (or any kind of multiple
         //     video files) before; in that case, use the changeLrdFile method
         boolean hasVideoBeenAdded = (!hasVideoOrStillImage || hasClips);

         if (hasVideoBeenAdded)
         {
            // a video has been added, ensure it's here:
            boolean success = copyVideoFile();

            if (!success)
               return false;

            // ok, now we know that we have a valid AVI file in the
            // source directory.

            File srcFile = new File(videoFileNameField_.getText());
            String srcFileName = srcFile.getName();
            
            // fix the LRD file
            if (!hasVideoOrStillImage)
            {
               // no kind of video presently in presentation
               if (!addVideoToLrdFile(srcFileName))
                  return false;
            }
            else
            {
               // we already had sg clips in the presentation:
               if (!changeVideoInLrdFile(srcFileName))
                  return false;
            }

            return true;
         }
         else
         {
            // has video changed? or have SG clips been removed?
            if (!videoFileNameField_.getText().equals(dd.GetVideoFileName()))
            {
               // video file has changed, ensure it's here:
               boolean success = copyVideoFile();
               
               if (!success)
                  return false;
            }
            else
            {
               // do not try to copy files that are already there.
            }

            File srcFile = new File(videoFileNameField_.getText());
            String srcFileName = srcFile.getName();

            if (!changeVideoInLrdFile(srcFileName))
               return false;

            return true;
         }
      }
//       else if (multiVideoBox_.isSelected())
//       {
//          // do nothing here, nothing has changed.
//          return true;
//       }
      else
      {
         // no video has been selected
         if (hasVideoOrStillImage || hasClips)
         {
//             System.out.println("REMOVE!");

            if (hasVideoOrStillImage)
            {
               // the video file has been removed
               if (!removeVideoFromLrdFile())
                  return false;
            }
            else if (hasClips)
            {
               // no video before, any change in sg clips?
               if (!multiVideoBox_.isSelected())
               {
                  // yes, remove SG clips
                  if (!removeVideoFromLrdFile())
                     return false;
               }
            }

            return true;
         }
         else
         {
            // no video before, no video now.
            // Note: No SG clips either.
            return true;
         }
      }
   }
   
   public boolean verifyExtensions()
   {
      String fileName = videoFileNameField_.getText().toUpperCase();

      if (useVideoRadio_.isSelected())
      {
         boolean isOk = fileName.endsWith(".AVI") || fileName.endsWith(".MOV");
         if (!isOk)
            JOptionPane.showMessageDialog(this, 
                                          EXT_ERROR_AVI + videoFileNameField_.getText(), 
                                          ERROR,
                                          JOptionPane.ERROR_MESSAGE);

         return isOk;
      }
      
      if (stillImageRadio_.isSelected())
      {
         boolean isOk = (fileName.endsWith(".JPG") ||
                         fileName.endsWith(".JPEG") ||
                         fileName.endsWith(".GIF"));

         if (!isOk)
            JOptionPane.showMessageDialog(this, 
                                          EXT_ERROR_IMG + videoFileNameField_.getText(), 
                                          ERROR,
                                          JOptionPane.ERROR_MESSAGE);

         return isOk;
      }

      return false;
   }

   private boolean copyFile(File src, File dest, String message)
   {
      try
      {
         FileUtils.copyFile(src, dest, this, message);
         return true;
      }
      catch (IOException e)
      {
         e.printStackTrace();
         JOptionPane.showMessageDialog(this, COPY_ERROR + e.getMessage(),
                                       ERROR, JOptionPane.ERROR_MESSAGE);
         return false;
      }
   }

   private boolean backupLrdFile()
   {
      String fileName = pwData_.GetDocumentData().GetPresentationFileName();
      
      int lastDotPos = fileName.lastIndexOf('.');
      String suffix = fileName.substring(lastDotPos);
      String prefix = fileName.substring(0, lastDotPos);
      
      String origFileName = prefix + "_orig" + suffix;
      
      File origFile = new File(origFileName);
      if (!origFile.exists())
      {
         boolean success = copyFile(new File(fileName), origFile, "");
         if (success)
            JOptionPane.showMessageDialog(this, BACKUP_LRD_SUCCESS + origFileName, 
                                          INFORMATION,
                                          JOptionPane.INFORMATION_MESSAGE);
         else
            JOptionPane.showMessageDialog(this, BACKUP_LRD_FAILED, WARNING,
                                          JOptionPane.WARNING_MESSAGE);

         return success;
      }

      return true;
   }

   private boolean determineVideoSize(String videoFileName)
   {
      // Gather some useful information on the source directory
      File presFile = new File(pwData_.GetDocumentData().GetPresentationFileName());
      File sourceDirFile = presFile.getParentFile();
      String sourceDir = sourceDirFile.toString() + File.separatorChar;
      // Determine width and height of the video
      VideoClipInfo vci = new VideoClipInfo(videoFileName, 0);
      if (NativeUtils.retrieveVideoInfo(sourceDir, vci))
      {
         System.out.println("Video '" + vci.clipFileName + "': " + vci.videoWidth + "x" + vci.videoHeight);
         pwData_.GetDocumentData().SetVideoDimension(vci.videoWidth, vci.videoHeight);
         
         return true;
      }
      
      // It failed; Pop up an error message and return false
      JOptionPane.showMessageDialog(this, ERR_AVISIZE, ERROR,
                                    JOptionPane.ERROR_MESSAGE);
      return false;
   }

   private boolean addVideoToLrdFile(String videoFileName)
   {
      if (!backupLrdFile())
         return false;

      try
      {
         DocumentData dd = pwData_.GetDocumentData();

         // If we have added a video we have to determine its size
         if (useVideoRadio_.isSelected())
         {
            boolean succeeded = determineVideoSize(videoFileName);
            if (!succeeded)
               return false;
         }

         FileInputStream fis = new FileInputStream(dd.GetPresentationFileName());
         BufferedReader in = FileUtils.createBufferedReader(fis, dd.GetCodepage());
         PrintWriter out = FileUtils.CreatePrintWriter(dd.GetPresentationFileName() + ".tmp");

         String line;
         int helperIndex = 0;
         while ((line = in.readLine()) != null)
         {
            if (line.trim().equals("<H>"))
               helperIndex++;

            if (line.trim().equals("</HELPERS>"))
            {
               // ok, insert video helper here
               out.println("      <H>");
               out.println("        <OPT>java</OPT>");
               out.println("        <OPT>imc.epresenter.player.JMFVideoPlayer</OPT>");
               if (stillImageRadio_.isSelected())
                  out.println("        <OPT>-still</OPT>");
               else if (useVideoRadio_.isSelected())
                  out.println("        <OPT>-video</OPT>");
               out.println("        <OPT>" + videoFileName + "</OPT>");
               if (multiVideoBox_.isSelected())
               {
                  out.println("        <OPT>-multi</OPT>");
                  for (int i = 0; i < dd.GetVideoClipInfos().length; ++i)
                  {
                     out.println("        <OPT>" + dd.GetVideoClipInfos()[i].clipFileName + "</OPT>");
                     out.println("        <OPT>" + dd.GetVideoClipInfos()[i].startTimeMs + "</OPT>");
                  }
               }
               out.println("      </H>");
            }

            out.println(line);
         }

         in.close();
         out.flush();
         out.close();

         // copy temporary file back to real LRD file:
         boolean success = copyFile(new File(dd.GetPresentationFileName() + ".tmp"),
                                    new File(dd.GetPresentationFileName()), "");
         // delete temporary file:
         File tempFile = new File(dd.GetPresentationFileName() + ".tmp");
         if (!tempFile.delete())
            JOptionPane.showMessageDialog
               (this, DEL_TEMP_ERROR + dd.GetPresentationFileName() + ".tmp",
                WARNING, JOptionPane.WARNING_MESSAGE);

         dd.SetVideoHelperIndex(helperIndex);
         if (useVideoRadio_.isSelected())
            dd.SetVideoFileName(videoFileName);
         else if (stillImageRadio_.isSelected())
            dd.SetStillImageName(videoFileName);

         return true;
      }
      catch (IOException e)
      {
         JOptionPane.showMessageDialog(this, ADD_VIDEO_LRD_ERROR + e.getMessage(), 
                                       ERROR, JOptionPane.ERROR_MESSAGE);
         e.printStackTrace();
         return false;
      }
   }

   private boolean removeVideoFromLrdFile()
   {
      if (!backupLrdFile())
         return false;

      try
      {
         DocumentData dd = pwData_.GetDocumentData();

         FileInputStream fis = new FileInputStream(dd.GetPresentationFileName());
         BufferedReader in = FileUtils.createBufferedReader(fis, dd.GetCodepage());
         PrintWriter out = FileUtils.CreatePrintWriter(dd.GetPresentationFileName() + ".tmp");

         String line;
         int helperIndex = -1;
         boolean inVideoHelper = false;
         int offsetIndex = -1;
         boolean inOffsets = false;
         boolean inVideoOffset = false;
         while ((line = in.readLine()) != null)
         {
            if (inVideoHelper)
            {
               if (line.trim().equals("</H>"))
               {
                  inVideoHelper = false;
                  // here's the right place to insert an eventual "-multi" video helper
                  if (multiVideoBox_.isSelected())
                  {
                     out.println("      <H>");
                     out.println("        <OPT>java</OPT>");
                     out.println("        <OPT>imc.epresenter.player.JMFVideoPlayer</OPT>");
                     out.println("        <OPT>-multi</OPT>");
                     for (int i = 0; i < dd.GetVideoClipInfos().length; ++i)
                     {
                        out.println("        <OPT>" + dd.GetVideoClipInfos()[i].clipFileName + "</OPT>");
                        out.println("        <OPT>" + dd.GetVideoClipInfos()[i].startTimeMs + "</OPT>");
                     }
                     out.println("      </H>");
                  }
                  
               }
            }
            else if (inOffsets)
            {
               if (line.trim().equals("</OFFSETS>"))
               {
//                   System.out.println("leaving <OFFSETS>");
                  inOffsets = false;
//                   System.out.println(line);
                  out.println(line);
               }
               else if (line.trim().startsWith("<O>"))
               {
                  offsetIndex++;
                  if (offsetIndex != dd.GetVideoHelperIndex())
                  {
//                      System.out.println(line);
                     out.println(line);
                  }
               }
               else
                  throw new IOException("found illegal line in LRD file: " + line);
            }
            else
            {
               if (line.trim().equals("<H>"))
               {
                  helperIndex++;
                  if (helperIndex == dd.GetVideoHelperIndex())
                     inVideoHelper = true;
               }
               else if (line.trim().equals("<OFFSETS>"))
               {
                  inOffsets = true;
               }

               if (!inVideoHelper)
               {
//                   System.out.println(line);
                  out.println(line);
               }
            }
         }

         in.close();
         out.flush();
         out.close();

         // copy temporary file back to real LRD file:
         boolean success = copyFile(new File(dd.GetPresentationFileName() + ".tmp"),
                                    new File(dd.GetPresentationFileName()), "");
         // delete temporary file:
         File tempFile = new File(dd.GetPresentationFileName() + ".tmp");
         if (!tempFile.delete())
            JOptionPane.showMessageDialog
               (this, DEL_TEMP_ERROR + dd.GetPresentationFileName() + ".tmp",
                WARNING, JOptionPane.WARNING_MESSAGE);

         dd.ClearVideoAndStillImage();
         dd.SetVideoHelperIndex(multiVideoBox_.isSelected() ? helperIndex : -1);
      }
      catch (IOException e)
      {
         JOptionPane.showMessageDialog(this, REMOVE_VIDEO_LRD_ERROR + e.getMessage(), 
                                       ERROR, JOptionPane.ERROR_MESSAGE);
         e.printStackTrace();
         return false;
      }

      return true;
   }

   private boolean changeVideoInLrdFile(String newVideoFileName)
   {
      if (!backupLrdFile())
         return false;

      try
      {
         DocumentData dd = pwData_.GetDocumentData();

         // If we have a video we have to determine its size
         if (useVideoRadio_.isSelected())
         {
            boolean succeeded = determineVideoSize(newVideoFileName);
            if (!succeeded)
               return false;
         }

         FileInputStream fis = new FileInputStream(dd.GetPresentationFileName());
         BufferedReader in = FileUtils.createBufferedReader(fis, dd.GetCodepage());
         PrintWriter out = FileUtils.CreatePrintWriter(dd.GetPresentationFileName() + ".tmp");
         
         String line;
         int helperIndex = -1;
         boolean inVideoHelper = false;

         while ((line = in.readLine()) != null)
         {
            if (line.trim().equals("<H>"))
            {
               helperIndex++;
               if (helperIndex == dd.GetVideoHelperIndex())
                  inVideoHelper = true;
               else
                  out.println(line);
            }
            else if (inVideoHelper)
            {
               if (line.trim().equals("</H>"))
               {
                  inVideoHelper = false;
                  // ok, insert video helper with new file name here
                  out.println("      <H>");
                  out.println("        <OPT>java</OPT>");
                  out.println("        <OPT>imc.epresenter.player.JMFVideoPlayer</OPT>");
                  if (stillImageRadio_.isSelected())
                     out.println("        <OPT>-still</OPT>");
                  else if (useVideoRadio_.isSelected())
                     out.println("        <OPT>-video</OPT>");
                  out.println("        <OPT>" + newVideoFileName + "</OPT>");
                  if (multiVideoBox_.isSelected())
                  {
                     out.println("        <OPT>-multi</OPT>");
                     for (int i = 0; i < dd.GetVideoClipInfos().length; ++i)
                     {
                        out.println("        <OPT>" + dd.GetVideoClipInfos()[i].clipFileName + "</OPT>");
                        out.println("        <OPT>" + dd.GetVideoClipInfos()[i].startTimeMs + "</OPT>");
                     }
                  }
                  out.println("      </H>");
               }
               // else: ignore
            }
            else
               out.println(line);
         }

         in.close();
         out.flush();
         out.close();

         // copy temporary file back to real LRD file:
         boolean success = copyFile(new File(dd.GetPresentationFileName() + ".tmp"),
                                    new File(dd.GetPresentationFileName()), "");
         // delete temporary file:
         File tempFile = new File(dd.GetPresentationFileName() + ".tmp");
         if (!tempFile.delete())
            JOptionPane.showMessageDialog
               (this, DEL_TEMP_ERROR + dd.GetPresentationFileName() + ".tmp",
                WARNING, JOptionPane.WARNING_MESSAGE);

         dd.SetVideoFileName(newVideoFileName);
      }
      catch (IOException e)
      {
         JOptionPane.showMessageDialog(this, CHANGE_VIDEO_LRD_ERROR + e.getMessage(), 
                                       ERROR, JOptionPane.ERROR_MESSAGE);
         e.printStackTrace();
         return false;
      }

      return true;
   }

   private void findVideoFile()
   {
      JFileChooser fileChooser = new JFileChooser();
      if (lastDirectory_ != null)
         fileChooser.setCurrentDirectory(lastDirectory_);
      else
      {
         File tmp = new File(pwData_.GetDocumentData().GetPresentationFileName());
         File parentDir = tmp.getParentFile();
         if (parentDir != null)
            fileChooser.setCurrentDirectory(parentDir);
      }

      FileFilter ff = null;
      if (useVideoRadio_.isSelected())
         ff = new VideoFileFilter();
      else // stillImageRadio_.isSelected()
         ff = new StillImageFilter();

      fileChooser.addChoosableFileFilter(ff);
      fileChooser.addChoosableFileFilter(fileChooser.getAcceptAllFileFilter());
      fileChooser.setFileFilter(ff);
      if (useVideoRadio_.isSelected())
         fileChooser.setDialogTitle(CHOOSER_TITLE);
      else // stillImageRadio_.isSelected()
         fileChooser.setDialogTitle(CHOOSER_IMAGE);

      Dimension fcSize = fileChooser.getPreferredSize();
      fileChooser.setPreferredSize(new Dimension(fcSize.width + 100, fcSize.height + 50));

      int action = fileChooser.showOpenDialog(this);

      lastDirectory_ = fileChooser.getCurrentDirectory();

      if (action == fileChooser.APPROVE_OPTION)
      {
         File selectedFile = fileChooser.getSelectedFile();
         videoFileNameField_.setText(selectedFile.getAbsolutePath());
      }
   }

   private void checkEnabled()
   {
      boolean b = (useVideoRadio_.isSelected() || stillImageRadio_.isSelected());

      videoPanel_.setEnabled(b);
//       selectDesc_.setEnabled(b);
      videoFileNameField_.setEnabled(b);
      findButton_.setEnabled(b);

      if (parent_ != null)
      {
         parent_.setVideoTabEnabled(useVideoRadio_.isSelected());
      }
      
      if (multiVideoBox_.isSelected())
      {
         noVideoRadio_.setEnabled(true);
         useVideoRadio_.setEnabled(true);
         stillImageRadio_.setEnabled(true);
         multiVideoBox_.setEnabled(false);
      }
      else
      {
         if (pwData_.HasMultipleVideos())
         {
            noVideoRadio_.setEnabled(true);
            useVideoRadio_.setEnabled(true);
            stillImageRadio_.setEnabled(true);
            multiVideoBox_.setEnabled(false);
         }
         else
         {
            noVideoRadio_.setEnabled(true);
            useVideoRadio_.setEnabled(true);
            stillImageRadio_.setEnabled(true);
            multiVideoBox_.setEnabled(false);
         }
      }
   }

   private class FindButtonListener implements ActionListener
   {
      public void actionPerformed(ActionEvent e)
      {
         findVideoFile();
      }
   }

   private class UseVideoListener implements ActionListener
   {
      public void actionPerformed(ActionEvent e)
      {
         checkEnabled();
      }
   }

   private class VideoFileFilter extends FileFilter
   {
      public boolean accept(File f)
      {
         if (f.isDirectory())
            return true;
         String fileName = f.getName().toUpperCase();
         return (fileName.endsWith(".AVI") ||
                 fileName.endsWith(".MOV"));
      }

      public String getDescription()
      {
         return AVI_DESCRIPTION;
      }
   }
    
   private class StillImageFilter extends FileFilter
   {
      public boolean accept(File f)
      {
         if (f.isDirectory())
            return true;
         String fileName = f.getName().toUpperCase();
         return (fileName.endsWith(".JPG") ||
                 fileName.endsWith(".JPEG") ||
                 fileName.endsWith(".GIF"));
      }
      
      public String getDescription()
      {
         return IMAGE_DESCRIPTION;
      }
   }
}

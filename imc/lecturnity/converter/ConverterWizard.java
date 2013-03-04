package imc.lecturnity.converter;

/*
 * File:             ConverterWizard.java
 * Code conventions: 18.09.2001
 * Author:           Martin Danielsson
 *
 * $Id: ConverterWizard.java,v 1.106 2010-04-30 13:49:22 ziewer Exp $
 */

import imc.lecturnity.converter.wizard.*;
import imc.lecturnity.util.NativeUtils;
import imc.lecturnity.util.ColorManager;
import imc.lecturnity.util.FileUtils;
import imc.lecturnity.util.ui.SplashScreen;

import java.awt.*;
import java.io.File;
import java.io.IOException;

import javax.swing.*;

import java.util.Date;
import java.text.SimpleDateFormat;
import java.text.ParsePosition;


import imc.epresenter.filesdk.util.*;
import imc.epresenter.converter.PresentationConverter;
import imc.lecturnity.converter.ProfiledSettings;
import imc.lecturnity.util.wizards.*;

/* REVIEW UK
 *  ps.AdjustTargetNameAndDirectory(dd);
 *   -> Is also/alternatively done in (for example)
 *      in SelectPresentationWizardPanel.DetermineTargetPathsAndFiles(m_pwData, this);
 *      called from the WriterWizards.
 *   -> This should be done centrally.

/**
 * This class is used to convert Lecturnity Recording Documents into
 * other formats, like the Lecturnity Presentation Document format or
 * into the RealMedia Format. It uses a {@link
 * imc.lecturnity.util.wizards.Wizard Wizard} instance.
 *
 * @see imc.lecturnity.util.wizards.Wizard
 * @author Martin Danielsson
 */
public class ConverterWizard implements WizardListener, Runnable
{
   public static boolean m_bGenericProfilesEnabled = true;
   
//   private static String EXPIRED = "[!]";
//   private static String EVAL_PERIOD = "[!]";
//   private static String DAY = "[!]";
//   private static String DAYS = "[!]";
//   private static String EXP_CAPTION = "[!]";
   private static String DLL_ERROR = "[!]";
   private static String ERROR = "[!]";
   private static String WARNING = "[!]";   
   private static String VERSION_ERROR = "[!]";
//   private static String EXP_DATE = "[!]";
   private static String DEFAULT_FONT = "[!]";
   private static String DEFAULT_FONT_SIZE = "[!]";
   private static String MONOSPACED_FONT = "[!]";
   private static String PROFILE_ERROR_LOAD = "[!]";
   private static String PROFILE_ERROR_WRITE = "[!]";
   private static String DOC_ERROR_LOAD = "[!]";
   private static String PROFILE_TYPE_ERROR = "[!]";
   private static String CONTINUE = "[!]";
   private static String CANCEL = "[!]";   
//   private static String NO_ACTIVE_PROFILE = "[!]";   
   private static String ONLY_FLASH_SUPPORTED = "[!]";   
//   private static String IN_DPT_MODE = "[!]"; 
   private static String LIVECONTEXT_DIRECT = "[!]";
   

   private static int VERSION_TYPE_ = NativeUtils.UNIVERSITY_EVALUATION_VERSION;
   public static final boolean USE_CUSTOM_COLORS = false;

   private static boolean hasCdPublisher_  = false;
   private static boolean hasRealMedia_    = false;
   private static boolean hasWindowsMedia_ = false;
   private static boolean hasMp4Publisher_ = false;

    private static boolean s_bErrorOnStartup = false;

   static
   {
      if (imc.lecturnity.converter.ConverterWizard.USE_CUSTOM_COLORS)
         ColorManager.setImcColors();

      try
      {
         Localizer l = new Localizer(
            "/imc/lecturnity/converter/ConverterWizard_", "en");

//         EXPIRED = l.getLocalized("EXPIRED");
//         EVAL_PERIOD = l.getLocalized("EVAL_PERIOD");
//         DAY = l.getLocalized("DAY");
//         DAYS = l.getLocalized("DAYS");
//         EXP_CAPTION = l.getLocalized("EXP_CAPTION");
         DLL_ERROR = l.getLocalized("DLL_ERROR");
         ERROR = l.getLocalized("ERROR");
         WARNING = l.getLocalized("WARNING");
         VERSION_ERROR = l.getLocalized("VERSION_ERROR");
//         EXP_DATE = l.getLocalized("EXP_DATE");
         DEFAULT_FONT = l.getLocalized("DEFAULT_FONT");
         DEFAULT_FONT_SIZE = l.getLocalized("DEFAULT_FONT_SIZE");
         MONOSPACED_FONT = l.getLocalized("MONOSPACED_FONT");
         PROFILE_ERROR_LOAD = l.getLocalized("PROFILE_ERROR_LOAD");
         PROFILE_ERROR_WRITE = l.getLocalized("PROFILE_ERROR_WRITE");
         DOC_ERROR_LOAD = l.getLocalized("DOC_ERROR_LOAD");
         PROFILE_TYPE_ERROR = l.getLocalized("PROFILE_TYPE_ERROR");
         CONTINUE = l.getLocalized("CONTINUE");
         CANCEL = l.getLocalized("CANCEL");
//         NO_ACTIVE_PROFILE = l.getLocalized("NO_ACTIVE_PROFILE");
         ONLY_FLASH_SUPPORTED = l.getLocalized("ONLY_FLASH_SUPPORTED");
//         IN_DPT_MODE = l.getLocalized("IN_DPT_MODE");
         LIVECONTEXT_DIRECT = l.getLocalized("LIVECONTEXT_DIRECT");
         
      }
      catch (IOException e)
      {
         JOptionPane.showMessageDialog(null, "Could not open Locale database!\n" +
                                       "ConverterWizard\n" +
                                       e.toString(),
                                       "Error", JOptionPane.ERROR_MESSAGE);
         System.exit(-1);
      }

      // Set default font for Java GUI stuff
      Font fnt = createDefaultFont(Font.PLAIN, 12);
      UIManager.put("Button.font", fnt);
      UIManager.put("Label.font", fnt);
      UIManager.put("Panel.font", fnt);
      UIManager.put("RadioButton.font", fnt);
      UIManager.put("CheckBox.font", fnt);
      UIManager.put("TabbedPane.font", fnt);
      UIManager.put("TitledBorder.font", fnt);
      UIManager.put("TextField.font", fnt);
      UIManager.put("ComboBox.font", fnt);
      UIManager.put("OptionPane.messageFont", fnt);
      UIManager.put("OptionPane.buttonFont", fnt);
    }

   public static void main(String[] args)
   {   
      String strEnvParameters = System.getenv("LECTURNITY_PARAMETERS");
      if (strEnvParameters != null && strEnvParameters.length() > 1)
      {
         // javaw.exe cannot handle unicode characters: use the environment
         // variable instead as args
       
         args = FileUtils.SplitCommandLine(strEnvParameters);
      }
  
      ConverterWizard cw = new ConverterWizard(args);
      

      
      if (s_bErrorOnStartup) {
          // Startup might display an error; wait for that
          while (cw.IsErrorDisplayActive()) {
              try { Thread.sleep(100); } catch (InterruptedException exc) {}
          }
      
          System.exit(-1);
      }
      // else it will be the standard 0
   }

   /**
    * Returns the version type of the Converter instance. For values
    * that may be returned here, see the {@link
    * imc.lecturnity.util.NativeUtils NativeUtils} class.
    */
   public static int getVersionType()
   {
      return VERSION_TYPE_;
   }

   /**
    * If a font is needed for UI purposes, use this method.
    */
   public static Font createDefaultFont(int style, int size)
   {
      // Default font: "Arial"; default font size: 12
      int defaultSize = Integer.parseInt(DEFAULT_FONT_SIZE);
      float fScale = (float)defaultSize / 12.0f;
      size = Math.round( fScale * size );
      
      return new Font(DEFAULT_FONT, style, size);
   }

   /**
    * If a monospaced font is needed for UI purposes, use this method.
    */
   public static Font createMonospacedFont(int style, int size)
   {
      // Default font: "Courier New"; default font size: 12
      int defaultSize = Integer.parseInt(DEFAULT_FONT_SIZE);
      float fScale = (float)defaultSize / 12.0f;
      size = Math.round( fScale * size );
      
      return new Font(MONOSPACED_FONT, style, size);
   }

   
   private boolean m_bProfileMode = false;
   private Wizard m_ProfileWizard = null;
   private PublisherWizardData m_ProfileData = null;
   private String m_strAsyncError = null;
   private boolean m_bIsAsyncWarning = false;
   private boolean m_bIsErrorActive = false;
   
   /**
    * Creates a new instance of the <tt>ConverterWizard</tt>
    * class. The first argument string may be the file name of a
    * Lecturnity Recording Document.
    */
   public ConverterWizard(String[] args)
   {         
//      String argList = "";
//      for(String arg: args) {
//          argList += arg+" ";
//      }
//      JOptionPane.showMessageDialog(null, argList);
      try
      {
         javax.swing.UIManager.setLookAndFeel("com.sun.java.swing.plaf.windows.WindowsLookAndFeel");

         UIDefaults uid = UIManager.getLookAndFeelDefaults();

         java.util.Enumeration enumeration = uid.keys();
         while (enumeration.hasMoreElements())
         {
            Object o = enumeration.nextElement();
            Object v = uid.get(o);
            if (v instanceof java.awt.Font)
            {
               Font oldFont = (Font) v;
               Font font = createDefaultFont(oldFont.getStyle(), oldFont.getSize());

               uid.put(o, font);
            }
         }
      }
      catch (Exception e)
      {
         // if we're not on windows, the above code will not work.
      } 
 
      // check for Cd Publisher, WindowsMedia and RealMedia
      String hasCdPublisher  = System.getProperty("opt.cd");
      String hasRealMedia    = System.getProperty("opt.real");
      String hasWindowsMedia = System.getProperty("opt.wmt");
      String hasMp4Video     = System.getProperty("opt.mp4");

      if (hasCdPublisher  != null && hasCdPublisher.equals("true"))
         hasCdPublisher_  = true;
      if (hasRealMedia    != null && hasRealMedia.equals("true"))
         hasRealMedia_    = true;
      if (hasWindowsMedia != null && hasWindowsMedia.equals("true"))
         hasWindowsMedia_ = true;
      if (hasMp4Video     != null && hasMp4Video.equals("true"))
         hasMp4Publisher_ = true;
      
      boolean bFirstArgumentIsCommand = args.length > 0 && args[0].startsWith("-");
      
      boolean bCallAsCdPublisher = bFirstArgumentIsCommand && args[0].equalsIgnoreCase("-cdproject");
      boolean bCallWithLcp = args.length > 0 && args[0].toUpperCase().endsWith(".LCP");
      boolean bIsCdPublisher = hasCdPublisher() && (bCallAsCdPublisher || bCallWithLcp);
      
      boolean bIsProfileMode = bFirstArgumentIsCommand && args[0].equalsIgnoreCase("-profile");
      boolean bIsDirectMode = bFirstArgumentIsCommand && args[0].equalsIgnoreCase("-direct");
      boolean bIsPowerTrainerMode = bFirstArgumentIsCommand && args[0].equalsIgnoreCase("-dpt");
      boolean bStartWithSelect = !bIsProfileMode && !bIsDirectMode && !bIsPowerTrainerMode;
      boolean bIsAdvancedMode = false; // will be set later
      
      if (bIsPowerTrainerMode)
          bIsDirectMode = true;
      // an profile should be generated automatically according to the specified document
      
      if (!bIsDirectMode) {
          // This assumes that the proper start by/from LIVECONTEXT specifies -direct
          
          if (NativeUtils.failWithLiveContextInstallation(LIVECONTEXT_DIRECT))
              System.exit(-1);
          // If it is a LIVECONTEXT installation the Publisher will not start without proper parameters
      }
      
      
      int type = checkVersionType();
      if (bStartWithSelect)  {
         if (NativeUtils.lecturnityUpdateCheck() != 0) {
            System.exit(-1);
         }

         boolean bAlright = true;
         
         if ((type & NativeUtils.EVALUATION_VERSION) > 0)
            bAlright = checkEvaluationVersion();
         
         if (!bAlright)
             System.exit(-1);

         if (type == NativeUtils.FULL_VERSION ||
             type == NativeUtils.UNIVERSITY_VERSION)
            bAlright = checkFullVersionLimit();
         
         if (!bAlright)
             System.exit(-1);
      }
      // else
      // all these checks have been done before; 
      // eval dialog interferes with modal profile dialog
      
      // NOTE: The Publisher does not show a regular splash screen because this
      //       would interfere/double with the calling Studio which also showed a splash screen.
      
      // TODO: This should also be true for non-expired evaluation and runtime versions if called
      //       from Studio.
      
      VERSION_TYPE_ = type;

      // hack for the LadFileReader
      System.setProperty("nyckel.musik", "forte");

//      // create all default profiles
//      for (int i = PublisherWizardData.DOCUMENT_TYPE_AUDIO_PAGES; i <= PublisherWizardData.DOCUMENT_TYPE_DENVER ; i++) {
//         for (int j = PublisherWizardData.EXPORT_TYPE_LECTURNITY; j <= PublisherWizardData.EXPORT_TYPE_VIDEO; j++) {
//            DefaultProfiles.CreateDefaultProfile(i * 100 + j);            
//         }         
//      }

      if (!bIsCdPublisher)
      {
         // Profile mode or (old) standard mode
         
         /* Debugging easy
         if (bFirstArgumentIsCommand && !bIsProfileMode)
         {     
            UploaderAccess.TransferSettings ts = new UploaderAccess.TransferSettings();
            ts.m_strServer = "10.10.0.10";
            ts.m_iProtocol = 1;
            ts.m_iPort = 22;
            ts.m_strUser = "tester";
            ts.m_strPass = "testeR";
            ts.m_strDir = "/apache/htdocs/podcast/";
            
            System.out.println("Starting...");
            UploaderAccess.StartUpload(ts, new String[] { "c:\\install.log.txt" });
            while (UploaderAccess.GetUploadProgress() < 100)
            {
               try { Thread.sleep(50); } catch (InterruptedException exc) {}
            }
            System.out.println("Done.");
       
            // unknown command?
            System.err.println("Lecturnity Publisher:");
            System.err.println("  Unknown option: " + args[0]);
            System.exit(0);
         }
         */
         
         String strInputLrd = null;
         long iProfileId = 0; // means: no profile (to load) specified
         String strProfileFile = null; // used for Live Context profiles that are not located in LECTURNITY folders   
         int iExportFormat = PublisherWizardData.EXPORT_TYPE_UNDEFINED;
         
         // initialize Communicator in Power Trainer mode
         DptCommunicator dptCommunicator = null;
         if(bIsPowerTrainerMode)
            dptCommunicator = new DptCommunicator();
                            
         PublisherWizardData pd = new PublisherWizardData();
         // Initialize a Progress Manager for PublisherWizardData
         pd.m_progressManager =  new ProgressManager();
          
          
         boolean bIsPowerTrainerControlsActive = false;
          
         if (bStartWithSelect)
         {  
            if (args.length > 0) {
               strInputLrd = args[0];
               // BUGFIX 5486: publish advanced should open "Select Target Dialog" 
               //              instead of "Select Presentation Dialog"
               bStartWithSelect = false;
               bIsAdvancedMode = true;
            }
         }
         else
         {
            if (bIsProfileMode) {
               // command line of the form "-profile <id> <lrd-name>"
            
               if (args.length >= 3)
               {
                  // second parameter should be a profile id to load
                  // (or "0" if no profile should be loaded)
                  
                  try
                  {
                     iProfileId = Long.parseLong(args[1]);
                  }
                  catch (NumberFormatException exc)
                  {
                     System.err.println("Unknown profile parameter; not an id number: "+args[1]);
                     // ignore; iProfileId remains 0
                  }
         
                  strInputLrd = args[2];
               }
               else if (args.length == 2)
               {
                  strInputLrd = args[1];
                  // iProfileId remains 0
               }
               
            } else if (bIsDirectMode) {
//               ProfiledSettings.ProfileInfo pi = ProfiledSettings.GetActiveProfile();
//               if (pi != null)
//                  iProfileId = pi.m_id;
//               else
//                  // TODO: this is probably wrong because direct mode can create select a default profile 
//                  bStartWithSelect = true; // no profile selected: can't be direct
//               // TODO: This (profile missing) should be highlighted somehow
//               // TODO: lateron this last option should ask for a profile (-> selection dialog)
//               // 

               // <ExportFormat> as specified by PublisherWizardData.EXPORT_TYPEs or -1 for NOT_SPECIFIED                
                if (args.length > 3) {
                    // maybe "-direct <lrd-path> -output <output-path>"
                    // or    "-dpt <lrd-path> -config controls-on"
                    strInputLrd = args[1];
                
                    if (args[2].equalsIgnoreCase("-output")) {
                        System.setProperty("profile.output", args[3]);
                        // "Direct mode" and option "-output" together 
                        // indicate "Live Context" mode 
                        pd.m_bIsLiveContext = true;
                        // Will be used below
                    } else if (args[2].equalsIgnoreCase("-config")) {
                        if (args[3].equalsIgnoreCase("flash-control-on"))
                            bIsPowerTrainerControlsActive = true;
                    } else {
                        // TODO Some sort of proper error?
                        System.err.println("Parameter not recognized: "+args[2]);
                    }
                    
                    // Double code below
                    if (bIsPowerTrainerMode) {
                        iExportFormat = PublisherWizardData.EXPORT_TYPE_FLASH;
                        bStartWithSelect = false;
                    } else {
                                       
                        ProfiledSettings.ProfileInfo pi = ProfiledSettings.GetActiveProfile();
                        if (pi != null && pi.m_id != 0) {
                            iProfileId = pi.m_id;
                            if (pd.m_bIsLiveContext)
                                strProfileFile = pi.m_strFile;
                            bStartWithSelect = false;
                        } else {
                            bStartWithSelect = true;                           
                        }
                    }
                } else if (args.length == 3) {                   
                    // command line of the form "-direct <ExportFormat> <lrd-path>"
                    try {                     
                        // read ID specified in direct mode
                        iExportFormat = Integer.parseInt(args[1]);
                        if(iExportFormat == PublisherWizardData.EXPORT_TYPE_UNDEFINED) {
                            JOptionPane.showMessageDialog(null, "Should never happen: Publisher was called with '-direct -1'");
                            return;
                        } else
                            bStartWithSelect = false;
                    } catch (NumberFormatException e) {}
                    // read LRD file name
                  strInputLrd = args[2];
               } else if (args.length == 2) { 
                   // command line of the form "-direct <lrd-name>" or "-dpt ..."
                  strInputLrd = args[1];
                   
                   if (bIsPowerTrainerMode) {
                       iExportFormat = PublisherWizardData.EXPORT_TYPE_FLASH;
                       bStartWithSelect = false;
                   } else { //if(iProfileId == 0) {
                       ProfiledSettings.ProfileInfo pi = ProfiledSettings.GetActiveProfile();
                       if (pi != null && pi.m_id != 0) {
                           iProfileId = pi.m_id;
                           bStartWithSelect = false;
                       } else {
                           bStartWithSelect = true;                           
                       }
                  }
               }
               else
                  bStartWithSelect = true; // should not happen; just for testing
            } /* Not needed anymore: profile should be created dynamically
            else if (bIsPowerTrainerMode) {
               ProfiledSettings.ProfileInfo pi = ProfiledSettings.GetActiveProfile();
               if (pi != null)
                  iProfileId = pi.m_id;
               else {
                  // error: in Dynamic Power Trainer mode there must be a active profile
                  dptCommunicator.ShowErrorMessage(NO_ACTIVE_PROFILE);
               }
               strInputLrd = args[1];            
            }*/
         }
            
         // might not be needed/used lateron
         WizardPanel wpNormal = SelectPresentationWizardPanel.createInstance(pd);
         
         if (strInputLrd != null)
         {
            // BUGFIX 5132 - add finalize check
            // BUGFIX 5255 - no finalize check if format selection only
            // IMPROVEMENT 5176 - call finalize in Power Trainer mode
            if(bIsDirectMode || bIsPowerTrainerMode) {
               String strAutoFinLrd = 
                    SelectPresentationWizardPanel.GetAutomaticFinalizedLrd(strInputLrd, null, dptCommunicator);
               if (strAutoFinLrd != null)
                  strInputLrd = strAutoFinLrd;
               else
                  return; // either cancelled or Editor/Studio started; do not continue
            }
            
            pd.GetDocumentData().SetPresentationFileName(strInputLrd);
            
            ((SelectPresentationWizardPanel)wpNormal).checkConsistencyOnDisplay();
            // Only sets variable...
         }
         
         // TODO: reorder the below: read profile first (if specified) and then deal with it
         
         
         if (bStartWithSelect)
         {
             new Wizard(wpNormal, this, null, wpNormal.getFrameIcon()); 
         }
         else
         {
            if (strInputLrd == null || strInputLrd.length() == 0)
            {
               System.err.println("Lecturnity Publisher:");
               System.err.println("  An LRD file must be specified in profile mode.");
                s_bErrorOnStartup = true;
            }
            
            if (bIsProfileMode) {
               // Remember this for following dialogs
               pd.m_bIsProfileMode = true;
               m_bProfileMode = true;
            }
            
            // BUGFIX 5140: Direct Publish should use different dir (analog to Profile Mode)
            pd.m_bIsDirectMode = bIsDirectMode;
            
            pd.m_bIsPowerTrainerMode = bIsPowerTrainerMode;
            
            boolean bSuccessRead = strInputLrd != null && strInputLrd.length() > 0;
            if (bSuccessRead) {
               String strFinalizedLrd = NativeUtils.getFinalizedPathName(strInputLrd);
               if (strFinalizedLrd != null && strFinalizedLrd.length() > 0)
                  strInputLrd = strFinalizedLrd;
               
                // Note: The "direct" mode hopefully has a finalize step before calling this.
                // Note also: This is more or less double code to SelectPresentationWizardPanel.verifyNext().
               
               bSuccessRead = pd.ReadDocumentDataFromPresentationFile(strInputLrd);
               // this is what SelectPresentationWizardPanel.verifyNext() does
            }
            
            
            // TODO: Maybe solve this mode (profile or direct mode) like this:
            //       Invoke SelectPresentationWizardPanel but instantly advance from there.
            
            
            if (!bSuccessRead)
            {
               ShowModalErrorMessage(DOC_ERROR_LOAD + "\n" + strInputLrd, false);
                s_bErrorOnStartup = true;
            }
               
            boolean bSuccessProfile = true;
            
            if (bSuccessRead && !bIsAdvancedMode)
            {
               int iSuccess = 0;
               if ( bIsDirectMode && iExportFormat >= 0 ) 
                   iProfileId = DefaultProfiles.GetProfileId(iExportFormat, bIsPowerTrainerMode);

               // Note: removed conditional because Generic Default Profile for LPD has "iProfileId==0"
               //if (iProfileId != 0 ) 
                  // load profile and set as activated if in direct mode
                  iSuccess = pd.GetProfiledSettings().Load(iProfileId, strProfileFile);
                  // this is what SelectPresentationWizardPanel.getWizardData() does
               bSuccessProfile = iSuccess == 0;
                
                if (bSuccessProfile && System.getProperty("profile.output") != null) {
                    File fileOutput = new File(System.getProperty("profile.output"));
                    // TODO some sort of check? -> getParentFile().exists() -> !isDirectory()
                    
                    pd.m_bFixedOutputPathGiven = true;
                    
                    pd.GetProfiledSettings().SetStringValue(
                        "strTargetDirLocal", fileOutput.getParent() + File.separator);
                    pd.GetProfiledSettings().SetStringValue(
                        "strTargetPathlessDocName", 
                        FileUtils.extractNameFromPathFile(fileOutput.getName()));
                }
                   
                if (bSuccessProfile && bIsPowerTrainerMode) {
                    // iShow: "0" means "show" and "2" means "hide"
                    int iShow = PublisherWizardData.GUI_ELEMENT_HIDDEN;
                    if (bIsPowerTrainerControlsActive)
                        iShow = PublisherWizardData.GUI_ELEMENT_ACTIVE;
                    DefaultProfiles.SetControlVisibilityForPowertrainer(pd.GetProfiledSettings(), iShow);
                }
            }
                  
            if (bSuccessRead) {  
               ProfiledSettings ps = pd.GetProfiledSettings();
               DocumentData dd = pd.GetDocumentData();
               
               if (bSuccessProfile) {
                  if (bIsDirectMode || bIsPowerTrainerMode) {
                     if( bIsPowerTrainerMode && (ps.GetIntValue("iExportType") != PublisherWizardData.EXPORT_TYPE_FLASH))
                        dptCommunicator.ShowErrorMessage(ONLY_FLASH_SUPPORTED);
                     
                     boolean bTypesMatch = 
                        SelectPresentationWizardPanel.CheckDocumentTypes(ps, dd, null, false);
                  
                     if (bTypesMatch) {
                        // PZI: prepare automated structuring of Denver documents or reset
                        dd.setAddClipStructure(ps.GetBoolValue("bStructuredClips"));
                        dd.updateForStructuredScreengrabbing(ps);

                        // adjust path settings for Default Profiles and Generic Profiles
                        if (!pd.m_bFixedOutputPathGiven)
                            ps.AdjustTargetNameAndDirectory(dd);

                        String strTargetDocName = dd.GetPresentationFileName();
                        strTargetDocName = FileUtils.extractNameFromPathFile(strTargetDocName);
                        
                        if( dptCommunicator != null )
                           pd.m_dptCommunicator = dptCommunicator;

                        //
                        // Apply checks of SelectPresentationWizardPanel.verifyNext()
                        // (see there: checks in case of profile mode)
                        //
                        // Bugfix 5250: Check target path and file name for unsupported characters 
                        boolean bSuccess = true;
                        if (bSuccess) {
                           // Check (local) target path
                           String strTargetPath = ps.GetStringValue("strTargetDirLocal");
                           // Check for not supported (RFC 1630, RFC 3986) characters in the path
                           boolean bIsPathUrlOk = FileUtils.checkUrlPathName(strTargetPath);
                           if (!bIsPathUrlOk)
                              bSuccess = SelectPresentationWizardPanel.ContinueAfterNonSupportedCharacterCheckInPath(null, strTargetPath);

                           if (bSuccess) {
                              // Check target file name
                              // Check for not supported (non ASCII) characters in document name
                              boolean bIsFileUrlOk = FileUtils.checkUrlFileName(strTargetDocName);
                              if (!bIsFileUrlOk) {
                                 String strNewTargetDocName = 
                                    SelectPresentationWizardPanel.GetTargetFilenameAfterNonSupportedCharacterCheck(null, strTargetDocName);
                                 if ((strNewTargetDocName == null) || (strNewTargetDocName.length() == 0)) {
                                    bSuccess = false;
                                 } else {
                                    // set "strTargetPathlessDocName"
                                    ps.SetStringValue("strTargetPathlessDocName", strNewTargetDocName);
                                 }
                              }
                           }
                        }
                        if (bSuccess) {
                           // Bugfix 4160: Check for Logo image name and if the specified file exists
                           // (only necessary for Flash export)
                           String strLogo = ps.GetStringValue("strLogoImageName");
                           bSuccess = SelectPresentationWizardPanel.ContinueAfterLogoImageCheck(null, strLogo);
                        }
                        // Exit in case of no success for checks of SelectPresentationWizardPanel.verifyNext()
                        if (!bSuccess)
                           System.exit(-1);
                           
                        WizardPanel wpd = SelectPresentationWizardPanel.GetWriterWizardPanel(pd);
                        new Wizard(wpd, this, null, wpd.getFrameIcon(), bIsPowerTrainerMode);
                        
                     } else
                         s_bErrorOnStartup = true;
                     // else is already handled (Where?)
                  } else if (bIsProfileMode || bIsAdvancedMode) {
                     // profile change mode: start with select action
                     // BUGFIX 5486: "Publish advanced" should open "Select Format Dialog"
		             pd.m_bDisableSelectPresentation = true;
                     SelectActionWizardPanel sawp = new SelectActionWizardPanel(pd);
               
                     m_ProfileWizard = new Wizard(sawp, this, null, sawp.getFrameIcon());
                     m_ProfileData = pd;
                  }
               }
               

               if (!bSuccessProfile)
               {
                  ShowModalErrorMessage(PROFILE_ERROR_LOAD + " " + iProfileId, false);
                   s_bErrorOnStartup = true;
               }
               else
               {
                  if (bIsProfileMode) {
                     // Executed AFTER showing the SelectActionWizardPanel
                     // because then a proper window for showing the message on top exists
                     boolean bTypesMatch =
                        SelectPresentationWizardPanel.CheckDocumentTypes(ps, dd, m_ProfileWizard, true);
                     
                     if (!bTypesMatch) {
                        ShowModalErrorMessage(PROFILE_TYPE_ERROR, true);
                     
                        pd.GetProfiledSettings().SetType(dd.GetDocumentType());
                        // either it is cancelled (doesn't matter) or continued
                        // (new type needed)
                         
                         // s_bErrorOnStartup = true;
                         // Do not set error here: It is only important for
                         // LiveContext start (which should not get here). And the above
                         // is a warning message which also can be ignored/continued.
                         
                         // TODO s_bErrorOnStartup is a hack and should be made
                         //      unified with some sort of "LiveContext start mode".
                         //      (Can currently not be identified.)
                         // See wait loop in main().
                     }
                                    
                     // Note: The profile type does not need to be compared to the
                     // document type here (as is done in SelectPresentationWizardPanel.verifyNext().
                     // Either it is a new profile (none loaded) or it is an edit and
                     // then the type can be changed.
                     
                     // TODO: 9.7.09 ???
                  } // else already done above
               }
                 
            }
            
         }
      }
      else // bIsCdPublisher
      {
         String strLcpFilename = null;
         
         if (bCallWithLcp)
         {
            strLcpFilename = args[0];
          }
         else if (bCallAsCdPublisher && args.length > 1)
         {
            strLcpFilename = args[1];
         }
            
         CdProjectWizardData data = new CdProjectWizardData();
         WizardPanel wp = null;
         
         if (strLcpFilename != null)
         {
            data.projectFileName = strLcpFilename;
            
            wp = new CdProjectSettingsWizardPanel(data);
            ((CdProjectSettingsWizardPanel)wp).readProjectOnDisplay();
         }
         else
         {
            wp = new CdProjectWizardPanel(data);
         }
          
         new Wizard(wp, this, null, wp.getFrameIcon());
      }
      
   }
   
   public boolean IsErrorDisplayActive() {
       return m_bIsErrorActive;
   }
   
   public void ShowModalErrorMessage(String strError, boolean bIsWarning)
   {
      // Note: It is important that error messages (during startup)
      // are shown from the AWT event queue. Otherwise their modality will
      // conflict with the modality of the C++ in the background somehow.
      
      m_strAsyncError = strError;
      m_bIsAsyncWarning = bIsWarning;
      m_bIsErrorActive = true;
                     
      java.awt.EventQueue.invokeLater(this);
   }
   
   public void run()
   {
      boolean bFinishWizard = !m_bIsAsyncWarning; // if error finish anyway
      
      if (m_bIsAsyncWarning)
      {
         Object[] options = { CONTINUE, CANCEL };
         
         int res = 
            JOptionPane.showOptionDialog(m_ProfileWizard, m_strAsyncError,
                                         WARNING, JOptionPane.YES_NO_OPTION, 
                                         JOptionPane.WARNING_MESSAGE, null, 
                                         options, options[1]);
         
         if (res != 0) // Not 'continue'
            bFinishWizard = true;
      }
      else
      {
         JOptionPane.showMessageDialog(m_ProfileWizard, m_strAsyncError, 
                                       ERROR, JOptionPane.ERROR_MESSAGE);
      }
      
      if (bFinishWizard)
      {
         WizardEvent evt = new WizardEvent(
            m_ProfileWizard, WizardEvent.CANCELLED, null);
                  
         wizardFinished(evt);
      }
      
      m_bIsErrorActive = false;
   }
   
   /**
    * This method is called after the <tt>Wizard</tt> has finished.
    * @see imc.lecturnity.util.wizard.WizardListener
    */
   public void wizardFinished(WizardEvent e)
   {
      if (m_bProfileMode)
      {
         if (e.getEvent() == WizardEvent.FINISHED)
         {
             ProfiledSettings ps = m_ProfileData.GetProfiledSettings();
             
             boolean bIsGenericProfile = ConverterWizard.m_bGenericProfilesEnabled && ps.GetType() == PublisherWizardData.DOCUMENT_TYPE_UNKNOWN;
             boolean bIsOldFlash = ps.GetIntValue("iFlashTemplateType") == PublisherWizardData.TEMPLATE_FLASH_NORMAL;
             boolean bIsPspFlash = ps.GetIntValue("iFlashTemplateType") == PublisherWizardData.TEMPLATE_FLASH_PSP;
             if ( bIsGenericProfile && (bIsOldFlash || bIsPspFlash)) {
                 // Generic Profiles cannot handle old Flash player and PSP because size computation depends on document type
                 // hence set document type, i.e. create old style profile (for specific document type)
                 // Note: compare special case in constructor of TargetChoiceWizardPanel()
                 ps.SetType(m_ProfileData.GetDocumentData().GetDocumentType());
             }
             
            int iError = ps.Write();
            
            if (iError != 0)
            {
               JOptionPane.showMessageDialog(m_ProfileWizard, 
                                             PROFILE_ERROR_WRITE, ERROR, 
                                             JOptionPane.ERROR_MESSAGE);
            }
         }
         
         // make sure the Publisher does not quit: no exit()
         
         if (m_ProfileWizard != null)
            m_ProfileWizard.finish();
         
         if (e.getEvent() == WizardEvent.CANCELLED)
         {
            ProfiledSettings.SignalCancel();
         }
      }  else { // normal/old behavior
          if (e.getEvent() == WizardEvent.CANCELLED) {
              int EXIT_CODE_LC_PUBLISH_CANCEL = 3; // Also defined in Studio.cpp
              int iExitCode = EXIT_CODE_LC_PUBLISH_CANCEL;
                  
              if (s_bErrorOnStartup)
                  iExitCode = -1;

              System.exit(iExitCode);
              // For non-LiveContext mode this is some "arbitrary" return code
          } else
              System.exit(0); // old behavior
      }
   }

   private int checkVersionType()
   {
      int versionResult = -1;
      if (NativeUtils.isLibraryLoaded())
      {
         // Note the use of MAIN_VERSION here. The main version is
         // the version stored in the Security entry of the Kerberok
         // registry value. The main version does not include a patch
         // level!
         versionResult = NativeUtils.getVersionType(PresentationConverter.MAIN_VERSION);
         if (versionResult != NativeUtils.VERSION_ERROR)
            return versionResult;
         else
         {
            // some error occurred during read-out of licence information in the 
            // native library (registry).
            JOptionPane.showMessageDialog(null, VERSION_ERROR,
                                          ERROR, JOptionPane.ERROR_MESSAGE);
            System.exit(-1);
         }
      }
      else
      {
         // the native library could not be loaded, bail out!
         JOptionPane.showMessageDialog(null, DLL_ERROR,
                                       ERROR, JOptionPane.ERROR_MESSAGE);
         System.exit(-1);
      }

      return NativeUtils.VERSION_ERROR;
   }

   private boolean checkEvaluationVersion()
   {
       if (NativeUtils.isLibraryLoaded())
       {
           final int iRemainingDays = getRemainingEvaluationDays();
          
           boolean bExpired = iRemainingDays < 0;
          
           if (bExpired) {
               SplashScreen s = new SplashScreen(true, iRemainingDays, null);
               s.exhibit();
               s.waitForDispose();
           }
          
           return !bExpired;
       }
      
       return true;
   }

   private Date getInstallationDate()
   {
      SimpleDateFormat sdf = new SimpleDateFormat("yyMMdd");
      Date d = null;
      try
      {
         String temp = NativeUtils.getInstallationDate();
         if (!temp.startsWith("00"))
            d = sdf.parse(temp);
         else
            d = null;
      }
      catch (Exception e)
      {
         e.printStackTrace();
         // ignore, treated as expired licence
      }

      return d;
   }

   public int getRemainingEvaluationDays()
   {
      Date install = getInstallationDate();

      if (install == null)
         return -1;

      int evalPeriod = NativeUtils.getEvaluationPeriod();

      Date today = new Date();

      long diffMillis = today.getTime()-install.getTime();
      int days = (int)(diffMillis/(1000*60*60*24));

      return days >= 0 ? evalPeriod - days : -1;
   }

   private boolean checkFullVersionLimit()
   {
      if (NativeUtils.isLibraryLoaded())
      {
         String strExpDate = NativeUtils.getExpirationDate();
         if (strExpDate == null)
         {
            // An error occurred.
            JOptionPane.showMessageDialog(null, VERSION_ERROR,
                                          ERROR, JOptionPane.ERROR_MESSAGE);
            System.exit(-1);
         }
         
         if (strExpDate.equals("")) // Unbound license
            return true;
         else {
             SimpleDateFormat sdf = new SimpleDateFormat("yyyyMMdd");
             Date dateExpire = sdf.parse(strExpDate, new ParsePosition(0));

             int days = -1; // fallback is "expired"; same as in Player
             
             if (dateExpire != null) {
                 long dateMs  = dateExpire.getTime();
                 long todayMs = (new Date()).getTime();

                 long diff = dateMs - todayMs;
                 days = (int)(diff / 86400000);
             }
         
             boolean bHasExpired = (days < 0);
             
             if (bHasExpired) {
                 SplashScreen s = new SplashScreen(false, days, dateExpire);
                 s.exhibit();
                 s.waitForDispose();
             }
             
             return !bHasExpired;
         }         
      }
      
      return true;
   }
   


//   private static void centerOnScreen(Window w)
//   {
//      Dimension windowD = w.getPreferredSize();
//      Dimension screenD = Toolkit.getDefaultToolkit().getScreenSize();
//
//      w.setLocation(screenD.width/2-windowD.width/2,
//                    screenD.height/2-windowD.height/2);
//   }

   public static boolean hasRealMedia()
   {
      return hasRealMedia_;
   }

   public static boolean hasWindowsMedia()
   {
      return hasWindowsMedia_;
   }

   public static boolean hasCdPublisher()
   {
      return hasCdPublisher_;
   }
   
   public static boolean hasMp4Publisher()
   {
      return hasMp4Publisher_;
   }   
}


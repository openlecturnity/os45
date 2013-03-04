// PZI
package imc.lecturnity.converter;

import imc.lecturnity.converter.wizard.PublisherWizardData;

public class ScreengrabbingExtended {
    static {
        try {
            System.loadLibrary("lsutl32");
        } catch (UnsatisfiedLinkError e) {
            e.printStackTrace();
            javax.swing.JOptionPane
                    .showMessageDialog(
                            null,
                            "The dynamic link library 'lsutl32.dll' was not found in the path. Reinstalling the application may help.",
                            "Error", javax.swing.JOptionPane.ERROR_MESSAGE);
            System.exit(-1);
        }

        // BUGFIX 4960: check removed - crash was caused by check
        // this was probably caused by some side-effect since the check was not modified for a long time
//        try {
//            runPostProcessing(null);
//        } catch (UnsatisfiedLinkError e) {
//            e.printStackTrace();
//            javax.swing.JOptionPane
//                    .showMessageDialog(
//                            null,
//                            "The dynamic link library 'lsutl32.dll' is outdated. Reinstalling the application may help.",
//                            "Error", javax.swing.JOptionPane.ERROR_MESSAGE);
//            System.exit(0);
//        }
    }

    // call post processing to generate structure and full text search data
    public static native boolean runPostProcessing(String lrdFile);

    // call post processing to remove structure and full text search data
    public static native boolean removeStructure(String lrdFile);

    // add/remove structure and fulltextsearch to/of screengrabbing clips according to document data
    public static boolean updateClipStructure(ProfiledSettings profiledSettings, DocumentData documentData) {
        boolean bResult = true;

        if (documentData.isClipStructured() && !documentData.isAddClipStructure()) {
            // remove structure and create pure SG clips
            System.out.println("\nCall Extended Screengrabbing: Remove Structure and Full Text Search");
            
            // nasty hack to adjust Flash output size
            // required because Flash size is calculated on basis of structured clip BEFORE structuring checkbox sets status
            if(profiledSettings.GetIntValue("iExportType") == PublisherWizardData.EXPORT_TYPE_FLASH)
                if(profiledSettings.GetIntValue("iVideoSizeType") == PublisherWizardData.VIDEOSIZE_STANDARD)
                    if(profiledSettings.GetIntValue("iSlideWidth") > profiledSettings.GetIntValue("iVideoWidth") 
                            || profiledSettings.GetIntValue("iSlideHeight") > profiledSettings.GetIntValue("iVideoHeight")) {
                        System.out.println("- fix Flash output size");
                        profiledSettings.SetIntValue("iClipSizeType", PublisherWizardData.CLIPSIZE_STANDARD);
                        profiledSettings.SetIntValue("iVideoSizeType", PublisherWizardData.VIDEOSIZE_MAXCLIP ); 
                        profiledSettings.SetIntValue("iVideoWidth", profiledSettings.GetIntValue("iSlideWidth")); 
                        profiledSettings.SetIntValue("iVideoHeight", profiledSettings.GetIntValue("iSlideHeight"));                        
                    }

            
            bResult = ScreengrabbingExtended.removeStructure(documentData.GetPresentationFileName());
        } else if (documentData.isAddClipStructure() || documentData.isClipStructured()) {
            // add structure to pure SG clips or update existing structure
            System.out.println("\nCall Extended Screengrabbing: Add/Update Structure and Full Text Search");
            
            // nasty hack to adjust Flash output size
            // required because Flash size is calculated on basis of pure SG BEFORE structuring checkbox sets status
            if(profiledSettings.GetIntValue("iExportType") == PublisherWizardData.EXPORT_TYPE_FLASH)
                if(profiledSettings.GetIntValue("iVideoSizeType") == PublisherWizardData.VIDEOSIZE_MAXCLIP)
                    if(profiledSettings.GetIntValue("iSlideWidth") > profiledSettings.GetIntValue("iVideoWidth") 
                            || profiledSettings.GetIntValue("iSlideHeight") > profiledSettings.GetIntValue("iVideoHeight")) {
                        System.out.println("- fix Flash output size");
                        profiledSettings.SetIntValue("iSlideWidth", profiledSettings.GetIntValue("iVideoWidth")); 
                        profiledSettings.SetIntValue("iSlideHeight", profiledSettings.GetIntValue("iVideoHeight"));                        
                    }

            bResult = ScreengrabbingExtended.runPostProcessing(documentData.GetPresentationFileName());
        }

        if (bResult)
            // re-read document (including structure)
            documentData.ReadDocumentDataFromPresentationFile(documentData.GetPresentationFileName());
        else
            // failed/canceled
            System.out.println(" - Extended Screengrabbing canceled (or failed).");
        System.out.println();

        return bResult;
    }
}

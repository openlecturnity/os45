package imc.epresenter.filesdk;

/*
 * File:             PresentationData.java
 * Code conventions: 18.09.2001
 * Author:           Martin Danielsson
 *
 * $Id: PresentationData.java,v 1.12 2009-05-04 12:43:07 ziewer Exp $
 */

import java.awt.Dimension;

// import imc.epresenter.filesdk.Metadata;

public class PresentationData
{
   public String presentationFileName = null;
   public String presentationPath     = null;
   public String audioFileName        = null;
   public Dimension whiteboardSize    = null;
   public String objectFileName       = null;
   public String eventFileName        = null;

   public boolean usesVideo               = false;
   public int videoHelperIndex            = -1;
   public String videoFileName            = null;
   public VideoClipInfo normalVideoInfo   = null;
   public int videoWidth                  = -1;
   public int videoHeight                 = -1;
   public boolean hasVideoOffset          = false;
   public int videoOffsetMs               = 0;
   public boolean videoIsStillImage       = false; 
   public boolean containsMultipleVideos  = false;
   public boolean containsNormalVideo     = false;
   public VideoClipInfo[] multiVideoClips = null;
   public boolean isSgStandAloneMode      = false;
   
   // PZI
   public boolean hasStructuredClips      = false;

   public int masterOffset            = 0;

   public String strCodepage          = null;
   
   public String metadataFileName     = null;
   public Metadata metadata           = null;
}

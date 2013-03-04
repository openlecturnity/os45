package imc.lecturnity.converter;

import imc.lecturnity.util.NativeUtils;

public class ProgressCommunicator {
    public void SendProgress(float fProgress) {
        String strMsgText = "Progress " + fProgress;
        System.out.println("**ProgressCommunicator: " + strMsgText);
        NativeUtils.sendPublishMessage(strMsgText);
    }

    public void SendProgressLabel(String strLabel) {
        String strMsgText = "Label " + strLabel;
        System.out.println("**ProgressCommunicator: " + strMsgText);
        // TODO: Send message to the interface
        //NativeUtils.sendPublishMessage(strMsgText);
    }
    
    public void SendWarningMessage(String strMessage) {
        String strMsgText = "Warning " + strMessage;
        System.out.println("**ProgressCommunicator: " + strMsgText);
        // TODO: Send message to the interface
        //NativeUtils.sendPublishMessage(strMsgText);
    }
    
    public void SendErrorMessage(String strMessage) {
        String strMsgText = "Error " + strMessage;
        System.out.println("**ProgressCommunicator: " + strMsgText);
        // TODO: Send message to the interface
        //NativeUtils.sendPublishMessage(strMsgText);
    }
    
    public void SendArbitraryMessage(String strId, String strMessage) {
        String strMsgText = "Info"+strId+" " + strMessage;
        System.out.println("**ProgressCommunicator: " + strMsgText);
        // TODO: Send message to the interface
        //NativeUtils.sendPublishMessage(strMsgText);
    }
}
package imc.lecturnity.util.net;

import java.awt.BorderLayout;
import java.awt.Container;
import java.awt.Dimension;
import java.awt.Frame;
import java.awt.Point;
import java.awt.Toolkit;
import java.awt.Font;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.*;

import java.net.PasswordAuthentication;
import java.net.Authenticator;

import imc.lecturnity.util.wizards.ButtonPanel;
import imc.lecturnity.util.wizards.WizardButton;
import imc.lecturnity.util.wizards.WizardPanel;
import imc.lecturnity.converter.ConverterWizard;

import imc.epresenter.filesdk.util.Localizer;

/**
 * <p>This class is intended for use with HTTP connections that require
 * authentication for information retrieval. Usage: Import the
 * <tt>Authenticator</tt> from <tt>java.net</tt> and this class, and
 * then add the following statement to the initialization part of your
 * program:
 * <code>
 *  AuthenticationManager authenticationManager = new AuthenticationManager(frame);
 *  Authenticator.setDefault(authenticationManager);
 * </code>
 * Before opening a connection, you should always call the {@link
 * #reset() reset()} method:
 * <code>
 *  authenticationManager.reset()
 * </code>
 * The authentication manager will try the last user and password
 * automatically if the sites and prompts of the authorization request
 * match.</p>
 *
 * <p><b>Note</b>: This class will fail with empty user names!</p>
 *
 * @author <a href="mailto:martin.danielsson@im-c.de">Martin Danielsson</a>
 */
public class AuthenticationManager extends Authenticator
{
   private static final int ACTION_CANCEL = 0;
   private static final int ACTION_OK     = 1;

   private static String CAPTION = "[!]";
   private static String AUTHENTICATE = "[!]";
   private static String AREA = "[!]";
   private static String USERNAME = "[!]";
   private static String PASSWORD = "[!]";
   
   static
   {
      try
      {
         Localizer l = new Localizer
            ("/imc/lecturnity/util/net/AuthenticationManager_", "en");
         
         CAPTION = l.getLocalized("CAPTION");
         AUTHENTICATE = l.getLocalized("AUTHENTICATE");
         AREA = l.getLocalized("AREA");
         USERNAME = l.getLocalized("USERNAME");
         PASSWORD = l.getLocalized("PASSWORD");
      }
      catch (Exception e)
      {
         e.printStackTrace();
         JOptionPane.showMessageDialog(null, "Could not open language database for AuthenticationManager" +
                                       e.toString(), "Error", JOptionPane.ERROR_MESSAGE);
         System.exit(0);
      }
   }

   private Frame frame_;

   private String lastUsername_ = "";
   private char[] lastPassword_ = new char[] {};

   private String lastSite_ = "";
   private String lastArea_ = "";
   private boolean isReset_ = true;
   private boolean isCancelled_ = false;

   /**
    * Creates a new <tt>AuthenticationManager</tt>. The password
    * dialog will be modal to the entire screen and will also be
    * centered with respect to the screen.
    */
   public AuthenticationManager()
   {
      this(null);
   }

   /**
    * Creates a new <tt>AuthenticationManager</tt>. The password
    * dialog will be modal to the given framee and will also be
    * centered with respect to that frame.
    */
   public AuthenticationManager(Frame frame)
   {
      super();

      frame_ = frame;
   }

   /**
    * Resets the authentication state. Call this method of your
    * <tt>AuthenticationManager</tt> instance before opening a new
    * http connection which might be subject to an authorization. This
    * method resets the "cancelled" state and also tries the last
    * settings of the password manager before displaying a passwort
    * request dialog.
    */
   public void reset()
   {
      isReset_ = true;
      isCancelled_ = false;
   }

   /**
    * Use this method if the parent frame to use for dialog modality
    * has changed since creation of the <tt>AuthenticationManager</tt>.
    */
   public void setParentFrame(Frame frame)
   {
      frame_ = frame;
   }

   /**
    * <b>Note:</b> Overridden method from the <tt>Authenticator</tt>
    * class. This method handles the creation of the password dialog
    * etc. <i>Not to be called manually.</i>.
    */
   public PasswordAuthentication getPasswordAuthentication()
   {
      if (isCancelled_)
         return null;
                        
      if (lastSite_.equals(getRequestingSite()) &&
          lastArea_.equals(getRequestingPrompt()) &&
          isReset_ &&
          !lastUsername_.equals(""))
      {
         // let's retry the last request:
         isReset_ = false;
         return new PasswordAuthentication(lastUsername_, lastPassword_);
      }

      isReset_ = false;

      PasswordDialog dialog = new PasswordDialog(frame_,
                                                 lastUsername_,
                                                 lastPassword_);
      dialog.show();
      int result = dialog.getAction();
      if (result == ACTION_CANCEL)
      {
         isCancelled_ = true;
         return null;
      }

      lastUsername_ = dialog.getUsername();
      lastPassword_ = dialog.getPassword();

      lastSite_ = getRequestingSite().toString();
      lastArea_ = getRequestingPrompt();

      return new PasswordAuthentication(lastUsername_, lastPassword_);
   }

   private class PasswordDialog extends JDialog
   {
      private JTextField userField_;
      private JPasswordField passField_;
      private JLabel     areaLabel_;
      private int        returnValue_ = ACTION_CANCEL;

      public PasswordDialog(Frame frame, String defaultUser, char[] defaultPass)
      {
         super(frame, CAPTION, true);

         setDefaultCloseOperation(DISPOSE_ON_CLOSE);

         Container r = getContentPane();
         r.setLayout(new BorderLayout());

         JPanel panel = new JPanel()
            {
               public Dimension getPreferredSize()
               {
                  return new Dimension(400, 165);
               }
            };
         panel.setLayout(null);
         panel.setFont(imc.lecturnity.converter.ConverterWizard.createDefaultFont(Font.PLAIN, 12));
         r.add(panel, BorderLayout.CENTER);
         ButtonListener buttonListener = new ButtonListener();
         ButtonPanel buttonPanel = new ButtonPanel();
         JButton button = new WizardButton(WizardPanel.OK);
         button.addActionListener(buttonListener);
         button.setDefaultCapable(true);
         getRootPane().setDefaultButton(button);
         buttonPanel.addRightButton(button);
         buttonPanel.addRightButton(Box.createHorizontalStrut(10));
         button = new WizardButton(WizardPanel.CANCEL);
         button.addActionListener(buttonListener);
         buttonPanel.addRightButton(button);
         r.add(buttonPanel, BorderLayout.SOUTH);

         JLabel label = new JLabel(AUTHENTICATE);
         label.setSize(360, 15);
         label.setLocation(20, 20);
         panel.add(label);

         label = new JLabel(AREA + getRequestingPrompt());
         label.setSize(360, 15);
         label.setLocation(20, 40);
         panel.add(label);

         label = new JLabel(USERNAME);
         label.setSize(360, 15);
         label.setLocation(20, 70);
         panel.add(label);
         
         userField_ = new JTextField(defaultUser);
         userField_.setSize(360, 22);
         userField_.setLocation(20, 86);
         panel.add(userField_);
         
         label = new JLabel(PASSWORD);
         label.setSize(360, 15);
         label.setLocation(20, 110);
         panel.add(label);

         passField_ = new JPasswordField(new String(defaultPass));
         passField_.setEchoChar('*');
         passField_.setSize(360, 22);
         passField_.setLocation(20, 126);
         panel.add(passField_);

         setResizable(false);

         pack();

         center();
      }

      private void center()
      {
         if (frame_ == null)
         {
            // center on screen
            Dimension screenSize = (Toolkit.getDefaultToolkit()).getScreenSize();
            Dimension mySize = getSize();

            setLocation((screenSize.width - mySize.width)/2, 
                        (screenSize.height - mySize.height)/2);
         }
         else
         {
            // center to frame
            Dimension frameSize = frame_.getSize();
            Point     frameLocation = frame_.getLocation();
            Dimension mySize = getSize();
            
            int x = frameLocation.x + (frameSize.width - mySize.width)/2;
            int y = frameLocation.y + (frameSize.height - mySize.height)/2;
            
            if (x < 0)
               x = 0;
            if (y < 0)
               y = 0;

            setLocation(x, y);
         }
      }

      private void doOk()
      {
         returnValue_ = ACTION_OK;
         dispose();
      }

      private void doCancel()
      {
         returnValue_ = ACTION_CANCEL;
         dispose();
      }

      public int getAction()
      {
         return returnValue_;
      }

      public String getUsername()
      {
         return userField_.getText();
      }

      public char[] getPassword()
      {
         return passField_.getPassword();
      }

      private class ButtonListener implements ActionListener
      {
         public void actionPerformed(ActionEvent e)
         {
            String cmd = e.getActionCommand();
            if (cmd.equals(WizardPanel.OK))
               doOk();
            else if (cmd.equals(WizardPanel.CANCEL))
               doCancel();
         }
      }
   }
}

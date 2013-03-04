package imc.lecturnity.util.ui;

import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.Date;
import javax.swing.*;

import imc.epresenter.filesdk.util.Localizer;
import imc.lecturnity.util.NativeUtils;

public class SplashScreen extends JWindow implements ActionListener
{
    private static Localizer s_Localizer;
    static {
        try { 
            s_Localizer = new Localizer("/imc/lecturnity/util/ui/SplashScreen_", "en");
        } catch (IOException exc) {
             System.err.println("SplashScreen: Could not instanciate Localizer.");
             exc.printStackTrace();
        }
    }
    
    private boolean m_bShowMe;
    private boolean m_bIsEvaluation;
    private boolean m_bIsExpired;
    private JLabel m_lblStatus;
    private String m_strInitialStatus;

    public SplashScreen(Window wndParent) {
        this(false, 100, null, wndParent);
    }
    
    public SplashScreen(boolean bIsEvaluation, int iRemainingDays, Date endDate) {
        this(bIsEvaluation, iRemainingDays, endDate, null);
    }
    
    private SplashScreen(boolean bIsEvaluation, int iRemainingDays, Date endDate, Window wndParent) {
        super(wndParent);
        
        if (wndParent == null)
            setAlwaysOnTop(true);
        
        m_bIsEvaluation = bIsEvaluation;
        m_bIsExpired = endDate != null && endDate.before(new Date());
       
        ImageIcon imgBackground = new ImageIcon(
            getClass().getResource("/imc/lecturnity/util/ui/images/lecturnity_splash.png"));
       
        int iWidth = 100;
        int iHeight = 100;
       
        if (imgBackground.getIconWidth() > 0)
            iWidth = imgBackground.getIconWidth();
        if (imgBackground.getIconHeight() > 0)
            iHeight = imgBackground.getIconHeight();
      
        setPreferredSize(new Dimension(iWidth, iHeight));
       
        JLabel lblRoot = new JLabel(imgBackground);
        setContentPane(lblRoot);
        lblRoot.setLayout(new GridLayout(1, 1));
       
        JPanel pnlWithBorder = new JPanel(new BorderLayout());
        pnlWithBorder.setOpaque(false);      
        pnlWithBorder.setBorder(BorderFactory.createEmptyBorder(0, 0, 9, 30)); 
       
        lblRoot.add(pnlWithBorder);

     
        JPanel pnlLower = new JPanel(new BorderLayout(0, 5));
        pnlLower.setOpaque(false);
        pnlWithBorder.add("South", pnlLower);
       
        m_strInitialStatus = wndParent == null ? s_Localizer.getLocalized("starting") : "";
        m_lblStatus = new JLabel(m_strInitialStatus, JLabel.RIGHT);
        Font f = m_lblStatus.getFont();
        f = f.deriveFont(Font.BOLD);
        m_lblStatus.setFont(f);
        pnlLower.add("North", m_lblStatus);
       
        String strVersion = "LECTURNITY ";
        if (NativeUtils.isLibraryLoaded()) {
            strVersion += NativeUtils.getVersionStringShortShort() + " | ";
            strVersion += "Rev. " + NativeUtils.getVersionRevision() + " | ";
        }
        strVersion += s_Localizer.getLocalized("versionSuffix");
        JLabel lblVersion = new JLabel(strVersion, JLabel.RIGHT);
        f = f.deriveFont(Font.PLAIN);
        lblVersion.setFont(f);
        pnlLower.add("South", lblVersion);
       
        JPanel pnlInner = new JPanel(new BorderLayout(10, 0));
        pnlInner.setOpaque(false);
        pnlLower.add("Center", pnlInner);
       
        JButton btnOk = new JButton("OK");
        Dimension d = btnOk.getPreferredSize();
        if (d.width < 80) {
            d.width = 80;
            btnOk.setPreferredSize(d);
        }
        btnOk.addActionListener(this);
        pnlInner.add("East", btnOk);
       
        JLabel lblVersionResult = new JLabel("...", JLabel.RIGHT);
        lblVersionResult.setForeground(Color.red);
        f = f.deriveFont(Font.BOLD);
        lblVersionResult.setFont(f);
        pnlInner.add("Center", lblVersionResult);
           
        pack();
  
        String strVersionResult = "";
        
        if (bIsEvaluation) {
            strVersionResult = s_Localizer.getLocalized("evaluationPeriod") + ": ";
            if (iRemainingDays >= 0) {
                strVersionResult += iRemainingDays + " "
                + (iRemainingDays == 1 ? s_Localizer.getLocalized("day") : s_Localizer.getLocalized("days"))
                + ".";
            } else {
                strVersionResult += s_Localizer.getLocalized("expired")+".";
            }           
        } else if (endDate != null) {
            strVersionResult = s_Localizer.getLocalized("runtime")+": ";
            if (endDate.after(new Date())) { // not yet expired
                
                /* automatically localized
                DateFormat odf = DateFormat.getDateInstance(DateFormat.MEDIUM);
                labelText += odf.format(endDate);
                //*/
                
                String language = null;
                if (NativeUtils.isLibraryLoaded())
                    language = NativeUtils.getLanguageCode();
                else
                    language = System.getProperty("user.language", "en");
            
                SimpleDateFormat dateFormater = new SimpleDateFormat("MM/dd/yyyy");
                if (language.equals("de"))
                    dateFormater = new SimpleDateFormat("dd.MM.yyyy");

                strVersionResult += dateFormater.format(endDate);
                
                lblVersionResult.setForeground(Color.black);
            } else {
                strVersionResult += s_Localizer.getLocalized("expired")+".";
            }
        }
        
        lblVersionResult.setText(strVersionResult); // is probably empty
 
        centerOnScreen(this);
        
        btnOk.requestFocus();
    } // <init>

    public void replaceMessage(String message) {
        m_lblStatus.setText(message);
    }

    public void resetMessage()  {
        m_lblStatus.setText(m_strInitialStatus);
    }

    public void exhibit() {
        m_bShowMe = true;
        setVisible(true);
    }

    public void stopJostling() {
        setAlwaysOnTop(false);
    }

    public synchronized void waitForDispose() {
        if (m_bShowMe)
            try { wait(); } catch (InterruptedException e) {}
    }


    public void duckAndCover() {
        duckAndCover(false);
    }

    public void duckAndCover(boolean bForce) {
        boolean bStay = m_bIsEvaluation || m_bIsExpired;
        if (!bStay || bForce) {
            shutDown();
        } // else only click works
    }
    
    public void closeSoon() {
        Thread t = new Thread("Closing Splash") {
            public void run() {
                try { Thread.sleep(5000); } catch (InterruptedException exc) {}
                shutDown();
            }
        };
        t.start();
    }

    public void actionPerformed(ActionEvent e) {
        shutDown();
    }

    private void shutDown() {
        m_bShowMe = false;
        dispose();

        synchronized(this) {
            notify();
        }
    }


    private void centerOnScreen(Window w) {
        Dimension windowD = w.getPreferredSize();
        Dimension screenD = Toolkit.getDefaultToolkit().getScreenSize();

        w.setLocation(screenD.width/2-windowD.width/2,
                      screenD.height/2-windowD.height/2);
    }

}

    

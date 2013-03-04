package imc.epresenter.player;

import java.io.File;
import java.awt.Component;

import imc.epresenter.filesdk.FileResources;

/**
 * Dieses Interface dient als Abstraktion der Vermittlungsschicht
 * zwischen den einzelnen Helpern und dem Master.
 * <p>
 * Das hei&szlig;t ein Helper mu&szlig; dieses Interface implementieren.<br>
 * Durch die Methoden dieses Interfaces (Ausnahme <code>addPlayerListener</code>)
 * wird der Steuerungskanal vom Master zum Helper realisiert. Durch die
 * Methode <code>addPlayerListener</code>, und die and diese 
 * {@link PlayerListener} zu verschickenden {@link PlayerEvent}s,
 * wird der R&uuml;ckkanal vom Helper zum Master realisiert.
 * <p>
 * <b>Zur Helperintegration in einen Vortrag</b>:<br>
 * In der Konfigurationsdatei des AOF-Vortrags (*.aof) mu&szlig;
 * ein weiterer Helper-Eintrag vorgenommen werden. Als Kommandoaufruf
 * sollte ein Standard-Java-Aufruf verwendet werden. Z. B.
 * <code>java FamousPackage.Helper17</code>, wenn sich der Helper
 * im Classpath befindet oder im AOF-Daten-Verzeichnis lokal
 * vorhanden ist; oder auch durch eine explizite Ortsangabe des
 * entsprechenden JAR-Archives: <code>java -cp famous.jar ...</code>.
 *
 * @see PlayerListener
 * @version 1.0, 29.03.2001
 * @author Ulrich Kuhn
 */
public interface SRStreamPlayer {
   /**
    *
    */
   public abstract void setStartOffset(int millis);


	/**
	 * Initialisiert diese Komponente.
	 * <p>
	 * Normalerweise befinden sich in der Konfigurationsdatei (*.aof)
	 * zu jedem Helper-Eintrag einige Parameter (z. B. Dateinamen
	 * der Daten-Dateien). Diese Parameter werden direkt an den
	 * jeweiligen Helper weitergegeben.
	 *
	 * @param rootDir das Verzeichnis, das die Konfigurationsdatei
	 *                enth&auml;lt
	 * @param args die f&uuml;r diesen Helper angegebenen Parameter
	 */
	public abstract void init(FileResources resources, String[] args, Coordinator c);
	
	/**
	 * Enables/disables the visual components.
	 */
	public abstract void enableVisualComponents(boolean bEnabled);

	/**
	 *
	 * @returns Grafische Komponente zur Medienwiedergabe. Und 
	 * eventuell vorhandene helper-spezifische Wiedergabekontrollen
	 * (z.B. <code>next</code> und <code>previousSlide</code> bei
	 * einem WhiteBoardHelper).
	 */
	public abstract Component[] getVisualComponents();

	/**
	 *
	 */
	public abstract String getTitle();

	/**
	 *
	 */
	public abstract String getDescription();
												
	/**
	 * Startet die Wiedergabe.
	 *
	 * @param info enth&auml;lt Daten warum (von wem) diese Methode
	 *             aufgerufen wird
	 */
	public abstract void start(EventInfo info);
	
	/**
	 * Stoppt die Wiedergabe und gibt eventuell belegte Resourcen
	 * wieder frei (z. B. Audio-Ports oder Netzwerkverbindungen).
	 * <p>
	 * F&uuml;r einen "Schnellstop" (ohne Resourcen-Freigabe) dient
	 * die Methode <code>pause</code>.
	 *
	 * @param info enth&auml;lt Daten warum (von wem) diese Methode
	 *             aufgerufen wird
	 */
	public abstract void stop(EventInfo info);
	
	/**
	 * Neben <code>stop</code> eine weitere Möglichkeit die Wiedergabe
	 * zu stoppen. Im Unterschied zu <code>stop</code> sollen hier keine 
	 * Resourcen 
	 * freigegeben werden, da ein baldiger Wieder-Aufruf von 
	 * <code>start</code> zu erwarten ist.
	 * <p>
	 * Nach dem Aufruf von <code>pause</code> befindet sich der Helper
	 * im Pause-Modus. Dieser kann nur durch einen Aufruf von
	 * <code>start</code> verlassen werden (nicht <code>stop</code>).
	 * <p>
	 * Derzeit ist der Pause-Modus mit "Scrollen w&auml;hrend der
	 * Wiedergabe" identisch.
	 *
	 * @param info enth&auml;lt Daten warum (von wem) diese Methode
	 *             aufgerufen wird
	 */
	public abstract void pause(EventInfo info);
	
	/**
	 * Informiert den Helper von der aktuell wiederzugebenden Medienzeit.
	 * <p>
	 * Auf Helper-Seite dient diese Methode
	 * vor allem der kontinuierlichen &Uuml;bermittlung der
	 * Synchronisierungszeit.<br>
	 * Im Allgemeinen mu&szlig; davon ausgegangen werden,
	 * dass sukkzessive Aufrufe sehr schnell hintereinander erfolgen
	 * (z. B. alle 40 Millisekunden). Es ist deshalb zwingend erforderlich,
	 * dass ein Aufruf von <code>setMediaTime</code> sofort zur&uuml;ckkehrt.
	 * Dies macht es eventuell notwendig, dass damit verbundene Berechnungen
	 * in einen zus&auml;tzlichen <code>Thread</code> verlagert werden und
	 * mehrere Methodenaufrufe, die w&auml;hrend dieser Berechnung eintreffen
	 * und deshalb nicht bearbeitet werden k&ouml;nnen zu einem einzigen
	 * zusammengefa&szlig;t werden.<br>
	 * Bei grafischen Helpern ist eine einfache M&ouml;glichkeit dies
	 * zu erreichen der Aufruf von <code>repaint</code> und die Verlagerung
	 * aller Berechnungen in die <code>paintComponent</code>-Methode.
	 * 
	 * @param millis die wiederzugebende Medienzeit in Millisekunden
	 *
	 * @param info enth&auml;lt Daten warum (von wem) diese Methode
	 *             aufgerufen wird
	 */
	public abstract void setMediaTime(int millis, EventInfo info);
	
	/**
	 * Erfragt die aktuell wiedergegebene Medienzeit.
	 * <p>
	 * Wird auf Helper-Seite nicht verwendet.
	 *
	 * @returns aktuell (wiedergegebene) Medienzeit
	 */
	public abstract int getMediaTime();
	
	/**
	 * Dient zur Ermittlung der Gesamtl&auml;nge der vorliegenden 
	 * Mediendaten.
	 * <p>
	 * Wird auf Helper-Seite nicht verwendet.
	 *
	 * @returns Gesamtl&auml;nge der Daten
	 */
	public abstract int getDuration();
	
	/**
	 * Teilt die Vortrags-Gesamtl&auml;nge mit.
	 *
	 * @param millis Vortrags-Gesamtl&auml;nge in Millisekunden
	 */
    public abstract void setDuration(int millis);

	/**
	 * Teilt vortragsspezifische (nicht numerische) Daten mit.
	 *
	 * @param key der Informationstyp; normalerweise 'author' und 'title'
	 * @param value der Inforamationswert
	 */
	public abstract void setInfo(String key, String value);

	/**
	 * Gibt alle von diesem Helper permanent belegten Ressourcen frei.
	 */
	public abstract void close(EventInfo info);
}
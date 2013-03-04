package imc.epresenter.player;

/**
 * Enth&auml;lt n&auml;here Informationen dar&uuml;ber, warum eine
 * Methode des Interface {@link SRStreamPlayer} aufgerufen wird.
 * Das hei&szlig;t die Quelle (Helper) von dem diese
 * Aktion ausgeht und die Zeit zu der diese Aktion angestossen wurde.
 */
public class EventInfo {
	public static final byte MASTER = 0;
	public static final byte HELPER = 1;
	public static final byte EXTERNAL = 2;


	/**
	 * Als Quellangaben (Wert der Variable) kommen in Frage:
	 * MASTER = 0, HELPER = 1 und EXTERNAL = 2.
	 */
    public byte source;
	
	/**
	 * Die Zeit zu der diese Aktion angestossen wurde in Millisekunden
	 * (Systemzeit nicht Medienzeit).
	 */
    public long creationTime;
    
	/**
	 * Die ID des Helpers von dem diese Aktion ausgeht.
	 * Bei EXTERNAL als Quelle (source) ist die ID 0.
	 */
    public long helperID;

    public EventInfo(byte s, long ct, long id) {
        source = s;
        creationTime = ct;
        helperID = id;
    }
	
	public String toString() {
		return "EventInfo "+source+":"+creationTime+":"+helperID;
	}
}


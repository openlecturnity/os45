========================================================================
       MICROSOFT FOUNDATION CLASS BIBLIOTHEK : Publisher
========================================================================


Diese Publisher-Anwendung hat der Klassen-Assistent für Sie erstellt. Diese Anwendung
zeigt nicht nur die prinzipielle Verwendung der Microsoft Foundation Classes, 
sondern dient auch als Ausgangspunkt für die Erstellung Ihrer eigenen DLLs.

Diese Datei enthält die Zusammenfassung der Bestandteile aller Dateien, die 
Ihre Publisher-Anwendung bilden.

Publisher.dsp
    Diese Datei (Projektdatei) enthält Informationen auf Projektebene und wird zur
    Erstellung eines einzelnen Projekts oder Teilprojekts verwendet. Andere Benutzer können
    die Projektdatei (.dsp) gemeinsam nutzen, sollten aber die Makefiles lokal exportieren.

Publisher.h
    Hierbei handelt es sich um die Haupt-Header-Datei der Anwendung. Diese enthält 
	andere projektspezifische Header (einschließlich Resource.h) und deklariert die
	Anwendungsklasse CPublisherApp.

Publisher.cpp
    Hierbei handelt es sich um die Haupt-Quellcodedatei der Anwendung. Diese enthält die
    Anwendungsklasse CPublisherApp.

Publisher.rc
	Hierbei handelt es sich um eine Auflistung aller Ressourcen von Microsoft Windows, die 
	vom Programm verwendet werden. Sie enthält die Symbole, Bitmaps und Cursors, die im 
	Unterverzeichnis RES abgelegt sind. Diese Datei lässt sich direkt in Microsoft
	Visual C++ bearbeiten.

Publisher.clw
    Diese Datei enthält Informationen, die vom Klassen-Assistenten verwendet wird, um bestehende
    Klassen zu bearbeiten oder neue hinzuzufügen.  Der Klassen-Assistent verwendet diese Datei auch,
    um Informationen zu speichern, die zum Erstellen und Bearbeiten von Nachrichtentabellen und
    Dialogdatentabellen benötigt werden und um Prototyp-Member-Funktionen zu erstellen.

res\Publisher.ico
    Dies ist eine Symboldatei, die als Symbol für die Anwendung verwendet wird. Dieses 
	Symbol wird durch die Haupt-Ressourcendatei Publisher.rc eingebunden.

res\Publisher.rc2
    Diese Datei enthält Ressourcen, die nicht von Microsoft Visual C++ bearbeitet wurden.
	In diese Datei werden alle Ressourcen abgelegt, die vom Ressourcen-Editor nicht bearbeitet 
	werden können.

Publisher.reg
    	Dies ist eine .REG-Beispieldatei, die Ihnen anzeigt, welche Registrierungseinstellungen 
	durch die Programmumgebung festlegt werden. Diese können Sie als .REG-
    	-Datei für Ihre Anwendung festlegen, oder diese löschen und die Standardregistrierung 
	RegisterShellFileTypes verwenden.



/////////////////////////////////////////////////////////////////////////////

Für das Hauptfenster:

MainFrm.h, MainFrm.cpp
    	Diese Dateien enthalten die Frame-Klasse CMainFrame, die von
    	CFrameWnd abgeleitet wurde und alle SDI-Frame-Merkmale steuert.

res\Toolbar.bmp
    Diese Bitmap-Datei wird zum Erstellen unterteilter Bilder für die Symbolleiste verwendet.
    Die erste Symbol- und Statusleiste wird in der Klasse CMainFrame erstellt.
    Bearbeiten Sie diese Bitmap der Symbolleiste mit dem Ressourcen-Editor, und
    aktualisieren Sie IDR_MAINFRAME TOOLBAR in Publisher.rc, um Schaltflächen für die
    Symbolleiste hinzuzufügen.
/////////////////////////////////////////////////////////////////////////////

Der Klassen-Assistent erstellt einen Dokumenttyp und eine Ansicht(View):

PublisherDoc.h, PublisherDoc.cpp - das Dokument
    	Diese Dateien enthalten die Klasse CPublisherDoc. Bearbeiten Sie diese Dateien,
  	um Ihre speziellen Dokumentdaten hinzuzufügen und das Speichern und Laden von 
	Dateien zu implementieren (mit Hilfe von CPublisherDoc::Serialize).

DocumentPropsView.h, DocumentPropsView.cpp - die Ansicht des Dokuments
    	Diese Dateien enthalten die Klasse CDocumentPropsView.
    	CDocumentPropsView-Objekte werden verwendet, um CPublisherDoc-Objekte anzuzeigen.



/////////////////////////////////////////////////////////////////////////////
Andere Standarddateien:

StdAfx.h, StdAfx.cpp
    	Mit diesen Dateien werden vorkompilierte Header-Dateien (PCH) mit der Bezeichnung 
	Publisher.pch und eine vorkompilierte Typdatei mit der Bezeichnung StdAfx.obj
	erstellt.

Resource.h
    	Dies ist die Standard-Header-Datei, die neue Ressourcen-IDs definiert.
    	Microsoft Visual C++ liest und aktualisiert diese Datei.

/////////////////////////////////////////////////////////////////////////////
Weitere Hinweise:

Der Klassen-Assistent fügt "ZU ERLEDIGEN:" im Quellcode ein, um die Stellen anzuzeigen, 
an denen Sie Erweiterungen oder Anpassungen vornehmen können.

Wenn Ihre Anwendung die MFC in einer gemeinsam genutzten DLL verwendet und Ihre Anwendung
eine andere als die aktuell auf dem Betriebssystem eingestellte Sprache verwendet, muss 
die entsprechend lokalisierte Ressource MFC42XXX.DLL von der Microsoft Visual C++ CD-ROM 
in das Verzeichnis system oder system32 kopiert und in MFCLOC.DLL umbenannt werden ("XXX" 
steht für die Abkürzung der Sprache. So enthält beispielsweise MFC42DEU.DLL die ins Deutsche 
übersetzten Ressourcen). Anderenfalls erscheinen einige Oberflächenelemente Ihrer Anwendung 
in der Sprache des Betriebssystems.

/////////////////////////////////////////////////////////////////////////////

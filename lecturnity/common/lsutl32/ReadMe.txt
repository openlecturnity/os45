========================================================================
       MICROSOFT FOUNDATION CLASS BIBLIOTHEK : lsutl32
========================================================================


Diese lsutl32-DLL hat der Klassen-Assistent für Sie erstellt. Die DLL zeigt 
die prinzipielle Anwendung der Microsoft Foundation Classes und dient Ihnen 
als Ausgangspunkt für die Erstellung Ihrer eigenen DLLs.

Diese Datei enthält die Zusammenfassung der Bestandteile aller Dateien, die 
Ihre lsutl32-DLL bilden.

lsutl32.dsp
    Diese Datei (Projektdatei) enthält Informationen auf Projektebene und wird zur
    Erstellung eines einzelnen Projekts oder Teilprojekts verwendet. Andere Benutzer können
    die Projektdatei (.dsp) gemeinsam nutzen, sollten aber die Makefiles lokal exportieren.

lsutl32.h
	Dies ist die Haupt-Header-Datei für die DLL, in der die Klasse 
	CLsutl32App deklariert ist.

lsutl32.cpp
	Hierbei handelt es sich um die Haupt-Quellcodedatei der DLL. Diese enthält 
	die Klasse CLsutl32App.


lsutl32.rc
	Hierbei handelt es sich um eine Auflistung aller Ressourcen von Microsoft Windows, die 
	vom Programm verwendet werden. Sie enthält die Symbole, Bitmaps und Cursors, die im 
	Unterverzeichnis RES abgelegt sind. Diese Datei lässt sich direkt in Microsoft
	Visual C++ bearbeiten.

lsutl32.clw
    Diese Datei enthält Informationen, die vom Klassen-Assistenten verwendet wird, um bestehende
    Klassen zu bearbeiten oder neue hinzuzufügen.  Der Klassen-Assistent verwendet diese Datei auch,
    um Informationen zu speichern, die zum Erstellen und Bearbeiten von Nachrichtentabellen und
    Dialogdatentabellen benötigt werden und um Prototyp-Member-Funktionen zu erstellen.

res\lsutl32.rc2
    Diese Datei enthält Ressourcen, die nicht von Microsoft Visual C++ bearbeitet wurden.
	In diese Datei werden alle Ressourcen abgelegt, die vom Ressourcen-Editor nicht bearbeitet 
	werden können.

lsutl32.def
    Diese Datei enthält Informationen über die DLL, die für das Ausführen unter
	Microsoft Windows erforderlich sind. Es werden Parameter definiert, wie beipielsweise
	der Name und die Beschreibung der DLL. Außerdem werden die von der DLL exportierten
	Funktionen angegeben.

/////////////////////////////////////////////////////////////////////////////
Andere Standarddateien:

StdAfx.h, StdAfx.cpp
    	Mit diesen Dateien werden vorkompilierte Header-Dateien (PCH) mit der Bezeichnung 
	lsutl32.pch und eine vorkompilierte Typdatei mit der Bezeichnung StdAfx.obj
	erstellt.

Resource.h
    	Dies ist die Standard-Header-Datei, die neue Ressourcen-IDs definiert.
    	Microsoft Visual C++ liest und aktualisiert diese Datei.

/////////////////////////////////////////////////////////////////////////////
Weitere Hinweise:

Der Klassen-Assistent fügt "ZU ERLEDIGEN:" im Quellcode ein, um die Stellen anzuzeigen, 
an denen Sie Erweiterungen oder Anpassungen vornehmen können.

/////////////////////////////////////////////////////////////////////////////

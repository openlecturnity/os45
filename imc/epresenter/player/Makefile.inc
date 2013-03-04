# 04.10.01 Ulrich Kuhn
# In dieser Datei befinden sich die allgemeinen Produktionsregeln, die fuer
# alle Verzeichnisse gelten.
#

TODODIRS=imc/epresenter/player imc/epresenter/filesdk imc/lecturnity/util imc/epresenter/media
FILEFILTER=grep -v ".*java" | grep -v "/CVS/" | grep -v "/.svn/" | grep -v "Makefile" | grep -v "/META-INF/"
TMPNAME=ZZvXX01
TMPDIR=$(TOPDIR)/$(TMPNAME)
JARNAME=player-real.jar

%.class : %.java
	cd $(TOPDIR) && javac $(LOCALDIR)/$< && cd $(LOCALDIR)


all: $(CLASSES) subdirs

subdirs: 
	@for x in $(SUBDIRS) ""; do\
      if [ -z $$x ] ; then continue; fi; \
		(cd $$x && make all) || exit 1; done;

clean: 
	rm -f $(CLASSES) *.class *~ ChangeLog
	@for x in $(SUBDIRS) ""; do\
		(if [ ! -z $$x ]; then cd $$x; make clean; fi); done;

old-real:
	date +%y%m%d%H > $(TOPDIR)/imc/epresenter/player/buildDate; cd $(TOPDIR); jar cfm player-real.jar imc/epresenter/player/META-INF/manifest.mf `find imc -type f | grep -v ".*java" | grep -v "CVS" | grep -v "/converter/" | grep -v "/docs/" | grep -v "/installshield/" | grep -v "Makefile" | grep -v "META-INF"`; cd $(LOCALDIR); jar uf $(TOPDIR)/player-real.jar META-INF/services;

real:
	date +%y%m%d%H > $(TOPDIR)/imc/epresenter/player/buildDate
	[ ! -d $(TMPDIR) ] && mkdir $(TMPDIR) || rm -rf $(TMPDIR)/*
	cp -RL META-INF $(TMPDIR)
	cd $(TOPDIR);\
	for x in $(TODODIRS); do\
                mkdir -p $(TMPNAME)/$$x; cp -RL $$x/* $(TMPNAME)/$$x; done
	rm -rf $(TMPDIR)/imc/epresenter/player/META-INF
	rm -rf `find $(TMPDIR) -type d | grep "/CVS\|/.svn\|/\.xvpics\|/tests"`
	rm `find $(TMPDIR) -type f | grep "\.java$$\|Makefile\|\.h$$\|\.cvsignore$$"`
	jar cfm $(TOPDIR)/$(JARNAME) META-INF/manifest.mf -C $(TMPDIR) .
	rm -rf $(TMPDIR)	

tag:
	[ ! -z $(STRING) ] && \
        cd $(TOPDIR)/imc; cvs tag $(STRING);
#       for x in $(TODODIRS); do\
#               cd $(TOPDIR)/$$x; pwd; cvs tag $(STRING); done

source:
	[ ! -d $(TMPDIR) ] && mkdir $(TMPDIR) || rm -rf $(TMPDIR)/*
	cp -RL META-INF $(TMPDIR)
	mkdir -p $(TMPDIR)/imc/epresenter
	mkdir -p $(TMPDIR)/imc/lecturnity
	cp -r ../player $(TMPDIR)/imc/epresenter
	cp -r ../media $(TMPDIR)/imc/epresenter
	cp -r ../filesdk $(TMPDIR)/imc/epresenter
	cp -r ../../lecturnity/util $(TMPDIR)/imc/lecturnity
	rm -rf $(TMPDIR)/imc/epresenter/player/META-INF
	rm -f $(TMPDIR)/META-INF/manifest.mf
	rm -rf `find $(TMPDIR) -type d | grep "/CVS\|/\.xvpics\|/tests\|/gif2png\|/wizards"`
	rm `find $(TMPDIR) -type f | grep "Makefile\|\.h$$\|\.cvsignore$$\|\.txt$$"`
	rm $(TMPDIR)/imc/epresenter/filesdk/audio/LadFileReader.java
	rm $(TMPDIR)/imc/epresenter/filesdk/ArchivedFileResources.java
	rm $(TMPDIR)/imc/epresenter/filesdk/SimpleFileResources.java
	rm $(TMPDIR)/imc/epresenter/filesdk/Archiver.java
	rm $(TMPDIR)/imc/epresenter/player/audio/AQSFileReader.java
	rm $(TMPDIR)/imc/epresenter/player/audio/AQSFileWriter.java
	rm $(TMPDIR)/imc/epresenter/player/audio/AQSInputStream.java
	rm $(TMPDIR)/imc/epresenter/player/audio/Audio2Wav.java
	jar cf $(TOPDIR)/player_source.zip -C $(TMPDIR) .
	rm -rf $(TMPDIR)

.PHONY: subdirs clean export real tag source

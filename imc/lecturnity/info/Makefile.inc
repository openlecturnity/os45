# 04.10.01 Ulrich Kuhn, modified 30.8.05 and 16.1.06
# In dieser Datei befinden sich die allgemeinen Produktionsregeln, die fuer
# alle Verzeichnisse gelten.
#

TODODIRS=imc/epresenter/player/whiteboard imc/epresenter/filesdk \
   imc/lecturnity/info imc/lecturnity/util imc/epresenter/player/util \
   imc/epresenter/player/audio
TMPNAME=ZZvXX02
DOCTMPNAME=ZZvXXdoc34
TMPDIR=$(TOPDIR)/$(TMPNAME)
JARNAME=lpdinfo.jar
DOCZIP=lpdinfo-doc.zip

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

doc1:
	[ ! -d $(TOPDIR)/$(DOCTMPNAME) ] && mkdir $(TOPDIR)/$(DOCTMPNAME) || rm -rf $(TOPDIR)/$(DOCTMPNAME)/*
	mkdir $(TOPDIR)/$(DOCTMPNAME)/doc
	mkdir -p $(TOPDIR)/$(DOCTMPNAME)/imc/lecturnity/info/
	mkdir -p $(TOPDIR)/$(DOCTMPNAME)/imc/epresenter/filesdk/
	cp $(TOPDIR)/imc/lecturnity/info/*.java $(TOPDIR)/$(DOCTMPNAME)/imc/lecturnity/info/
	cp $(TOPDIR)/imc/epresenter/filesdk/Metadata.java $(TOPDIR)/$(DOCTMPNAME)/imc/epresenter/filesdk/
	cp $(TOPDIR)/imc/epresenter/filesdk/ThumbnailData.java $(TOPDIR)/$(DOCTMPNAME)/imc/epresenter/filesdk/
	javadoc -d $(TOPDIR)/$(DOCTMPNAME)/doc -sourcepath $(TOPDIR)/$(DOCTMPNAME) \
      -public -notree -noindex -nohelp -nodeprecatedlist \
      -windowtitle "Retrieval interface of LECTURNITY" \
      imc.lecturnity.info imc.epresenter.filesdk

# the above fails (error code) but produces output, so this is the continuing of the operation:
doc2:
	jar cf $(TOPDIR)/$(DOCZIP) -C $(TOPDIR)/$(DOCTMPNAME)/doc .
	rm -rf $(TOPDIR)/$(DOCTMPNAME)

export:
	[ ! -d $(TMPDIR) ] && mkdir $(TMPDIR) || rm -rf $(TMPDIR)/*
	cd $(TOPDIR);\
	for x in $(TODODIRS); do\
                mkdir -p $(TMPNAME)/$$x; cp -RL $$x/* $(TMPNAME)/$$x; done
	cp $(TOPDIR)/imc/epresenter/player/SRStreamPlayer.class $(TOPDIR)/$(TMPNAME)/imc/epresenter/player/
	cp $(TOPDIR)/imc/epresenter/player/Manager*.class $(TOPDIR)/$(TMPNAME)/imc/epresenter/player/
	cp $(TOPDIR)/imc/epresenter/player/HelpDisplay.class $(TOPDIR)/$(TMPNAME)/imc/epresenter/player/
	cp $(TOPDIR)/imc/epresenter/player/Daemon*.class $(TOPDIR)/$(TMPNAME)/imc/epresenter/player/
	cp $(TOPDIR)/imc/epresenter/player/texts_en.properties $(TOPDIR)/$(TMPNAME)/imc/epresenter/player/
	cp -RL $(TOPDIR)/imc/epresenter/player/META-INF $(TOPDIR)/$(TMPNAME)
	rm -rf `find $(TMPDIR) -type d | grep "/CVS\|/\.xvpics\|/tests\|/fonts\|/net\|/ui\|/wizards\|/gif2png"`
	rm `find $(TMPDIR) -type f | grep "\.java$$\|Makefile\|\.h$$\|\.cvsignore$$"`
	jar cf $(TOPDIR)/$(JARNAME) -C $(TMPDIR) .
	rm -rf $(TMPDIR)	

.PHONY: subdirs clean export real tag export copy doc1 doc2

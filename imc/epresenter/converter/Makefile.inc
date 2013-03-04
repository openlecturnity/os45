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

# export:
#	cd $(TOPDIR); jar cvfm converter_$(VERSION).jar imc/epresenter/converter/META-INF/manifest.mf `find imc/epresenter/converter -type f | grep -v ".*java" | grep -v ".*~" | grep -v "CVS" | grep -v "Makefile*" | grep -v ".xvpics" | grep -v ".cvsignore" | grep -v "META-INF"` `find imc/epresenter/filesdk -type f | grep -v ".*java" | grep -v ".*~" | grep -v "CVS" | grep -v "Makefile*" | grep -v ".xvpics" | grep -v ".cvsignore"` imc/epresenter/player/audio/*.class; cp converter_$(VERSION).jar converter.jar

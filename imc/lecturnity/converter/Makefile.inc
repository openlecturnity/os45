FILEFILTER="grep -v \"/CVS/\|/.svn/\|~$$\|\.xvpics\|\.cvsignore\|/META-INF/\|/Makefile\|\.java$$\|/\.\#\|\.psp\|/docs/\|\#\|/templates/\|/templates2/\|/cdtemplates/\""

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

export:
	cd $(TOPDIR) && \
	echo > files.tmp && \
	find imc/lecturnity/converter -type f | eval $(FILEFILTER) >> files.tmp && \
	find imc/lecturnity/util -type f | eval $(FILEFILTER) >> files.tmp && \
	find imc/epresenter/converter -type f | eval $(FILEFILTER) >> files.tmp && \
	find imc/epresenter/filesdk -type f | eval $(FILEFILTER) >> files.tmp && \
	find imc/epresenter/player -type f | eval $(FILEFILTER) >> files.tmp && \
	find imc/epresenter/media -type f | eval $(FILEFILTER) >> files.tmp && \
	jar cvfm publisher.jar imc/lecturnity/converter/META-INF/manifest.mf @files.tmp && \
	cd imc/epresenter/player && \
	jar uvf ../../../publisher.jar `find META-INF/services -type f | eval $(FILEFILTER)` && \
	cd ../../.. && \
	cp publisher.jar publisher_$(VERSION)_b$(RELEASE).jar

release: clean cleantemplate all export copy

releasetag: clean cleantemplate all export tag copy

tag:
	cd $(TOPDIR); \
	for x in $(MODULES); do\
		cvs tag -F publisher_$(SCOREVERSION)_b$(RELEASE) $$x; \
	done;

cleantemplate:
	rm templates/*.* && \
	rm templates/images/*.* && \
   rm -rf templates/* && \
	cvs update -d -P templates && \
   cvs update -d -P templates2

copy:
	cd $(TOPDIR); \
	[ ! -d S:/Releases/Publisher/$(SCOREVERSION)_b$(RELEASE) ] && mkdir S:/Releases/Publisher/$(SCOREVERSION)_b$(RELEASE); \
	[ ! -d "S:/Releases/Publisher/$(SCOREVERSION)_b$(RELEASE)/Streaming Templates" ] && mkdir "S:/Releases/Publisher/$(SCOREVERSION)_b$(RELEASE)/Streaming Templates"; \
	[ ! -d "S:/Releases/Publisher/$(SCOREVERSION)_b$(RELEASE)/Web Templates" ] && mkdir "S:/Releases/Publisher/$(SCOREVERSION)_b$(RELEASE)/Web Templates"; \
	[ ! -d "S:/Releases/Publisher/$(SCOREVERSION)_b$(RELEASE)/Streaming Templates/images" ] && mkdir "S:/Releases/Publisher/$(SCOREVERSION)_b$(RELEASE)/Streaming Templates/images"; \
	[ ! -d "S:/Releases/Publisher/latest/Streaming Templates" ] && mkdir "S:/Releases/Publisher/latest/Web Templates"; \
	[ ! -d "S:/Releases/Publisher/latest/Streaming Templates" ] && mkdir "S:/Releases/Publisher/latest/Streaming Templates"; \
	[ ! -d "S:/Releases/Publisher/latest/Streaming Templates/images" ] && mkdir "S:/Releases/Publisher/latest/Streaming Templates/images"; \
	rm -rf "S:/Releases/Publisher/$(SCOREVERSION)_b$(RELEASE)/Web Templates/*" && \
	rm -f "S:/Releases/Publisher/$(SCOREVERSION)_b$(RELEASE)/Streaming Templates/*.*" && \
	rm -f "S:/Releases/Publisher/$(SCOREVERSION)_b$(RELEASE)/Streaming Templates/images/*.*" && \
	cp -R imc/lecturnity/converter/templates2/* "S:/Releases/Publisher/$(SCOREVERSION)_b$(RELEASE)/Web Templates" && \
	cp imc/lecturnity/converter/templates/*.* "S:/Releases/Publisher/$(SCOREVERSION)_b$(RELEASE)/Streaming Templates" && \
	cp imc/lecturnity/converter/templates/images/*.* "S:/Releases/Publisher/$(SCOREVERSION)_b$(RELEASE)/Streaming Templates/images" && \
	rm -rf "S:/Releases/Publisher/latest/Web Templates/*" && \
	rm -f "S:/Releases/Publisher/latest/Streaming Templates/*.*" && \
	rm -f "S:/Releases/Publisher/latest/Streaming Templates/images*.*" && \
	cp -R imc/lecturnity/converter/templates2/* "S:/Releases/Publisher/latest/Web Templates" && \
	cp imc/lecturnity/converter/templates/*.* "S:/Releases/Publisher/latest/Streaming Templates" && \
	cp imc/lecturnity/converter/templates/images/*.* "S:/Releases/Publisher/latest/Streaming Templates/images" && \
	cp publisher.jar S:/Releases/Publisher/$(SCOREVERSION)_b$(RELEASE)/publisher.jar; \
	cp publisher.jar S:/Releases/Publisher/latest/publisher.jar

old-export:
	cd $(TOPDIR); jar cvfm publisher_$(VERSION).jar imc/lecturnity/converter/META-INF/manifest.mf `find imc/epresenter/converter -type f | grep -v ".*java" | grep -v ".*~" | grep -v "CVS" | grep -v "Makefile*" | grep -v ".xvpics" | grep -v ".cvsignore" | grep -v "META-INF"` `find imc/epresenter/filesdk -type f | grep -v ".*java" | grep -v ".*~" | grep -v "CVS" | grep -v "Makefile*" | grep -v ".xvpics" | grep -v ".cvsignore"` imc/epresenter/player/audio/*.class imc/epresenter/player/whiteboard/*.class imc/epresenter/player/util/*.class `find imc/lecturnity/converter -type f | grep -v ".*java" | grep -v ".*~" | grep -v "CVS" | grep -v "Makefile*" | grep -v ".xvpics" | grep -v ".cvsignore" | grep -v "META-INF"` `find imc/lecturnity/util -type f | grep -v ".*java" | grep -v ".*~" | grep -v "CVS" | grep -v "Makefile*" | grep -v ".xvpics" | grep -v ".cvsignore" | grep -v "META-INF"`; cp publisher_$(VERSION).jar publisher.jar

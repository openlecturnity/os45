FILEFILTER="grep -v \"/CVS/\|~$$\|\.xvpics\|\.cvsignore\|/META-INF/\|/Makefile\|\.java$$\|/\.\#\|\.psp\|/docs/\""

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

gif2png: clean all gif2pngexport

gif2pngexport:
	cd $(TOPDIR) && \
	echo > files.tmp && \
	find imc/lecturnity/util -type f | eval $(FILEFILTER) >> files.tmp && \
	find imc/epresenter/filesdk -type f | eval $(FILEFILTER) >> files.tmp && \
	find imc/epresenter/converter -type f | eval $(FILEFILTER) >> files.tmp && \
	find imc/lecturnity/converter/images/wizard_left_side.gif >> files.tmp && \
	find imc/lecturnity/converter/images/wizard_header_right.gif >> files.tmp && \
	find imc/lecturnity/converter/images/lecturnity_logo.gif >> files.tmp && \
	find imc/lecturnity/converter/images/icon_conv.gif >> files.tmp && \
	jar cvfm docopt.jar imc/lecturnity/util/gif2png/META-INF/manifest.mf @files.tmp && \
	cp docopt.jar docopt_$(SCOREGIF2PNGVERSION).jar

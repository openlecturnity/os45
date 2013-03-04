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


eppC.exe v0.1b 03/09/01 QUICK DOCUMENTATION
======================================================================

               eppC - external Controller for aofJSync

Table of Contents
-----------------

     1) Introduction
     2) System requirements


1) Introduction :
-----------------

	eppC is a command-line utility that interacts with ePresentationPlayer (ePP).


	The supported commands are:

	-help: eppC reports the possible parameters
	-time timeValue: if there is an ePP with a loaded Presentation, 
		the ePP jumps to the time (in milliseconds) given by timeValue;
	-start: if there is an ePP with a loaded Presentation, the 
		ePP start the replay;
	-stop: if there is an ePP with a loaded Presentation, the 
	       ePP stop the replay;
	ap-Filename: if there is an ePP, eppC prompts ePP 
		to load the presentation and to jump to the time given by ap-File. 
		Otherwise aofControl starts ePP with the presentation and 
		the time given by the ap-File.
	epf-Filename: if there is an ePP, eppC prompts ePP
		to load the presentation given by epf-File. 
		Otherwise eppC starts ePP with the presentation 
		given by the epf-File.

2.) System requirements:
------------------------

	eppC works under Windows (98, NT, 2000), Linux and Solaris
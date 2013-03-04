# Makefile 11.07.02

TOPDIR = ../../..
LOCALDIR = imc/lecturnity/converter

SUBDIRS = wizard \
	    ../util \
	    ../../epresenter/converter \
            ../../epresenter/player

MODULES = imc/lecturnity/converter \
	  imc/lecturnity/util \
	  imc/epresenter/converter \
	  imc/epresenter/filesdk \
	  imc/epresenter/player \
	  imc/epresenter/media \
	  libs \
	  imc/native/util/win32 \
	  imc/epresenter/controller

CLASSES = ConverterWizard.class StreamingMediaConverter.class \
	WindowsMediaConverter.class RealMediaConverter.class \
	SlideEntry.class TelepointerEntry.class \
	ThumbnailEntry.class \
	PictureExtracter.class \
	MyButtonArea.class \
	WmpProfileInfo.class \
	Snippet.class SnippetCreator.class \
	TemplateSnippetCreator.class FlashSnippetCreator.class \
	StreamInformation.class \
	DocumentData.class ProfiledSettings.class \
	LPLibs.class DocumentInteractionInfo.class \
	ScreengrabbingExtended.class DptCommunicator.class \
	ProgressManager.class ProgressCommunicator.class \
	DefaultProfiles.class

VERSION = `cat $(LOCALDIR)/VERSION`
SCOREVERSION = `cat $(LOCALDIR)/VERSION | tr \. _`
RELEASE = `cat $(LOCALDIR)/INTERNAL_RELEASE`

include Makefile.inc

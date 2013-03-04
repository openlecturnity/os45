#ifndef __AUDIOMIXER
#define __AUDIOMIXER

#include "global.h"

typedef struct
{
   int srcID;
   int srcIndex;
   _TCHAR tszName[MIXER_LONG_NAME_CHARS];
   bool isActive;
   bool hasVolumeControl;
   bool isEnabled;
   DWORD dwMinVolume;
   DWORD dwMaxVolume;
   DWORD dwSteps;
} AudioSource;

typedef LRESULT (CALLBACK* MIXERCALLBACK) (HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

class AVGRABBER_EXPORT AudioMixer
{
private:
	HMIXER mixer_;
	bool mixerIsOpen_;
	bool mixerInitialized_;
   bool mixerEventThreadExited_;
   bool isBusy_;
   HWND mixerHwnd_;

	MIXERCONTROLDETAILS_BOOLEAN*  sourceActive_;
	MIXERCONTROLDETAILS_LISTTEXT* sourceLabels_;
	MIXERCONTROLDETAILS_UNSIGNED* sourceVolumes_;
   MIXERCONTROLDETAILS*          sourceDetails_;
   MIXERCONTROLDETAILS*          muxDetails_;

   MIXERCALLBACK lpMixerCallback_;
   HANDLE        closeEvent_;
   HANDLE        createEvent_;
   HANDLE        mixerEvent_;
   bool          eventThreadQuitRequest_;

   AudioSource* audioSources_;

	int sourceCount_;
   int activeSource_;

   static AudioMixer* instance;

   void initSources();
   HWND createMixerHwnd();
   void destroyMixerHwnd();

   //static DWORD WINAPI startWindowLoop(LPVOID param);
   //static DWORD WINAPI mixerEventThread(LPVOID param);
   static void __cdecl startWindowLoop(LPVOID param);
   static void __cdecl mixerEventThread(LPVOID param);
   static LRESULT CALLBACK dummyWindowProc(HWND hWnd, UINT msg, 
                                           WPARAM wParam, LPARAM lParam);

public:

   /*
   Creates an AudioMixer instance for the given waveIn ID. This
   has to be a number between 0 and the number of devices returned
   by getAudioDeviceCount() of the AvGrabber class. Note: This
   constructor *never* throws any exceptions. It will always succeed.
   It may still be the case that a connection could not be
   established with the hardware mixer device. In that case, or if
   something else goes wrong, the isEnabled() method returns false.

   Even if the AudioMixer is not enabled, do not forget to delete it
   after use.

   The mixerCallback parameter has to be a pointer to a static callback 
   function having the following signature:

   static LRESULT CALLBACK mixerCallback(MSG* msg);

   If NULL, the callback is not used.
   */
   AudioMixer(int waveInID, MIXERCALLBACK mixerCallback=NULL);

   /*
   Destructor.
   */
   ~AudioMixer();

   /*
   Returns true if the Mixer is usable, otherwise false.
   */
   bool isEnabled();

   /*
   Returns the number of audio sources for the current waveIn
   device (this AudioMixer instance).
   */
   int getSourceCount();

   /*
   Fills the audioSources array with data for the audio sources
   of this AudioMixer instance. The array has to be as long as
   the value returned by getSourceCount() and must have been
   previously allocated.
   */
   void getSources(AudioSource* audioSources[]);

   /*
   Sets the audio source to use for waveIn recording purposes.
   */
   void setSource(int srcID);

   /*
   Returns the currently set audio source ID. If isEnabled() returns
   false, this value is not really defined and may be whatnot.
   */
   int getSource();

   /*
   Sets the recording volume of the selected audio source.
   Has to be a value between 0 and 100.
   */
   void setVolume(int volume);

   /* 
   Returns the recording volume of the selected audio source.
   Is a value between 0 and 100, or -1 if the current volume
   could not be retrieved.
   */
   int getVolume();

};

#endif

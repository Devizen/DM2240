#include "AudioPlayer.h"
CAudioPlayer* CAudioPlayer::s_instance = 0;

CAudioPlayer::CAudioPlayer(void) :
	 audioPlayer(nullptr)
{
}

CAudioPlayer::~CAudioPlayer(void)
{
}

CAudioPlayer * CAudioPlayer::GetInstance(void)
{
	if (s_instance == nullptr)
		s_instance = new CAudioPlayer();
	return s_instance;
}

irrklang::ISoundEngine * CAudioPlayer::GetISoundEngine(void)
{
	if (audioPlayer == nullptr)
		audioPlayer = irrklang::createIrrKlangDevice();
	return audioPlayer;
}

#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H
#include "../../irrKlang/include/irrKlang.h"
class CAudioPlayer
{
	static CAudioPlayer* s_instance;
public:
	CAudioPlayer(void);
	~CAudioPlayer(void);

	static CAudioPlayer* GetInstance(void);
	irrklang::ISoundEngine* GetISoundEngine(void);
private:
	irrklang::ISoundEngine* audioPlayer;
protected:
};

#endif
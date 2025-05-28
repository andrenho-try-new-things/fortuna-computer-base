#ifndef AUDIO_HH
#define AUDIO_HH

#include <cstdint>
#include <cstddef>

namespace audio {

enum Note : uint32_t
{
    C4 = 261626, D4 = 293665, E4 = 329628, F4 = 349228,
    Pause = 0,
};

struct Sound {
    Note     note;
    uint32_t time;
};

void init();
void set_music(Sound* sounds, size_t sz);
void play_music(bool nonstop);
void stop_music();

}

#endif //AUDIO_HH

#include "enf_sound.hpp"
#include "../common/assert.hpp"

#ifdef _WIN32
#warning "NOT TESTED"
#include <windows.h>
#else
#include <alsa/asoundlib.h>
#include <fstream>
#include <vector>
#endif

namespace Enforcer {
void Sound::Play() {}
Sound::Sound(u32 id, [[maybe_unused]] const char *filePath) : id{id} {
  /*
#ifdef _WIN32
  ASSERT_LOG(PlaySound(TEXT(filePath), NULL, SND_FILENAME | SND_ASYNC),
             "Failed to play sound {}!", filePath);
#else
  snd_pcm_t *handle;
  int err{snd_pcm_open(&handle, "default", SND_PCM_STREAM_PLAYBACK, 0)};
  ASSERT_LOG(err >= 0, "Unable to open PCM device => {}!", snd_strerror(err));

  std::ifstream file{filePath, std::ios::binary};
  if (!file) {
    snd_pcm_close(handle);
    ASSERT_LOG(false, "Could not open sound file => {}", filePath);
  }

  std::vector<u8> buffer(44100 * 2 * 5);
  file.read(reinterpret_cast<char *>(buffer.data()), buffer.size());
  if (file.gcount() == 0) {
    snd_pcm_close(handle);
    ASSERT_LOG(false, "Error reading from sound file => {}", filePath);
  }

  err = snd_pcm_writei(handle, buffer.data(),
                       file.gcount() / 4); // 2 bytes per sample, 2 channels
  if (err < 0) {
    snd_pcm_close(handle);
    ASSERT_LOG(false, "Error while writing to PCM device: {}",
               snd_strerror(err));
  }

  snd_pcm_close(handle);
#endif
*/
}

} // namespace Enforcer

#ifndef FLORA_CHARACTER_STREAM
#define FLORA_CHARACTER_STREAM

#include <fstream>

#include "flora.h"
#include "character-predicates.h"

namespace flora {

class CharacterStream {
public:
  NOCOPY_CLASS(CharacterStream)

  CharacterStream() = default;
  virtual ~CharacterStream() = default;
  virtual char Advance() = 0;
};

class FileCharacterStream {
public:
  NOCOPY_CLASS(FileCharacterStream)

  FileCharacterStream(const char *filename) : stream_(filename) { }

  virtual int Advance() {
    if (!stream_) return character::EOS;
    return stream_.get();
  }
private:
  std::ifstream stream_;
};

}

#endif
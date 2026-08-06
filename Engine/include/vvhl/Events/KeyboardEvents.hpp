#pragma once
#include <vvhl/Events/Event.hpp>

namespace vvhl {

class KeyboardEvent : public Event {
public:
  KeyboardEvent(int key, int mods) : Key(key), Mods(mods) {}

  int Key;
  int Mods;
};

//------------------------------------------------------------

class KeyPressedEvent final : public KeyboardEvent {
public:
  KeyPressedEvent(int key, int mods, bool repeated)
      : KeyboardEvent(key, mods), Repeated(repeated) {}

  bool Repeated;
};

//------------------------------------------------------------

class KeyReleasedEvent final : public KeyboardEvent {
public:
  KeyReleasedEvent(int key, int mods) : KeyboardEvent(key, mods) {}
};

//------------------------------------------------------------

class CharInputEvent final : public Event {
public:
  explicit CharInputEvent(uint32_t codepoint) : Codepoint(codepoint) {}

  uint32_t Codepoint;
};

} // namespace vvhl
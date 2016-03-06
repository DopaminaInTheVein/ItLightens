#ifndef INC_IO_KEYBOARD_H_
#define INC_IO_KEYBOARD_H_

#include "digital_button.h"
#include <map>

class TKeyBoard {
  
public:
	std::map< int, CDigitalButton > active_keys;
  void update(float dt);
  void sysSysStatus(int key_code, const bool &is_pressed);
  const CDigitalButton &operator[](int key_code);
};

// io.keys['3'].isPressed()
bool isKeyPressed(int key_code);

#endif


#ifndef INC_IO_MOUSE_H_
#define INC_IO_MOUSE_H_

#define MOUSE_DELTA_MAX	10
#include "digital_button.h"

class TMouse {
  void centerSysMouse();
  HWND hWnd;

public:

  // Coords to center the mouse in client space
  int cx, cy;

  // Last delta position
  int dx;
  int dy;

  // Current position in client space
  int x, y;

  //
  bool captured_by_app;

  CDigitalButton left;
  CDigitalButton right;
  CDigitalButton middle;
  int            wheel;     // 0 in pause

  TMouse();
  void start(HWND hWnd);
  void update(float dt);

  // Para recibir los mensajes de windows
  // incluso cuando el cursor se vaya fuera
  // de la ventana.
  void capture();
  void release();
  void toggle();

  // Para que el OS me informe de la current
  // mouse position
  void setSysMouseLoc(int new_x, int new_y);

};


#endif


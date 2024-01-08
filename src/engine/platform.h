#ifndef AG_PLATFORM_H
#define AG_PLATFORM_H

struct Window;

void createWindow(Window&);
void destroyWindow(Window&);


/* TODO: Reword
  Returns a pointer to the function used to retrieve function pointers from OpenGL by name.To use the returned pointer cast it to
  void* (const char* procname), when called the returned function will return the OpenGL function requested so long as the provided
  window is initialized. If the window is not initialized the function will return 0.
*/
void* getGLProcFunctionPointer(Window&);


void updateWindow(Window&);
unsigned long long getClock();
double getTime();
bool getKeyDown(Window&, int key);
void getCursorPosition(Window&, double& x, double& y);

#endif
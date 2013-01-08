static char*NAME="ckill |b087ec466c8101293bb9d934e37d7ab0|";

typedef struct{
  WINDOW*leftbox;
  WINDOW*rightbox;
  WINDOW*main_window;
}windows;

void window_setup();
void *ckill_ui(void*ptr);

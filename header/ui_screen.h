static char*NAME="ckill |b087ec466c8101293bb9d934e37d7ab0|";

typedef struct{
  WINDOW*leftbox;
  WINDOW*rightbox;
  WINDOW*main_window;
}windows;

void window_setup();
void *ckill_ui(void*ptr);
void print_in_middle(WINDOW*win, int starty,int startx,int width,
		     char*string, chtype color);

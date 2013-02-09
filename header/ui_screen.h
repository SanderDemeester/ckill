#define __SCREEN_UI_H
#define SIZE(a) (sizeof(a)/sizeof(a[0]))

typedef struct{
  WINDOW*leftbox;
  WINDOW*rightbox;
  WINDOW*main_window;
  int d; //spacing between x-axis
  MENU*menu; //the main menu
  ITEM **items; //the item list;
  
}ncurses_data;

void window_setup();
void *ckill_ui(void*ptr);
void print_in_middle(ncurses_data*w, int starty,int startx,int width,
		     char*string, chtype color,int c);

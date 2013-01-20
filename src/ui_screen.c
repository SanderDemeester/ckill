#ifndef CKILL_HEADER
#include "header/ckill.h"
#endif
#include "header/ui_screen.h"

void *ckill_ui(void*ptr){
  windows*win_struct = (windows*) malloc(sizeof(windows));

  int width = 0;
  int height = 0;
  int row = 0;
  int col = 0;
  int input = 0;
  char* hostname = (char*) malloc(sizeof(char)*1024);
  char*list[] = {"t1","t2","t3",(char*)NULL};
  int current = 1;
  int n_element = SIZE(list);
  MENU*menu;
  ITEM **items = (ITEM**) calloc(n_element,sizeof(ITEM*));
  for(int i = 0; i < n_element; i++)
    items[i] = new_item(list[i],list[i]);  

  
  //make menu
  menu = new_menu((ITEM**)items);
  set_menu_mark(menu,"*");

  gethostname(hostname,1023);


  window_setup(win_struct);  //setup different window, now just stub.

  initscr(); //start ncurses
  cbreak(); //interperter control character as normal characters

  getmaxyx(stdscr,row,col); row++;//get terminal cordinate
  height = 8; //height left and rightbox
  width = col/2; //width for left and rightbox
  
  start_color(); //enable coloring
  init_pair(1,COLOR_BLUE,COLOR_WHITE);

  // init out three different windows.
  win_struct->leftbox = newwin(height,width,0,0);
  win_struct->rightbox = newwin(height,width,0,col/2);
  win_struct->main_window = newwin(col-height,col,height,0);
  
  //give our windows there color
  wbkgd(win_struct->leftbox,COLOR_PAIR(1));
  wbkgd(win_struct->rightbox,COLOR_PAIR(1));
  wbkgd(win_struct->main_window,COLOR_PAIR(1));

  //give our windows a nice box
  box(win_struct->leftbox,0,0);
  box(win_struct->rightbox,0,0);
  box(win_struct->main_window,0,0);

  //place our windows on the screen
  wrefresh(win_struct->main_window);
  wrefresh(win_struct->leftbox);
  wrefresh(win_struct->rightbox);

  //fill in the information fielfd
  mvwprintw(win_struct->leftbox,0,(col/8)-5,NAME);
  mvwprintw(win_struct->leftbox,1,1,"hostname: %s",hostname);
  mvwprintw(win_struct->leftbox,2,1,"interface: ");
  mvwprintw(win_struct->leftbox,3,1,"number of flows: ");
  
  wrefresh(win_struct->leftbox);

  mvwprintw(win_struct->main_window,1,1,"flow id");
  mvwprintw(win_struct->main_window,1,9,"dst_ip");
  mvwprintw(win_struct->main_window,1,19,"src_ip");

  //put focus in main window
  
  
  //enable keypad on main_window
  keypad(win_struct->main_window,TRUE);

  set_menu_win(menu,win_struct->main_window);
  set_menu_sub(menu,win_struct->main_window);
  set_menu_format(menu,5,1);

  print_in_middle(win_struct->main_window,1,0,40,"Menu",COLOR_PAIR(1));

  mvwaddch(win_struct->main_window,2,0,ACS_LTEE);
  mvwhline(win_struct->main_window,2,1,ACS_HLINE,38);
  mvwaddch(win_struct->main_window,2,39,ACS_RTEE);
  
  post_menu(menu);
  wrefresh(win_struct->main_window);
  
  while((input = wgetch(win_struct->main_window)) != KEY_F(1)){
    switch(input){
    case KEY_DOWN:
      menu_driver(menu,REQ_DOWN_ITEM);
      break;
    case KEY_UP:
      menu_driver(menu,REQ_UP_ITEM);
      break;
    }
    wrefresh(win_struct->main_window);
  }
  unpost_menu(menu);
  free_menu(menu);
  for(int i = 0; i < n_element; i++)
    free_item(items[i]);
  
  endwin();
  return 0;
}

void window_setup(windows*win_struct){

  win_struct->leftbox = NULL;
  win_struct->rightbox = NULL;
  win_struct->main_window = NULL;
}

void print_in_middle(WINDOW*win, int starty,int startx,int width,
		     char*string, chtype color){
  int l = strlen(string);
  int x;
  int y;
  int t;

  if(startx != 0) x = startx;
  if(starty != 0) y = starty;

  if(width == 0) width = 80;
  
  t = (width - l/2);
  x = startx + (int)t;
  mvwprintw(win,y,x,"%s",string);
  refresh();
}

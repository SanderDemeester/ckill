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
  char* hostname = (char*) malloc(sizeof(char)*1024);
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

  //put focus in main window
  wrefresh(win_struct->main_window);


  while(1){    
  }

  
  endwin();
  return 0;
}

void window_setup(windows*win_struct){

  win_struct->leftbox = NULL;
  win_struct->rightbox = NULL;
  win_struct->main_window = NULL;
}

void print_menu(WINDOW *w, int current, char*list[], int n){
  
  int x = 2; //x-cordinate
  int y = 2; //y-cordinate
  int i = 0; //index counter


  for(; i < n; i++){
    if(current == i+1){
      wattron(w, A_REVERSE);
      mvwprintw(w,y,x,"%s",list[i]);
      wattroff(w,A_REVERSE);
    }else{
      mvwprintw(w, y,x,"%s",list[i]);
    }
    ++y;
  }
  wrefresh(w);
  
}

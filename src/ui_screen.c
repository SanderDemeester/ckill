#ifndef CKILL_HEADER
#include "header/ckill.h"
#endif
#ifndef __SCREEN_UI_H
#include "header/ui_screen.h"
#endif

static char*NAME="ckill |b087ec466c8101293bb9d934e37d7ab0|";

void *ckill_ui(void*ptr){
  ncurses_data*win_struct = (ncurses_data*) malloc(sizeof(ncurses_data));
  pthread_context*pcontext = (pthread_context*)ptr;

  int width = 0;
  int height = 0;
  int row = 0;
  int col = 0;
  int input = 0;
  char* hostname = (char*) malloc(sizeof(char)*1024);

  pcontext->items = (ITEM**) calloc(NUMBER_OF_MENU_ENTRYS,sizeof(ITEM*));
  pcontext->items[0] = new_item((char*)NULL,(char*)NULL);

  //make menu
  pcontext->menu = new_menu((ITEM**)pcontext->items);

  gethostname(hostname,1023);

  pthread_mutex_lock(pcontext->ui_mutex);
  window_setup(win_struct);  //setup different window, now just stub.



  initscr(); //start ncurses
  cbreak(); //interperter control character as normal characters
  //  noecho(); //disable printing characters typed

  getmaxyx(stdscr,row,col); 
  height = 8; //height left and rightbox
  width = col/2; //width for left and rightbox
  win_struct->d = col/7;
  
  start_color(); //enable coloring
  init_pair(1,COLOR_YELLOW,COLOR_BLACK);

  pcontext->row = row;
  pcontext->height = height;
  pcontext->widht = width;
  pcontext->col = col;

  // init out three different windows.
  win_struct->leftbox = newwin(height,width,0,0);
  win_struct->rightbox = newwin(height,width,0,col/2);
  win_struct->main_window = newwin(row-height,col,height,0);
  
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
  
  mvwprintw(win_struct->leftbox,2,1,"interface: %s",pcontext->arg->inf);
  mvwprintw(win_struct->leftbox,3,1,"number of flows: ");

  //for now just print in rightbox some debug info
  mvwprintw(win_struct->rightbox,1,1,"row: %d",row);
  mvwprintw(win_struct->rightbox,2,1,"col: %d",col);
  mvwprintw(win_struct->rightbox,3,1,"width: %d",width);
  mvwprintw(win_struct->rightbox,4,1,"height: %d",height);
  
  //enable keypad on main_window  
  wtimeout(win_struct->main_window,5); //non-blocking
  keypad(win_struct->main_window,TRUE);
  set_menu_back(pcontext->menu,COLOR_RED);

  set_menu_win(pcontext->menu,win_struct->main_window);
  set_menu_sub(pcontext->menu,derwin(win_struct->main_window,(row-height)-4,width+10,3,1));
  set_menu_format(pcontext->menu,(row-height)-4,1);
  set_menu_mark(pcontext->menu,"* ");

  print_in_middle(win_struct,1,2,1,"Menu",COLOR_PAIR(1),col);

  mvwaddch(win_struct->main_window,2,0,ACS_LTEE);
  mvwhline(win_struct->main_window,2,1,ACS_HLINE,col-2);
  mvwaddch(win_struct->main_window,2,col-1,ACS_RTEE);
  
  post_menu(pcontext->menu);

  wrefresh(win_struct->main_window);
  pcontext->ncurses_window = win_struct;
  pthread_mutex_unlock(pcontext->ui_mutex);

  while((input = wgetch(win_struct->main_window)) != KEY_F(1)){
    switch(input){
    case KEY_DOWN:
      menu_driver(pcontext->menu,REQ_DOWN_ITEM);
        break;
    case KEY_UP:
      menu_driver(pcontext->menu,REQ_UP_ITEM);
        break; 
    case KEY_NPAGE:
      menu_driver(pcontext->menu, REQ_SCR_DPAGE);
        break;
    case KEY_PPAGE:
      menu_driver(pcontext->menu, REQ_SCR_UPAGE);
        break;
    }

    pthread_mutex_lock(pcontext->list_mutex);
    pthread_mutex_lock(pcontext->ui_mutex);

    if(pcontext->new_data && pcontext->number_of_menu_elements > 0){
      short index = item_index(current_item(pcontext->menu));
      /* Start making updated menu */
      for(int i = 0; i < pcontext->number_of_menu_elements; i++)
	pcontext->items[i] = new_item(pcontext->list[i],
				      pcontext->ip[i]);
      pcontext->menu = new_menu((ITEM**)pcontext->items);

      set_menu_sub(pcontext->menu,derwin(win_struct->main_window,(row-height)-4,width,3,1));
      set_menu_format(pcontext->menu,(row-height)-4,1);
      set_menu_mark(pcontext->menu,"* ");

      print_in_middle(win_struct,1,2,1,"Menu",COLOR_PAIR(1),col);
      
      mvwaddch(win_struct->main_window,2,0,ACS_LTEE);
      mvwhline(win_struct->main_window,2,1,ACS_HLINE,col-2);
      mvwaddch(win_struct->main_window,2,col-1,ACS_RTEE);

      if(index > 0)
	set_current_item(pcontext->menu, pcontext->items[index]);

      post_menu(pcontext->menu);      
      pcontext->new_data = 0;      
      /* End making updated menu */
    }
    /* Refresh main_window */
    wrefresh(win_struct->main_window);
    
    /* Release mutex */
    pthread_mutex_unlock(pcontext->ui_mutex);
    pthread_mutex_unlock(pcontext->list_mutex);
    
  }
  unpost_menu(pcontext->menu);
  endwin();
  return 0;
}

void window_setup(ncurses_data*win_struct){

  win_struct->leftbox = NULL;
  win_struct->rightbox = NULL;
  win_struct->main_window = NULL;
}

void print_in_middle(ncurses_data*w, int starty,int startx,int width,
		     char*string, chtype color,int c){
  if(width == 0) width = 80;
  int d = c/7;
  //y,x
  mvwprintw(w->main_window,1,3,"id",NULL);
  mvwprintw(w->main_window,1,d-2,"dest ip",NULL);
  
  mvwprintw(w->main_window,1,2*d,"src ip",NULL);
  mvwprintw(w->main_window,1,3*d,"BH",NULL);
  mvwprintw(w->main_window,1,4*d,"flow rate/s",NULL);
  mvwprintw(w->main_window,1,5*d,"FLAGS",NULL);
  mvwprintw(w->main_window,1,6*d,"STATE",NULL);
  wrefresh(w->main_window);
  wrefresh(w->leftbox);
  wrefresh(w->rightbox);

}

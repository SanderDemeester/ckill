#ifndef CKILL_HEADER
#include "header/ckill.h"
#endif

void *ckill_ui(void*ptr){
  int row;
  int col;
  initscr();
  clear();
  noecho();
  cbreak(); //line buffering disabled.
  
  getmaxyx(stdscr,row,col);
  refresh();
  
  while(1){
  } 
}

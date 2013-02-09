#include "header/process_ui_queue_events.h"
#ifndef CKILL_HEADER
#include "header/ckill.h"
#endif

void *process_ui_queue_events(void*ptr){
  pthread_context*pcontext = (pthread_context*)ptr;
  int c = 1; //Control variabel for while loop.
  khiter_t itr; //Iterator for khash
  flow*f; // A temp location for the flow
  int counter = 0;
  char*list_element = NULL; //we use this as a temp location for constructing to first element of ITEM.
  char*item_element = NULL; //we use this as a temp location for construction to the second element of ITEM.
  char*dummy = "IP";
  int number_of_spaces = 0;

  /* Take pointer to khash */
  pthread_mutex_lock(pcontext->khash_mutex);
  khash_t(32)*flow_hashmap = pcontext->arg->flow_hashmap;
  pthread_mutex_unlock(pcontext->khash_mutex);

  list_element = (char*) malloc(sizeof(int));
  
  while(c){
    int r = sleep(2); //suspend execution for 2 seconds
    if(!r){
      pthread_mutex_lock(pcontext->khash_mutex);
      pthread_mutex_lock(pcontext->item_mutex);
      

      counter = 1; //start with 1.
      for(itr = kh_begin(flow_hashmap); itr != kh_end(flow_hashmap);++itr){

      
      	  // Check if bucket is used
      	if(kh_exist(flow_hashmap,itr) && counter > NUMBER_OF_MENU_ENTRYS){
      	  f = kh_value(flow_hashmap,itr);

      	  list_element = (char) counter;
      	  pcontext->items[counter] = new_item(list_element+48,dummy);
      	  counter++;
	  
      	  printf("ip: %d.%d.%d.%d:%d - seq: %d && ack: %d %d\n",
      		 ((f->iph->src_adr)       & 0x000000FF),
      		 ((f->iph->src_adr >> 8)  & 0x000000FF),
      		 ((f->iph->src_adr >> 16) & 0x000000FF),
      		 ((f->iph->src_adr >> 24)    & 0x000000FF),
      		 htons(f->tcph->src_port),
      		 htons(f->tcph->seq),
      		 htons(f->tcph->ack),
      		 f->size
      		 );
	  
      	  printf("ip: %d.%d.%d.%d:%d\n",
      		 ((f->iph->dst_adr)       & 0x000000FF),
      		 ((f->iph->dst_adr >> 8)  & 0x000000FF),
      		 ((f->iph->dst_adr >> 16) & 0x000000FF),
      		 ((f->iph->dst_adr >> 24) & 0x000000FF),
      		 htons(f->tcph->dst_port)
      		 );
      	}
      }
      pthread_mutex_lock(pcontext->ui_mutex);
      pcontext->items[counter] = new_item((char*)NULL,(char*)NULL);
      pcontext->menu = new_menu((ITEM**)pcontext->items);
      set_menu_sub(pcontext->menu,derwin(pcontext->ncurses_window->main_window,
					 (pcontext->row-pcontext->height)-4,pcontext->widht,3,1));
      set_menu_format(pcontext->menu,(pcontext->row-pcontext->height)-4,1);
      set_menu_mark(pcontext->menu,"* ");
      print_in_middle(pcontext->ncurses_window->main_window,
		      1,2,1,"Menu",COLOR_PAIR(1),pcontext->col);
      
      mvwaddch(pcontext->ncurses_window->main_window,2,0,ACS_LTEE);
      mvwhline(pcontext->ncurses_window->main_window,2,1,ACS_HLINE,pcontext->col-2);
      mvwaddch(pcontext->ncurses_window->main_window,2,pcontext->col-1,ACS_RTEE);
      post_menu(pcontext->menu);
      wrefresh(pcontext->ncurses_window->main_window);

      pthread_mutex_unlock(pcontext->ui_mutex);
      pthread_mutex_unlock(pcontext->item_mutex);
      pthread_mutex_unlock(pcontext->khash_mutex);
    }else{
      //something went wrong using the thread. Abort
      c = !c; //flip 
    }
  }
  
}

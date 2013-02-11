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
  char*test = (char*) calloc(100,sizeof(char));
  int number_of_spaces = 0;

  /* Take pointer to khash */
  pthread_mutex_lock(pcontext->khash_mutex);
  khash_t(32)*flow_hashmap = pcontext->arg->flow_hashmap;
  pthread_mutex_unlock(pcontext->khash_mutex);

  list_element = (char*) malloc(sizeof(int));
  
  while(c){
    int r = sleep(2); //suspend execution for 2 seconds
    if(!r){

      /* Take hashmap mutex */
      pthread_mutex_lock(pcontext->khash_mutex);
      
      /* Take list mutex */
      pthread_mutex_lock(pcontext->list_mutex);
      
      /* Reset counter every time we re-run throug our hashmap */
      counter = 0;

      for(itr = kh_begin(flow_hashmap); itr != kh_end(flow_hashmap); ++itr){
	if(kh_exist(flow_hashmap,itr)){
	  f = kh_value(flow_hashmap,itr);
	  snprintf(test,100,"%d",f->flow_id);
	  printf("%s \n",test);
	  pcontext->list[counter] = "sander\0";
	  pcontext->ip[counter]   = "nok\0";
	  counter++;
	}
      }
      
      /* The counter will now be 1 to high, so we end  */
      /* 	our list with a (char*) NULL pointer */
      pcontext->list[counter] = (char*)NULL;
      pcontext->ip[counter]   = (char*)NULL;
      pcontext->number_of_menu_elements = counter;
      pcontext->new_data = 1;
      pthread_mutex_unlock(pcontext->khash_mutex);
      pthread_mutex_unlock(pcontext->list_mutex);

    }else{
      //something went wrong using the thread. Abort
      c = !c; //flip 
    }
  }
  
}

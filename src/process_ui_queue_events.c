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

    }else{
      //something went wrong using the thread. Abort
      c = !c; //flip 
    }
  }
  
}

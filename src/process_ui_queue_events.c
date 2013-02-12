#include "header/process_ui_queue_events.h"
#ifndef CKILL_HEADER
#include "header/ckill.h"
#endif

int compare(const void*a,
	    const void*b){
  return (*(int*)a - *(int*)b);
}

void *process_ui_queue_events(void*ptr){
  pthread_context*pcontext = (pthread_context*)ptr;
  int c = 1; //Control variabel for while loop.
  khiter_t itr; //Iterator for khash
  flow*f; // A temp location for the flow
  int counter = 0;

  /* Take pointer to khash */
  pthread_mutex_lock(pcontext->khash_mutex);
  khash_t(32)*flow_hashmap = pcontext->arg->flow_hashmap;
  pthread_mutex_unlock(pcontext->khash_mutex);

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
	  snprintf(
		   pcontext->list[counter],
		   100,
		   "%u",(uint32_t)f->flow_id);
	  snprintf(pcontext->ip[counter],
		   300,
		   "%*s%d.%d.%d.%d %*s %d.%d.%d.%d|%d|size:%d%*s",
		   (pcontext->col/7)-(7+strlen(pcontext->list[counter])),
		   " ",
		   (uint32_t)(f->iph->src_adr & 0x000000FF),
		   (uint32_t)(((f->iph->src_adr >> 8 ) & 0x000000FF)),
		   (uint32_t)(((f->iph->src_adr >> 16) & 0x000000FF)),
		   (uint32_t)(((f->iph->src_adr >> 24) & 0x000000FF)),
		   10," ",
		   (uint32_t)(f->iph->dst_adr & 0x000000FF),
		   (uint32_t)(((f->iph->dst_adr >> 8 ) & 0x000000FF)),
		   (uint32_t)(((f->iph->dst_adr >> 16) & 0x000000FF)),
		   (uint32_t)(((f->iph->dst_adr >> 24) & 0x000000FF)),
		   strlen(pcontext->list[counter]),
		   f->size,
		   80," "
		   );
	  counter++;
	}
      }
      /* The counter will now be 1 to high, so we end  */
      /* 	our list with a (char*) NULL pointer */
      qsort(pcontext->list,counter-1,LEN_MENU_STR,compare);
      *pcontext->list[counter] = (char*)NULL;
      *pcontext->ip[counter]   = (char*)NULL;
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

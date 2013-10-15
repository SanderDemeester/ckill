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

  //Control variabel for while loop.
  int c = 1; 

  //Iterator for khash
  khiter_t itr; 

  //A temp location for the flow
  flow*f; 

  // This label will hold new information from our flow
  label_item*l_item = NULL;

  // This label will hold temp info
  label_item*l_item_c = NULL;

  // Identify the number of flows in each round
  int counter = 0;
  
  /* 
     While updating our items we check if
     we already found a flow with the same id.
     If this is true, we set this flag on true to single this information.
  */
  int found_flag = 0; 
  
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
	  // Get our flow from the hashmap.
	  f = kh_value(flow_hashmap,itr);
	  
	  // Allocate some space for the label.
	  l_item = (label_item*) malloc(sizeof(label_item));
	  
	  l_item->flow_id_label = (char*) malloc(sizeof(char)*100);
	  l_item->flow_info_label = (char*) malloc(sizeof(char)*300);
	  
	  // Copy our id information into our label.
	  snprintf(
		   l_item->flow_id_label,
		   100,
		   "%u",(uint32_t)f->flow_id
		   );

	  // Copy the rest of our string into the label.
	  snprintf(
		   l_item->flow_info_label,
		   300,
		   "%*s%d.%d.%d.%d %*s %d.%d.%d.%d|%d|size:%d%*s",
		   ((int)pcontext->col/7)-(7+(int)10),
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
		   (uint32_t)10,
		   (int)f->size,
		   80," "
		   );

	  for(int i = 0; i < pcontext->number_of_menu_elements; i++){
	    // Hold the userpointer.
	    l_item_c = pcontext->label_list[i];
#ifdef _DEBUG
	    if(l_item_c == NULL)
	      syslog(LOG_INFO,"->%p",l_item_c);
#endif
	    // It should NOT happen that l_item_c == NULL.
	    if(l_item_c != NULL && !strcmp(l_item->flow_id_label, l_item_c->flow_id_label)){
	      /*
		If we find a match -> we have the index in our
		label array to update our label.
	       */
#ifdef _DEBUG
	      syslog(LOG_INFO,"%s","match");
#endif
	      
	      // Update the label.
	      pcontext->label_list[i] = l_item;

	      found_flag = 1;
	      break;
	    }
	  }
	  
	  // If not found any existing label -> make a new one.
	  if(!found_flag){
	    l_item->weight = pcontext->number_of_menu_elements+counter;
	    pcontext->label_list = (label_item**) realloc(pcontext->label_list, 
							  sizeof(label_item*)*(pcontext->number_of_menu_elements+counter)+1);
	    pcontext->label_list[pcontext->number_of_menu_elements] = l_item;
	  }

	  //Free our temp variable
	  found_flag = 0;
	  counter++;
	}
      }

      /* The counter will now be 1 to high, so we end  */
      /* 	our list with a (char*) NULL pointer */
      pcontext->number_of_menu_elements = counter-1;
      pcontext->new_data = 1;
      number_of_new_items = 0;
      pthread_mutex_unlock(pcontext->khash_mutex);
      pthread_mutex_unlock(pcontext->list_mutex);
      
    }else{
      //something went wrong using the thread. Abort
      c = !c; //flip 
    }
  }
  #ifdef _DEBUG
  closelog();
  #endif
}

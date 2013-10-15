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

  // Number of new flows each iteration
  int new_flows = 0;

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
	  l_item->src_ip_label = (char*) malloc(sizeof(char)*MAX_IPV4_STRLEN);
	  l_item->dst_ip_label = (char*) malloc(sizeof(char)*MAX_IPV4_STRLEN);
	  
	  // Copy our id information into our label.
	  snprintf(
		   l_item->flow_id_label,
		   100,
		   "%u",(uint32_t)f->flow_id
		   );

	  // Create source ip label string
	  snprintf(l_item->src_ip_label,
		   MAX_IPV4_STRLEN,
		   "%d.%d.%d.%d",
		   (uint32_t)(f->iph->src_adr & 0x000000FF),
		   (uint32_t)(((f->iph->src_adr >> 8 ) & 0x000000FF)),
		   (uint32_t)(((f->iph->src_adr >> 16) & 0x000000FF)),
		   (uint32_t)(((f->iph->src_adr >> 24) & 0x000000FF)));
	  
	  // Create dest ip label string
	  snprintf(l_item->dst_ip_label,
		   MAX_IPV4_STRLEN,
		   "%d.%d.%d.%d",
		   (uint32_t)(f->iph->dst_adr & 0x000000FF),
		   (uint32_t)(((f->iph->dst_adr >> 8 ) & 0x000000FF)),
		   (uint32_t)(((f->iph->dst_adr >> 16) & 0x000000FF)),
		   (uint32_t)(((f->iph->dst_adr >> 24) & 0x000000FF)));
	  
	  // Copy the rest of our string into the label.
	  snprintf(l_item->flow_info_label,
		   300,
		   "%*s%s %*s %s %*s %s %*s %s %*s size:%d%*s",
		   ((int)pcontext->col/7)-(6+(int)10),
		   " ",
		   l_item->src_ip_label,
		   ((int)pcontext->col/7)-((int)strlen(l_item->src_ip_label)),
		   " ",
		   l_item->dst_ip_label,
		   ((int)pcontext->col/7)-(3+(int)strlen(l_item->dst_ip_label)-8),
		   " ",
		   "nill",
		   ((int)pcontext->col/7),
		   " ",
		   "nill",
		   ((int)pcontext->col/7),
		   " ",
		   (int)f->size,
		   (int)80,
		   " ");

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

	      // We found a match, so dont make a new label.
	      found_flag = 1;
	      break;
	    }
	  }
	  
	  // If not found any existing label -> make a new one.
	  if(!found_flag){
#ifdef _DEBUG
	    syslog(LOG_INFO, "create new label. New flows: %d, menu elements: %d",counter,new_flows);
#endif	   
	    l_item->weight = pcontext->number_of_menu_elements+new_flows;
	    pcontext->label_list = (label_item**) realloc(pcontext->label_list, 
							  sizeof(label_item*)*(pcontext->number_of_menu_elements+new_flows+1));
	    pcontext->label_list[pcontext->number_of_menu_elements+new_flows] = l_item;
	    new_flows++;
	  }

	  //Free our temp variable
	  found_flag = 0;
	  counter++;
	}
      }

      /* The counter will now be 1 to high, so we end  */
      /* 	our list with a (char*) NULL pointer */
#ifdef _DEBUG
      syslog(LOG_INFO, "set number of elements: %d", counter-1);
#endif
      pcontext->number_of_menu_elements = counter-1;
      // There is new data
      pcontext->new_data = 1;
      
      // Reset new flow counter.
      new_flows = 0;

      // Release mutex.
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
  return 0;
}

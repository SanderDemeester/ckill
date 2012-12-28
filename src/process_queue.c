#ifndef CKILL_HEADER
#include "header/ckill.h"
#endif

void *process_queue(void*ptr){
  pthread_context*pcontext = (pthread_context*)ptr;
  queue*q = pcontext->q;
  queue_element*element = NULL;
  ip_header*iph = NULL;
  tcp_header*tcph = NULL;
  int size = 0;
  uint32_t sip = 0;
  uint32_t dip = 0;
  uint16_t sp  = 0;
  uint16_t dp  = 0;
  uint32_t work_flowid = 0;
  khiter_t itr;
  khash_t(32)*flow_hashmap = pcontext->arg->flow_hashmap;
  while(1){
    //neem de mutex en controleer of er werk is voor ons
    pthread_mutex_lock(pcontext->mutex); //take mutex
    if(q->number_of_elements>0){
      //er is werk
      while(q->number_of_elements > 0){
	element = q->list[q->number_of_elements-1];
	iph = element->iph;
	tcph = element->tcph;
	size = element->size;
	
	if(iph->proto == 6){
	  //match tcp payload

	  sip = iph->src_adr;
	  dip = iph->dst_adr;
	  sp  = tcph->src_port;
	  dp  = tcph->dst_port;

	  //compute flowid
	  work_flowid = sip^dip;
	  work_flowid ^= sp;
	  work_flowid ^= dp;
	  
	  //check if element exist in hashmap
	  itr = kh_get(32,flow_hashmap,work_flowid);
	  int b = (itr == kh_end(flow_hashmap));
	  if(b){
	    //new flow detected
	    flow*f = (flow*) malloc(sizeof(flow));
	    int r = 0;
	    f->flow_id = work_flowid;
	    f->iph = iph;
	    f->tcph = tcph;
	    itr = kh_put(32,flow_hashmap,work_flowid,&r);
	    kh_value(flow_hashmap,itr) = f;
	  }else{
	    //update existing flow
	    flow*f = kh_value(flow_hashmap,itr);
	    int r;
	    f->iph = iph;
	    f->tcph = tcph;
	    kh_put(32,flow_hashmap,work_flowid,&r);
	  }	    
	  
	  printf("ip: %d.%d.%d.%d:%d - seq: %d && ack: %d %d\n",
	  	 ((iph->src_adr)       & 0x000000FF),
	  	 ((iph->src_adr >> 8)  & 0x000000FF),
	  	 ((iph->src_adr >> 16) & 0x000000FF),
	  	 ((iph->src_adr >> 24) & 0x000000FF),
	  	 htons(tcph->src_port),
	  	 htons(tcph->seq),
	  	 htons(tcph->ack),
	  	 size
	  	 );
	  
	  printf("ip: %d.%d.%d.%d:%d\n",
	  	 ((iph->dst_adr)       & 0x000000FF),
	  	 ((iph->dst_adr >> 8)  & 0x000000FF),
	  	 ((iph->dst_adr >> 16) & 0x000000FF),
	  	 ((iph->dst_adr >> 24) & 0x000000FF),
	  	 htons(tcph->dst_port)
	  	 );
	}
	free(element); //geef element terug vrij
	q->number_of_elements--; //decrement aantal elementen.
      }
      pthread_mutex_unlock(pcontext->mutex);
      pthread_cond_signal(pcontext->conditie); //unblock thread blocking on pcontext->conditie
      
    }else{
      //er is geen werk
      pthread_cond_wait(pcontext->conditie,
			pcontext->mutex);
      pthread_mutex_unlock(pcontext->mutex);

    }
  }
  
  pthread_cond_signal(pcontext->conditie);
  pthread_mutex_unlock(pcontext->mutex); 
}

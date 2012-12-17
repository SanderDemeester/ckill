#ifndef CKILL_HEADER
#include "header/ckill.h"
#endif

void *process_queue(void*ptr){
  pthread_context*pcontext = (pthread_context*)ptr;
  queue*q = pcontext->q;
  queue_element*element = NULL;
  ip_header*iph = NULL;
  tcp_header*tcph = NULL;
  while(1){
    //neem de mutex en controleer of er werk is voor ons
    pthread_mutex_lock(pcontext->mutex);
    if(q->number_of_elements>0){
      //er is werk
      while(q->number_of_elements > 0){
	element = q->list[q->number_of_elements];
	iph = element->iph;
	tcph = element->tcph;
	
	if(iph->proto == 6){
	  //match tcp payload
	  printf("ip: %d.%d.%d.%d:%d - seq: %d && ack: %d\n",
		 ((iph->src_adr)       & 0x000000FF),
		 ((iph->src_adr >> 8)  & 0x000000FF),
		 ((iph->src_adr >> 16) & 0x000000FF),
		 ((iph->src_adr >> 24) & 0x000000FF),
		 htons(tcph->src_port),
		 htons(tcph->seq),
		 htons(tcph->ack)
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
    }else{
      //er is geen werk
      pthread_cond_signal(pcontext->conditie);
      pthread_mutex_unlock(pcontext->mutex);
    }
    sleep(10);
  }
}

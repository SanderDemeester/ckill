#ifndef CKILL_HEADER
#include "header/ckill.h"
#endif

void *process_incoming_packets(void*ptr){
  queue_element *element_to_add = NULL;
  int listen_socket = socket(AF_INET,SOCK_RAW, IPPROTO_TCP);
  unsigned char buffer[500];
  int number_of_flows = 0;
  
  
  struct sockaddr_in *me = (struct sockaddr_in*) malloc(sizeof(struct sockaddr_in));
  me->sin_family = AF_INET;
  me->sin_addr.s_addr = INADDR_ANY;


  bind(listen_socket,(struct sockaddr*)me,sizeof(*me));
  while(1){
    int size = recv(listen_socket,buffer,500,0);
    if(size == -1)
      exit(-1);
    ip_header *iph = (ip_header*)buffer;
    tcp_header *tcph = (tcp_header*)(buffer + 4 * (iph->version_ihl & 0x0F));

    element_to_add = (queue_element*) malloc(sizeof(queue_element));
    element_to_add->iph = iph;
    element_to_add->tcph = tcph;
    
  
    if(iph->proto == 6){
      printf("ip: %d.%d.%d.%d:%d - payload size: %d, seq: %d && ack: %d\n",
	     ((iph->src_adr)       & 0x000000FF),
	     ((iph->src_adr >> 8)  & 0x000000FF),
	     ((iph->src_adr >> 16) & 0x000000FF),
	     ((iph->src_adr >> 24) & 0x000000FF),
	     htons(tcph->src_port),
	     size,
	     htons(tcph->seq),
	     htons(tcph->ack)
	     );
      
      printf("ip: %d.%d.%d.%d:%d - payload size: %d\n", 
	     ((iph->dst_adr)       & 0x000000FF),
	     ((iph->dst_adr >> 8)  & 0x000000FF),
	     ((iph->dst_adr >> 16) & 0x000000FF),
	     ((iph->dst_adr >> 24) & 0x000000FF),
	     htons(tcph->dst_port),
	     size);
    }
  }
}
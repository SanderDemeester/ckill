#ifndef CKILL_HEADER
#include "header/ckill.h"
#endif

void *process_incoming_packets(void*ptr){

  pthread_context*pcontext = (pthread_context*)ptr;
  queue_element *element_to_add = NULL;
  int listen_socket = socket(AF_INET,SOCK_RAW, IPPROTO_TCP);
  unsigned char buffer[500];
  struct sockaddr_in *me = (struct sockaddr_in*) malloc(sizeof(struct sockaddr_in));
  queue*copy_q = pcontext->q;
  me->sin_family = AF_INET;
  me->sin_addr.s_addr = INADDR_ANY;
  me->sin_port = 0;

  bind(listen_socket,(struct sockaddr*)me,sizeof(*me));

  while(1){
    int size = recv(listen_socket,buffer,500,0);
    pthread_mutex_lock(pcontext->mutex); //take mutex

    if(size == -1)
      exit(-1);

    ip_header *iph = (ip_header*)buffer;
    tcp_header *tcph = (tcp_header*)(buffer + 4 * (iph->version_ihl & 0x0F));


    element_to_add = (queue_element*) malloc(sizeof(queue_element));
    element_to_add->iph = iph;
    element_to_add->tcph = tcph;
    element_to_add->data = NULL; //voorlopig is er geen data.

    if(!pcontext->error){
      //geen fout
      copy_q->number_of_elements++;
      printf("hier: %d \n",copy_q->number_of_elements);
      printf("%d \n", sizeof(element_to_add));
      
      
      copy_q->list[copy_q->number_of_elements-1] = element_to_add;
      if(copy_q->number_of_elements == N){
	printf("hier\n");
	pthread_cond_signal(pcontext->conditie);
	pthread_mutex_unlock(pcontext->mutex);
      }
      
    }else{
      pthread_mutex_unlock(pcontext->mutex);
      return NULL; //fout
    }
    
    pthread_cond_signal(pcontext->conditie);
    pthread_mutex_unlock(pcontext->mutex);
  
  }
}


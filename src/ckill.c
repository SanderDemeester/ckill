#include "header/ckill.h"
#include "header/process_queue.h"
#include "header/process_incoming_packet.h"

int main(void){

  if(getuid()){
    printf("U root?\n");
    exit(-1);
  }

  volatile pthread_context pcontext;
  pcontext.conditie    = (pthread_cond_t*) malloc(sizeof(pthread_cond_t));
  pcontext.mutex       = (pthread_mutex_t*)   malloc(sizeof(pthread_mutex_t));
  pcontext.error       = 0;
  pcontext.connections = (flow*) malloc(sizeof(flow));

  queue*q = (queue*) malloc(sizeof(queue));
  q->number_of_elements = 0;
  q->list = (queue_element**) malloc(sizeof(queue_element*));
  q->list[0] = (queue_element*) malloc(sizeof(queue_element));
  
  pcontext.q = (queue*) malloc(sizeof(queue));
  pcontext.q = q;
  
       
  pthread_t *process_packet_engine  = (pthread_t*) malloc(sizeof(pthread_t));
  pthread_t *process_queue_engine  = (pthread_t*) malloc(sizeof(pthread_t));

  pthread_create(process_packet_engine,NULL,process_incoming_packets,(void*)&pcontext);
  sleep(10000);
  printf("hier1\n");

  pthread_mutex_destroy(pcontext.mutex);
  pthread_cond_destroy(pcontext.conditie);

  pthread_create(process_queue_engine,NULL,process_queue,(void*)&pcontext);

  pthread_join(*process_queue_engine,NULL);
  pthread_join(*process_packet_engine,NULL);

  pthread_mutex_destroy(pcontext.mutex);
  pthread_cond_destroy(pcontext.conditie);

  

  free(process_packet_engine);
  free(process_queue_engine);

  free(pcontext.conditie);
  free(pcontext.mutex);
  
  
  return 0;
}

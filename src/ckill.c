#include "header/ckill.h"
#include "header/process_queue.h"
#include "header/process_incoming_packet.h"
int main(void){

  if(getuid()){
    printf("U root?\n");
    exit(-1);
  }

  volatile pthread_context*pcontext;
  pcontext->conditie = (pthread_cond_t*) malloc(sizeof(pthread_cond_t));
  pcontext->mutex    = (pthread_mutex_t*)   malloc(sizeof(pthread_mutex_t));
  pcontext->error    = 0;

  flow *connections  = (flow*) calloc(1,sizeof(flow));
  
       
  pthread_t *process_packet_engine  = (pthread_t*) malloc(sizeof(pthread_t));
  pthread_t *process_queue_engine  = (pthread_t*) malloc(sizeof(pthread_t));

  pthread_create(process_packet_engine,NULL,process_incoming_packets,(void*)pcontext);
  pthread_create(process_queue_engine,NULL,process_queue,(void*)pcontext);

  pthread_join(*t,NULL);
  pthread_join(*engine,NULL);
  return 0;
}

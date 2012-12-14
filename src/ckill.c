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
  
       
  pthread_t *engine  = (pthread_t*) malloc(sizeof(pthread_t));
  pthread_t *t  = (pthread_t*) malloc(sizeof(pthread_t));
  pthread_create(engine,NULL,process_incoming_packets,NULL);
  pthread_create(t,NULL,process_queue,NULL);
  pthread_join(*t,NULL);
  pthread_join(*engine,NULL);
  return 0;
}

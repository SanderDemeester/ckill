#ifndef CKILL_HEADER
#include "header/ckill.h"
#endif

void *process_queue(void*ptr){
  volatile pthread_context*pcontext = (pthread_context*)ptr;
  while(1){
    sleep(10);
  }
}

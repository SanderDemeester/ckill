#ifndef CKILL_HEADER
#include "header/ckill.h"
#endif

void *process_queue(void*ptr){
  volatile pthread_context*pcontext = (pthread_context*)ptr;
  queue*q = pcontext->q;
  while(1){
    //neem de mutex en controleer of er werk is voor ons
    pthread_mutex_lock(pcontext->mutex);
    if(q->number_of_elements>0){
      //er is werk
    }else{
      //er is geen werk
      pthread_cond_signal(pcontext->conditie);
      pthread_mutex_unlock(pcontext->mutex);
    }
    sleep(10);
  }
}

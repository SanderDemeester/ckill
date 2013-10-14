#include "header/ckill.h"
#include "header/process_queue.h"
#include "header/process_incoming_packet.h"
#include "header/process_ui_queue_events.h"

#ifndef __SCREEN_UI_H
#include "header/ui_screen.h"
#endif


int main(int argc, char*argv[]){
  
  if(getuid()){
    printf("U root?\n");
    exit(-1);
  }
 
#ifdef _DEBUG
  openlog("ckill",0,LOG_USER);
#endif

  volatile pthread_context pcontext;
  pcontext.conditie     = (pthread_cond_t*)   malloc(sizeof(pthread_cond_t));
  pcontext.mutex        = (pthread_mutex_t*)  malloc(sizeof(pthread_mutex_t));
  pcontext.ui_mutex     = (pthread_mutex_t*)  malloc(sizeof(pthread_mutex_t));
  pcontext.khash_mutex  = (pthread_mutex_t*)  malloc(sizeof(pthread_mutex_t));
  pcontext.list_mutex   = (pthread_mutex_t*)  malloc(sizeof(pthread_mutex_t));
  pcontext.flows_mutex  = (pthread_mutex_t*)  malloc(sizeof(pthread_mutex_t));
  pcontext.arg          = (pthread_arg*)      malloc(sizeof(pthread_arg));
  pcontext.error        = 0;
  pcontext.connections  = (flow*) malloc(sizeof(flow));

  pcontext.new_data = 0;
  pcontext.number_of_menu_elements = 0;
  

  queue*q = (queue*) malloc(sizeof(queue));
  q->number_of_elements = 0;
  q->list = (queue_element**) malloc(sizeof(queue_element*)*N);

  for(int i = 0; i < N; i++)
    q->list[i] = (queue_element*) malloc(sizeof(queue_element));
  
  //init of kash
  pcontext.arg->flow_hashmap = kh_init(32);
  pcontext.q = (queue*) malloc(sizeof(queue));
  pcontext.q = q;
  
  pthread_cond_init(pcontext.conditie,NULL);
  pthread_mutex_init(pcontext.mutex,NULL);
  pthread_mutex_init(pcontext.ui_mutex,NULL);
  pthread_mutex_init(pcontext.khash_mutex,NULL);
  pthread_mutex_init(pcontext.list_mutex,NULL);
  pthread_mutex_init(pcontext.flows_mutex,NULL);
  
  pthread_t *process_packet_engine = (pthread_t*) malloc(sizeof(pthread_t));
  pthread_t *process_queue_engine  = (pthread_t*) malloc(sizeof(pthread_t));
  pthread_t *ui_screen_thread      = (pthread_t*) malloc(sizeof(pthread_t));
  pthread_t *process_ui_events     = (pthread_t*) malloc(sizeof(pthread_t));

  if(argc == 2){
    //default is all interfaces
    if(!strcmp(argv[1],"all")){
      pcontext.arg->inf = (char*) malloc(sizeof(char)*3);
      pcontext.arg->inf = "all";
    }else{
      pcontext.arg->inf = argv[1];
    }
  }else if(argc == 1){
    pcontext.arg->inf = (char*) malloc(sizeof(char)*3);
    pcontext.arg->inf = "all";
  }

  pthread_create(ui_screen_thread,NULL,ckill_ui,(void*)&pcontext);
  pthread_create(process_packet_engine,NULL,process_incoming_packets,(void*)&pcontext);
  pthread_create(process_queue_engine,NULL,process_queue,(void*)&pcontext);
  pthread_create(process_ui_events,NULL,process_ui_queue_events,(void*)&pcontext);

  pthread_join(*ui_screen_thread,NULL);
  pthread_join(*process_packet_engine,NULL);
  pthread_join(*process_queue_engine,NULL);
  pthread_join(*process_ui_events,NULL);


  pthread_mutex_destroy(pcontext.mutex);
  pthread_cond_destroy(pcontext.conditie);

  free(process_packet_engine);
  free(process_queue_engine);

  free(pcontext.conditie);
  free(pcontext.mutex);
  
  
  return 0;
}

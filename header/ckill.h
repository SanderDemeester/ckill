#define CKILL_HEADER
#include <ncurses.h>
#include <menu.h>

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <pthread.h>

#ifndef __AC_KHASH_H
#include "khash.h"
#endif

#ifndef __SCREEN_UI_H
#include "ui_screen.h"
#endif
#define N 100
#define NUMBER_OF_MENU_ENTRYS 1023
#define LEN_MENU_STR 300

typedef struct{
  unsigned char version_ihl; //version
  unsigned char tos; //type of service
  uint16_t len; //length 
  uint32_t id_flag_frag; //identification & flags & flag offset
  unsigned char ttl; //time to live
  unsigned char proto; //protoco field
  uint16_t crc; //standaard checksum
  uint32_t src_adr; //src adr
  uint32_t dst_adr; //dest adr
  uint32_t options; //if IHL > 5
}ip_header;

typedef struct{
  uint16_t src_port; //source port
  uint16_t dst_port; //dest port
  uint32_t seq; //seq number
  uint32_t ack; //ack number
  uint16_t hlen_re_flag; //HLEN and reserved & flags
  uint16_t window; //window size
  uint16_t crc; //standaard checksum
  uint16_t urgent_pnt; //Urgent Pointer
  uint32_t opt_pad; //options and paddding
}tcp_header;

typedef struct{
  uint8_t ns:1;  //ENC-nonce
  uint8_t cwr:1; //congestion window reduced
  uint8_t ece:1; //ENC echo indication
  uint8_t urg:1; //urgent flag
  uint8_t ack:1; //ack
  uint8_t psh:1; //push ... the button
  uint8_t rst:1; //reset 
  uint8_t syn:1; //syc
  uint8_t fin:1; //fin
}tcp_options;

typedef struct{
  uint32_t flow_id;
  int size;
  ip_header*iph;
  tcp_header*tcph;
  tcp_options tcpop;
  
  struct sockaddr_in *src;
  struct sockaddr_in *dst;
  
}flow;

KHASH_MAP_INIT_INT(32,flow*);
typedef struct{
  char*inf;
  khash_t(32) *flow_hashmap;
}pthread_arg;

typedef struct{
  ip_header*iph;
  tcp_header*tcph;
  tcp_options*tcpo;
  unsigned char*data;
  int size;
}queue_element;

typedef struct{
  int number_of_elements;
  queue_element **list;
}queue;

typedef struct{
   pthread_cond_t*conditie;

   /* Mutex to control acces between  */
   /* process_queue thread and packet_engine thread */
   pthread_mutex_t*mutex; 

   /* Mutex to control access between */
   /* process_queue thread and ui_screen thread */
   pthread_mutex_t*ui_mutex;

   /* Mutex to control acces between  */
   /* ui-events process and packet_engine processor */     
   pthread_mutex_t*khash_mutex;

   /* A mutex to control acces for char**list and char**ip */
   pthread_mutex_t*list_mutex;

   /* A mutex to control acces for number_of_flows */
   pthread_mutex_t*flows_mutex;
    

   queue*q;
   int error;
   flow*connections;
   pthread_arg*arg;

   /* record number of flows, */
   /* this number is equiv to number of menu entrys */
   int number_of_flows;

   /* a pointer to our windows */
   ncurses_data*ncurses_window;
   
   /* Array to pointers ITEMS */
   ITEM**items;
   
   /* Menu */
   MENU*menu; 
   
   char**list;
   char**ip;
   int row;
   int height;
   int widht;
   int col;
}pthread_context;


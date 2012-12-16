#define CKILL_HEADER
#include <ncurses.h>
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
  uint32_t flow_id;
  struct sockaddr_in *src;
  struct sockaddr_in *dst;
}flow;

struct tcp_options{
  int urg; //urgent flag
  int ack; //ack
  int psh; //push ... the button
  int rst; //reset 
  int syn; //syc
  int fin; //fin
};

typedef struct{
  ip_header*iph;
  tcp_header*tcph;
  unsigned char*data;
}queue_element;

typedef struct{
  int number_of_elements;
  queue_element **list;
}queue;

 typedef struct{
   pthread_cond_t*conditie;
   pthread_mutex_t*mutex;
   queue*q;
   int error;
 }pthread_context;

//funcion definitions
//process incoming packets
void *process_incoming_packets(void*ptr);
void *process_queue(void*ptr);

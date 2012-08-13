#ifndef _CODEC_H
#define _CODEC_H

#include <stdint.h>
#include <pthread.h>

#include "vo_pi.h"

struct packet_t
{
  unsigned char* buf;     /* The buffer to be freed after use */
  unsigned char* packet;  /* Pointer to the actual video data (within buf) */
  int packetlength;       /* Number of bytes in packet */
  int64_t PTS;
  int64_t DTS;
};

struct codec_queue_t
{
  struct packet_t* data;
  struct codec_queue_t* prev;
  struct codec_queue_t* next;
};

struct codec_t
{
  void* codecstate;
  pthread_t thread;
  pthread_mutex_t queue_mutex;
  pthread_cond_t queue_count_cv;
  struct codec_queue_t* queue_head;
  struct codec_queue_t* queue_tail;
  int queue_count;
  double nextframetime;
  RECT_VARS_T vars;    // Used by video codecs
};

void codec_queue_init(struct codec_t* codec);
void codec_queue_add_item(struct codec_t* codec, struct packet_t* packet);
void codec_queue_free_item(struct codec_t* codec,struct codec_queue_t* item);
struct codec_queue_t* codec_queue_get_next_item(struct codec_t* codec);

#endif
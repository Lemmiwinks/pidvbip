#ifndef _CODEC_H
#define _CODEC_H

#include <stdint.h>
#include <pthread.h>

#include "vo_pi.h"
#include "omx_utils.h"
#include "htsp.h"

struct codec_init_args_t
{
  struct codec_t* codec;
  struct omx_pipeline_t* pipe;
  char* audio_dest;
};

struct packet_t
{
  unsigned char* buf;     /* The buffer to be freed after use */
  unsigned char* packet;  /* Pointer to the actual video data (within buf) */
  int packetlength;       /* Number of bytes in packet */
  int frametype;
  int64_t PTS;
  int64_t DTS;
};

#define MSG_PACKET       1
#define MSG_PLAY         2
#define MSG_STOP         3
#define MSG_PAUSE        4
#define MSG_NEW_CHANNEL  5
#define MSG_ZOOM         6
#define MSG_SET_ASPECT_4_3   7
#define MSG_SET_ASPECT_16_9  8
#define MSG_HTSP_STARTED 9
#define MSG_CODECDATA 10
#define MSG_CROP         11

struct codec_queue_t
{
  int msgtype;
  struct packet_t* data;
  struct codec_queue_t* prev;
  struct codec_queue_t* next;
};

struct codec_t
{
  void* codecstate;
  int is_running;
  pthread_t thread;
  pthread_mutex_t queue_mutex;
  pthread_cond_t queue_count_cv;
  pthread_cond_t resume_cv;
  struct codec_queue_t* queue_head;
  struct codec_queue_t* queue_tail;
  int queue_count;
  int64_t PTS;
  pthread_mutex_t PTS_mutex;
  pthread_mutex_t isrunning_mutex;
  struct codec_t* acodec;
  OMX_VIDEO_CODINGTYPE vcodectype;
  int width;
  int height;
  OMX_AUDIO_CODINGTYPE acodectype;
  int first_packet;
};

struct codecs_t {
  pthread_mutex_t playback_mutex;  /* Locked by the thread with access to playback - htsp/avplay/etc */
  
  struct codec_t vcodec;
  struct codec_t acodec; // Audio
  struct codec_t scodec; // Subtitles
  struct htsp_subscription_t subscription;  // Details of the currently tuned channel
  int is_paused;
};

void codec_queue_init(struct codec_t* codec);
void codec_new_channel(struct codec_t* codec);
void codec_stop(struct codec_t* codec);
void codec_send_message(struct codec_t* codec, int m, void* data);
void codec_pause(struct codec_t* codec);
void codec_resume(struct codec_t* codec);
void codec_queue_add_item(struct codec_t* codec, struct packet_t* packet, int msgtype);
void codec_queue_free_item(struct codec_t* codec,struct codec_queue_t* item);
struct codec_queue_t* codec_queue_get_next_item(struct codec_t* codec);
void codec_set_pts(struct codec_t* codec, int64_t PTS);
int64_t codec_get_pts(struct codec_t* codec);
int codec_is_running(struct codec_t* codec);
void codec_flush_queue(struct codec_t* codec);

#endif

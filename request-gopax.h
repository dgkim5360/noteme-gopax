#ifndef _REQUEST_GOPAX
#define _REQUEST_GOPAX

#include <json-c/json.h>
#include <curl/curl.h>

typedef struct ticker_info_st {
  char *response;
  json_object *json;
  size_t size;
  long price;
  long ask;
  long bid;
  double volume;
  char ticker[8];
  char times[30];
  /* struct tm timestamp; */
  time_t t;
} *ticker_info;

void url_construct(const char *);

ticker_info ticker_init();
void ticker_destroy();

void curl_session_init();
void curl_session_destroy();

int get_gopax();

#endif

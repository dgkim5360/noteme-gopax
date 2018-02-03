#include <string.h>
#include <json-c/json.h>
#include <curl/curl.h>

#include "request-gopax.h"

static CURL *curl;
static char url[50];
static struct ticker_info_st ticker_info_struct;
static ticker_info ticker = &ticker_info_struct;

void url_construct(const char *pair) {
  sprintf(url, "https://api.gopax.co.kr/trading-pairs/%s/ticker", pair);
}

ticker_info ticker_init() {
  ticker->response = NULL;
  ticker->json = NULL;
  return ticker;
}

void ticker_destroy() {
  if (ticker->response != NULL)
    free(ticker->response);
}

size_t curl_callback(char *, size_t, size_t, void *);

void curl_session_init() {
  curl = curl_easy_init();
  if (curl == NULL)
    fprintf(stderr, "ERROR at curl_easy_init\n");

  curl_easy_setopt(curl, CURLOPT_URL, url);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *) ticker);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_callback);
  curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
  curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5);
}

void curl_session_destroy() {
  curl_easy_cleanup(curl);
}

int get_gopax() {
  /* CURLcode status_code; */

  /* printf("%s\n", url); */

  curl_easy_perform(curl);
  /* status_code = curl_easy_perform(curl); */

  /* printf("RESPONSE %d: %s\n", status_code, ticker->response); */
  return 0;
}

size_t
curl_callback(char *contents, size_t size, size_t nmemb, void *userp) {
  size_t realsize = size * nmemb;
  ticker_info p = (ticker_info) userp;
  json_object *field;
  enum json_tokener_error jerr = json_tokener_success;

  /* printf("CALLBACK CONTENT: %s\n", contents); */
  p->response = (char *) realloc(p->response, realsize);

  if (p->response == NULL) {
    fprintf(stderr, "ERROR at realloc in curl_callback\n");
    return 1;
  }

  strncpy(p->response, contents, realsize);

  p->json = json_tokener_parse_verbose(p->response, &jerr);
  /* printf("JSON: %s\n", json_object_to_json_string(p->json)); */

  field = json_object_object_get(p->json, "price");
  p->price = json_object_get_int64(field);

  field = json_object_object_get(p->json, "ask");
  p->ask = json_object_get_int64(field);

  field = json_object_object_get(p->json, "bid");
  p->bid = json_object_get_int64(field);

  field = json_object_object_get(p->json, "volume");
  p->volume = json_object_get_double(field);

  field = json_object_object_get(p->json, "time");
  strncpy(p->times, json_object_get_string(field), 30);
  /* strptime(json_object_get_string(field), "%FT%T%z", &(p->timestamp)); */
  /* p->t = mktime(&(p->timestamp)); */
  /* localtime_r(&(p->t), &(p->timestamp)); */
  /* strftime(p->times, 30, "%F %T", &(p->timestamp)); */

  /* printf("PRICE: %ld\nASK: %ld\nBID: %ld\nVOLUME %lf\nTIME %s\n", */
  /*        p->price, p->ask, p->bid, p->volume, p->times); */

  json_object_put(p->json);
  return realsize;
}

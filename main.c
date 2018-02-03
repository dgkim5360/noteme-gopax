#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>

#include "request-gopax.h"
#include "noteme.h"

#define HELP "\nUsage: noteme-gopax [-hstulv] [trading-pair]\n"\
  "A Gopax watcher with X desktop notifications\n\n"\
  "-h           print this help message\n"\
  "-s int=1800  time to repeat in seconds (30 minutes by default)\n"\
  "-t int=3     time interval to check in seconds\n"\
  "-u int       upper threshold for critical notifications\n"\
  "-l int       lower threshold for critical notifications\n"\
  "-verbose     show all normal notifications\n"
#define HELP_PAIR "\x1b[91mAt least the trading-pair should be specified.\x1b[0m\n"
#define HELP_XORG "\x1b[91mX server is not running.\x1b[0m\n"
#define HELP_TIME "\x1b[91mThe seconds to wait should be a positive integer.\x1b[0m\n"
#define HELP_UPPER "\x1b[91mThe upper threshold should be a positive integer.\x1b[0m\n"
#define HELP_LOWER "\x1b[91mThe lower threshold should be a positive integer.\x1b[0m\n"

bool is_arg_num(const char *optarg);

int main(const int argc, char *argv[]) {
  char c, *pair, title[50], message[100];
  int sleep_sec = 3, repeat = 1800;
  long upper = 0, lower = 0;
  bool verbose = false;
  ticker_info ticker = NULL;

  while ((c = getopt(argc, argv, "hs:t:u:l:v")) != -1) {
    switch (c) {
      case 'h':
        fprintf(stdout, HELP);
        return 0;
      case 's':
        if (!is_arg_num(optarg)) {
          fprintf(stderr, HELP_TIME);
          return 1;
        }
        repeat = atoi(optarg);
        break;
      case 't':
        if (!is_arg_num(optarg)) {
          fprintf(stderr, HELP_TIME);
          return 1;
        }
        sleep_sec = atoi(optarg);
        if (sleep_sec <= 0) {
          fprintf(stderr, HELP_TIME);
          return 1;
        }
        break;
      case 'v':
        verbose = true;
        break;
      case 'u':
        if (!is_arg_num(optarg)) {
          fprintf(stderr, HELP_UPPER);
          return 1;
        }
        upper = atol(optarg);
        break;
      case 'l':
        if (!is_arg_num(optarg)) {
          fprintf(stderr, HELP_LOWER);
          return 1;
        }
        lower = atol(optarg);
        break;
      case '?':
      default:
        fprintf(stdout, HELP);
        return 0;
    }
  }
  if (optind >= argc) {
    fprintf(stderr, HELP_PAIR);
    fprintf(stdout, HELP);
    return 1;
  }
  pair = argv[optind];
  repeat /= sleep_sec;

  url_construct(pair);
  curl_session_init();
  ticker = ticker_init();

  while (repeat--) {
    sleep(sleep_sec);
    get_gopax();

    snprintf(title, 50, "%s %s", pair, ticker->times);
    snprintf(message, 100, "Price: %ld\nAsk: %ld\nBid: %ld\nVolume: %lf\n",
             ticker->price, ticker->ask, ticker->bid, ticker->volume);
    if (upper && ticker->price >= upper) {
      noteme_xorg(title, message, 0, 2, 1);
      continue;
    }
    if (lower && ticker->price <= lower) {
      noteme_xorg(title, message, 0, 2, 1);
      continue;
    }
    if (verbose)
      noteme_xorg(title, message, 0, 1, 1);
  }

  ticker_destroy();
  curl_session_destroy();

  return 0;
}

bool is_arg_num(const char *optarg) {
  int i;

  for (i = 0; *(optarg+i) != '\0'; ++i)
    if (!isdigit(*(optarg+i)))
      return false;
  return true;
}

PREFIX=/usr/local

CC=gcc
CFLAGS=-Wall -pedantic-errors -O2 `pkg-config --cflags libcurl json-c x11 libnotify`
LDFLAGS=`pkg-config --libs libcurl json-c x11 libnotify`

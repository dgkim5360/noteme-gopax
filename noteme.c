#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <libnotify/notify.h>

#include "noteme.h"

int
noteme_xorg(const char *title, const char *message, const int sec,
            const int urgency, int repeat) {
  Display *display = XOpenDisplay(NULL);
  NotifyNotification *noteme;

  if (!display)
    return 1;

  notify_init("noteme \u2709");

  while (repeat--) {
    sleep(sec);
    noteme = notify_notification_new(title, message, NULL);
    notify_notification_set_urgency(noteme, urgency);
    notify_notification_show(noteme, NULL);
    g_object_unref(G_OBJECT(noteme));
  }
  if (notify_is_initted())
    notify_uninit();
  XCloseDisplay(display);
  return 0;
}

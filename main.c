/*
    keylock-tray
    Copyright (C) 2016 Justin Jacobs

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <X11/XKBlib.h>
#include <glib.h>
#include <gtk/gtk.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

#ifndef PIXMAPDIR
#define PIXMAPDIR "/usr/share/pixmaps"
#endif

char *BG_COLOR = "#333333";
char *FG_COLOR = "#ffffff";

typedef struct KeyLockState {
    bool caps_on;
    bool num_on;

    GtkStatusIcon *caps_icon;
    GtkStatusIcon *num_icon;
    Display *x_display;
}KeyLockState;

gboolean check_keylock_state(KeyLockState* kls) {
    if (kls->x_display) {
        unsigned n;
        XkbGetIndicatorState(kls->x_display, XkbUseCoreKbd, &n);

        kls->caps_on = (n & 0x01) == 1;
        kls->num_on = (n & 0x02) == 2;
    }

    if (kls->caps_on) {
        gtk_status_icon_set_tooltip_markup(kls->caps_icon, "Caps Lock\n<b>ON</b>");
        gtk_status_icon_set_visible(kls->caps_icon, TRUE);

    }
    else {
        gtk_status_icon_set_visible(kls->caps_icon, FALSE);
    }

    if (kls->num_on) {
        gtk_status_icon_set_tooltip_markup(kls->num_icon, "Num Lock\n<b>ON</b>");
        gtk_status_icon_set_visible(kls->num_icon, TRUE);
    }
    else {
        gtk_status_icon_set_visible(kls->num_icon, FALSE);
    }

    return TRUE;
}

GdkPixbuf *load_icon(char *filename, char *color1, char *color2) {
    FILE *test = fopen(filename, "r");
    char *buf = 0;
    if (test) {
        size_t len;
        size_t bytes_read = getdelim(&buf, &len, '\0', test);
        if (bytes_read == -1) {
            free(buf);
            return NULL;
        }

        fclose(test);
    }
    else {
        return NULL;
    }

    gchar *str = g_strconcat(
        "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
        "<svg version=\"1.1\"\n"
        "     xmlns=\"http://www.w3.org/2000/svg\"\n"
        "     width=\"64\"\n"
        "     height=\"64\">\n"
        "  <style type=\"text/css\">\n"
        "    .bg {\n"
        "      fill: ", color1, " !important;\n"
        "      stroke: ", color2, " !important;\n"
        "    }\n"
        "    .fg {\n"
        "      fill: ", color2, " !important;\n"
        "    }\n"
        "  </style>\n" , buf, "\n"
        "</svg>",
        NULL);

    free(buf);

    GInputStream *stream = g_memory_input_stream_new_from_data(str, -1, g_free);

    GdkPixbuf *ret = gdk_pixbuf_new_from_stream(stream, NULL, NULL);

    g_object_unref(stream);

    return ret;
}

int main(int argc, char* argv[]) {
    char c;
    while ((c = getopt(argc, argv, "b:f:")) != -1) {
        switch (c) {
            case 'b':
                BG_COLOR = optarg;
                break;
            case 'f':
                FG_COLOR = optarg;
                break;
        }
    }

    KeyLockState kls;
    kls.caps_on = false;
    kls.num_on = false;
    kls.x_display = XOpenDisplay(0);
    if (!kls.x_display) {
        fprintf(stderr, "keylock-tray: Unable to get X display. Aborting.\n");
        return 1;
    }

    gtk_init(&argc, &argv);

    GdkPixbuf *pixbuf_caps = load_icon(PIXMAPDIR"/keylock-tray-caps.svg", BG_COLOR, FG_COLOR);
    if (!pixbuf_caps) {
        pixbuf_caps = load_icon("./keylock-tray-caps.svg", BG_COLOR, FG_COLOR);
    }
    kls.caps_icon = gtk_status_icon_new_from_pixbuf(pixbuf_caps);
    g_object_unref(pixbuf_caps);

    GdkPixbuf *pixbuf_num = load_icon(PIXMAPDIR"/keylock-tray-num.svg", BG_COLOR, FG_COLOR);
    if (!pixbuf_num) {
        pixbuf_num = load_icon("./keylock-tray-num.svg", BG_COLOR, FG_COLOR);
    }
    kls.num_icon = gtk_status_icon_new_from_pixbuf(pixbuf_num);
    g_object_unref(pixbuf_num);

    gtk_status_icon_set_title(kls.caps_icon, "KeyLockTray: Caps Lock");
    gtk_status_icon_set_title(kls.caps_icon, "KeyLockTray: Num Lock");

    check_keylock_state(&kls);
    g_timeout_add(33, (GSourceFunc) check_keylock_state, &kls);

    gtk_main();

    gtk_main_quit();

    return 0;
}

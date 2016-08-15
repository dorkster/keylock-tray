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

#ifndef PIXMAPDIR
#define PIXMAPDIR "/usr/share/pixmaps"
#endif

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

int main(int argc, char* argv[]) {
    KeyLockState kls;
    kls.caps_on = false;
    kls.num_on = false;
    kls.x_display = XOpenDisplay(0);
    if (!kls.x_display) {
        fprintf(stderr, "keylock-tray: Unable to get X display. Aborting.\n");
        return 1;
    }

    gtk_init(&argc, &argv);

    kls.caps_icon = gtk_status_icon_new_from_file(PIXMAPDIR"/keylock-tray-caps.svg");
    if (!gtk_status_icon_get_pixbuf(kls.caps_icon)) {
        g_object_unref(kls.caps_icon);
        kls.caps_icon = gtk_status_icon_new_from_file("./keylock-tray-caps.svg");
    }
    gtk_status_icon_set_title(kls.caps_icon, "KeyLockTray: Caps Lock");

    kls.num_icon = gtk_status_icon_new_from_file(PIXMAPDIR"/keylock-tray-num.svg");
    if (!gtk_status_icon_get_pixbuf(kls.num_icon)) {
        g_object_unref(kls.num_icon);
        kls.num_icon = gtk_status_icon_new_from_file("./keylock-tray-num.svg");
    }
    gtk_status_icon_set_title(kls.caps_icon, "KeyLockTray: Num Lock");

    check_keylock_state(&kls);
    g_timeout_add_seconds(1, (GSourceFunc) check_keylock_state, &kls);

    gtk_main();

    gtk_main_quit();

    return 0;
}

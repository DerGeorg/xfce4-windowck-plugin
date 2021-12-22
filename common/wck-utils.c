/*  $Id$
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 *  Copyright (C) 2013 Cedric Leporcq  <cedl38@gmail.com>
 *
 *  This code is derived from original 'Window Applets' from Andrej Belcijan.
 */

#include "wck-utils.h"
//#include <gtk-3.0/gdk/gdk.h>
//#include <gtk-3.0/gdk/gdkmonitor.h>
//#include <gtk-3.0/gdk/gdkwayland.h>
//#include <gdk/gdk.h>
//#include <gtk/gtk.h>

/* Prototypes */
static WnckWindow *get_root_window(WnckScreen *screen);
static WnckWindow *get_upper_maximized(WckUtils *);
static void track_controled_window (WckUtils *);
static void active_workspace_changed(WnckScreen *, WnckWorkspace *, WckUtils *);
static void active_window_changed(WnckScreen *, WnckWindow *, WckUtils *);
static void track_changed_max_state(WnckWindow *, WnckWindowState, WnckWindowState, WckUtils *);
static void on_umaxed_window_state_changed(WnckWindow *, WnckWindowState, WnckWindowState, WckUtils *);
static void on_viewports_changed(WnckScreen *, WckUtils *);
static void on_window_closed(WnckScreen *, WnckWindow *, WckUtils *);
static void on_window_opened(WnckScreen *, WnckWindow *, WckUtils *);




gboolean wck_signal_handler_disconnect (GObject *object, gulong handler)
{
    if (object && handler > 0)
    {
        if (g_signal_handler_is_connected(object, handler))
        {
            g_signal_handler_disconnect(object, handler);
            return TRUE;
        }
    }
    return FALSE;
}


static WnckWindow *get_root_window (WnckScreen *screen)
{
    GList *winstack = wnck_screen_get_windows_stacked(screen);
    // we can't access data directly because sometimes we will get NULL or not desktop window
    if (winstack && wnck_window_get_window_type (winstack->data) == WNCK_WINDOW_DESKTOP)
        return winstack->data;
    else
        return NULL;
}


/* Trigger when activewindow's workspaces changes */
static void umax_window_workspace_changed (WnckWindow *window,                                                WckUtils *win)
{
        track_controled_window (win);
}


/* Trigger when a specific window's state changes */
static void track_changed_max_state (WnckWindow *window,
                                         WnckWindowState changed_mask,
                                         WnckWindowState new_state,
                                         WckUtils *win)
{
    /* track the window max state only if it isn't the control window */
    if (window != win->controlwindow)
    {
        if (window
            && !wnck_window_is_minimized(window)
            && wnck_window_is_maximized(window))
        {
            track_controled_window (win);
        }
    }
}


/* Triggers when umaxedwindow's state changes */
static void on_umaxed_window_state_changed (WnckWindow *window,
                                          WnckWindowState changed_mask,
                                          WnckWindowState new_state,
                                          WckUtils *win)
{
    /* WARNING : only if window is unmaximized to prevent growing loop !!!*/
    if (!wnck_window_is_maximized(window)
        || wnck_window_is_minimized(window)
        || changed_mask & (WNCK_WINDOW_STATE_ABOVE))
    {
        track_controled_window (win);
    }
    else {
        on_wck_state_changed(win->controlwindow, win->data);
    }
}


/* Returns the highest maximized window */
static WnckWindow *get_upper_maximized (WckUtils *win)
{
    WnckWindow      *umaxedwindow = NULL;

    GList *windows = wnck_screen_get_windows_stacked(win->activescreen);

    while (windows && windows->data)
    {
        if (!win->activeworkspace
            || wnck_window_is_in_viewport(windows->data, win->activeworkspace))
            if (wnck_window_is_maximized(windows->data)
                && !wnck_window_is_minimized(windows->data))
            {
                if(get_active_window_number() == 0)
                    umaxedwindow = windows->data;
            }
        windows = windows->next;
    }
    /* NULL if no maximized window found */
    return umaxedwindow;
}

//gint get_active_window_number_from_window(int plugin_monitor){
//    GdkDisplay *display;
//    GdkScreen *screen;
//    GdkWindow *window;
//    display = gdk_display_get_default();
//    screen = gdk_display_get_default_screen(display);
//    window = gdk_screen_get_active_window(screen);
//
//    GList *allwindows = gdk_screen_get_window_stack(screen);
//
//    while (allwindows && allwindows->data) {
//
//       gint monitor_screen_number = gdk_screen_get_n_monitors(screen);
//        gint activeWindowAtMonitor = gdk_screen_get_monitor_at_window(screen, allwindows->data);
//
//        allwindows = allwindows->next;
//
//
//        char command[100], msg[100];
//
//        strcpy(command,"/usr/bin/notify-send MessageSubject");
//        strcpy(msg," \"number ");
//        char str[12];
//        sprintf(str, "%d", activeWindowAtMonitor);
//        char str2[12];
//        sprintf(str2, "%d", plugin_monitor);
//        strcat(msg, str);
//        strcat(msg, str2);
//        strcat(msg," \" ");
//        strcat(command,msg);
//
//        system(command);
//
//        return activeWindowAtMonitor;
//    }
//
//}


gint get_active_window_number(){
    GdkDisplay *display;
    GdkScreen *screen;
    GdkWindow *window;
    display = gdk_display_get_default();
    screen = gdk_display_get_default_screen(display);
    window = gdk_screen_get_active_window(screen);

    gint monitor_screen_number = gdk_screen_get_n_monitors(screen);

    gint activeWindowAtMonitor = gdk_screen_get_monitor_at_window(screen, window);


//            char command[100], msg[100];
//
//        strcpy(command,"/usr/bin/notify-send MessageSubject");
//        strcpy(msg," \"number ");
//        char str[12];
//        sprintf(str, "%d", activeWindowAtMonitor);
//        char str2[12];
//        sprintf(str2, "%d", monitor_screen_number);
//        strcat(msg, str);
//        strcat(msg, str2);
//        strcat(msg," \" ");
//        strcat(command,msg);
//
//        system(command);


    return activeWindowAtMonitor;
}

gint get_monitor_from_xid(int plugin_monitor, int xid){
    GdkDisplay *display;
    GdkScreen *screen;
    GdkWindow *window;
    display = gdk_display_get_default();
    screen = gdk_display_get_default_screen(display);
    window = gdk_screen_get_active_window(screen);

    gint monitor_screen_number = gdk_screen_get_n_monitors(screen);

    gint activeWindowAtMonitor = gdk_screen_get_monitor_at_window(screen, window);


//            char command[100], msg[100];
//
//        strcpy(command,"/usr/bin/notify-send MessageSubject");
//        strcpy(msg," \"number ");
//        char str[12];
//        sprintf(str, "%d", activeWindowAtMonitor);
//        char str2[12];
//        sprintf(str2, "%d", monitor_screen_number);
//        strcat(msg, str);
//        strcat(msg, str2);
//        strcat(msg," \" ");
//        strcat(command,msg);
//
//        system(command);


    return activeWindowAtMonitor;
}


/* track the new controled window according to preferences */
static void track_controled_window (WckUtils *win)
{
    WnckWindow      *previous_umax = NULL;
    WnckWindow      *previous_control = NULL;

    previous_control = win->controlwindow;

    if (win->only_maximized)
    {
        previous_umax = win->umaxwindow;
        win->umaxwindow = get_upper_maximized(win);
        win->controlwindow = win->umaxwindow;
    }
    else if (win->activewindow
            && (!win->activeworkspace
                || wnck_window_is_in_viewport(win->activewindow, win->activeworkspace))
            && !wnck_window_is_minimized(win->activewindow)
            && !wnck_window_is_sticky(win->activewindow))
    {
            win->controlwindow = win->activewindow;
    }

    if (!win->umaxwindow || (win->umaxwindow != previous_umax))
    {
        wck_signal_handler_disconnect (G_OBJECT(previous_umax), win->msh);
        wck_signal_handler_disconnect (G_OBJECT(previous_umax), win->mwh);
    }

    if (win->only_maximized)
    {
        if (win->umaxwindow && (win->umaxwindow != previous_umax))
        {
            /* track the new upper maximized window state */
            win->msh = g_signal_connect(G_OBJECT(win->umaxwindow),
                                           "state-changed",
                                           G_CALLBACK (on_umaxed_window_state_changed),
                                           win);
            win->mwh = g_signal_connect(G_OBJECT (win->umaxwindow),
                                        "workspace-changed",
                                        G_CALLBACK (umax_window_workspace_changed),
                                        win);
        }
        else if (win->controlwindow == previous_control)
        {
            /* track previous upper maximized window state on desktop */
            win->umaxwindow = previous_umax;
            if (win->umaxwindow) {
                win->msh = g_signal_connect(G_OBJECT(win->umaxwindow),
                                               "state-changed",
                                               G_CALLBACK (track_changed_max_state),
                                               win);
            }
        }
    }
    if (!win->controlwindow)
        win->controlwindow = get_root_window(win->activescreen);

    if (win->controlwindow != previous_control)
        on_control_window_changed(win->controlwindow, previous_control, win->data);
    else
        on_wck_state_changed(win->controlwindow, win->data);


    //Mein neues feature

    int monitor_number, monitor_screen_number, x, y;
    GdkDisplay *display;
    GdkScreen *screen;
    GdkWindow *window;
//    GdkMonitor *monitorfscreen;
//    GdkMonitor *monitorfdisplay;
    display = gdk_display_get_default();
    screen = gdk_display_get_default_screen(display);
    window = gdk_screen_get_active_window(screen);

//    monitor_number = gdk_display_get_n_monitors(display);
    monitor_screen_number = gdk_screen_get_n_monitors(screen);

//    GdkRectangle *rect;
    gint activeWindowsAtMonitor = gdk_screen_get_monitor_at_window(screen, window);


    GList *window_stack = gdk_screen_get_window_stack(screen);
//    g_list_foreach(window_stack, (GFunc)g_free, NULL);
//    gdk_screen_get_monitor_geometry(screen, activeWindowsAtMonitor, rect);
//
//    x = gdk_window_get_width(window);
//    y = gdk_window_get_height(window);

//    if x < 0: x = 0
//    if y < 0: y = 0

//    monitorfscreen = ;
//    monitorfdisplay =  gdk_display_get_monitor(display, monitor_number);







//    GList *window_l;
//
//
//    for (window_l = wnck_screen_get_windows (win->activescreen); window_l != NULL; window_l = window_l->next)
//    {
//        WnckWindow *window = WNCK_WINDOW (window_l->data);
//        char command[100], msg[100];
//
//        strcpy(command,"/usr/bin/notify-send MessageSubject");
//        strcpy(msg," \"active ");
//        strcat(msg, wnck_window_get_name (window));
//        strcat(msg, window == win->activewindow ? " (active)" : "");
//        strcat(command,msg);
//
//        system(command);
//        g_print ("%s%s\n", wnck_window_get_name (window),
//                 window == win->activewindow ? " (active)" : "");
//    }

//    system("/usr/bin/notify-send MessageSubject \"active window changed\"");

    //activewindow != upper max window --> hide buttons
//    WnckScreen *screencontrol;
//    WnckScreen *screenold;
//    int screencontrol;
    int screenold = 0;
//    wnck_screen_force_update(win->activescreen);
//    screencontrol = wnck_screen_get_number(win->activescreen);
//    screencontrol = wnck_screen_get_number(wnck_window_get_screen(win->activewindow));
//    screencontrol = wnck_window_get_screen(win->activewindow);
//    screencontrol = wnck_screen_get_default();
//    screenold = 0; //plugin screen
//
//    char command[100], msg[100];
//
//        strcpy(command,"/usr/bin/notify-send MessageSubject");
//        strcpy(msg," \"number ");
//        char str[12];
//        sprintf(str, "%d", activeWindowsAtMonitor);
//        char str2[12];
//        sprintf(str2, "%d", screenold);
//        strcat(msg, str);
//        strcat(msg, str2);
//        strcat(msg," \" ");
//        strcat(command,msg);
//
//        system(command);

//    if (activeWindowsAtMonitor != screenold){
//        system("/usr/bin/notify-send MessageSubject \"not same screen\"");
//    }else{
//        system("/usr/bin/notify-send MessageSubject \"same screen\"");
//    }

    if(activeWindowsAtMonitor == screenold) {
//        if (win->activewindow != win->umaxwindow && win->only_maximized) {
//            on_control_window_changed(NULL, NULL, win->data); //Hide Buttons
//        } else {
//        system("/usr/bin/notify-send MessageSubject \"same screen\"");
//        previous_on_screen = win->controlwindow;
//        previous_on_screen_data = win->data;
//        previous_on_screen_prev = previous_control;
            on_control_window_changed(win->controlwindow, previous_control, win->data);
//        }
    }
//    else{
//        if (win->activewindow != win->umaxwindow && win->only_maximized) {
//            system("/usr/bin/notify-send MessageSubject \"not ss active not max\"");
//            on_control_window_changed(NULL, NULL, win->data); //Hide Buttons
//        } else {
//            system("/usr/bin/notify-send MessageSubject \"not ss active max\"");
//            on_control_window_changed(previous_on_screen, previous_on_screen, win->data); //Hide Buttons //TODO hier sollen die buttons disabled sein
//            on_control_window_changed(previous_control, previous_control, win->data);
//        }
//        on_control_window_changed(NULL, NULL, win->data); //Hide Buttons
//        on_control_window_changed(win->controlwindow, previous_control, win->data);
//    }
}


/* Triggers when a new window has been opened */
static void on_window_opened (WnckScreen *screen,
                           WnckWindow *window,
                           WckUtils *win)
{
    // track new maximized window
    if (wnck_window_is_maximized(window))
        track_controled_window (win);
}


/* Triggers when a window has been closed */
static void on_window_closed (WnckScreen *screen,
                           WnckWindow *window,
                           WckUtils *win)
{
    // track closed maximized window
    if (wnck_window_is_maximized(window))
        track_controled_window (win);
}


/* Triggers when a new active window is selected */
static void active_window_changed (WnckScreen *screen,
                                   WnckWindow *previous,
                                   WckUtils *win)
{

    gint activewindowmonitornumber = get_active_window_number();
//    system("/usr/bin/notify-send MessageSubject \"active window changed\"");
    win->activewindow = wnck_screen_get_active_window(screen);

    if (win->activewindow != previous && activewindowmonitornumber == 0)
    {
        wck_signal_handler_disconnect (G_OBJECT(previous), win->ash);

        track_controled_window (win);
    }

    if (win->activewindow
        && (win->activewindow != previous)
        && (wnck_window_get_window_type (win->activewindow) != WNCK_WINDOW_DESKTOP) && activewindowmonitornumber == 0)
    {
        /* Start tracking the new active window */
        win->ash = g_signal_connect(G_OBJECT (win->activewindow), "state-changed", G_CALLBACK (track_changed_max_state), win);
    }
}


/* Triggers when user changes viewports on Compiz */
// We ONLY need this for Compiz (Marco doesn't use viewports)
static void on_viewports_changed (WnckScreen *screen, WckUtils *win)
{
    system("/usr/bin/notify-send MessageSubject \"viewport\"");
    reload_wnck (win, win->only_maximized, win->data);
}


/* Triggers when user changes workspace on Marco (?) */
static void active_workspace_changed (WnckScreen *screen,
                                      WnckWorkspace *previous,
                                      WckUtils *win)
{
    system("/usr/bin/notify-send MessageSubject \"workspace changed\"");
    reload_wnck (win, win->only_maximized, win->data);
}


void toggle_maximize (WnckWindow *window)
{
    char command[100], msg[100];

    strcpy(command,"/usr/bin/notify-send MessageSubject");
    strcpy(msg,"\"Toogle max ");
    strcat(msg, "etetests \"");
    strcat(command,msg);

    system(command);
    if (window && wnck_window_is_maximized(window))
        wnck_window_unmaximize(window);
    else
        wnck_window_maximize(window);
}


void reload_wnck (WckUtils *win, gboolean only_maximized, gpointer data)
{
    /* disconnect all signal handlers */
    wck_signal_handler_disconnect (G_OBJECT(win->controlwindow), win->ash);
    wck_signal_handler_disconnect (G_OBJECT(win->controlwindow), win->msh);
    wck_signal_handler_disconnect (G_OBJECT(win->controlwindow), win->mwh);

    wck_signal_handler_disconnect (G_OBJECT(win->activescreen), win->sch);
    wck_signal_handler_disconnect (G_OBJECT(win->activescreen), win->soh);
    wck_signal_handler_disconnect (G_OBJECT(win->activescreen), win->svh);
    wck_signal_handler_disconnect (G_OBJECT(win->activescreen), win->swh);

    init_wnck (win, only_maximized, data);
}


void init_wnck (WckUtils *win, gboolean only_maximized, gpointer data)
{
    /* save data */
    win->data = data;

    /* get window proprieties */
    win->activescreen = wnck_screen_get_default();
    win->activeworkspace = wnck_screen_get_active_workspace(win->activescreen);
    if (!win->activeworkspace)
        win->activeworkspace = wnck_screen_get_workspace(win->activescreen, 0);
    win->activewindow = wnck_screen_get_active_window(win->activescreen);
    win->umaxwindow = NULL;
    win->controlwindow = NULL;
    win->only_maximized = only_maximized;

    /* Global window tracking */
    g_signal_connect(win->activescreen, "active-window-changed", G_CALLBACK (active_window_changed), win);

    if (win->only_maximized)
    {
        win->sch = g_signal_connect(win->activescreen, "window-closed", G_CALLBACK (on_window_closed), win);
        win->soh = g_signal_connect(win->activescreen, "window-opened", G_CALLBACK (on_window_opened), win);
    }

        win->svh = g_signal_connect(win->activescreen, "viewports-changed", G_CALLBACK (on_viewports_changed), win);
        win->swh = g_signal_connect(win->activescreen, "active-workspace-changed", G_CALLBACK (active_workspace_changed), win);

    /* Get controled window */
    track_controled_window (win);
    
    if (!win->controlwindow)
        on_control_window_changed (NULL, NULL, win->data);
}

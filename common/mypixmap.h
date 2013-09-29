/*      $Id$

        This program is free software; you can redistribute it and/or modify
        it under the terms of the GNU General Public License as published by
        the Free Software Foundation; either version 2, or (at your option)
        any later version.

        This program is distributed in the hope that it will be useful,
        but WITHOUT ANY WARRANTY; without even the implied warranty of
        MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
        GNU General Public License for more details.

        You should have received a copy of the GNU General Public License
        along with this program; if not, write to the Free Software
        Foundation, Inc., Inc., 51 Franklin Street, Fifth Floor, Boston,
        MA 02110-1301, USA.


        oroborus - (c) 2001 Ken Lynch
        xfwm4    - (c) 2002-2011 Olivier Fourdan

 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <glib.h>

#ifdef HAVE_RENDER
#include <X11/extensions/Xrender.h>
#endif

#ifndef INC_MYPIXMAP_H
#define INC_MYPIXMAP_H

#define MYPIXMAP_XPIXMAP(p) (p.pixmap)

typedef struct
{
    gchar *name;
    const gchar *value;
}
xfwmColorSymbol;

typedef struct
{
    gchar *color_string;
    guint16 red;
    guint16 green;
    guint16 blue;
    gint transparent;
}
XPMColor;

GdkPixbuf *             xfwmPixbufLoad (const gchar *, const gchar *, xfwmColorSymbol *);

#endif /* INC_MYPIXMAP_H */
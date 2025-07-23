/*
 * xfce4-axisclock-plugin - A macOS-style clock plugin for the Xfce panel
 *
 * Copyright (c) 2025 Kamil 'Novik' Nowicki <novik@axisos.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Website: https://axisos.org
 * Repository: https://github.com/AxisOS/xfce4-axisclock-plugin
 */

#ifndef __CALENDAR_POPUP_H__
#define __CALENDAR_POPUP_H__

#include <gtk/gtk.h>
#include <time.h>

G_BEGIN_DECLS

/* Calendar popup structure */
typedef struct _AxisClockCalendar {
    GtkWidget *window;          /* Main popup window */
    GtkWidget *grid;            /* Calendar grid */
    
    /* Calendar state */
    gint current_year;
    gint current_month;         /* 0-11 */
    gint today_day;
    gint today_month;
    gint today_year;
    
    /* Day buttons array - 6 weeks x 7 days */
    GtkWidget *day_buttons[42];
    
    /* Parent widget for positioning */
    GtkWidget *parent_widget;
    
    /* Transparency level (0.0 - 1.0) */
    gdouble transparency;
} AxisClockCalendar;

/* Function prototypes */
AxisClockCalendar *axisclock_calendar_new(GtkWidget *parent);
void axisclock_calendar_show(AxisClockCalendar *calendar);
void axisclock_calendar_hide(AxisClockCalendar *calendar);
void axisclock_calendar_destroy(AxisClockCalendar *calendar);
void axisclock_calendar_set_transparency(AxisClockCalendar *calendar, gdouble transparency);

G_END_DECLS

#endif /* !__CALENDAR_POPUP_H__ */

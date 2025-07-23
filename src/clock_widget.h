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

#ifndef __CLOCK_WIDGET_H__
#define __CLOCK_WIDGET_H__

#include <gtk/gtk.h>
#include <libxfce4panel/libxfce4panel.h>
#include <xfconf/xfconf.h>
#include "calendar_popup.h"
#include "plugin_config.h"

G_BEGIN_DECLS

/* Update interval in milliseconds (1 second) */
#define AXISCLOCK_UPDATE_INTERVAL 1000

typedef struct _AxisClockPlugin AxisClockPlugin;

/* Plugin structure */
struct _AxisClockPlugin {
    XfcePanelPlugin *plugin;
    
    /* Widgets */
    GtkWidget *ebox;
    GtkWidget *label;
    
    /* Calendar popup */
    AxisClockCalendar *calendar;
    
    /* Configuration */
    PluginConfig *config;
    XfconfChannel *channel;
    
    /* Update timeout */
    guint timeout_id;
};

/* Function prototypes */
AxisClockPlugin *axisclock_create_plugin(XfcePanelPlugin *plugin);
void axisclock_destroy_plugin(AxisClockPlugin *axisclock);
void axisclock_update_time(AxisClockPlugin *axisclock);

G_END_DECLS

#endif /* !__CLOCK_WIDGET_H__ */

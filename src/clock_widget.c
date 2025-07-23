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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gtk/gtk.h>
#include <libxfce4panel/libxfce4panel.h>

#include "clock_widget.h"
#include "time_formatter.h"
#include "plugin_config.h"
#include "calendar_popup.h"

/* Update the time display */
void
axisclock_update_time(AxisClockPlugin *axisclock)
{
    gchar *time_string;
    
    g_return_if_fail(axisclock != NULL);
    
    /* Get formatted time */
    time_string = axisclock_get_formatted_time(axisclock->config);
    
    /* Update label */
    gtk_label_set_text(GTK_LABEL(axisclock->label), time_string);
    
    /* Free the string */
    g_free(time_string);
}

/* Timeout callback for updating time */
static gboolean
axisclock_update_timeout(gpointer data)
{
    AxisClockPlugin *axisclock = (AxisClockPlugin *)data;
    
    axisclock_update_time(axisclock);
    
    /* Continue timeout */
    return TRUE;
}

/* Click handler for showing calendar */
static gboolean
axisclock_button_clicked(GtkWidget *widget G_GNUC_UNUSED, GdkEventButton *event, gpointer data)
{
    AxisClockPlugin *axisclock = (AxisClockPlugin *)data;
    
    if (event->button == 1) { /* Left click */
        if (gtk_widget_get_visible(axisclock->calendar->window)) {
            axisclock_calendar_hide(axisclock->calendar);
        } else {
            axisclock_calendar_show(axisclock->calendar);
        }
        return TRUE;
    }
    
    return FALSE;
}

/* Create the plugin */
AxisClockPlugin *
axisclock_create_plugin(XfcePanelPlugin *plugin)
{
    AxisClockPlugin *axisclock;
    
    /* Allocate plugin structure */
    axisclock = g_new0(AxisClockPlugin, 1);
    axisclock->plugin = plugin;
    
    /* Initialize configuration */
    axisclock->config = plugin_config_new();
    
    /* Open xfconf channel */
    axisclock->channel = xfconf_channel_new_with_property_base("xfce4-panel",
                                                               xfce_panel_plugin_get_property_base(plugin));
    
    /* Load configuration */
    plugin_config_load(axisclock->config, axisclock->channel);
    
    /* Create event box for clicking */
    axisclock->ebox = gtk_event_box_new();
    gtk_widget_show(axisclock->ebox);
    
    /* Create label for time display */
    axisclock->label = gtk_label_new("");
    gtk_widget_show(axisclock->label);
    
    /* Add padding around the label */
    gtk_widget_set_margin_start(axisclock->label, 4);
    gtk_widget_set_margin_end(axisclock->label, 4);
    
    /* Add label to event box */
    gtk_container_add(GTK_CONTAINER(axisclock->ebox), axisclock->label);
    
    /* Add event box to plugin */
    gtk_container_add(GTK_CONTAINER(plugin), axisclock->ebox);
    
    /* Create calendar popup */
    axisclock->calendar = axisclock_calendar_new(GTK_WIDGET(plugin));
    
    /* Apply transparency from configuration */
    axisclock_calendar_set_transparency(axisclock->calendar, axisclock->config->calendar_transparency);
    
    /* Connect click signal */
    g_signal_connect(G_OBJECT(axisclock->ebox), "button-press-event",
                     G_CALLBACK(axisclock_button_clicked), axisclock);
    
    /* Initial time update */
    axisclock_update_time(axisclock);
    
    /* Setup timeout for updates */
    axisclock->timeout_id = g_timeout_add(AXISCLOCK_UPDATE_INTERVAL,
                                         axisclock_update_timeout,
                                         axisclock);
    
    return axisclock;
}

/* Destroy the plugin */
void
axisclock_destroy_plugin(AxisClockPlugin *axisclock)
{
    g_return_if_fail(axisclock != NULL);
    
    /* Remove timeout */
    if (axisclock->timeout_id != 0) {
        g_source_remove(axisclock->timeout_id);
        axisclock->timeout_id = 0;
    }
    
    /* Destroy calendar popup */
    if (axisclock->calendar != NULL) {
        axisclock_calendar_destroy(axisclock->calendar);
        axisclock->calendar = NULL;
    }
    
    /* Free configuration */
    if (axisclock->config != NULL) {
        plugin_config_free(axisclock->config);
        axisclock->config = NULL;
    }
    
    /* Close xfconf channel */
    if (axisclock->channel != NULL) {
        g_object_unref(axisclock->channel);
        axisclock->channel = NULL;
    }
    
    /* Free plugin structure */
    g_free(axisclock);
}

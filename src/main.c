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

#include <libxfce4panel/libxfce4panel.h>
#include <libxfce4util/libxfce4util.h>
#include "clock_widget.h"
#include "preferences_dialog.h"

/* Show preferences dialog */
static void
axisclock_show_preferences(XfcePanelPlugin *plugin G_GNUC_UNUSED, AxisClockPlugin *axisclock)
{
    axisclock_preferences_dialog_show(axisclock);
}

/* Show about dialog */
static void
axisclock_show_about(XfcePanelPlugin *plugin G_GNUC_UNUSED)
{
    const gchar *authors[] = {
        "Kamil 'Novik' Nowicki <novik@axisos.org>",
        NULL
    };
    
    gtk_show_about_dialog(NULL,
                          "program-name", "AxisClock",
                          "version", "0.1",
                          "logo-icon-name", "xfce4-axisclock-plugin",
                          "license-type", GTK_LICENSE_GPL_2_0,
                          "authors", authors,
                          "comments", "A macOS-style clock plugin for the Xfce panel",
                          "copyright", "Copyright © 2025 Kamil 'Novik' Nowicki",
                          "website", "https://axisos.org",
                          "website-label", "AxisOS Website",
                          NULL);
}

/* Plugin constructor */
static void
axisclock_construct(XfcePanelPlugin *plugin)
{
    AxisClockPlugin *axisclock;
    
    /* Create the plugin */
    axisclock = axisclock_create_plugin(plugin);
    
    /* Connect plugin signals */
    g_signal_connect(G_OBJECT(plugin), "free-data",
                     G_CALLBACK(axisclock_destroy_plugin), axisclock);
    g_signal_connect(G_OBJECT(plugin), "configure-plugin",
                     G_CALLBACK(axisclock_show_preferences), axisclock);
    g_signal_connect(G_OBJECT(plugin), "about",
                     G_CALLBACK(axisclock_show_about), NULL);
    
    /* Show menu item for configuration */
    xfce_panel_plugin_menu_show_configure(plugin);
    xfce_panel_plugin_menu_show_about(plugin);
    
    /* Show the plugin */
    gtk_widget_show_all(GTK_WIDGET(plugin));
}

/* Register the plugin with XFCE Panel */
XFCE_PANEL_PLUGIN_REGISTER(axisclock_construct);

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

#ifndef __PLUGIN_CONFIG_H__
#define __PLUGIN_CONFIG_H__

#include <glib.h>
#include <xfconf/xfconf.h>

G_BEGIN_DECLS

/**
 * PluginConfig:
 * @time_format: The time format string
 * @show_date: Whether to show the date
 * @calendar_transparency: Transparency level for the calendar popup (0.0 to 1.0)
 * @font_name: Font name for the clock display
 * @use_custom_font: Whether to use a custom font
 *
 * Configuration structure for the AxisClock plugin.
 */
typedef struct _PluginConfig {
    gchar    *time_format;
    gboolean  show_date;
    gdouble   calendar_transparency;
    gchar    *font_name;
    gboolean  use_custom_font;
} PluginConfig;

/* Function prototypes */
PluginConfig *plugin_config_new         (void);
void          plugin_config_free        (PluginConfig       *config);
void          plugin_config_load        (PluginConfig       *config,
                                         XfconfChannel      *channel);
void          plugin_config_save        (const PluginConfig *config,
                                         XfconfChannel      *channel);

G_END_DECLS

#endif /* __PLUGIN_CONFIG_H__ */

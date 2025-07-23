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

#include <glib.h>
#include <string.h>
#include <libxfce4util/libxfce4util.h>
#include <xfconf/xfconf.h>
#include "plugin_config.h"

/* Default configuration values */
#define DEFAULT_TIME_FORMAT "%a %-d %b %-l:%M %p"
#define DEFAULT_SHOW_DATE TRUE
#define DEFAULT_CALENDAR_TRANSPARENCY 0.95
#define DEFAULT_FONT_NAME "Sans 10"
#define DEFAULT_USE_CUSTOM_FONT FALSE

/* Xfconf property names */
#define PROPERTY_TIME_FORMAT "/time-format"
#define PROPERTY_SHOW_DATE "/show-date"
#define PROPERTY_CALENDAR_TRANSPARENCY "/calendar-transparency"
#define PROPERTY_FONT_NAME "/font-name"
#define PROPERTY_USE_CUSTOM_FONT "/use-custom-font"

/**
 * plugin_config_new:
 *
 * Creates a new plugin configuration structure with default values.
 *
 * Returns: A newly allocated #PluginConfig structure.
 */
PluginConfig *
plugin_config_new(void)
{
    PluginConfig *config;
    
    config = g_new0(PluginConfig, 1);
    
    /* Set default values */
    config->time_format = g_strdup(DEFAULT_TIME_FORMAT);
    config->show_date = DEFAULT_SHOW_DATE;
    config->calendar_transparency = DEFAULT_CALENDAR_TRANSPARENCY;
    config->font_name = g_strdup(DEFAULT_FONT_NAME);
    config->use_custom_font = DEFAULT_USE_CUSTOM_FONT;
    
    return config;
}

/**
 * plugin_config_free:
 * @config: A #PluginConfig structure
 *
 * Frees a plugin configuration structure and all its resources.
 */
void
plugin_config_free(PluginConfig *config)
{
    if (config == NULL)
        return;
    
    g_free(config->time_format);
    g_free(config->font_name);
    g_free(config);
}

/**
 * plugin_config_load:
 * @config: A #PluginConfig structure
 * @channel: The Xfconf channel to load from
 *
 * Loads the plugin configuration from the Xfconf channel.
 * If a property doesn't exist, the default value is kept.
 */
void
plugin_config_load(PluginConfig *config, XfconfChannel *channel)
{
    gchar *value;
    
    g_return_if_fail(config != NULL);
    g_return_if_fail(XFCONF_IS_CHANNEL(channel));
    
    /* Load time format */
    value = xfconf_channel_get_string(channel, PROPERTY_TIME_FORMAT, NULL);
    if (value != NULL) {
        g_free(config->time_format);
        config->time_format = value;
    }
    
    /* Load show date option */
    config->show_date = xfconf_channel_get_bool(channel, PROPERTY_SHOW_DATE, DEFAULT_SHOW_DATE);
    
    /* Load calendar transparency */
    config->calendar_transparency = xfconf_channel_get_double(channel, PROPERTY_CALENDAR_TRANSPARENCY, DEFAULT_CALENDAR_TRANSPARENCY);
    
    /* Ensure transparency is within valid range */
    if (config->calendar_transparency < 0.0)
        config->calendar_transparency = 0.0;
    else if (config->calendar_transparency > 1.0)
        config->calendar_transparency = 1.0;
    
    /* Load font name */
    value = xfconf_channel_get_string(channel, PROPERTY_FONT_NAME, NULL);
    if (value != NULL) {
        g_free(config->font_name);
        config->font_name = value;
    }
    
    /* Load use custom font option */
    config->use_custom_font = xfconf_channel_get_bool(channel, PROPERTY_USE_CUSTOM_FONT, DEFAULT_USE_CUSTOM_FONT);
}

/**
 * plugin_config_save:
 * @config: A #PluginConfig structure
 * @channel: The Xfconf channel to save to
 *
 * Saves the plugin configuration to the Xfconf channel.
 */
void
plugin_config_save(const PluginConfig *config, XfconfChannel *channel)
{
    g_return_if_fail(config != NULL);
    g_return_if_fail(XFCONF_IS_CHANNEL(channel));
    
    /* Save time format */
    xfconf_channel_set_string(channel, PROPERTY_TIME_FORMAT, config->time_format);
    
    /* Save show date option */
    xfconf_channel_set_bool(channel, PROPERTY_SHOW_DATE, config->show_date);
    
    /* Save calendar transparency */
    xfconf_channel_set_double(channel, PROPERTY_CALENDAR_TRANSPARENCY, config->calendar_transparency);
    
    /* Save font name */
    xfconf_channel_set_string(channel, PROPERTY_FONT_NAME, config->font_name);
    
    /* Save use custom font option */
    xfconf_channel_set_bool(channel, PROPERTY_USE_CUSTOM_FONT, config->use_custom_font);
}

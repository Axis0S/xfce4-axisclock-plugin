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
#include <time.h>
#include <string.h>
#include <ctype.h>

#include "time_formatter.h"
#include "plugin_config.h"

/* Get the current time formatted according to configuration */
gchar *
axisclock_get_formatted_time(PluginConfig *config)
{
    time_t current_time;
    struct tm *time_info;
    gchar buffer[256];
    gchar *formatted_time;
    gchar *p;
    const gchar *format;
    
    /* Get current time */
    time(&current_time);
    time_info = localtime(&current_time);
    
    /* Use configured format or default */
    format = (config && config->time_format) ? config->time_format : AXISCLOCK_TIME_FORMAT;
    
    /* Format time string */
    strftime(buffer, sizeof(buffer), format, time_info);
    
    /* Convert AM/PM to lowercase and remove space before it */
    formatted_time = g_strdup(buffer);
    
    /* Find AM or PM in the string */
    p = strstr(formatted_time, "AM");
    if (p == NULL) {
        p = strstr(formatted_time, "PM");
    }
    
    if (p != NULL) {
        /* Convert to lowercase */
        p[0] = tolower(p[0]);
        p[1] = tolower(p[1]);
        
        /* Remove space before AM/PM if present */
        if (p > formatted_time && *(p - 1) == ' ') {
            memmove(p - 1, p, strlen(p) + 1);
        }
    }
    
    return formatted_time;
}

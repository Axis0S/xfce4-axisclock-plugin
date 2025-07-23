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

#ifndef __TIME_FORMATTER_H__
#define __TIME_FORMATTER_H__

#include <glib.h>
#include "plugin_config.h"

G_BEGIN_DECLS

/* Time format constants */
#define AXISCLOCK_TIME_FORMAT "%a %b %-d %-l:%M%p"

/* Function prototypes */
gchar *axisclock_get_formatted_time(PluginConfig *config);

G_END_DECLS

#endif /* !__TIME_FORMATTER_H__ */

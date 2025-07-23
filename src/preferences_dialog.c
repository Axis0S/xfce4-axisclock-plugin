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
#include <libxfce4ui/libxfce4ui.h>

#include "preferences_dialog.h"
#include "clock_widget.h"
#include "plugin_config.h"

/* Dialog response callback */
static void
dialog_response_cb(GtkWidget *dialog, gint response, AxisClockPlugin *axisclock)
{
    if (response == GTK_RESPONSE_OK || response == GTK_RESPONSE_APPLY) {
        /* Save configuration */
        plugin_config_save(axisclock->config, axisclock->channel);
        
        /* Apply transparency immediately */
        axisclock_calendar_set_transparency(axisclock->calendar, 
                                           axisclock->config->calendar_transparency);
    }
    
    if (response != GTK_RESPONSE_APPLY) {
        gtk_widget_destroy(dialog);
    }
}

/* Transparency scale changed callback */
static void
transparency_changed_cb(GtkRange *range, AxisClockPlugin *axisclock)
{
    gdouble value = gtk_range_get_value(range);
    axisclock->config->calendar_transparency = value;
    
    /* Update calendar transparency immediately for preview */
    axisclock_calendar_set_transparency(axisclock->calendar, value);
}

/* Time format changed callback */
static void
time_format_changed_cb(GtkEntry *entry, AxisClockPlugin *axisclock)
{
    const gchar *text = gtk_entry_get_text(entry);
    g_free(axisclock->config->time_format);
    axisclock->config->time_format = g_strdup(text);
    
    /* Update time display immediately */
    axisclock_update_time(axisclock);
}

/* Format radio button toggled callback */
static void
format_radio_toggled_cb(GtkToggleButton *button, AxisClockPlugin *axisclock)
{
    const gchar *format;
    
    if (!gtk_toggle_button_get_active(button))
        return;
    
    format = g_object_get_data(G_OBJECT(button), "format");
    if (g_strcmp0(format, "custom") != 0) {
        g_free(axisclock->config->time_format);
        axisclock->config->time_format = g_strdup(format);
        
        /* Update the custom format entry */
        GtkWidget *entry = g_object_get_data(G_OBJECT(button), "format-entry");
        if (entry) {
            gtk_entry_set_text(GTK_ENTRY(entry), format);
        }
        
        /* Update time display immediately */
        axisclock_update_time(axisclock);
    }
}

/* Show date toggle callback */
static void
show_date_toggled_cb(GtkToggleButton *button, AxisClockPlugin *axisclock)
{
    axisclock->config->show_date = gtk_toggle_button_get_active(button);
    axisclock_update_time(axisclock);
}

/* Use custom font toggle callback */
static void
use_custom_font_toggled_cb(GtkToggleButton *button, AxisClockPlugin *axisclock)
{
    GtkWidget *font_button;
    
    axisclock->config->use_custom_font = gtk_toggle_button_get_active(button);
    
    /* Enable/disable font button based on checkbox state */
    font_button = g_object_get_data(G_OBJECT(button), "font-button");
    if (font_button) {
        gtk_widget_set_sensitive(font_button, axisclock->config->use_custom_font);
    }
    /* TODO: Apply font changes */
}

/* Font changed callback */
static void
font_changed_cb(GtkFontButton *button, AxisClockPlugin *axisclock)
{
    const gchar *font_name = gtk_font_button_get_font_name(button);
    g_free(axisclock->config->font_name);
    axisclock->config->font_name = g_strdup(font_name);
    /* TODO: Apply font changes */
}

/* Show preferences dialog */
void
axisclock_preferences_dialog_show(AxisClockPlugin *axisclock)
{
    GtkWidget *dialog;
    GtkWidget *content_area;
    GtkWidget *time_grid, *font_grid, *calendar_grid;
    GtkWidget *label;
    GtkWidget *widget;
    GtkWidget *frame;
    GtkWidget *vbox;
    GtkWidget *font_button;
    gint row;
    
    g_print("DEBUG: preferences_dialog_show called\n");
    
    /* Create dialog */
    dialog = gtk_dialog_new_with_buttons("AxisClock Preferences",
                                         GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(axisclock->plugin))),
                                         GTK_DIALOG_DESTROY_WITH_PARENT,
                                         "_Close", GTK_RESPONSE_OK,
                                         "_Apply", GTK_RESPONSE_APPLY,
                                         NULL);
    
    gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER);
    gtk_window_set_icon_name(GTK_WINDOW(dialog), "xfce4-time-out-plugin");
    gtk_window_set_default_size(GTK_WINDOW(dialog), 400, -1);
    
    /* Get content area */
    content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    gtk_container_set_border_width(GTK_CONTAINER(content_area), 12);
    
    /* Create main vertical box */
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 18);
    gtk_container_add(GTK_CONTAINER(content_area), vbox);
    
    /* Time Display Settings Frame */
    frame = gtk_frame_new("Time Display");
    gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, FALSE, 0);
    time_grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(frame), time_grid);
    gtk_container_set_border_width(GTK_CONTAINER(time_grid), 6);
    gtk_grid_set_column_spacing(GTK_GRID(time_grid), 12);
    gtk_grid_set_row_spacing(GTK_GRID(time_grid), 6);
    row = 0;
    
    /* Time format - provide radio buttons for common formats */
    label = gtk_label_new_with_mnemonic("Time _format:");
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(time_grid), label, 0, row, 1, 1);
    
    /* Create a box for format radio buttons */
    GtkWidget *format_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_widget_set_hexpand(format_box, TRUE);
    
    /* 12-hour format */
    widget = gtk_radio_button_new_with_label(NULL, "12-hour (3:45 PM)");
    if (g_strstr_len(axisclock->config->time_format, -1, "%p") != NULL ||
        g_strstr_len(axisclock->config->time_format, -1, "%P") != NULL) {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), TRUE);
    }
    g_object_set_data(G_OBJECT(widget), "format", (gpointer)"%a %-d %b %-l:%M %p");
    g_signal_connect(widget, "toggled", G_CALLBACK(format_radio_toggled_cb), axisclock);
    gtk_box_pack_start(GTK_BOX(format_box), widget, FALSE, FALSE, 0);
    
    /* 24-hour format */
    widget = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(widget), "24-hour (15:45)");
    if (g_strstr_len(axisclock->config->time_format, -1, "%H") != NULL) {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), TRUE);
    }
    g_object_set_data(G_OBJECT(widget), "format", (gpointer)"%a %-d %b %H:%M");
    g_signal_connect(widget, "toggled", G_CALLBACK(format_radio_toggled_cb), axisclock);
    gtk_box_pack_start(GTK_BOX(format_box), widget, FALSE, FALSE, 0);
    
    /* Custom format */
    widget = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(widget), "Custom");
    g_object_set_data(G_OBJECT(widget), "format", (gpointer)"custom");
    g_signal_connect(widget, "toggled", G_CALLBACK(format_radio_toggled_cb), axisclock);
    gtk_box_pack_start(GTK_BOX(format_box), widget, FALSE, FALSE, 0);
    
    gtk_grid_attach(GTK_GRID(time_grid), format_box, 1, row, 1, 1);
    row++;
    
    /* Custom format entry */
    label = gtk_label_new_with_mnemonic("Custom _format:");
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(time_grid), label, 0, row, 1, 1);
    
    widget = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(widget), axisclock->config->time_format);
    gtk_widget_set_hexpand(widget, TRUE);
    gtk_label_set_mnemonic_widget(GTK_LABEL(label), widget);
    g_signal_connect(widget, "changed", G_CALLBACK(time_format_changed_cb), axisclock);
    gtk_grid_attach(GTK_GRID(time_grid), widget, 1, row, 1, 1);
    row++;
    
    /* Format help text */
    label = gtk_label_new("Format codes: %H (24h), %l (12h), %M (min), %S (sec), %p (AM/PM)");
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_style_context_add_class(gtk_widget_get_style_context(label), "dim-label");
    gtk_label_set_line_wrap(GTK_LABEL(label), TRUE);
    gtk_grid_attach(GTK_GRID(time_grid), label, 0, row, 2, 1);
    row++;
    
    /* Show date */
    widget = gtk_check_button_new_with_mnemonic("Show _date");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), axisclock->config->show_date);
    g_signal_connect(widget, "toggled", G_CALLBACK(show_date_toggled_cb), axisclock);
    gtk_grid_attach(GTK_GRID(time_grid), widget, 0, row, 2, 1);
    row++;
    
    /* Font Settings Frame */
    frame = gtk_frame_new("Font");
    gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, FALSE, 0);
    font_grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(frame), font_grid);
    gtk_container_set_border_width(GTK_CONTAINER(font_grid), 6);
    gtk_grid_set_column_spacing(GTK_GRID(font_grid), 12);
    gtk_grid_set_row_spacing(GTK_GRID(font_grid), 6);
    row = 0;
    
    /* Use custom font */
    widget = gtk_check_button_new_with_mnemonic("Use _custom font");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), axisclock->config->use_custom_font);
    gtk_grid_attach(GTK_GRID(font_grid), widget, 0, row, 2, 1);
    row++;
    
    /* Font chooser */
    font_button = gtk_font_button_new_with_font(axisclock->config->font_name);
    gtk_widget_set_sensitive(font_button, axisclock->config->use_custom_font);
    g_signal_connect(font_button, "font-set", G_CALLBACK(font_changed_cb), axisclock);
    gtk_grid_attach(GTK_GRID(font_grid), font_button, 0, row, 2, 1);
    
    /* Store reference to font button in checkbox for toggling */
    g_object_set_data(G_OBJECT(widget), "font-button", font_button);
    g_signal_connect(widget, "toggled", G_CALLBACK(use_custom_font_toggled_cb), axisclock);
    row++;
    
    /* Calendar Settings Frame */
    frame = gtk_frame_new("Calendar Popup");
    gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, FALSE, 0);
    calendar_grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(frame), calendar_grid);
    gtk_container_set_border_width(GTK_CONTAINER(calendar_grid), 6);
    gtk_grid_set_column_spacing(GTK_GRID(calendar_grid), 12);
    gtk_grid_set_row_spacing(GTK_GRID(calendar_grid), 6);
    row = 0;
    
    /* Transparency */
    label = gtk_label_new_with_mnemonic("_Transparency:");
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(calendar_grid), label, 0, row, 1, 1);
    
    widget = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0.1, 1.0, 0.05);
    gtk_range_set_value(GTK_RANGE(widget), axisclock->config->calendar_transparency);
    gtk_scale_set_digits(GTK_SCALE(widget), 2);
    gtk_scale_set_draw_value(GTK_SCALE(widget), TRUE);
    gtk_scale_set_value_pos(GTK_SCALE(widget), GTK_POS_RIGHT);
    gtk_widget_set_hexpand(widget, TRUE);
    gtk_label_set_mnemonic_widget(GTK_LABEL(label), widget);
    g_signal_connect(widget, "value-changed", G_CALLBACK(transparency_changed_cb), axisclock);
    gtk_grid_attach(GTK_GRID(calendar_grid), widget, 1, row, 1, 1);
    row++;
    
    /* Add help text */
    label = gtk_label_new("Lower values make the calendar more transparent");
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_style_context_add_class(gtk_widget_get_style_context(label), "dim-label");
    gtk_grid_attach(GTK_GRID(calendar_grid), label, 0, row, 2, 1);
    
    /* Connect response signal */
    g_signal_connect(dialog, "response", G_CALLBACK(dialog_response_cb), axisclock);
    
    /* Show all widgets */
    gtk_widget_show_all(dialog);
    
    g_print("DEBUG: Dialog should be visible now\n");
}

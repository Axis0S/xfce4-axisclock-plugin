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
#include <time.h>
#include "calendar_popup.h"

/* Forward declarations */
static void update_calendar(AxisClockCalendar *calendar);
static gboolean on_draw(GtkWidget *widget, cairo_t *cr, gpointer user_data);
static gboolean on_button_press(GtkWidget *widget, GdkEventButton *event, gpointer user_data);
static gboolean on_focus_out(GtkWidget *widget, GdkEventFocus *event, gpointer user_data);

/* Create a new calendar popup */
AxisClockCalendar *
axisclock_calendar_new(GtkWidget *parent)
{
    AxisClockCalendar *calendar = g_new0(AxisClockCalendar, 1);
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);

    calendar->parent_widget = parent;
    calendar->today_day = tm_info->tm_mday;
    calendar->today_month = tm_info->tm_mon;
    calendar->today_year = tm_info->tm_year + 1900;
    calendar->current_month = calendar->today_month;
    calendar->current_year = calendar->today_year;

    /* Create popup window */
    calendar->window = gtk_window_new(GTK_WINDOW_POPUP);
    gtk_window_set_type_hint(GTK_WINDOW(calendar->window), GDK_WINDOW_TYPE_HINT_TOOLTIP);
    gtk_window_set_resizable(GTK_WINDOW(calendar->window), FALSE);
    gtk_window_set_decorated(GTK_WINDOW(calendar->window), FALSE);
    gtk_window_set_skip_taskbar_hint(GTK_WINDOW(calendar->window), TRUE);
    gtk_window_set_skip_pager_hint(GTK_WINDOW(calendar->window), TRUE);
    gtk_window_set_keep_above(GTK_WINDOW(calendar->window), TRUE);
    gtk_widget_set_size_request(calendar->window, 240, 240);
    
    /* Set window name and role */
    gtk_window_set_title(GTK_WINDOW(calendar->window), "AxisClock Calendar");
    gtk_window_set_role(GTK_WINDOW(calendar->window), "axisclock-calendar-popup");
    
    /* Enable transparency */
    gtk_widget_set_app_paintable(calendar->window, TRUE);
    GdkScreen *screen = gtk_widget_get_screen(calendar->window);
    GdkVisual *visual = gdk_screen_get_rgba_visual(screen);
    if (visual != NULL) {
        gtk_widget_set_visual(calendar->window, visual);
    }
    
    /* Set default transparency */
    calendar->transparency = 0.9; /* 90% opaque by default */
    
    /* Add CSS for rounded corners and theme colors */
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider,
        "window {"
        "  border-radius: 12px;"
        "  background-color: transparent;"
        "}"
        "label {"
        "  color: @theme_fg_color;"
        "}",
        -1, NULL);
    
    GtkStyleContext *context = gtk_widget_get_style_context(calendar->window);
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider),
                                   GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    g_object_unref(provider);
    
    /* Connect signals */
    g_signal_connect(calendar->window, "draw", G_CALLBACK(on_draw), calendar);
    g_signal_connect(calendar->window, "button-press-event", G_CALLBACK(on_button_press), calendar);
    g_signal_connect(calendar->window, "focus-out-event", G_CALLBACK(on_focus_out), calendar);

    /* Main container */
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 8);
    gtk_container_add(GTK_CONTAINER(calendar->window), vbox);

    /* Add grid for calendar days */
    calendar->grid = gtk_grid_new();
    gtk_grid_set_row_homogeneous(GTK_GRID(calendar->grid), TRUE);
    gtk_grid_set_column_homogeneous(GTK_GRID(calendar->grid), TRUE);
    gtk_grid_set_row_spacing(GTK_GRID(calendar->grid), 2);
    gtk_grid_set_column_spacing(GTK_GRID(calendar->grid), 2);
    gtk_box_pack_start(GTK_BOX(vbox), calendar->grid, TRUE, TRUE, 0);

    /* Day names */
    const gchar *day_names[] = {"S", "M", "T", "W", "T", "F", "S"};
    for (int i = 0; i < 7; ++i) {
        GtkWidget *label = gtk_label_new(day_names[i]);
        gtk_grid_attach(GTK_GRID(calendar->grid), label, i, 0, 1, 1);
    }

    /* Create labels for the days */
    for (int i = 0; i < 42; ++i) {
        calendar->day_buttons[i] = gtk_label_new("");
        gtk_widget_set_size_request(calendar->day_buttons[i], 30, 30);
        gtk_grid_attach(GTK_GRID(calendar->grid), calendar->day_buttons[i], i % 7, (i / 7) + 1, 1, 1);
    }

    /* Update the calendar */
    update_calendar(calendar);

    return calendar;
}

/* Custom draw callback for transparency and rounded corners */
static gboolean
on_draw(GtkWidget *widget, cairo_t *cr, gpointer user_data)
{
    AxisClockCalendar *calendar = (AxisClockCalendar *)user_data;
    gint width = gtk_widget_get_allocated_width(widget);
    gint height = gtk_widget_get_allocated_height(widget);
    double radius = 12.0;
    GtkStyleContext *context;
    GdkRGBA bg_color, border_color;
    
    /* Get colors from the system theme */
    context = gtk_widget_get_style_context(widget);
    gtk_style_context_get_background_color(context, gtk_widget_get_state_flags(widget), &bg_color);
    gtk_style_context_get_border_color(context, gtk_widget_get_state_flags(widget), &border_color);
    
    /* Clear the surface */
    cairo_set_source_rgba(cr, 0.0, 0.0, 0.0, 0.0);
    cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
    cairo_paint(cr);
    
    /* Draw rounded rectangle path */
    cairo_new_sub_path(cr);
    cairo_arc(cr, width - radius, radius, radius, -90.0 * G_PI / 180.0, 0);
    cairo_arc(cr, width - radius, height - radius, radius, 0, 90.0 * G_PI / 180.0);
    cairo_arc(cr, radius, height - radius, radius, 90.0 * G_PI / 180.0, 180.0 * G_PI / 180.0);
    cairo_arc(cr, radius, radius, radius, 180.0 * G_PI / 180.0, 270.0 * G_PI / 180.0);
    cairo_close_path(cr);
    
    /* Fill with semi-transparent theme background color */
    cairo_set_source_rgba(cr, bg_color.red, bg_color.green, bg_color.blue, calendar->transparency);
    cairo_fill_preserve(cr);
    
    /* Add a subtle border using theme border color */
    cairo_set_source_rgba(cr, border_color.red, border_color.green, border_color.blue, calendar->transparency);
    cairo_set_line_width(cr, 1.0);
    cairo_stroke(cr);
    
    /* Let child widgets draw themselves */
    cairo_set_operator(cr, CAIRO_OPERATOR_OVER);
    return FALSE;
}

static void
update_calendar(AxisClockCalendar *calendar)
{

    /* Calculate first day of the month */
    struct tm tm = {0};
    tm.tm_year = calendar->current_year - 1900;
    tm.tm_mon = calendar->current_month;
    tm.tm_mday = 1;
    mktime(&tm);
    
    /* tm_wday: 0 = Sunday, 1 = Monday, ..., 6 = Saturday */
    int start_day_of_week = tm.tm_wday;
    
    /* Get number of days in current month */
    int days_in_month;
    if (calendar->current_month == 1) { /* February */
        /* Check for leap year */
        if ((calendar->current_year % 4 == 0 && calendar->current_year % 100 != 0) || 
            (calendar->current_year % 400 == 0)) {
            days_in_month = 29;
        } else {
            days_in_month = 28;
        }
    } else if (calendar->current_month == 3 || calendar->current_month == 5 || 
               calendar->current_month == 8 || calendar->current_month == 10) {
        days_in_month = 30;
    } else {
        days_in_month = 31;
    }

    /* Fill the calendar grid with days */
    for (int i = 0; i < 42; ++i) {
        if (i >= start_day_of_week && i < start_day_of_week + days_in_month) {
            int day = i - start_day_of_week + 1;
            gchar *day_label = g_strdup_printf("%d", day);
            gtk_label_set_text(GTK_LABEL(calendar->day_buttons[i]), day_label);
            g_free(day_label);

            /* Highlight today's date */
            if (day == calendar->today_day && calendar->current_month == calendar->today_month && calendar->current_year == calendar->today_year) {
                gchar *markup = g_strdup_printf("<b>%d</b>", day);
                gtk_label_set_markup(GTK_LABEL(calendar->day_buttons[i]), markup);
                g_free(markup);
            }
        } else {
            gtk_label_set_text(GTK_LABEL(calendar->day_buttons[i]), "");
        }
    }
}


/* Show the calendar popup */
void
axisclock_calendar_show(AxisClockCalendar *calendar)
{
    GdkWindow *window;
    GdkDisplay *display;
    GdkMonitor *monitor;
    GdkRectangle workarea;
    gint x, y, width, height;
    gint popup_width = 240, popup_height = 240;
    gint final_x, final_y;
    
    /* First realize the window to get its actual size */
    gtk_widget_realize(calendar->window);
    gtk_widget_show_all(calendar->window);
    
    /* Get parent widget position and size */
    window = gtk_widget_get_window(calendar->parent_widget);
    if (!window) return;
    
    gdk_window_get_origin(window, &x, &y);
    width = gtk_widget_get_allocated_width(calendar->parent_widget);
    height = gtk_widget_get_allocated_height(calendar->parent_widget);
    
    /* Get monitor workarea */
    display = gdk_window_get_display(window);
    monitor = gdk_display_get_monitor_at_window(display, window);
    gdk_monitor_get_workarea(monitor, &workarea);
    
    /* Calculate position - try to center below the clock */
    final_x = x + (width / 2) - (popup_width / 2);
    final_y = y + height + 5; /* 5px gap */
    
    /* Ensure the popup stays within screen bounds */
    /* Check right edge */
    if (final_x + popup_width > workarea.x + workarea.width) {
        final_x = workarea.x + workarea.width - popup_width - 5;
    }
    
    /* Check left edge */
    if (final_x < workarea.x) {
        final_x = workarea.x + 5;
    }
    
    /* Check bottom edge - if not enough space below, show above */
    if (final_y + popup_height > workarea.y + workarea.height) {
        final_y = y - popup_height - 5;
    }
    
    /* Check top edge */
    if (final_y < workarea.y) {
        final_y = workarea.y + 5;
    }
    
    /* Position the window */
    gtk_window_move(GTK_WINDOW(calendar->window), final_x, final_y);
    
    /* Grab focus so we can detect when user clicks outside */
    gtk_widget_grab_focus(calendar->window);
    gtk_window_present(GTK_WINDOW(calendar->window));
}

/* Hide the calendar popup */
void
axisclock_calendar_hide(AxisClockCalendar *calendar)
{
    gtk_widget_hide(calendar->window);
}

/* Destroy the calendar popup */
void
axisclock_calendar_destroy(AxisClockCalendar *calendar)
{
    gtk_widget_destroy(calendar->window);
    g_free(calendar);
}

/* Set transparency level for the calendar popup */
void
axisclock_calendar_set_transparency(AxisClockCalendar *calendar, gdouble transparency)
{
    g_return_if_fail(calendar != NULL);
    g_return_if_fail(transparency >= 0.0 && transparency <= 1.0);
    
    calendar->transparency = transparency;
    
    /* Redraw the window if it's visible */
    if (gtk_widget_get_visible(calendar->window)) {
        gtk_widget_queue_draw(calendar->window);
    }
}

/* Handle button press events */
static gboolean
on_button_press(GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
    /* Keep the window focused */
    gtk_widget_grab_focus(widget);
    return FALSE;
}

/* Handle focus out events - hide calendar when it loses focus */
static gboolean
on_focus_out(GtkWidget *widget, GdkEventFocus *event, gpointer user_data)
{
    AxisClockCalendar *calendar = (AxisClockCalendar *)user_data;
    axisclock_calendar_hide(calendar);
    return FALSE;
}


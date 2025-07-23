# Source files for the plugin
axisclock_sources = [
  'main.c',
  'clock_widget.c',
  'time_formatter.c',
  'calendar_popup.c',
  'plugin_config.c',
  'preferences_dialog.c'
]

# Build the plugin as a shared library
shared_library('axisclock',
  axisclock_sources,
  dependencies: [
    gtk_dep,
    libxfce4panel_dep,
    libxfce4util_dep,
    libxfce4ui_dep,
    xfconf_dep
  ],
  install: true,
  install_dir: plugindir
)

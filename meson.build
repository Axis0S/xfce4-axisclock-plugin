project('xfce4-axisclock-plugin', 'c',
  version: '0.1',
  default_options: [
    'warning_level=2',
    'c_std=c11'
  ],
  meson_version: '>= 0.50.0'
)

# Dependencies
gtk_dep = dependency('gtk+-3.0', version: '>= 3.22')
libxfce4panel_dep = dependency('libxfce4panel-2.0', version: '>= 4.12')
libxfce4util_dep = dependency('libxfce4util-1.0', version: '>= 4.12')
libxfce4ui_dep = dependency('libxfce4ui-2', version: '>= 4.12')
xfconf_dep = dependency('libxfconf-0', version: '>= 4.12')

# Plugin installation directory
plugin_libdir = libxfce4panel_dep.get_pkgconfig_variable('libdir')
plugindir = plugin_libdir / 'xfce4' / 'panel' / 'plugins'

# Desktop file installation directory
desktop_datadir = get_option('datadir')
desktopdir = desktop_datadir / 'xfce4' / 'panel' / 'plugins'

# Add subdirectories
subdir('src')
subdir('data')

# Post-install script to update icon cache
meson.add_install_script('sh', '-c',
  'if [ -z "$DESTDIR" ]; then gtk-update-icon-cache -q -t -f ${MESON_INSTALL_PREFIX}/share/icons/hicolor || true; fi'
)

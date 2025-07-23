# Desktop file configuration
desktop_in = 'xfce4-axisclock-plugin.desktop.in'

# Generate desktop file from .in template
desktop_file = configure_file(
  input: desktop_in,
  output: 'xfce4-axisclock-plugin.desktop',
  configuration: configuration_data()
)

# Install desktop file
install_data(desktop_file,
  install_dir: desktopdir
)

# Install icons
icon_sizes = ['16x16', '22x22', '24x24']

foreach size : icon_sizes
  install_data(
    'icons/hicolor/' + size + '/apps/xfce4-axisclock-plugin.svg',
    install_dir: get_option('datadir') / 'icons' / 'hicolor' / size / 'apps'
  )
endforeach

# Install scalable icon
install_data(
  'icons/hicolor/scalable/apps/xfce4-axisclock-plugin.svg',
  install_dir: get_option('datadir') / 'icons' / 'hicolor' / 'scalable' / 'apps'
)

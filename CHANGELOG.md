# Changelog

All notable changes to xfce4-axisclock-plugin will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [0.1] - 2025-01-23

### Added
- Initial release of xfce4-axisclock-plugin
- macOS-style clock display for Xfce panel
- Calendar popup with month view
- Click to show/hide calendar functionality
- Theme support - automatically adapts to system light/dark themes
- Transparency control for calendar popup (0.1 to 1.0)
- Preferences dialog with multiple configuration options:
  - Time format selection (12-hour, 24-hour, or custom)
  - Custom time format support using strftime codes
  - Show/hide date option
  - Custom font selection
  - Calendar transparency adjustment
- Automatic calendar closing when clicking outside
- Proper window hints to prevent taskbar appearance
- Round corners and smooth transparency effects
- Today's date highlighting in calendar
- Full internationalization support ready

### Technical Details
- Built with GTK+ 3 and Xfce panel libraries
- Uses Meson build system
- Xfconf integration for persistent settings
- Modular code structure for easy maintenance
- Cairo-based custom drawing for transparency effects

### Author
- Kamil 'Novik' Nowicki <novik@axisos.org>

### Links
- Website: https://axisos.org
- Repository: https://github.com/AxisOS/xfce4-axisclock-plugin

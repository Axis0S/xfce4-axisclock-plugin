#!/bin/bash

#=============================================================================
# XFCE4 MacClock Plugin Uninstallation Script
# 
# This script removes the MacClock plugin from your system.
#=============================================================================

set -e  # Exit on error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Print colored output
print_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

# Check if running as root
check_root() {
    if [ "$EUID" -eq 0 ]; then
        print_error "Please do not run this script as root"
        exit 1
    fi
}

# Check if sudo is available
check_sudo() {
    if ! command -v sudo &> /dev/null; then
        print_error "sudo is required but not installed"
        exit 1
    fi
}

# Find installed plugin files
find_installed_files() {
    print_info "Searching for installed MacClock plugin files..."
    
    # Common locations to check
    local plugin_locations=(
        "/usr/lib/xfce4/panel/plugins/libmacclock.so"
        "/usr/lib64/xfce4/panel/plugins/libmacclock.so"
        "/usr/local/lib/xfce4/panel/plugins/libmacclock.so"
        "/usr/lib/x86_64-linux-gnu/xfce4/panel/plugins/libmacclock.so"
        "/usr/lib/i386-linux-gnu/xfce4/panel/plugins/libmacclock.so"
        "/usr/lib/aarch64-linux-gnu/xfce4/panel/plugins/libmacclock.so"
    )
    
    local desktop_locations=(
        "/usr/share/xfce4/panel/plugins/xfce4-macclock-plugin.desktop"
        "/usr/local/share/xfce4/panel/plugins/xfce4-macclock-plugin.desktop"
    )
    
    local icon_dirs=(
        "/usr/share/icons/hicolor"
        "/usr/local/share/icons/hicolor"
    )
    
    FILES_TO_REMOVE=()
    
    # Check for plugin library
    for file in "${plugin_locations[@]}"; do
        if [ -f "$file" ]; then
            FILES_TO_REMOVE+=("$file")
            print_info "Found plugin library: $file"
        fi
    done
    
    # Check for desktop file
    for file in "${desktop_locations[@]}"; do
        if [ -f "$file" ]; then
            FILES_TO_REMOVE+=("$file")
            print_info "Found desktop file: $file"
        fi
    done
    
    # Check for icons
    for dir in "${icon_dirs[@]}"; do
        for size in 16x16 22x22 24x24 scalable; do
            local icon_file="$dir/$size/apps/xfce4-macclock-plugin.svg"
            if [ -f "$icon_file" ]; then
                FILES_TO_REMOVE+=("$icon_file")
                print_info "Found icon: $icon_file"
            fi
        done
    done
    
    if [ ${#FILES_TO_REMOVE[@]} -eq 0 ]; then
        print_warning "No MacClock plugin files found on the system"
        return 1
    fi
    
    return 0
}

# Remove plugin from panel configuration
remove_from_panel() {
    print_info "Checking if plugin is currently in use..."
    
    # Kill any running instances
    if pgrep -f libmacclock.so > /dev/null 2>&1; then
        print_warning "MacClock plugin is currently running"
        print_info "Please remove it from your panel before uninstalling"
        print_info "Right-click on the MacClock plugin in your panel and select 'Remove'"
        echo
        read -p "Press Enter when you have removed the plugin from the panel..." -r
        echo
    fi
}

# Uninstall the plugin
uninstall_plugin() {
    print_info "Removing MacClock plugin files..."
    
    for file in "${FILES_TO_REMOVE[@]}"; do
        print_info "Removing: $file"
        sudo rm -f "$file"
    done
    
    # Update icon cache
    print_info "Updating icon cache..."
    sudo gtk-update-icon-cache -q -t -f /usr/share/icons/hicolor 2>/dev/null || true
    if [ -d "/usr/local/share/icons/hicolor" ]; then
        sudo gtk-update-icon-cache -q -t -f /usr/local/share/icons/hicolor 2>/dev/null || true
    fi
    
    print_success "Plugin files removed successfully"
}

# Clean build directory
clean_build() {
    if [ -d "build" ]; then
        print_info "Cleaning build directory..."
        rm -rf build
        print_success "Build directory cleaned"
    fi
}

# Main uninstallation function
main() {
    echo "===================================================="
    echo "   XFCE4 MacClock Plugin Uninstallation Script     "
    echo "===================================================="
    echo
    
    # Checks
    check_root
    check_sudo
    
    # Find installed files
    if ! find_installed_files; then
        echo
        print_info "MacClock plugin is not installed on this system"
        
        # Ask if user wants to clean build directory
        if [ -d "build" ]; then
            read -p "Do you want to clean the build directory? [Y/n] " -n 1 -r
            echo
            if [[ $REPLY =~ ^[Yy]$ ]] || [[ -z $REPLY ]]; then
                clean_build
            fi
        fi
        exit 0
    fi
    
    # Show files to be removed
    echo
    print_info "The following files will be removed:"
    for file in "${FILES_TO_REMOVE[@]}"; do
        echo "  - $file"
    done
    
    # Ask for confirmation
    echo
    read -p "Do you want to uninstall the MacClock plugin? [y/N] " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        remove_from_panel
        uninstall_plugin
        clean_build
        
        echo
        print_success "MacClock plugin has been uninstalled successfully!"
        print_info "You may need to restart your panel with: xfce4-panel -r"
    else
        print_warning "Uninstallation cancelled"
    fi
}

# Run main function
main "$@"

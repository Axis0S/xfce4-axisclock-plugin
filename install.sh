#!/bin/bash

#=============================================================================
# XFCE4 MacClock Plugin Installation Script
# 
# This script automatically detects your Linux distribution, installs
# required dependencies, and builds/installs the MacClock plugin.
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

# Detect operating system
detect_os() {
    if [ -f /etc/os-release ]; then
        . /etc/os-release
        OS=$ID
        OS_LIKE=$ID_LIKE
        VERSION=$VERSION_ID
        print_info "Detected OS: $PRETTY_NAME"
    else
        print_error "Cannot detect operating system"
        exit 1
    fi
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

# Install dependencies based on distribution
install_dependencies() {
    print_info "Installing dependencies..."
    
    case "$OS" in
        debian|ubuntu|linuxmint|elementary|pop)
            print_info "Using APT package manager"
            sudo apt update
            sudo apt install -y libgtk-3-dev libxfce4panel-2.0-dev libxfce4util-dev meson ninja-build pkg-config gcc
            ;;
            
        fedora|rhel|centos)
            print_info "Using DNF package manager"
            sudo dnf install -y gtk3-devel xfce4-panel-devel libxfce4util-devel meson ninja-build pkgconfig gcc
            ;;
            
        opensuse*|sles)
            print_info "Using Zypper package manager"
            sudo zypper install -y gtk3-devel xfce4-panel-devel libxfce4util-devel meson ninja gcc
            ;;
            
        arch|manjaro|endeavouros)
            print_info "Using Pacman package manager"
            sudo pacman -Syu --needed --noconfirm gtk3 xfce4-panel meson ninja pkgconf gcc
            ;;
            
        void)
            print_info "Using XBPS package manager"
            sudo xbps-install -Syu gtk+3-devel xfce4-panel-devel libxfce4util-devel meson ninja gcc
            ;;
            
        alpine)
            print_info "Using APK package manager"
            sudo apk add --no-cache gtk+3.0-dev xfce4-panel-dev libxfce4util-dev meson ninja pkgconf gcc musl-dev
            ;;
            
        gentoo)
            print_info "Using Portage package manager"
            sudo emerge --ask=n x11-libs/gtk+:3 xfce-base/xfce4-panel xfce-base/libxfce4util dev-util/meson dev-util/ninja
            ;;
            
        *)
            # Try to detect based on ID_LIKE
            if [[ "$OS_LIKE" == *"debian"* ]] || [[ "$OS_LIKE" == *"ubuntu"* ]]; then
                print_warning "Trying Debian-based installation for $OS"
                sudo apt update
                sudo apt install -y libgtk-3-dev libxfce4panel-2.0-dev libxfce4util-dev meson ninja-build pkg-config gcc
            elif [[ "$OS_LIKE" == *"fedora"* ]] || [[ "$OS_LIKE" == *"rhel"* ]]; then
                print_warning "Trying Fedora-based installation for $OS"
                sudo dnf install -y gtk3-devel xfce4-panel-devel libxfce4util-devel meson ninja-build pkgconfig gcc
            elif [[ "$OS_LIKE" == *"arch"* ]]; then
                print_warning "Trying Arch-based installation for $OS"
                sudo pacman -Syu --needed --noconfirm gtk3 xfce4-panel meson ninja pkgconf gcc
            elif [[ "$OS_LIKE" == *"suse"* ]]; then
                print_warning "Trying openSUSE-based installation for $OS"
                sudo zypper install -y gtk3-devel xfce4-panel-devel libxfce4util-devel meson ninja gcc
            else
                print_error "Unsupported distribution: $OS"
                print_info "Please install the following packages manually:"
                print_info "  - GTK+ 3 development files"
                print_info "  - XFCE4 Panel development files"
                print_info "  - libxfce4util development files"
                print_info "  - Meson build system"
                print_info "  - Ninja build tool"
                print_info "  - pkg-config"
                print_info "  - GCC compiler"
                exit 1
            fi
            ;;
    esac
    
    print_success "Dependencies installed successfully"
}

# Check if all required tools are available
check_build_tools() {
    print_info "Checking build tools..."
    
    local missing_tools=()
    
    for tool in meson ninja pkg-config gcc; do
        if ! command -v $tool &> /dev/null; then
            missing_tools+=("$tool")
        fi
    done
    
    if [ ${#missing_tools[@]} -ne 0 ]; then
        print_error "Missing required tools: ${missing_tools[*]}"
        print_info "Please run the dependency installation again"
        exit 1
    fi
    
    print_success "All build tools are available"
}

# Find XFCE4 plugin directory
find_plugin_directory() {
    print_info "Finding XFCE4 plugin directory..."
    
    # Try pkg-config first
    if command -v pkg-config &> /dev/null; then
        PLUGIN_DIR=$(pkg-config --variable=libdir libxfce4panel-2.0 2>/dev/null || true)
        if [ -n "$PLUGIN_DIR" ]; then
            PLUGIN_DIR="$PLUGIN_DIR/xfce4/panel/plugins"
        fi
    fi
    
    # Fallback: search common locations
    if [ -z "$PLUGIN_DIR" ] || [ ! -d "$(dirname "$PLUGIN_DIR")" ]; then
        print_warning "Could not determine plugin directory via pkg-config, searching..."
        
        local search_dirs=(
            "/usr/lib/xfce4/panel/plugins"
            "/usr/lib64/xfce4/panel/plugins"
            "/usr/local/lib/xfce4/panel/plugins"
            "/usr/lib/x86_64-linux-gnu/xfce4/panel/plugins"
            "/usr/lib/i386-linux-gnu/xfce4/panel/plugins"
            "/usr/lib/aarch64-linux-gnu/xfce4/panel/plugins"
        )
        
        for dir in "${search_dirs[@]}"; do
            if [ -d "$dir" ] && [ -n "$(ls -A "$dir" 2>/dev/null)" ]; then
                PLUGIN_DIR="$dir"
                print_info "Found existing plugins in: $dir"
                break
            fi
        done
    fi
    
    if [ -z "$PLUGIN_DIR" ]; then
        print_error "Could not find XFCE4 plugin directory"
        print_info "Please specify the plugin directory manually"
        exit 1
    fi
    
    print_success "Plugin directory: $PLUGIN_DIR"
}

# Build the plugin
build_plugin() {
    print_info "Building MacClock plugin..."
    
    # Clean any existing build
    if [ -d "build" ]; then
        print_info "Cleaning existing build directory"
        rm -rf build
    fi
    
    # Determine the correct prefix
    local prefix="/usr"
    if [[ "$PLUGIN_DIR" == "/usr/local/"* ]]; then
        prefix="/usr/local"
    fi
    
    # Setup build
    print_info "Configuring build with prefix: $prefix"
    meson setup build --prefix="$prefix" --buildtype=release
    
    # Build
    print_info "Compiling plugin"
    ninja -C build
    
    print_success "Build completed successfully"
}

# Install the plugin
install_plugin() {
    print_info "Installing MacClock plugin..."
    
    # Install
    sudo ninja -C build install
    
    # Update icon cache
    print_info "Updating icon cache..."
    sudo gtk-update-icon-cache -q -t -f /usr/share/icons/hicolor 2>/dev/null || true
    if [ -d "/usr/local/share/icons/hicolor" ]; then
        sudo gtk-update-icon-cache -q -t -f /usr/local/share/icons/hicolor 2>/dev/null || true
    fi
    
    print_success "Installation completed successfully"
}

# Main installation function
main() {
    echo "=================================================="
    echo "    XFCE4 MacClock Plugin Installation Script     "
    echo "=================================================="
    echo
    
    # Checks
    check_root
    check_sudo
    detect_os
    
    # Ask user before proceeding
    echo
    read -p "Do you want to install dependencies? [Y/n] " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]] || [[ -z $REPLY ]]; then
        install_dependencies
    else
        print_warning "Skipping dependency installation"
    fi
    
    # Check build tools
    check_build_tools
    
    # Find plugin directory
    find_plugin_directory
    
    # Build and install
    echo
    read -p "Do you want to build and install the plugin? [Y/n] " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]] || [[ -z $REPLY ]]; then
        build_plugin
        install_plugin
        
        echo
        print_success "MacClock plugin has been installed successfully!"
        print_info "To use it:"
        print_info "  1. Right-click on your XFCE panel"
        print_info "  2. Select 'Panel' → 'Add New Items...'"
        print_info "  3. Find 'MacClock' in the list and add it"
        echo
        print_info "You may need to restart the panel with: xfce4-panel -r"
    else
        print_warning "Installation cancelled"
    fi
}

# Run main function
main "$@"

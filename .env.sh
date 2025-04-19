#!/bin/sh

OS_NAME="$(uname)"
ARCH="$(uname -m)"

case "$OS_NAME" in
    Darwin)
        case "$ARCH" in
            arm64)
                export PATH="/opt/homebrew/opt/flex/bin:/opt/homebrew/opt/bison/bin:/opt/homebrew/opt/gperf/bin:$PATH"
                ;;
            x86_64)
                export PATH="/usr/local/opt/flex/bin:/usr/local/opt/bison/bin:/usr/local/opt/gperf/bin:$PATH"
                ;;
            *)
                echo "Unsupported architecture: $ARCH"
                ;;
        esac
        ;;
    Linux)
        echo "Running on Linux"
        # Linux-specific logic here
        ;;
    FreeBSD)
        echo "Running on FreeBSD"
        # FreeBSD-specific logic here
        ;;
    *)
        echo "Unknown OS: $OS_NAME"
        ;;
esac

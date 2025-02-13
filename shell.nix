{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {
  # Define build inputs (dependencies)
  buildInputs = [
    pkgs.gcc                     # Compiler
    pkgs.raylib                  # Raylib library
    pkgs.xorg.libX11             # X11 library
    pkgs.mesa.drivers            # OpenGL library (includes libGL)
    pkgs.zlib                    # Compression library (used by raylib)
  ];

  # Set environment variables
  shellHook = ''
    export CC=gcc
    export CFLAGS="-Wall -I${pkgs.raylib}/include"
    export LIBS="-lraylib -lGL -lm -lpthread -ldl -lrt -lX11"

    # Create necessary directories if they don't exist
    mkdir -p src build bin

    # Provide a helpful message when entering the shell
    echo "Environment ready for building your C project."
    echo "Run 'make' to build the project."
  '';
}
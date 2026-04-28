{ pkgs ? import <nixpkgs> {} }:
let
  pkgs32 = pkgs.pkgsi686Linux;
in
pkgs32.mkShell {
  buildInputs = [ pkgs32.gcc pkgs32.nasm pkgs32.gnumake ];
  shellHook = ''
    export CC="${pkgs32.gcc}/bin/gcc"
    export LD="${pkgs32.binutils}/bin/ld"
    export AS="${pkgs32.nasm}/bin/nasm"
    export AR="${pkgs32.binutils}/bin/ar"
    echo "Active toolchain: $(gcc -dumpmachine)"
  '';
}

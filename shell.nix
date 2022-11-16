{ lib ? import <nixpkgs/lib>
, pkgs ? import <nixpkgs> { }
, mkShell ? pkgs.mkShell
, bashInteractive ? pkgs.bashInteractive
, git
, myshogi ? pkgs.callPackage ./. { }
}:

mkShell {
  packages = [
    bashInteractive
    git
  ];
  inputsFrom = [
    myshogi
  ];
  shellHook = ''
    SHELL=${bashInteractive}/bin/bash; export SHELL
  '';
}

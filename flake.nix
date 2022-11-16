{
  description = "MyShogi, a program to play shogi created for NCU CO2012 Homowork 2 and 3";

  inputs.flake-utils.url = "github:numtide/flake-utils";
  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-21.05";

  outputs = { self, flake-utils, nixpkgs, ... }@inputs:
    flake-utils.lib.eachSystem flake-utils.lib.allSystems (system:
      let
        inherit (nixpkgs) lib;
        pkgs = nixpkgs.legacyPackages.${system} or (import nixpkgs { localSystem = system; });
        myshogi = (pkgs.callPackage ./. { }).overrideAttrs (oldAttrs: {
          version = self.lastModifiedDate;
        });
      in
      {
        packages = {
          inherit myshogi;
          default = myshogi;
        };
        devShell = pkgs.callPackage ./shell.nix {
          inherit myshogi;
        };
      }
    );
}

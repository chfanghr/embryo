{
  description = "embryo: yet another lua compiler/vm";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs";

    utils.url = "github:numtide/flake-utils";
    utils.inputs.nixpkgs.follows = "nixpkgs";
  };

  outputs = { self, nixpkgs, utils, ... }@inputs:
    utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs {
          inherit system;
        };
      in
      {
        devShell = pkgs.mkShell rec {
          name = "embryo";

          packages = with pkgs; [
            # Development Tools
            llvmPackages_11.clang
            cmake
            cmakeCurses
            treefmt
            nixpkgs-fmt
            cmake-format
            luaformatter

            # Development time dependencies
            gtest

            # Build time and Run time dependencies
            fmt
            docopt_cpp
          ];

        };

        defaultPackage = pkgs.callPackage ./default.nix { };
      });
}

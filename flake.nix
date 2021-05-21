{
  description = "Gate controller";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs";
    flake-utils.url = "github:numtide/flake-utils";
  };
  
  outputs = { self, nixpkgs, flake-utils }:
    let
      pkgs = import nixpkgs { system = "x86_64-linux"; };
    in 

      flake-utils.lib.eachDefaultSystem (system: {
        devShell = pkgs.mkShell {
          name = "development-environment";
          buildInputs = [
            pkgs.platformio
          ];
          
          shellHook = ''
          '';     
        };
      });
}

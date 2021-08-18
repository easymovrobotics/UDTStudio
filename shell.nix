with (import (builtins.fetchTarball {
  url =
    "https://github.com/NixOS/nixpkgs/tarball/08970e66abb4c599af819e3db864dc6106c34632";
  sha256 = "sha256:09gzx1n4myl9kai0cf0crjxygccyhrkkzs3gfihfrz69igv3g9xq";
}) { });
mkShell { buildInputs = with qt512; [ just which qmake qtcharts qtserialbus ]; }


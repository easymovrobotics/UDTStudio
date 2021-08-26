{ pkgs ? import <nixpkgs> { } }:
with pkgs;
with qt512;
mkDerivation rec {
  name = "udtstudio";
  src = ./.;
  qmakeFlags = [ "src" ];
  nativeBuildInputs = [ qmake wrapQtAppsHook ];
  buildInputs = [ qtserialbus qtcharts ];
  installPhase = ''
    install -D -t $out/share/eds eds/*
    cd bin
    install -m755 -D -t $out/bin cood udtstudio uds ubl *.sh
    install -D -t $out/lib libudtgui.so* libod.so* libcanopen.so*
    wrapProgram $out/bin/udtstudio --prefix EDS_PATH : $out/share/eds
  '';
}

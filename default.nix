{pkgs ? import <nixpkgs> { }}:
with pkgs;
with qt512;
mkDerivation rec {
  name = "udtstudio";
  src = ./.;
  qmakeFlags = [ "src" ];
  nativeBuildInputs = [ qmake wrapQtAppsHook ];
  buildInputs = [ qtserialbus qtcharts ];
  prePatch = ''
    substituteInPlace src/tools/ubl/main.cpp --replace 'Qt::endl' "endl"
  '';
  installPhase = ''
    cd bin
    install -m755 -D -t $out/bin cood udtstudio uds ubl *.sh
    install -D -t $out/lib libudtgui.so* libod.so* libcanopen.so*
  '';
}

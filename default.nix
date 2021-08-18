with (import <nixpkgs> { });
with qt5;
mkDerivation rec {
  name = "udtstudio";
  src = ./.;
  qmakeFlags = [ "src" ];
  nativeBuildInputs = [ qmake ];
  buildInputs = [ qtserialbus qtcharts ];
  prePatch = ''
    substituteInPlace src/tools/ubl/ubl.pro --replace '/opt/$${TARGET}' "$out"
  '';
}

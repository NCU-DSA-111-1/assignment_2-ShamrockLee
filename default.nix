{ lib
, stdenv
, meson
, ninja
, gnushogi ? null
, libev
}:

stdenv.mkDerivation {
  pname = "myshogi";
  version = "0.1.0";

  src = lib.cleanSource ./.;

  nativeBuildInputs = [
    meson
    ninja
  ];

  buildInputs = [
    gnushogi
    libev
  ];

  meta = with lib; {
    description = "A shogi program";
    platform = platforms.all;
    maintainers = with maintainers; [ ShamrockLee ];
    mainProgram = "myshogi";
  };
}

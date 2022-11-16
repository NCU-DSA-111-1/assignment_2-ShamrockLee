{ lib
, stdenv
, meson
, ninja
, gnushogi ? null
, libev
, enableBuiltinGnushogi ? false
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
    libev
  ]
  ++ lib.optional (!enableBuiltinGnushogi) gnushogi
  ;

  meta = with lib; {
    description = "A shogi program";
    platform = platforms.all;
    maintainers = with maintainers; [ ShamrockLee ];
  };
}

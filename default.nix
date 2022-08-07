{ lib
, llvmPackages_14
, cmake
, spdlog
, abseil-cpp
}:

llvmPackages_14.stdenv.mkDerivation rec {
  pname = "embryo";
  version = "0.1.0";

  src = ./.;

  nativeBuildInputs = [ cmake ];
  buildInputs = [ fmt docopt_cpp ];

  cmakeFlags = [
    "-DENABLE_TESTING=OFF"
    "-DENABLE_INSTALL=ON"
  ];

  meta = with lib; {
    homepage = "https://github.com/chfanghr/embryo";
    description = ''
      Yet another lua compiler/vm, written in C++17.";
    '';
    licencse = licenses.mit;
    platforms = with platforms; linux ++ darwin;
  };
}

#!/bin/bash

set -e

BLUE='\033[0;34m'
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m'

# Carpeta donde está el script (la raíz del repo)
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

echo -e "${BLUE}Instalador del TP Need For Speed${NC}"
echo -e "${BLUE}Preparando entorno...${NC}"

# ---------------------------------------------------------------------
# 1) Instalar dependencias del sistema
# ---------------------------------------------------------------------

sudo apt-get update

# Herramientas de compilación y git
sudo apt-get install -y \
  build-essential \
  cmake \
  pkg-config \
  make \
  gcc \
  g++ \
  git

# Qt5 (Core + Widgets)
sudo apt-get install -y \
  qtbase5-dev \
  qtchooser \
  qt5-qmake \
  qtbase5-dev-tools

# SDL2 y add-ons
sudo apt-get install -y \
  libsdl2-dev \
  libsdl2-image-dev \
  libsdl2-ttf-dev \
  libsdl2-mixer-dev

# Box2D
sudo apt-get install -y \
  libbox2d-dev

# yaml-cpp
sudo apt-get install -y \
  libyaml-cpp-dev

# FreeType + texto
sudo apt-get install -y \
  libfreetype6-dev \
  libharfbuzz-dev \
  libgraphite2-dev

# Audio codecs y formatos
sudo apt-get install -y \
  libpulse-dev \
  libopusfile-dev \
  libmodplug-dev \
  libsndfile1-dev \
  libmpg123-dev \
  libmp3lame-dev \
  libogg-dev \
  libvorbis-dev \
  libopus-dev

# Sistema y red
sudo apt-get install -y \
  libdbus-1-dev \
  libglib2.0-dev

# Compresión
sudo apt-get install -y \
  libbz2-dev \
  libzstd-dev \
  liblzma-dev \
  liblz4-dev

# OpenGL y X11
sudo apt-get install -y \
  libx11-dev \
  libxext-dev \
  libgl1-mesa-dev \
  libglu1-mesa-dev

sudo apt-get install -y \
  build-essential cmake git \
  libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev libsdl2-mixer-dev \
  libgl1-mesa-dev


echo -e "${GREEN}Dependencias de sistema instaladas.${NC}"

echo "Instalando SDL2pp 0.8.0 con soporte de imagen..."

TMP_DIR="$(mktemp -d /tmp/libsdl2pp-XXXXXX)"
cd "$TMP_DIR"

# Clonar release 0.8.0 (la que viste en GitHub)
git clone --branch 0.8.0 https://github.com/libSDL2pp/libSDL2pp.git
cd libSDL2pp

mkdir -p build
cd build

cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DSDL2PP_WITH_IMAGE=ON \
  -DSDL2PP_WITH_TTF=ON \
  -DSDL2PP_WITH_MIXER=OFF \
  -DCMAKE_INSTALL_PREFIX=/usr/local

cmake --build . --config Release -j"$(nproc)"
sudo cmake --install .

cd ~
rm -rf "$TMP_DIR"



# ---------------------------------------------------------------------
# 3) Usar SIEMPRE ./source como carpeta de código
# ---------------------------------------------------------------------

INSTALL_DIR="$SCRIPT_DIR/need-for-speed-tp"
if [ ! -d "$INSTALL_DIR" ]; then
    echo -e "${BLUE}Clonando rama 'entrega' en ${INSTALL_DIR}...${NC}"
    git clone --branch entrega https://github.com/Martinonervi/TpFinalNeedForSpeed.git "$INSTALL_DIR"
fi


SRC_DIR="$INSTALL_DIR/source"

if [ ! -f "$SRC_DIR/CMakeLists.txt" ]; then
    echo -e "${RED}No se encontró CMakeLists.txt en la raíz del repo (${SRC_DIR}).${NC}"
    exit 1
fi

echo -e "${BLUE}Usando carpeta de código fuente: ${SRC_DIR}${NC}"
BUILD_DIR="$SRC_DIR/build-installer"


# ---------------------------------------------------------------------
# 4) Compilar tu proyecto con CMake
# ---------------------------------------------------------------------

echo -e "${BLUE}Compilando cliente y servidor...${NC}"

rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"

cmake -S "$SRC_DIR" -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Release
cmake --build "$BUILD_DIR" --config Release -j"$(nproc)"

CLIENT_BIN="$BUILD_DIR/client"
SERVER_BIN="$BUILD_DIR/server"

if [ ! -x "$CLIENT_BIN" ]; then
  echo -e "${RED}No se encontró el ejecutable 'client' en $BUILD_DIR.${NC}"
  exit 1
fi

if [ ! -x "$SERVER_BIN" ]; then
  echo -e "${RED}No se encontró el ejecutable 'server' en $BUILD_DIR.${NC}"
  exit 1
fi

echo -e "${GREEN}Compilación terminada.${NC}"

# ---------------------------------------------------------------------
# 5) Crear carpeta de instalación y copiar todo ahí
# ---------------------------------------------------------------------

INSTALL_DIR="/opt/need-for-speed-tp"

echo -e "${BLUE}Instalando en ${INSTALL_DIR}...${NC}"

sudo rm -rf "$INSTALL_DIR"
sudo mkdir -p "$INSTALL_DIR"

# Binarios
sudo mkdir -p "$INSTALL_DIR/bin"
sudo cp "$CLIENT_BIN" "$INSTALL_DIR/bin/nfs-client"
sudo cp "$SERVER_BIN" "$INSTALL_DIR/bin/nfs-server"
sudo chmod +x "$INSTALL_DIR/bin/nfs-client" "$INSTALL_DIR/bin/nfs-server"

# Código fuente / recursos
sudo mkdir -p "$INSTALL_DIR/source"
sudo cp -r "$SRC_DIR"/* "$INSTALL_DIR/source/"

# Assets, maps, settings.yaml si existen en la raíz del repo
if [ -d "$SCRIPT_DIR/assets" ]; then
  sudo mkdir -p "$INSTALL_DIR/assets"
  sudo cp -r "$SCRIPT_DIR/assets/"* "$INSTALL_DIR/assets/"
fi

if [ -d "$SCRIPT_DIR/maps" ]; then
  sudo mkdir -p "$INSTALL_DIR/maps"
  sudo cp -r "$SCRIPT_DIR/maps/"* "$INSTALL_DIR/maps/"
fi

if [ -f "$SCRIPT_DIR/settings.yaml" ]; then
  sudo cp "$SCRIPT_DIR/settings.yaml" "$INSTALL_DIR/settings.yaml"
fi

# ---------------------------------------------------------------------
# 6) Crear symlinks en /usr/local/bin
# ---------------------------------------------------------------------

echo -e "${BLUE}Creando accesos rápidos en /usr/local/bin...${NC}"

sudo ln -sf "$INSTALL_DIR/bin/nfs-client" /usr/local/bin/nfs-client
sudo ln -sf "$INSTALL_DIR/bin/nfs-server" /usr/local/bin/nfs-server

# ---------------------------------------------------------------------
# 7) Verificación básica de dependencias de los binarios
# ---------------------------------------------------------------------

echo -e "${RED}Verificando dependencias faltantes del cliente...${NC}"
ldd "$INSTALL_DIR/bin/nfs-client" | grep "not found" || echo "Cliente OK"

echo -e "${RED}Verificando dependencias faltantes del servidor...${NC}"
ldd "$INSTALL_DIR/bin/nfs-server" | grep "not found" || echo "Servidor OK"

echo -e "${GREEN}Instalación completa.${NC}"
echo -e "${GREEN}Podés ejecutar:${NC}"
echo -e "  ${GREEN}nfs-client${NC}  (cliente)"
echo -e "  ${GREEN}nfs-server${NC}  (servidor)"

echo -e "  ${GREEN}EJECUTANDO PRUEBAS${NC}"

cd ${INSTALL_DIR}/source/build-installer/


./protocol_tests

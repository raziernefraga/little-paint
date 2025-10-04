#!/bin/bash
set -e

echo "=== Atualizando pacotes ==="
sudo apt update

echo "=== Instalando dependências mínimas (g++, cmake, Qt Base) ==="
sudo apt install -y build-essential cmake qtbase5-dev

echo "=== Baixando linuxdeployqt.AppImage ==="
mkdir -p ~/AppImages
cd ~/AppImages
if [ ! -f linuxdeployqt-continuous-x86_64.AppImage ]; then
  wget https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage
  chmod +x linuxdeployqt-continuous-x86_64.AppImage
fi

echo "=== Compilando projeto ==="
cd -
mkdir -p build
cd build
cmake ..
make -j"$(nproc)"

echo "=== Gerando AppImage ==="
~/AppImages/linuxdeployqt-continuous-x86_64.AppImage ./LitePaint -appimage

echo "=== Pronto! Arquivo LitePaint-x86_64.AppImage gerado em $(pwd) ==="

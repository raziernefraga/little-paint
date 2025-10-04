#!/bin/bash
set -e

echo "=== Atualizando pacotes ==="
sudo apt update

echo "=== Instalando dependências (compilador + Qt + CMake) ==="
sudo apt install -y build-essential cmake qtbase5-dev qtchooser qt5-qmake qtbase5-dev-tools

echo "=== Criando pasta de build ==="
mkdir -p build
cd build

echo "=== Configurando projeto com CMake ==="
cmake ..

echo "=== Compilando com make ==="
make -j"$(nproc)"

echo "=== Pronto! Executável gerado em ./build/LitePaint ==="
echo "Para rodar, use: ./LitePaint"

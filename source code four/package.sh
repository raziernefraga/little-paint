#!/bin/bash

APP_NAME="LittlePaint"
BUILD_DIR="build"
APPDIR="$APP_NAME.AppDir"

# Verifica e instala linuxdeploy se necessário
if ! command -v linuxdeploy &> /dev/null; then
    echo "🔧 Instalando linuxdeploy..."
    wget https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage -O linuxdeploy
    chmod +x linuxdeploy
    sudo mv linuxdeploy /usr/local/bin/
fi

# Verifica e instala o plugin Qt se necessário
if ! command -v linuxdeploy-plugin-qt &> /dev/null; then
    echo "🔧 Instalando linuxdeploy-plugin-qt..."
    wget https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-x86_64.AppImage -O linuxdeploy-plugin-qt
    chmod +x linuxdeploy-plugin-qt
    sudo mv linuxdeploy-plugin-qt /usr/local/bin/
fi

# Limpa e recompila
rm -rf "$BUILD_DIR" "$APPDIR"
mkdir "$BUILD_DIR"
cd "$BUILD_DIR"

echo "🚧 Compilando com CMake..."
cmake ..
make -j$(nproc)

# Cria estrutura AppDir
mkdir -p "$APPDIR/usr/bin"
cp "$APP_NAME" "$APPDIR/usr/bin/"

# Cria .desktop mínimo para o plugin Qt funcionar
mkdir -p "$APPDIR/usr/share/applications"
cat <<EOF > "$APPDIR/usr/share/applications/$APP_NAME.desktop"
[Desktop Entry]
Name=$APP_NAME
Exec=$APP_NAME
Type=Application
Categories=Graphics;
EOF

# Gera AppImage
echo "📦 Empacotando com linuxdeploy..."
linuxdeploy --appdir "$APPDIR" --output appimage --plugin qt

echo "✅ AppImage gerado com sucesso!"

#!/bin/bash

APP_NAME="LittlePaint"
BUILD_DIR="build"
APPDIR="$APP_NAME.AppDir"
ICON_SOURCE="littlepaint.png"  # ícone genérico obrigatório

# 🔧 Verifica e instala linuxdeploy
if ! command -v linuxdeploy &> /dev/null; then
    echo "🔧 Instalando linuxdeploy..."
    wget https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage -O linuxdeploy
    chmod +x linuxdeploy
    sudo mv linuxdeploy /usr/local/bin/
fi

# 🔧 Verifica e instala plugin Qt
if ! command -v linuxdeploy-plugin-qt &> /dev/null; then
    echo "🔧 Instalando plugin Qt..."
    wget https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-x86_64.AppImage -O linuxdeploy-plugin-qt
    chmod +x linuxdeploy-plugin-qt
    sudo mv linuxdeploy-plugin-qt /usr/local/bin/
fi

# 🚧 Limpa e recompila
rm -rf "$BUILD_DIR" "$APPDIR"
mkdir "$BUILD_DIR"
cd "$BUILD_DIR"

echo "🚧 Compilando com CMake..."
cmake ..
make -j$(nproc)

# ✅ Verifica se binário foi gerado
if [ ! -f "$APP_NAME" ]; then
    echo "❌ Erro: binário '$APP_NAME' não foi gerado."
    exit 1
fi

# 📁 Cria estrutura AppDir
mkdir -p "$APPDIR/usr/bin"
cp "$APP_NAME" "$APPDIR/usr/bin/"
chmod +x "$APPDIR/usr/bin/$APP_NAME"

# 🖼️ Adiciona ícone genérico obrigatório
mkdir -p "$APPDIR/usr/share/icons/hicolor/128x128/apps"
if [ ! -f "../$ICON_SOURCE" ]; then
    echo "❌ Erro: ícone '$ICON_SOURCE' não encontrado no diretório raiz."
    exit 1
fi
cp "../$ICON_SOURCE" "$APPDIR/usr/share/icons/hicolor/128x128/apps/littlepaint.png"

# 📝 Cria .desktop funcional
mkdir -p "$APPDIR/usr/share/applications"
cat <<EOF > "$APPDIR/usr/share/applications/$APP_NAME.desktop"
[Desktop Entry]
Type=Application
Name=$APP_NAME
Exec=$APP_NAME
Icon=littlepaint
Categories=Graphics;
Terminal=false
StartupNotify=true
EOF

# ✅ Verifica se .desktop foi criado
if [ ! -f "$APPDIR/usr/share/applications/$APP_NAME.desktop" ]; then
    echo "❌ Erro: arquivo .desktop não foi criado."
    exit 1
fi

# 📦 Empacota com linuxdeploy
echo "📦 Empacotando com linuxdeploy..."
linuxdeploy --appdir "$APPDIR" --output appimage --plugin qt

# ✅ Verifica se AppImage foi gerado
cd ..
if [ -f "$APP_NAME"-x86_64.AppImage ]; then
    echo "✅ AppImage gerado com sucesso: $APP_NAME-x86_64.AppImage"
else
    echo "❌ Erro: AppImage não foi gerado."
    exit 1
fi

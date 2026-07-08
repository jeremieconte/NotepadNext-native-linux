# Build natif — Ubuntu 26.04 LTS

Ce dépôt est une **réimplémentation native Linux de Notepad++** : *NotepadNext*
(https://github.com/dail8859/NotepadNext), bâtie sur le même moteur d'édition
Scintilla/Lexilla que Notepad++, mais avec une interface Qt6 native (pas de
Win32, pas de Wine). Le code C++ Win32 d'origine de Notepad++ n'est **pas**
portable par simple recompilation ; cette base fournit l'équivalent natif.

Version construite : upstream commit `6354f66`, application v0.14, contre
**Qt 6.10.2** sur **Ubuntu 26.04 LTS**.

## Dépendances de build

```bash
sudo apt install \
    build-essential cmake \
    qt6-base-dev qt6-base-private-dev qt6-base-dev-tools \
    qt6-5compat-dev qt6-tools-dev qt6-tools-dev-tools linguist-qt6 \
    libxkbcommon-dev libxcb1-dev pkgconf
```

Les dépendances tierces suivantes sont récupérées automatiquement par **CPM**
au moment du `cmake` configure (connexion Internet requise une fois) :
Qt-Advanced-Docking-System, QSimpleUpdater, SingleApplication,
editorconfig-core-qt. Les composants Lua, Scintilla, Lexilla et uchardet sont
déjà vendorés dans `thirdparty/`.

## Compilation

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j"$(nproc)"
./build/src/NotepadNext
```

Le binaire est produit dans `build/src/NotepadNext`. Options de durcissement
déjà activées par le `CMakeLists.txt` racine sous Linux : `RELRO`, `-z now`,
`-fstack-protector-strong`, `-D_FORTIFY_SOURCE=2`.

## Installation / packaging

Le fichier `cmake/PackagingLinux.cmake` définit les règles `install`
(binaire + `.desktop` + icônes). Pour obtenir l'arborescence d'installation :

```bash
cmake --install build --prefix pkgroot/usr
```

Un paquet `.deb` peut ensuite être assemblé à partir de `pkgroot/usr`
(voir `notepadnext-native_0.14.0_amd64.deb` fourni à côté de cette archive).

## Notes de portage / contournements appliqués

Ces points ne concernent que des environnements de build **sans droits root**
ou aux paquets Qt6 incomplets ; sur un système où les paquets ci-dessus sont
installés normalement, la compilation fonctionne sans modification.

1. **Qt-Advanced-Docking-System ↔ `Qt6::GuiPrivate`** — sur Qt ≥ 6.9, ADS lie
   `Qt6::GuiPrivate` pour `<qpa/qplatformnativeinterface.h>` (barre de titre
   flottante X11). Cette cible exige le paquet `qt6-base-private-dev`. S'il est
   absent, la cible importée pointe vers un chemin `/usr/include/.../QtGui/<ver>`
   inexistant et CMake échoue. Solution propre : installer `qt6-base-private-dev`.

2. **XKB / pkg-config** — `Qt6GuiPrivate` dépend de `xkbcommon` via `pkg-config`.
   Installer `libxkbcommon-dev` et `pkgconf`.

Aucune modification du code source de l'application n'est nécessaire : le port
Linux est assuré nativement par NotepadNext lui-même.

## Correctif inclus : crash intermittent à la fermeture (Qt Wayland)

Ce build corrige un **use-after-free** dans `src/docks/DebugLogDock.cpp`. Le
gestionnaire de messages de log (`debugLogDockMessageHandler`) conservait un
pointeur statique `output` vers le `QPlainTextEdit` du dock ; ce pointeur
n'était pas remis à zéro dans `~DebugLogDock()`, et le gestionnaire restait
installé. Un message de log émis tardivement (par le plugin Qt Wayland pendant
l'arrêt, après destruction du dock) déréférençait alors le widget libéré,
provoquant un segfault intermittent (~40 % des fermetures).

Correctif : remise à `nullptr` de `output` dans `~DebugLogDock()` (avant
`delete ui`) et null-check dans le gestionnaire. Validé : 0 crash sur 20
fermetures Wayland (contre ~40 % avant correctif). Backtrace d'origine :
`appendPlainText` → `QMetaObject::cast` sur objet détruit → saut vers 0xffffffff.

## Fonctionnalité : thème Clair / Sombre / Système (compatible GNOME 50)

Préférences → « Theme » propose trois modes :

- **System** : suit la préférence de style du bureau. Sur GNOME 50, la valeur est
  lue directement via le portail freedesktop `org.freedesktop.appearance`
  (`color-scheme`), et l'application se met à jour **en direct** quand on bascule
  le « Style sombre » de GNOME (signal `SettingChanged` du portail). Un repli sur
  `QStyleHints::colorScheme()` est prévu si le portail est absent.
- **Light** / **Dark** : force le thème indépendamment du bureau.

Le thème pilote simultanément : la palette native Qt (menus, dialogues, barres,
fenêtre de préférences), une feuille de style dédiée pour les onglets/docks
(`stylesheets/npp-dark.css`), et les couleurs de l'éditeur Scintilla
(fond/texte/numéros de ligne/caret/marges). Dépendance ajoutée : `Qt6::DBus`
(paquet `qt6-base-dev` fournit déjà les en-têtes ; runtime `libqt6dbus6`).

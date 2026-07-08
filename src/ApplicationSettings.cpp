/*
 * This file is part of Notepad Next.
 * Copyright 2024 Justin Dailey
 *
 * Notepad Next is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Notepad Next is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Notepad Next.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "ApplicationSettings.h"

#include <QApplication>
#include <QFont>
#include <QGuiApplication>
#include <QStyleHints>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusVariant>

#define CREATE_SETTING(group, name, lname, type, default) \
ApplicationSetting<type> name{#group "/" #name, default};\
    type ApplicationSettings::lname() const\
{\
        return get(name);\
}\
    void ApplicationSettings::set##name(type lname)\
{\
        set(name, lname);\
        emit lname##Changed(lname);\
}



ApplicationSettings::ApplicationSettings(QObject *parent)
    : QSettings{parent}
{
}

// Reads the desktop's dark-style preference straight from the freedesktop
// appearance portal (org.freedesktop.appearance / color-scheme). This is the
// canonical GNOME 50 signal and works even when Qt's own platform theme does
// not surface it via QStyleHints. color-scheme values: 0 = no preference,
// 1 = prefer dark, 2 = prefer light. Returns false (and sets *ok=false) if the
// portal is unavailable.
static bool queryPortalPrefersDark(bool *ok)
{
    *ok = false;

    if (!QDBusConnection::sessionBus().isConnected())
        return false;

    QDBusMessage call = QDBusMessage::createMethodCall(
        QStringLiteral("org.freedesktop.portal.Desktop"),
        QStringLiteral("/org/freedesktop/portal/desktop"),
        QStringLiteral("org.freedesktop.portal.Settings"),
        QStringLiteral("Read"));
    call << QStringLiteral("org.freedesktop.appearance") << QStringLiteral("color-scheme");

    const QDBusMessage reply = QDBusConnection::sessionBus().call(call, QDBus::Block, 300);
    if (reply.type() != QDBusMessage::ReplyMessage || reply.arguments().isEmpty())
        return false;

    // The reply is a variant, possibly nested (variant-in-variant); unwrap it.
    QVariant v = reply.arguments().constFirst();
    while (v.canConvert<QDBusVariant>())
        v = v.value<QDBusVariant>().variant();

    bool converted = false;
    const uint scheme = v.toUInt(&converted);
    if (!converted)
        return false;

    *ok = true;
    return scheme == 1; // prefer dark
}

bool ApplicationSettings::isDarkMode() const
{
    switch (colorScheme()) {
    case DarkColorScheme:
        return true;
    case LightColorScheme:
        return false;
    case SystemColorScheme:
    default: {
        // Follow the desktop preference. On GNOME 50 this reflects the system
        // "Dark Style" toggle. Prefer the freedesktop portal (authoritative),
        // and fall back to Qt's own detection if the portal is unavailable.
        bool ok = false;
        const bool portalDark = queryPortalPrefersDark(&ok);
        if (ok)
            return portalDark;
        return QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark;
    }
    }
}

CREATE_SETTING(Gui, ColorScheme, colorScheme, ApplicationSettings::ColorSchemeEnum, ApplicationSettings::SystemColorScheme)

CREATE_SETTING(Gui, ShowMenuBar, showMenuBar, bool, true)
CREATE_SETTING(Gui, ShowToolBar, showToolBar, bool, true)
CREATE_SETTING(Gui, ShowTabBar, showTabBar, bool, true)
CREATE_SETTING(Gui, ShowStatusBar, showStatusBar, bool, true)
CREATE_SETTING(Gui, CenterSearchDialog, centerSearchDialog, bool, true)

CREATE_SETTING(Gui, TabsClosable, tabsClosable, bool, true)
CREATE_SETTING(Gui, ExitOnLastTabClosed, exitOnLastTabClosed, bool, false)

CREATE_SETTING(Gui, CombineSearchResults, combineSearchResults, bool, false)

CREATE_SETTING(App, RestorePreviousSession, restorePreviousSession, bool, false)
CREATE_SETTING(App, RestoreUnsavedFiles, restoreUnsavedFiles, bool, false)
CREATE_SETTING(App, RestoreTempFiles, restoreTempFiles, bool, false)

CREATE_SETTING(App, DefaultDirectoryBehavior, defaultDirectoryBehavior, ApplicationSettings::DefaultDirectoryBehaviorEnum, ApplicationSettings::FollowCurrentDocument)
CREATE_SETTING(App, DefaultDirectory, defaultDirectory, QString, QString())

CREATE_SETTING(App, Translation, translation, QString, QStringLiteral(""))

CREATE_SETTING(Editor, ShowWhitespace, showWhitespace, bool, false);
CREATE_SETTING(Editor, ShowEndOfLine, showEndOfLine, bool, false);
CREATE_SETTING(Editor, ShowWrapSymbol, showWrapSymbol, bool, false);
CREATE_SETTING(Editor, ShowIndentGuide, showIndentGuide, bool, true);
CREATE_SETTING(Editor, WordWrap, wordWrap, bool, false)
CREATE_SETTING(Editor, FontName, fontName, QString, QStringLiteral("Courier New"))
CREATE_SETTING(Editor, FontSize, fontSize, int, []() { return qApp->font().pointSize() + 2; })
CREATE_SETTING(Editor, AdditionalWordChars, additionalWordChars, QString, QStringLiteral(""));
CREATE_SETTING(Editor, DefaultEOLMode, defaultEOLMode, QString, QStringLiteral(""))
CREATE_SETTING(Editor, URLHighlighting, urlHighlighting, bool, true)
CREATE_SETTING(Editor, ShowLineNumbers, showLineNumbers, bool, true)
CREATE_SETTING(Editor, AutoCompletion, autoCompletion, bool, true)

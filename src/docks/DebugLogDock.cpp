/*
 * This file is part of Notepad Next.
 * Copyright 2022 Justin Dailey
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


#include "DebugLogDock.h"
#include "ui_DebugLogDock.h"
#include "DebugManager.h"

#include <QScrollBar>

static QPlainTextEdit *output = Q_NULLPTR;

static void debugLogDockMessageHandler(const QString &msg)
{
    // 'output' points into the dock's UI and is cleared in ~DebugLogDock().
    // Late log messages (e.g. emitted by the Qt platform plugin during
    // application shutdown, after the dock has been destroyed) must not
    // dereference the now-freed widget -- see the use-after-free fix below.
    if (output)
        output->appendPlainText(msg);
}

DebugLogDock::DebugLogDock(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::DebugLogDock)
{
    ui->setupUi(this);

    output = ui->txtDebugOutput;
    DebugManager::addMessageHandler(debugLogDockMessageHandler);

    connect(this, &QDockWidget::visibilityChanged, this, [=](bool visible) {
        if (visible) {
            ui->txtDebugOutput->horizontalScrollBar()->setValue(0);
        }
    });
}

DebugLogDock::~DebugLogDock()
{
    // Clear the static back-pointer *before* the UI (and its txtDebugOutput)
    // is destroyed, so any log message delivered during/after teardown is
    // dropped by debugLogDockMessageHandler() instead of writing into freed
    // memory. Fixes an intermittent shutdown segfault on Qt Wayland.
    output = Q_NULLPTR;

    delete ui;
}

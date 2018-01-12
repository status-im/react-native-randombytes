/**
 * Copyright (C) 2016, Canonical Ltd.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree. An additional grant
 * of patent rights can be found in the PATENTS file in the same directory.
 *
 * Author: Justin McPherson <justin.mcpherson@canonical.com>
 *
 */

#include <memory>

#include "bridge.h"
#include "rnrandombytesdesktop.h"
#include <QCryptographicHash>
#include <QDateTime>
#include <QDebug>
#include <QMap>
#include <QNetworkDiskCache>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QQuickImageProvider>

namespace {
struct RegisterQMLMetaType {
    RegisterQMLMetaType() {
        qRegisterMetaType<RNRandomBytes*>();
    }
} registerMetaType;
} // namespace

class RNRandomBytesPrivate {

public:
    RNRandomBytesPrivate() {
        seed = generateRandomBytes(4096);
    }

    QString generateRandomBytes(int size);

    QString seed;
    Bridge* bridge = nullptr;
};

RNRandomBytes::RNRandomBytes(QObject* parent) : QObject(parent), d_ptr(new RNRandomBytesPrivate) {}

RNRandomBytes::~RNRandomBytes() {}

QString RNRandomBytesPrivate::generateRandomBytes(int size) {
    QString randomString;
    qsrand(QDateTime::currentDateTime().currentMSecsSinceEpoch());
    while (randomString.size() < size) {
        randomString.append(QChar(qrand()));
    }
    return randomString.toUtf8().toBase64();
}

void RNRandomBytes::randomBytes(int length, double funcId) {
    Q_D(RNRandomBytes);
    if (d->bridge) {
        d->bridge->invokePromiseCallback(funcId, QVariantList{"", d->generateRandomBytes(length)});
    }
}

QString RNRandomBytes::moduleName() {
    return "RNRandomBytes";
}

QList<ModuleMethod*> RNRandomBytes::methodsToExport() {
    return QList<ModuleMethod*>{};
}

QVariantMap RNRandomBytes::constantsToExport() {
    return QVariantMap{{"seed", d_ptr->seed}};
}

void RNRandomBytes::setBridge(Bridge* bridge) {
    Q_D(RNRandomBytes);
    d->bridge = bridge;
}

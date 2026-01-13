#include <QSettings>
#include <QtCore/qdebug.h>
#include <ntcore.h>
#include "NetworkTables.h"
#include "ntcore_cpp.h"

void NetworkTables::UpdateSettings(QVariant address, uint port) {
    QSettings settings;
    settings.setValue("ntAddress", address);
    settings.setValue("ntPort", port);
    RefreshServer();
    // nt::SetServer(nt::GetDefaultInstance(), address.toByteArray(), port);
}

void NetworkTables::RefreshServer() {
    nt::Disconnect(nt::GetDefaultInstance());

    QSettings settings;
    nt::SetServer(
        nt::GetDefaultInstance(),
        settings.value("ntAddress").toByteArray(),
        settings.value("ntPort").toUInt()
    );

    // qDebug()<<"Address: "<<settings.value("ntAddress").toByteArray();
    // qDebug()<<"Port: "<<settings.value("ntPort").toByteArray();
}

void NetworkTables::StartClient() {
    RefreshServer();
    nt::StartClient4(nt::GetDefaultInstance(), "QuickStatus2");
}

NetworkTables::NetworkTables() {
    // RefreshServer();
}

NetworkTables::~NetworkTables() {
    nt::StopClient(nt::GetDefaultInstance());
}
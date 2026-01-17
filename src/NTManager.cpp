#include <QSettings>
#include <ntcore.h>
#include "NTManager.h"

void NTManager::UpdateSettings(QVariant address, uint port) {
    QSettings settings;
    settings.setValue("ntAddress", address);
    settings.setValue("ntPort", port);
    RefreshServer();
}

void NTManager::RefreshServer() {
    nt::Disconnect(nt::GetDefaultInstance());
    QSettings settings;
    
    nt::SetServer(
        nt::GetDefaultInstance(),
        settings.value("ntAddress").toByteArray(),
        settings.value("ntPort").toUInt()
    );
}

void NTManager::StartClient() {
    RefreshServer();
    nt::StartClient4(nt::GetDefaultInstance(), "QuickStatus2");
}

NTManager::NTManager() {
    // RefreshServer();
}

NTManager::~NTManager() {
    nt::StopClient(nt::GetDefaultInstance());
}
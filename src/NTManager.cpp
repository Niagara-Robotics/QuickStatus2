#include <QSettings>
#include <ntcore.h>
#include "NTManager.h"

void NTManager::UpdateSettings(QVariant address, uint port) {
    QSettings settings;
    settings.setValue("ntAddress", address);
    settings.setValue("ntPort", port);
    RefreshServer();
}

void checkSettings() {
    QSettings settings;

    if (!settings.contains("ntAddress")) settings.setValue("ntAddress", "localhost");
    if (!settings.contains("ntPort")) settings.setValue("ntPort", "5810");
}

void NTManager::RefreshServer() {
    nt::Disconnect(nt::GetDefaultInstance());

    checkSettings();

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
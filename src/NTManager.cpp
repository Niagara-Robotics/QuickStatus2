#include <QSettings>
#include <ntcore.h>
#include "NTManager.h"

void NTManager::UpdateSettings(QVariant address, uint port) {
    QSettings settings;
    auto type = settings.value("ntType").toString();
    if (type == "Address") settings.setValue("ntAddress", address);
    if (type == "Team Number") settings.setValue("ntTeam", address);
    settings.setValue("ntPort", port);
    RefreshServer();
}

void NTManager::checkSettings() {
    QSettings settings;
    if (!settings.contains("ntAddress")) settings.setValue("ntAddress", "localhost");
    if (!settings.contains("ntTeam")) settings.setValue("ntTeam", "6978");
    if (!settings.contains("ntPort")) settings.setValue("ntPort", "5810");
    if (!settings.contains("ntType")) settings.setValue("ntType", "Address");
}

void NTManager::RefreshServer() {
    NT_Inst inst = nt::GetDefaultInstance();

    nt::Disconnect(nt::GetDefaultInstance());

    checkSettings();
    QSettings settings;
    std::string teamStr = settings.value("ntTeam").toString().toStdString();
    std::stringstream ss(teamStr);
    auto type = settings.value("ntType").toString();

    uint teamNum;
    ss >> teamNum;
    if (!ss.fail() && type == "Team Number") {
        //team number
        // qDebug()<<"yo"<<teamNum;
        nt::SetServerTeam(
            nt::GetDefaultInstance(), 
            teamNum,
            settings.value("ntPort").toUInt()
        );
    } else {
        nt::SetServer(
            nt::GetDefaultInstance(),
            settings.value("ntAddress").toString().toStdString(),
            settings.value("ntPort").toUInt()
        );
    }
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
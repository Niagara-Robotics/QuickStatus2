#include <QSettings>
#include "NTManager.h"

void NTManager::updateSettings(QVariant address, uint port) {
    QSettings settings;
    auto type = settings.value("ntType").toString();
    if (type == "Address") settings.setValue("ntAddress", address);
    if (type == "Team Number") settings.setValue("ntTeam", address);
    settings.setValue("ntPort", port);
    refreshServer();
}

void NTManager::checkSettings() {
    QSettings settings;
    if (!settings.contains("ntAddress")) settings.setValue("ntAddress", "localhost");
    if (!settings.contains("ntTeam")) settings.setValue("ntTeam", "6978");
    if (!settings.contains("ntPort")) settings.setValue("ntPort", "5810");
    if (!settings.contains("ntType")) settings.setValue("ntType", "Address");
}

void NTManager::refreshServer() {
    inst.Disconnect();

    checkSettings();
    QSettings settings;
    std::string teamStr = settings.value("ntTeam").toString().toStdString();
    std::stringstream ss(teamStr);
    auto type = settings.value("ntType").toString();

    uint teamNum;
    ss >> teamNum;

    if (!ss.fail() && type == "Team Number") {
        std::string rioIP = 
            "10." + std::to_string(teamNum / 100) + "." + 
            std::to_string(teamNum % 100) + ".2";
        inst.SetServer(
            rioIP.c_str(),
            settings.value("ntPort").toUInt()
        );

    } else {
        inst.SetServer(
            settings.value("ntAddress").toString().toStdString(),
            settings.value("ntPort").toUInt()
        );
    }
}

void NTManager::startClient() {
    refreshServer();
    inst.StartClient4("QuickStatus2");
}

NTManager::NTManager() {
    // RefreshServer();
}

NTManager::~NTManager() {
    inst.StopClient();
}
#pragma once

#include "networktables/NetworkTableInstance.h"
#include <QWidget>

class NTManager {
public:

    explicit NTManager();
    ~NTManager();
    static void updateSettings(QVariant address, uint port);
    static void checkSettings();
    static void startClient();
    static void test();
    
private:
    static void refreshServer();
    inline static nt::NetworkTableInstance inst = nt::NetworkTableInstance::GetDefault();
};
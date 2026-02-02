#pragma once

#include <QWidget>

class NTManager {
public:

    explicit NTManager();
    ~NTManager();
    static void UpdateSettings(QVariant address, uint port);
    static void checkSettings();
    static void StartClient();
    static void test();
    
private:
    static void RefreshServer();
};
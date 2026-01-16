#pragma once

#include <QWidget>

class NetworkTables {
public:

    explicit NetworkTables();
    ~NetworkTables();
    static void UpdateSettings(QVariant address, uint port);
    static void StartClient();
    static void test();
    
private:
    static void RefreshServer();
};
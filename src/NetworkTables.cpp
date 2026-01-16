#include <QSettings>
#include <ntcore.h>
#include "NetworkTables.h"
#include <frc/Timer.h>

void NetworkTables::test() {
    NT_Inst inst = nt::GetDefaultInstance();
    NT_Subscriber ySub = nt::Subscribe(nt::GetTopic(
        inst, "/QuickStatus2/RobotState"), NT_STRING, "string"
    );
    nt::TimestampedString teststr = nt::GetAtomicString(ySub, "");
    double timeSinceUpdate = frc::GetTime().value() - double(teststr.time)/1000000 ; //current time - send time (in seconds)
    qDebug()<<teststr.value;
    qDebug()<<timeSinceUpdate;
}

void NetworkTables::UpdateSettings(QVariant address, uint port) {
    QSettings settings;
    settings.setValue("ntAddress", address);
    settings.setValue("ntPort", port);
    RefreshServer();
    // nt::SetServer(nt::GetDefaultInstance(), address.toByteArray(), port);
}

void NetworkTables::RefreshServer() {
    test();
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
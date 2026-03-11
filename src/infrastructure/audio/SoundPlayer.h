#pragma once
#include <QObject>
#include <QSoundEffect>

class SoundPlayer : public QObject {
    Q_OBJECT
public:
    explicit SoundPlayer(QObject* parent = nullptr);

    void playAlarm();

private:
    QSoundEffect m_alarm;
};

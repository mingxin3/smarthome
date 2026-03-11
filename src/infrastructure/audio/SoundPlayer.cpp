#include "SoundPlayer.h"
#include <QUrl>

SoundPlayer::SoundPlayer(QObject* parent) : QObject(parent) {
    m_alarm.setSource(QUrl("qrc:/sounds/alarm.wav"));
    m_alarm.setLoopCount(1);
    m_alarm.setVolume(0.9f);
}

void SoundPlayer::playAlarm() {
    // QSoundEffect 内部异步播放；重复触发时可先 stop 再 play
    m_alarm.stop();
    m_alarm.play();
}

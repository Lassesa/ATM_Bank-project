#ifndef MEDIA_H
#define MEDIA_H

#include <QObject>
#include <QSoundEffect>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QVideoWidget>
#include <QVBoxLayout>
#include <QUrl>
#include <QDebug>

class Media : public QObject
{
    Q_OBJECT
public:
    explicit Media(QObject *parent = nullptr);

    void setupVideo(QWidget *container);
    void setupIdleVideo(QWidget *container);

    void playMoreVideo();
    void stopMoreVideo();

    void playIdleVideo();
    void stopIdleVideo();

    // Sounds
    void playKeypad();
    void playOk();
    void playCancel();
    void playClear();
    void playButton();
    void playSuccess();
    void playError();
    void playWithdraw();
    void playTimeout();

private:
    QMediaPlayer *moreVideoPlayer;
    QAudioOutput *moreVideoAudio;
    QVideoWidget *moreVideoWidget;

    QMediaPlayer *idleVideoPlayer;
    QAudioOutput *idleVideoAudio;
    QVideoWidget *idleVideoWidget;

    QSoundEffect *keypadSound;
    QSoundEffect *okSound;
    QSoundEffect *cancelSound;
    QSoundEffect *clearSound;
    QSoundEffect *buttonSound;
    QSoundEffect *successSound;
    QSoundEffect *errorSound;
    QSoundEffect *withdrawSound;
    QSoundEffect *timeoutSound;
};

#endif
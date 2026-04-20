#include "media.h"

Media::Media(QObject *parent) : QObject(parent)
{
    moreVideoPlayer = new QMediaPlayer(this);
    moreVideoAudio = new QAudioOutput(this);
    moreVideoPlayer->setAudioOutput(moreVideoAudio);


    keypadSound = new QSoundEffect(this);
    keypadSound->setSource(QUrl("qrc:/sounds/keypad.wav"));
    keypadSound->setVolume(0.5);

    okSound = new QSoundEffect(this);
    okSound->setSource(QUrl("qrc:/sounds/ok.wav"));
    okSound->setVolume(0.5);

    cancelSound = new QSoundEffect(this);
    cancelSound->setSource(QUrl("qrc:/sounds/cancel.wav"));
    cancelSound->setVolume(0.5);

    clearSound = new QSoundEffect(this);
    clearSound->setSource(QUrl("qrc:/sounds/clear.wav"));
    clearSound->setVolume(0.5);

    buttonSound = new QSoundEffect(this);
    buttonSound->setSource(QUrl("qrc:/sounds/button.wav"));
    buttonSound->setVolume(0.5);

    successSound = new QSoundEffect(this);
    successSound->setSource(QUrl("qrc:/sounds/success.wav"));
    successSound->setVolume(0.5);

    errorSound = new QSoundEffect(this);
    errorSound->setSource(QUrl("qrc:/sounds/error.wav"));
    errorSound->setVolume(0.5);

    withdrawSound = new QSoundEffect(this);
    withdrawSound->setSource(QUrl("qrc:/sounds/withdraw.wav"));
    withdrawSound->setVolume(0.5);

    timeoutSound = new QSoundEffect(this);
    timeoutSound->setSource(QUrl("qrc:/sounds/timeout.wav"));
    timeoutSound->setVolume(0.5);
}

void Media::setupVideo(QWidget *container)
{
    moreVideoWidget = new QVideoWidget(container);
    moreVideoWidget->setAspectRatioMode(Qt::IgnoreAspectRatio);
    moreVideoPlayer->setVideoOutput(moreVideoWidget);
    moreVideoPlayer->setLoops(QMediaPlayer::Infinite);

    QVBoxLayout *videoLayout = new QVBoxLayout(container);
    videoLayout->setContentsMargins(0, 0, 0, 0);
    videoLayout->addWidget(moreVideoWidget);

    moreVideoPlayer->setSource(QUrl("qrc:/videos/video1.mp4"));
    moreVideoPlayer->play();
}


void Media::playKeypad()   { keypadSound->play(); }
void Media::playOk()       { okSound->play(); }
void Media::playCancel()   { cancelSound->play(); }
void Media::playClear()    { clearSound->play(); }
void Media::playButton()   { buttonSound->play(); }
void Media::playSuccess()  { successSound->play(); }
void Media::playError()    { errorSound->play(); }
void Media::playWithdraw() { withdrawSound->play(); }
void Media::playTimeout()  { timeoutSound->play(); }

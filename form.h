#ifndef FORM_H
#define FORM_H
#include <QObject>
#include <QQmlApplicationEngine>
#include <QStringList>
#include <qqml.h>
#include <QTimer>

class Form : public QObject
{
    Q_OBJECT
public:
    Form(QObject* parent = 0);
    void setEngine(QQmlApplicationEngine* engine);
    Q_INVOKABLE QString getDir();
    Q_INVOKABLE void startEncryption();
private:
    QQmlApplicationEngine* engine;
    QStringList files;
    bool canEncrypt;
    QString savePath;
    long int encryptionVal;
    unsigned int timerPeriod;
    bool deleteOriginalFiles;
    bool overwriteExistingFiles;
    bool encryptEverythingAtOnce;

    void EncryptFiles();
private slots:
    void SingleFileEnc();
};

#endif // FORM_H

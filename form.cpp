#include "form.h"
#include <QQmlApplicationEngine>
#include <QQmlComponent>
#include <QVariant>
#include <QMessageBox>
#include <QFileDialog>
#include <QStringList>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QTimer>
#include <string>
#include <QDebug>
#include <windows.h>


QStringList getAllFiles(QString root, QStringList wildcard)
{
    QStringList res = QStringList();

    if (wildcard.isEmpty()) return QStringList();

    QString firstWildcardElement = wildcard.takeFirst();

    if (wildcard.size() == 0)
    {
        QDirIterator it((root == "") ? "." : root, QStringList() << firstWildcardElement, QDir::Files);
        while (it.hasNext()) {
            QString next = it.next();
            res.append(next);
        }
        return res;
    }

    if (firstWildcardElement == "*")
    {
        if (wildcard.size() == 1)
        {
            QDirIterator it((root == "") ? "." : root, QStringList() << wildcard.takeFirst(), QDir::Files, QDirIterator::Subdirectories);
            while (it.hasNext()) {
                QString next = it.next();
                res.append(next);
            }
        }
        else
        {
            QDirIterator it((root == "") ? "." : root, QStringList() << wildcard.takeFirst(), QDir::Dirs | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
            while (it.hasNext()) {
                QString next = it.next();

                res << getAllFiles(next + "/", wildcard);
            }
        }
    }
    else
    {
        QDirIterator it((root == "") ? "." : root, QStringList() << firstWildcardElement, QDir::Dirs | QDir::NoDotAndDotDot);
        while (it.hasNext()) {
            QString next = it.next();
            res << getAllFiles(next + "/", wildcard);
        }
    }

    return res;
}


bool FileIsDuplicate(QString fileName, QString originalName)
{
    if (fileName == originalName) return true;

    QString file = fileName.split('.').takeFirst().split('(').takeLast();
    file.chop(1);
    bool ok;
    int value = file.toInt(&ok);

    return ok;
}


bool FileIsBeingUsed(QString filepath)
{
    std::wstring ws = filepath.toStdWString();
    LPCWSTR sw = ws.c_str();

    HANDLE fh;
    fh = CreateFileW(sw, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
    if ((fh != NULL) && (fh != INVALID_HANDLE_VALUE))
    {
        CloseHandle(fh);
        return false;
    }
    else
    {
        CloseHandle(fh);
        return true;
    }
}


void EncryptTextFile(QString path, QString savePath, long int encryptionVal, bool overwriteExistingFiles)
{
    QString content = "";
    QByteArray ba;
    QString fileName = path.split('/').takeLast().split('.').takeFirst();

    QFile file(path);
    if (file.open(QIODevice::ReadOnly))
    {
        ba = file.readAll();
    }
    file.close();

    for (int i = 0; i < ba.length(); i++)
    {
        ba[i] ^= encryptionVal;
    }

    if (QFileInfo::exists(savePath + "/" + fileName + ".txt"))
    {
        QDir saveDir(savePath);
        QStringList duplicateCandidats = saveDir.entryList(QStringList() << fileName + ".txt" << fileName + "(*).txt", QDir::Files);
        QStringList duplicates = QStringList();

        for (QString d : duplicateCandidats)
        {
            if (FileIsDuplicate(d, fileName + ".txt"))
            {
                duplicates << d;
            }
        }

        if (overwriteExistingFiles)
        {
            QFile::remove(savePath + "/" + fileName + ".txt");
        }

        if (duplicates.size() > 0 && !overwriteExistingFiles)
        {
            fileName = fileName + "(" + QString::fromStdString(std::to_string(duplicates.size())) + ")";
        }
    }

    QFile file2(savePath + "/" + fileName + ".txt");
    if (file2.open(QIODevice::Truncate | QIODevice::ReadWrite))
    {
        file2.write(ba);
    }
    file2.close();
}


void EncryptBinaryFile(QString path, QString savePath, long int encryptionVal, bool overwriteExistingFiles)
{
    QString content = "";
    QByteArray ba;
    QString fileName = path.split('/').takeLast().split('.').takeFirst();

    QFile file(path);
    if (file.open(QIODevice::ReadWrite))
    {
        ba = file.readAll();
    }
    file.close();

    for (int i = 0; i < ba.length(); i++)
    {
        ba[i] ^= encryptionVal;
    }

    if (QFileInfo::exists(savePath + "/" + fileName + ".bin"))
    {
        QDir saveDir(savePath);
        QStringList duplicateCandidats = saveDir.entryList(QStringList() << fileName + ".bin" << fileName + "(*).bin", QDir::Files);
        QStringList duplicates = QStringList();

        for (QString d : duplicateCandidats)
        {
            if (FileIsDuplicate(d, fileName + ".bin"))
            {
                duplicates << d;
            }
        }

        if (overwriteExistingFiles)
        {
            QFile::remove(savePath + "/" + fileName + ".bin");
        }

        if (duplicates.size() > 0 && !overwriteExistingFiles)
        {
            fileName = fileName + "(" + QString::fromStdString(std::to_string(duplicates.size())) + ")";
        }
    }

    QFile file2(savePath + "/" + fileName + ".bin");
    if (file2.open(QIODevice::ReadWrite))
    {
        file2.write(ba);
    }
    file2.close();
}


void Form::EncryptFiles()
{
    while (!this->files.isEmpty())
    {
        QString file = this->files.takeFirst();
        if (FileIsBeingUsed(file) || QFile(file).handle() != -1)
        {
            QMessageBox messageBox;
            messageBox.critical(0,"Error", file + " is currently used by another program");
            messageBox.setFixedSize(500,200);
            continue;
        }

        if (file.endsWith(".txt"))
        {
            EncryptTextFile(file, savePath, encryptionVal, overwriteExistingFiles);
        }
        else if (file.endsWith(".bin"))
        {
            EncryptBinaryFile(file, savePath, encryptionVal, overwriteExistingFiles);
        }
        if (deleteOriginalFiles)
        {
            QFile::remove(file);
        }
    }
}


void Form::SingleFileEnc()
{
    if (!files.isEmpty())
    {
        QString file = files.takeFirst();
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(0, "Question", "Encrypt file " + file + "?");

        if (reply == QMessageBox::Yes)
        {
            if (FileIsBeingUsed(file) || QFile(file).handle() != -1)
            {
                QMessageBox messageBox;
                messageBox.critical(0,"Error", file + " is currently used by another program");
                messageBox.setFixedSize(500,200);
            }
            else
            {
                if (file.endsWith(".txt"))
                {
                    EncryptTextFile(file, savePath, encryptionVal, overwriteExistingFiles);
                }
                else if (file.endsWith(".bin"))
                {
                    EncryptBinaryFile(file, savePath, encryptionVal, overwriteExistingFiles);
                }
                if (deleteOriginalFiles)
                {
                    QFile::remove(file);
                }
            }
        }
        QTimer::singleShot(timerPeriod, this, SLOT(SingleFileEnc()));
    }
    else
    {
        this->canEncrypt = true;
    }
}


Form::Form(QObject* parent) : QObject(parent)
{
    this->canEncrypt = true;
}


void Form::setEngine(QQmlApplicationEngine* engine)
{
    this->engine = engine;
}


QString Form::getDir()
{
    return QFileDialog::getExistingDirectory(nullptr, "Select Directory", "", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
}


void Form::startEncryption()
{
    if (!canEncrypt)
    {
        QMessageBox messageBox;
        messageBox.critical(0,"Error","Encryption process is not finished yet!");
        messageBox.setFixedSize(500,200);
        return;
    }

    QString wildcard = this->engine->rootObjects().first()->findChild<QObject*>("wildcard")->property("text").toString();
    this->savePath = this->engine->rootObjects().first()->findChild<QObject*>("savepath")->property("text").toString();
    this->overwriteExistingFiles = (this->engine->rootObjects().first()->findChild<QObject*>("actionForRepeatedFiles")->property("currentIndex").toInt() == 0) ? true : false;
    this->encryptEverythingAtOnce = (this->engine->rootObjects().first()->findChild<QObject*>("mode")->property("currentIndex").toInt() == 0) ? false : true;
    QString timerPeriod = this->engine->rootObjects().first()->findChild<QObject*>("timerperiod")->property("text").toString();
    QString encryptionValue = this->engine->rootObjects().first()->findChild<QObject*>("encryptionvalue")->property("text").toString();
    this->deleteOriginalFiles = this->engine->rootObjects().first()->findChild<QObject*>("checkbox")->property("checked").toBool();

    bool timerPeriodIsOk;
    bool encryptionValueIsOk;
    this->timerPeriod = timerPeriod.toUInt(&timerPeriodIsOk);
    this->encryptionVal = encryptionValue.toLong(&encryptionValueIsOk);

    if (wildcard == "")
    {
        QMessageBox messageBox;
        messageBox.critical(0,"Error","Wildcard is empty!");
        messageBox.setFixedSize(500,200);
        return;
    }
    if (savePath == "")
    {
        QMessageBox messageBox;
        messageBox.critical(0,"Error","Save path is empty!");
        messageBox.setFixedSize(500,200);
        return;
    }
    if (!timerPeriodIsOk)
    {
        QMessageBox messageBox;
        messageBox.critical(0,"Error","Incorrect format for timer period!");
        messageBox.setFixedSize(500,200);
        return;
    }
    if (!encryptionValueIsOk)
    {
        QMessageBox messageBox;
        messageBox.critical(0,"Error","Incorrect format for encryption value!");
        messageBox.setFixedSize(500,200);
        return;
    }

    QStringList wildcardAsList = wildcard.split('/');
    if (!wildcardAsList[0].contains('*') && !wildcardAsList[0].contains('?') && !wildcardAsList[0].contains('.'))
    {
        QString root = wildcardAsList.takeFirst();
        files = getAllFiles(root + "/", wildcardAsList);
    }
    else
    {
        files = getAllFiles("", wildcardAsList);
    }

    if (this->encryptEverythingAtOnce)
    {
        canEncrypt = false;
        EncryptFiles();
        canEncrypt = true;
    }
    else
    {
        canEncrypt = false;
        this->SingleFileEnc();
    }
}

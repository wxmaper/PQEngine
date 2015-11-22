#include <QTextCodec>
#include "phpqt5.h"

QByteArray PHPQt5::W_CP = "Windows-1251";
MemoryManager PHPQt5::m_mmng = MemoryManager::Hybrid;

QByteArray PHPQt5::toW(const QByteArray &ba)
{
    QTextCodec *codec = QTextCodec::codecForName(W_CP);
    return codec->fromUnicode(ba);
}

QByteArray PHPQt5::toUTF8(const QByteArray &ba)
{
    QTextCodec *codec = QTextCodec::codecForName(W_CP);
    return codec->toUnicode(ba).toUtf8();
}

void PHPQt5::setMemoryManager(MemoryManager mmng) {
    m_mmng = mmng;
}

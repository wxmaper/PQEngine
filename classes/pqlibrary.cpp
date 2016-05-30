#include "pqlibrary.h"
#include "pqobject_private.h"

PQOBJECT_STANDARD_METHODS(QLibrary, QLibrary)

PQLibrary::PQLibrary(QObject *parent)
    : QLibrary(parent)
{

}

PQLibrary::PQLibrary(const QString &fileName, QObject *parent)
    : QLibrary(fileName, parent)
{
    m_types.insert("int", 0);
    m_types.insert("bool", 1);
}

PQLibrary::PQLibrary(const QString &fileName, int verNum, QObject *parent)
    : QLibrary(fileName, verNum, parent)
{

}

PQLibrary::PQLibrary(const QString &fileName, const QString &version, QObject *parent)
    : QLibrary(fileName, version, parent)
{

}

PQLibrary::~PQLibrary()
{

}

bool PQLibrary::load()
{
    return QLibrary::load();
}

bool PQLibrary::unload()
{
    return QLibrary::unload();
}

bool PQLibrary::isLoaded() const
{
    return QLibrary::isLoaded();
}

bool PQLibrary::isLibrary(const QString &fileName)
{
    return QLibrary::isLibrary(fileName);
}

void PQLibrary::setFileName(const QString &fileName)
{
    QLibrary::setFileName(fileName);
}

QString PQLibrary::fileName() const
{
    return QLibrary::fileName();
}

void PQLibrary::setFileNameAndVersion(const QString &fileName, int verNum)
{
    QLibrary::setFileNameAndVersion(fileName, verNum);
}

void PQLibrary::setFileNameAndVersion(const QString &fileName, const QString &version)
{
    QLibrary::setFileNameAndVersion(fileName, version);
}

QString PQLibrary::errorString() const
{
    return QLibrary::errorString();
}

void PQLibrary::setLoadHints(int hints)
{
    QLibrary::setLoadHints(QLibrary::LoadHints(hints));
}

int PQLibrary::loadHints() const
{
    return QLibrary::loadHints();
}

auto type_cast(int type, const QVariant &val) {
    switch(type) {
    case QMetaType::Bool: return val.toBool(); break;
   // case QMetaType::Int: return val.toInt(); break;
    }
}

typedef void (*PQFunctionPointer)(...);
void PQLibrary::call(const QString &functionSignature, int i)
{
    if(QLibrary::isLoaded()) {
        PQFunctionPointer function_ptr = (PQFunctionPointer) QLibrary::resolve("SetCursorPos");

        if (function_ptr) {
            QVariantList args;
            args << 1 << 1;

            function_ptr((void*) args.at(0).data(),
                         (void*) args.at(1).data());
        }
        else {
            pq_php_error("resolving error");
        }
    }
}

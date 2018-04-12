#ifndef DBCREATOR_H
#define DBCREATOR_H

#include <QObject>
#include <QString>

bool createDb(const char* dbPath, const char* cedictPath, const char* rankFilePath);

#endif // DBCREATOR_H

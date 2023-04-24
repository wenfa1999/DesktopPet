
#ifndef MYDATABASE_H
#define MYDATABASE_H


#include <QObject>

#include "sqlite3.h"

class MyDatabase : public QObject
{
    Q_OBJECT
public:
    ~MyDatabase();

    static MyDatabase *instance();
    void updateValue(const char* key, const QString column, const QString &data);
    void findValue(const QString key, const QString column, QString &data);

private:
    explicit MyDatabase(QObject *parent = nullptr);
    explicit MyDatabase(const MyDatabase&);
    MyDatabase& operator=(const MyDatabase&);
    sqlite3 *m_db;
    char * errMsg;

signals:

};

#endif // MYDATABASE_H

#include "mydatabase.h"
#include <QDebug>
#include <QMessageBox>

MyDatabase::MyDatabase(QObject *parent)
    : QObject{parent}
{
    // 打开数据库
    int rc = sqlite3_open("test.db", &m_db);

    if (rc == SQLITE_OK)
    {
        qDebug() << "打开成功";

        /// 检查是否存在表function_keys
        const char* query = "SELECT name FROM sqlite_master WHERE type='table' AND name='function_keys'";
        sqlite3_stmt *statement;

        // 编译查询语句
        if (sqlite3_prepare_v2(m_db, query, -1, &statement, NULL) != SQLITE_OK)
        {
            qDebug() << "Error preparing query: " << sqlite3_errmsg(m_db);
        }
        // 执行查询语句
        int result = sqlite3_step(statement);
        if (result == SQLITE_ROW)
        {
            qDebug() << "表存在";
        }
        else
        {
            qDebug() << "表不存在，创建表";
            // 表不存在，创建表
            const char* createTableQuery = "CREATE TABLE function_keys ( \
                                        keys TEXT PRIMARY KEY \
                                                  UNIQUE \
                                                  NOT NULL \
                                                  COLLATE NOCASE, \
                                        A    TEXT, \
                                        B    TEXT, \
                                        C    TEXT, \
                                        D    TEXT, \
                                        E    TEXT, \
                                        F    TEXT, \
                                        G    TEXT, \
                                        H    TEXT, \
                                        I    TEXT, \
                                        J    TEXT, \
                                        K    TEXT, \
                                        L    TEXT, \
                                        M    TEXT, \
                                        N    TEXT, \
                                        O    TEXT, \
                                        P    TEXT, \
                                        Q    TEXT, \
                                        R    TEXT, \
                                        S    TEXT, \
                                        T    TEXT, \
                                        U    TEXT, \
                                        V    TEXT, \
                                        W    TEXT, \
                                        X    TEXT, \
                                        Y    TEXT, \
                                        Z    TEXT \
                                    );";

            result = sqlite3_exec(m_db, createTableQuery, nullptr, nullptr, &errMsg);
            if (result == SQLITE_OK)
            {
                qDebug() << "创建表成功";
            }
            else
            {
                qDebug() << "创建表失败" << errMsg;
                sqlite3_free(errMsg);
            }
        }

        /// 检查主键是否存在keysF1-F12
        bool has_key = false;
        auto callback = [](void *data,int argc, char **argv, char **colname) ->int{
            bool *has_key = static_cast<bool *>(data);
            qDebug() << "argc:" << argc;
            if (argc == 1)
            {
                *has_key = true;
            }
            return 0;
        };

        const char *selectKeys = "SELECT keys FROM function_keys WHERE keys IN ('F1', 'F2', 'F3', 'F4', 'F5', 'F6', 'F7', 'F8', 'F9', 'F10', 'F11', 'F12')";
        rc = sqlite3_exec(m_db, selectKeys, callback, &has_key, &errMsg);
        if (rc != SQLITE_OK) {
            qDebug() << "SQL error: " << errMsg;
            sqlite3_free(errMsg);
        }

        if (!has_key)
        {   // 不存在则创建keys F1-F12
            const char *insertKeys = "INSERT INTO function_keys (keys) VALUES ('F1'), ('F2'), ('F3'), ('F4'), ('F5'), ('F6'), ('F7'), ('F8'), ('F9'), ('F10'), ('F11'), ('F12')";
            rc = sqlite3_exec(m_db, insertKeys, nullptr, nullptr, &errMsg);
            if (rc != SQLITE_OK) {
                qDebug() << "SQL error: " << errMsg;
                sqlite3_free(errMsg);
            }
        }
        sqlite3_finalize(statement);
    }
    else
    {
        qDebug() << "打开失败";
    }
}

MyDatabase::~MyDatabase()
{
    if (m_db)
    {
        sqlite3_close(m_db);
        m_db = nullptr;
    }
}

// 创建唯一实例
MyDatabase *MyDatabase::instance()
{
    static MyDatabase *s_instance = nullptr;

    if (!s_instance)
    {
        s_instance = new MyDatabase();
    }

    return s_instance;
}

void MyDatabase::updateValue(const char* key, const QString column, const QString &data)
{
    sqlite3_stmt *stmt;
    // 构建参数化查询 - 注意：SQLite不支持参数化表名和列名，所以列名需特殊处理
    QString sql = QString("UPDATE function_keys SET %1 = ? WHERE keys = ?").arg(column);
    qDebug() << "执行SQL: " << sql;

    int rc = sqlite3_prepare_v2(m_db, sql.toUtf8().constData(), -1, &stmt, nullptr);
    if (rc == SQLITE_OK) {
        // 绑定参数
        sqlite3_bind_text(stmt, 1, data.toUtf8().constData(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, key, -1, SQLITE_TRANSIENT);
        
        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE) {
            qDebug() << "写入SQL失败: " << sqlite3_errmsg(m_db);
            QString errorMsg = QString("数据库操作失败: %1").arg(sqlite3_errmsg(m_db));
            // 使用QMessageBox需要添加头文件
            QMessageBox::warning(nullptr, "数据库错误", errorMsg);
        }
        sqlite3_finalize(stmt);
    } else {
        qDebug() << "准备SQL失败: " << sqlite3_errmsg(m_db);
        if (errMsg != nullptr) {
            sqlite3_free(errMsg);
            errMsg = nullptr;
        }
    }
}

void MyDatabase::findValue(const QString key, const QString column, QString &data)
{
    sqlite3_stmt *stmt;
    // 构建参数化查询 - 注意：SQLite不支持参数化表名和列名
    QString sql = QString("SELECT %1 FROM function_keys WHERE keys = ?").arg(column);
    qDebug() << "执行SQL: " << sql;

    int rc = sqlite3_prepare_v2(m_db, sql.toUtf8().constData(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        qDebug() << "查询SQL失败: " << sqlite3_errmsg(m_db);
        return;
    }
    
    // 绑定参数
    sqlite3_bind_text(stmt, 1, key.toUtf8().constData(), -1, SQLITE_TRANSIENT);
    
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        const unsigned char* result = sqlite3_column_text(stmt, 0);
        if (result) {
            data = QString::fromUtf8(reinterpret_cast<const char*>(result));
            qDebug() << "查询结果: " << data;
        }
    }
    
    sqlite3_finalize(stmt);
}


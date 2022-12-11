#ifndef APPCONFIG_H
#define APPCONFIG_H

#include <QWidget>
#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QFile>
#include <QHBoxLayout>
#include <QVBoxLayout>

class AppConfig : public QWidget
{
    Q_OBJECT
public:
    explicit AppConfig(QWidget *parent = nullptr);

private:
    void InterfaceInit(void);

private:
    QListWidget *listWidget;
    QHBoxLayout *mainLayout;

signals:

};

#endif // APPCONFIG_H

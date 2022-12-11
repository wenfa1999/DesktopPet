#ifndef APPCONFIG_H
#define APPCONFIG_H

#include <QFile>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QSettings>
#include <QVBoxLayout>
#include <QWidget>
#include <QComboBox>
#include <QRadioButton>

class AppConfig : public QWidget {
    Q_OBJECT
public:
    explicit AppConfig(QWidget *parent = nullptr);

    ~AppConfig();
private:
    void InterfaceInit(void);
    void ShowGeneralSettings(void);
    void ShowInterfaceSettings(void);

private:
    QListWidget *listWidget;
    QWidget *generalSetWidget;

    QHBoxLayout *mainLayout;
    QVBoxLayout *generalSetLayout;

    QHBoxLayout *lanLayout;
    QLayout *InterfaceSetLayout;
    QLayout *AbouSetLayout;

//    QWidget *showSettingsWidget;

signals:
};

#endif // APPCONFIG_H

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
#include <QGridLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QScrollArea>
#include <QSizePolicy>
#include <QGroupBox>
#include <QFileDialog>
#include <QPoint>
#include <QMouseEvent>

#include "mydatabase.h"

class AppConfig : public QWidget {
    Q_OBJECT
public:
    explicit AppConfig(QWidget *parent = nullptr);

    ~AppConfig();
private:
    void InterfaceInit(void);
    void GeneralSettingsInit(void);
    void ShortcutSettingsInit(void);

private:
    QListWidget *listWidget;
    QWidget *generalSetWidget;  // 通用设置WIdget
    QWidget *shortcutSetWidget; // 快捷键设置Widget

    QHBoxLayout *mainLayout;
    QVBoxLayout *generalSetLayout;  // 通用设置Layout
    QVBoxLayout *shortcutLayout;    // 快捷键设置Layout

    QHBoxLayout *lanLayout;
    QLayout *InterfaceSetLayout;
    QLayout *AbouSetLayout;

    QPoint m_windowPos;
    QPoint m_mousePos;
    bool isMoving = false;

//    QWidget *showSettingsWidget;

signals:

    // QWidget interface
protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
};

#endif // APPCONFIG_H

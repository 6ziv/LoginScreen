#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QCheckBox>
#include <QLabel>
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private:
    QPushButton * btnInstall;
    QCheckBox * cbActivate;
    QPushButton* btnResource;
    QLabel* resPath;
    void rewrite();
};
#endif // MAINWINDOW_H

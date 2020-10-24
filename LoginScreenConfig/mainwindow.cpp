#include "mainwindow.h"
#include "config.h"
#include <QFileDialog>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    this->setFixedSize(400,250);
    btnInstall=new QPushButton(this);
    btnInstall->setGeometry(100,170,200,40);
    btnInstall->show();

    cbActivate=new QCheckBox(this);
    cbActivate->setGeometry(50,50,20,20);
    cbActivate->setText("activated");
    cbActivate->show();
    cbActivate->connect(cbActivate,&QCheckBox::stateChanged,this,[this](int checked){
        if(checked==Qt::CheckState::Checked){
            Config::activate();
        }
        else Config::deactivate();
        rewrite();
    });
    resPath=new QLabel(this);
    resPath->setGeometry(100,100,200,30);
    resPath->show();
    btnResource=new QPushButton(this);
    btnResource->setGeometry(320,100,60,30);
    btnResource->show();
    btnResource->connect(btnResource,&QPushButton::pressed,this,[this](){
        QUrl f=QFileDialog::getOpenFileUrl(this,"Choose a file",QUrl(),"zip(*.zip);");
        Config::resource(f.toLocalFile());

    });
    rewrite();
}
void MainWindow::rewrite(){
    cbActivate->blockSignals(true);
    cbActivate->setChecked(Config::isactivated());
    resPath->setText(Config::resource());
    if(Config::isinstalled()){
        cbActivate->setEnabled(true);
        btnResource->setEnabled(cbActivate->isChecked());
        btnInstall->setText("Uninstall");
        disconnect(btnInstall);
        connect(btnInstall,&QPushButton::pressed,this,[this](){Config::uninstall();rewrite();});
    }else{
        btnInstall->setText("Install");
        cbActivate->setEnabled(false);
        btnResource->setEnabled(false);
        disconnect(btnInstall);
        connect(btnInstall,&QPushButton::pressed,this,[this](){Config::install();rewrite();});
    }

    cbActivate->blockSignals(false);

}
MainWindow::~MainWindow()
{
}


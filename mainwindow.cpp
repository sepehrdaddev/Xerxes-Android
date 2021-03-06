#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <arpa/inet.h>
#include <netdb.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->spinBox->setValue(80);
    ui->spinBox_2->setValue(10);
    ui->spinBox_3->setValue(10);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    QString addr = ui->lineEdit->text();
    if(addr.isEmpty() || !valid_ip(addr)){
        QMessageBox::critical(this, tr("Error"), tr("Invalid input"));
    }else{
        Attack *engine = new Attack(this);
        engines.push_back(engine);
        engine->set_config(ui->lineEdit->text(), QString::number(ui->spinBox->value()), ui->spinBox_2->value(), ui->spinBox_3->value(), static_cast<Attack::Vector>(ui->comboBox->currentIndex()));
        engine->move(this->rect().center() - engine->rect().center());
        engine->showMaximized();
        engine->setModal(true);
        engine->exec();
    }
}

bool MainWindow::valid_ip(const QString &ip){
    hostent *record = gethostbyname(ip.toStdString().c_str());
    struct sockaddr_in sa{};
    return ((inet_pton(AF_INET, ip.toStdString().c_str(), &(sa.sin_addr))) || (record != nullptr));
}

void MainWindow::closeEvent(QCloseEvent *event){
    for(auto engine : engines){
        delete engine;
    }
    QWidget::closeEvent(event);
}

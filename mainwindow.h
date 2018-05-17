#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "attack.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::MainWindow *ui;
    bool valid_ip(const QString& ip);
    QList<Attack *> engines{};
};

#endif // MAINWINDOW_H

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QFile>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

struct Info
{
    int b;
    int w;
    int h;
    Info(int begin, int width, int height)
    {
        b = begin;
        w = width;
        h = height;
    }
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:
    void load_log_slot();
    void load_img_slot();
    void apply_log_slot();
private:
    int sum_Byts(QByteArray arr, bool forward = true);
    void overlay(Info log, Info img, int x, int y, int bc);
private:
    Ui::MainWindow *ui;
    QString path_img;
    QString path_log;
    QByteArray arr_log;
    QByteArray arr_img;
};
#endif // MAINWINDOW_H

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->pushButton_3, SIGNAL(clicked()), SLOT(apply_log_slot()));
    connect(ui->pushButton_2, SIGNAL(clicked()), SLOT(load_log_slot()));
    connect(ui->pushButton, SIGNAL(clicked()), SLOT(load_img_slot()));
}

int sum(QByteArray array, int begin, int length)
{
    int rez = 0;
    for(int i = begin, shift = 0; i < begin + length; i++, shift += 8){
        rez += QByteArray::fromRawData(&array[i], 1).toHex().toInt(nullptr, 16) << shift;
    }
    return rez;
}

int MainWindow::sum_Byts(QByteArray arr, bool forward)
{
    if(forward){
        return QByteArray::fromRawData(arr, arr.size()).toHex().toInt(nullptr, 16);
    }else{
        int n = arr.size();
        if(n % 2){
            n--;
        }
        for(int i = 0; i < n / 2; i++){
            qSwap(arr[i], arr[arr.size() - 1 - i]);
        }
        return QByteArray::fromRawData(arr, arr.size()).toHex().toInt(nullptr, 16);
    }
}

void MainWindow::overlay(Info log, Info img, int x, int y, int bc)
{
    QFile file("C:\\Qt\\Project\\GRI_Lab6\\result.bmp");
    file.open(QIODevice::WriteOnly);
    for(int i = 0; i < img.b; i++){
        file.putChar(arr_img[i]);
    }
    int bcc;
    for(int i = 0, lh = 0; i < img.h; i++){
        if(i == y){
            lh = log.h;
        } else {
            lh--;
        }
        for(int j = 0, lw = 0, col; j < img.w; j++){
            if(j == x && lh > 0){
                lw = log.w;
            } else {
                lw--;
            }
            if(lw > 0){
                bcc = sum_Byts(QByteArray::fromRawData(&arr_log[log.b + (log.h - lh) * log.w * 3 + (log.w - lw) * 3], 3), false);
                if(bcc != bc){
                    col = (QByteArray::fromRawData(&arr_img[img.b + i * img.w * 3 + j * 3], 1).toHex().toInt(nullptr, 16) +
                            QByteArray::fromRawData(&arr_log[log.b + (log.h - lh) * log.w * 3 + (log.w - lw) * 3], 1).toHex().toInt(nullptr, 16)) / 2;
//                    qDebug() << img.b + i * img.w * 3 + j * 3 << log.b + (log.h - lh) * log.w * 3 + (log.w - lw) * 3;
                    file.putChar((char)col);
                    col = (QByteArray::fromRawData(&arr_img[img.b + i * img.w * 3 + j * 3 + 1], 1).toHex().toInt(nullptr, 16) +
                            QByteArray::fromRawData(&arr_log[log.b + (log.h - lh) * log.w * 3 + (log.w - lw) * 3 + 1], 1).toHex().toInt(nullptr, 16)) / 2;

                    file.putChar((char)col);
                    col = (QByteArray::fromRawData(&arr_img[img.b + i * img.w * 3 + j * 3 + 2], 1).toHex().toInt(nullptr, 16) +
                            QByteArray::fromRawData(&arr_log[log.b + (log.h - lh) * log.w * 3 + (log.w - lw) * 3 + 2], 1).toHex().toInt(nullptr, 16)) / 2;

                    file.putChar((char)col);
                } else {
                    file.putChar(arr_img[img.b + i * img.w * 3 + j * 3]);
                    file.putChar(arr_img[img.b + i * img.w * 3 + j * 3 + 1]);
                    file.putChar(arr_img[img.b + i * img.w * 3 + j * 3 + 2]);
                }
            }else{
                file.putChar(arr_img[img.b + i * img.w * 3 + j * 3]);
                file.putChar(arr_img[img.b + i * img.w * 3 + j * 3 + 1]);
                file.putChar(arr_img[img.b + i * img.w * 3 + j * 3 + 2]);
            }
        }
    }
    file.close();
}

void MainWindow::load_log_slot()
{
    path_log = QFileDialog::getOpenFileName();
    if(path_log.size() == 0){
        return;
    }
    if(!arr_log.isEmpty()){
        arr_log.clear();
    }
    ui->label_2->setPixmap(QPixmap(path_log).scaled(200,
                                                    200,
                                                    Qt::KeepAspectRatio,
                                                    Qt::SmoothTransformation));
}

void MainWindow::load_img_slot()
{
    path_img = QFileDialog::getOpenFileName();
    if(path_img.size() == 0){
        return;
    }
    ui->label->setPixmap(QPixmap(path_img).scaled(400,
                                                  400,
                                                  Qt::KeepAspectRatio,
                                                  Qt::SmoothTransformation));
}



void MainWindow::apply_log_slot()
{
    QFile file(path_log);
    // Считываем логотип в массив байт
    file.open(QIODevice::ReadOnly);
    arr_log = file.readAll();
    file.close();
    // Запоминаем размеры и начало массива пикселей
    Info inf_log(14 + sum_Byts(QByteArray::fromRawData(&arr_log[14], 4), false),
            sum_Byts(QByteArray::fromRawData(&arr_log[18], 4), false),
            sum_Byts(QByteArray::fromRawData(&arr_log[22], 4), false));
    // Повторяем тоже самое с изображением
    file.setFileName(path_img);
    file.open(QIODevice::ReadOnly);
    arr_img = file.readAll();
    file.close();
    Info inf_img(14 + sum_Byts(QByteArray::fromRawData(&arr_img[14], 4), false),
            sum_Byts(QByteArray::fromRawData(&arr_img[18], 4), false),
            sum_Byts(QByteArray::fromRawData(&arr_img[22], 4), false));
    if(inf_log.h > inf_img.h || inf_log.w > inf_img.w){
        qDebug("Size not valid");
        return;
    }
    int col = sum_Byts(QByteArray::fromRawData(&arr_log[inf_log.b], 3), false);
    int col1 = sum_Byts(QByteArray::fromRawData(&arr_log[inf_log.b + inf_log.w * 3], 3), false);
    int col2 = sum_Byts(QByteArray::fromRawData(&arr_log[inf_log.b + inf_log.w * 3 + inf_log.h * 3], 3), false);
    int col3 = sum_Byts(QByteArray::fromRawData(&arr_log[inf_log.b + inf_log.h * 3], 3), false);
    if((col + col1 + col2 + col3) / 4 != col){
        qDebug("Not detected background color");
        return;
    }
    qDebug() << inf_log.h << inf_img.h;
    switch (ui->comboBox->currentIndex()) {
    case 0:
        overlay(inf_log, inf_img, 10, 10, col);
        break;
    case 1:
        overlay(inf_log, inf_img, inf_img.w - inf_log.w - 10, 10, col);
        break;
    case 2:
        overlay(inf_log, inf_img, inf_img.w - inf_log.w - 10, inf_img.h - inf_log.h - 10, col);
        break;
    case 3:
        overlay(inf_log, inf_img, 10, inf_img.h - inf_log.h - 10, col);
        break;
    }
    qDebug() << arr_img.size();
    ui->label->setPixmap(QPixmap("C:\\Qt\\Project\\GRI_Lab6\\result.bmp").scaled(400,
                                                                                 400,
                                                                                 Qt::KeepAspectRatio,
                                                                                 Qt::SmoothTransformation));
}

MainWindow::~MainWindow()
{
    delete ui;
}


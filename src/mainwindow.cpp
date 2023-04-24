#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "OpenCV.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QPushButton *button = new QPushButton("Button", this);
    button->setGeometry(QRect(QPoint(100, 100), QSize(100, 50)));
    connect(button, &QPushButton::clicked, this, &MainWindow::openFolder);

    lineEdit = new QLineEdit(this);
    lineEdit->setGeometry(QRect(QPoint(100, 200), QSize(200, 20)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openFolder()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), QDir::homePath());

    if (fileName.isEmpty())
    {
        return;
    }

    IDCardOCR ocr;
    QString text = ocr.RecognizeText(fileName);
    qDebug() << "识别结果: " << text;
}

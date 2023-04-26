#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "OpenCV.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QPushButton *button = new QPushButton("Button", this);
    button->setGeometry(QRect(QPoint(10, 10), QSize(50, 50)));
    connect(button, &QPushButton::clicked, this, &MainWindow::openFolder);

    lineEdit = new QLineEdit(this);
    lineEdit->setGeometry(QRect(QPoint(60, 10), QSize(200, 50)));

    QLabel *label = new QLabel(this);
    label->setGeometry(QRect(QPoint(10, 60), QSize(500, 500)));
//    QImage image("C:\\Users\\Siweya\\Desktop\\20230424163121.png");
//    label->setPixmap(QPixmap::fromImage(image));
//    label->show();

    textEdit = new QTextEdit(this);
    textEdit->setGeometry(QRect(QPoint(560, 60), QSize(500, 500)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openFolder()
{
    QString fileName = "C:/Users/Siweya/Desktop/20230424163121.png";
    //QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), QDir::homePath());

    if (fileName.isEmpty())
    {
        return;
    }
    lineEdit->setText(fileName);

    IDCardOCR ocr;
    QString text = ocr.RecognizeText(fileName);
    qDebug() << "识别结果: " << text;
    textEdit->setText(text);
}

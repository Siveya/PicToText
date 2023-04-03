#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

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

    if (!fileName.isEmpty())
    {
        cv::Mat image = cv::imread(fileName.toStdString());
        if (image.empty()) {
            printf("Could not read the image: %s\n", fileName.toStdString().c_str());
            return;
        }

        //imshow("srcImage", image);
        // 将 OpenCV 图像转换为 Leptonica 的 Pix 格式
        Pix *pix = pixCreate(image.size().width, image.size().height, 8);
        int wpl = pixGetWpl(pix);
        for (int y = 0; y < image.rows; y++) {
            l_uint32 *line = pixGetData(pix) + y * wpl;
            for (int x = 0; x < image.cols; x++) {
                l_uint8 *pixel = reinterpret_cast<l_uint8*>(image.data + y * image.step + x * image.elemSize());
                SET_DATA_BYTE(line, x, pixel[0]);
            }
        }
        // 初始化Tesseract OCR引擎
        tesseract::TessBaseAPI *ocr = new tesseract::TessBaseAPI();
        if (ocr->Init(nullptr, "chi_sim")) {
            printf("Could not initialize tesseract.\n");
            exit(1);
        }

        // 设置要识别的图像
        ocr->SetImage(pix);

        // 进行识别

        QString text = ocr->GetUTF8Text();
        qDebug() << "识别结果: " << text;

        // 释放内存
        ocr->End();
        pixDestroy(&pix);

        qDebug() << "Selected file: " << fileName;
        lineEdit->setText(fileName);
    }
}

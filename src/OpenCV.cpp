#include "OpenCV.h"

IDCardOCR::IDCardOCR()
{
    // 初始化 Tesseract OCR 引擎
    ocr = new tesseract::TessBaseAPI();
    if (ocr->Init(nullptr, "chi_sim")) {
        printf("Could not initialize tesseract.\n");
        exit(1);
    }
}

IDCardOCR::~IDCardOCR()
{
    // 释放内存
    ocr->End();
    delete ocr;
}

cv::Mat IDCardOCR::PreprocessImage(cv::Mat image)
{
    // 进行图像预处理，例如调整大小、灰度化、二值化等
    // ...

    return image;
}

cv::Rect IDCardOCR::DetectTextRegion(cv::Mat image)
{
    cv::Rect textRect;
    // 进行文字区域检测，例如基于边缘、颜色等特征的检测方法
    // ...

    return textRect;
}


QString IDCardOCR::RecognizeText(const QString &fileName)
{
    cv::Mat image = cv::imread(fileName.toStdString());
    if (image.empty()) {
        printf("Could not read the image: %s\n", fileName.toStdString().c_str());
        return "";
    }

    // 图像预处理
    image = PreprocessImage(image);

    // 文字区域检测
    cv::Rect textRect = DetectTextRegion(image);

    // 文字识别
//    cv::Mat textImage = image(textRect).clone();
//    cv::cvtColor(textImage, textImage, cv::COLOR_BGR2GRAY);

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

    // 设置要识别的图像
    ocr->SetImage(pix);

    // 进行识别
    char *text = ocr->GetUTF8Text();
    QString result = QString::fromUtf8(text);

    // 释放内存
    pixDestroy(&pix);
    delete[] text;

    return result;
}

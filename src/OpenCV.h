#ifndef OPENCV_H
#define OPENCV_H

#include <opencv2/opencv.hpp>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

#include <QString>

class IDCardOCR
{
public:
    IDCardOCR();
    ~IDCardOCR();
    QString RecognizeText(const QString &fileName);

private:
    tesseract::TessBaseAPI *ocr;
    cv::Mat PreprocessImage(cv::Mat image);
    cv::Rect DetectTextRegion(cv::Mat image);

};

#endif // OPENCV_H

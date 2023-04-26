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

// 进行图像预处理，例如调整大小、灰度化、二值化等
cv::Mat IDCardOCR::PreprocessImage(cv::Mat inputImage)
{
    cv::Mat outputImage = inputImage;

    // 调整大小
    //cv::resize(inputImage, outputImage, cv::Size(800, 600));

    // 灰度化
    cv::cvtColor(outputImage, outputImage, cv::COLOR_BGR2GRAY);
    cv::imshow("cvtColor", outputImage);

    // 高斯滤波
    cv::GaussianBlur(outputImage, outputImage, cv::Size(5, 5), 0);
    cv::imshow("GaussianBlur", outputImage);

    // 二值化
    cv::threshold(outputImage, outputImage, 128, 255, cv::THRESH_BINARY);
    cv::imshow("threshold", outputImage);

    // 进行图像边缘检测（例如使用Canny边缘检测算法）
    cv::Mat edges;
    cv::Canny(outputImage, edges, 75, 200); // 参数可以根据图像的不同进行调整

    // 进行轮廓检测
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(edges, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    // 绘制轮廓
    cv::Mat result = cv::Mat::zeros(outputImage.size(), CV_8UC3); // 创建一张空的彩色图像
    for (size_t i = 0; i < contours.size(); i++)
    {
        cv::Scalar color = cv::Scalar(0, 255, 0); // 轮廓线的颜色
        cv::drawContours(result, contours, i, color, 2, cv::LINE_8, hierarchy, 0);
    }

    // 显示结果
    cv::imshow("Contours", result);
    cv::waitKey(0);

    return outputImage;
}

cv::Rect IDCardOCR::DetectTextRegion(cv::Mat image)
{
    cv::Rect textRect;

    // 检查输入图像的通道数，如果不是 3 则进行通道数转换
    if (image.channels() == 1) {
        cv::cvtColor(image, image, cv::COLOR_GRAY2BGR);
    }

    // 将图像转换为HSV色彩空间
    cv::Mat hsv;
    cv::cvtColor(image, hsv, cv::COLOR_BGR2HSV);

    // 定义颜色阈值范围，这里使用蓝色文字的范围作为示例
    cv::Scalar lower_blue(100, 50, 50);
    cv::Scalar upper_blue(140, 255, 255);

    // 对图像进行颜色阈值分割
    cv::Mat mask;
    cv::inRange(hsv, lower_blue, upper_blue, mask);

    // 对二值化后的图像进行形态学操作，例如腐蚀和膨胀，以去除噪声和连接断裂的文字区域
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
    cv::erode(mask, mask, kernel);
    cv::dilate(mask, mask, kernel);

    // 查找轮廓
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    // 遍历轮廓，找到最大的矩形边界框作为文字区域
    double maxArea = 0;
    for (const auto& contour : contours) {
        double area = cv::contourArea(contour);
        if (area > maxArea) {
            maxArea = area;
            textRect = cv::boundingRect(contour);
        }
    }

//    // 获取透视变换矩阵
//    cv::Mat perspectiveMatrix = CreatePerspectiveMatrix();

//    // 应用透视变换
//    cv::Mat warpedImage;
//    cv::warpPerspective(image, warpedImage, perspectiveMatrix, image.size());

    return textRect;
}

//cv::Mat IDCardOCR::CreatePerspectiveMatrix()
//{
//    // 创建一个 3x3 的透视变换矩阵
//    cv::Mat perspectiveMatrix = cv::Mat::zeros(3, 3, CV_32F);

//    // 设置透视变换矩阵的值
//    perspectiveMatrix.at<float>(0, 0) = 1.0f;
//    perspectiveMatrix.at<float>(1, 1) = 1.0f;
//    perspectiveMatrix.at<float>(2, 2) = 1.0f;

//    // 设置透视变换矩阵的其他值，例如平移和缩放参数


//    return perspectiveMatrix;
//}

int test(cv::Mat img)
{
    // 预处理：转换为灰度图像并进行二值化
    cv::Mat gray;
    cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
    cv::threshold(gray, gray, 128, 255, cv::THRESH_BINARY);

    // 轮廓检测
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(gray, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    std::vector<double> contourAreas; // 存储轮廓的面积
    for (size_t i = 0; i < contours.size(); i++) {
        double area = cv::contourArea(contours[i]); // 计算轮廓面积
        contourAreas.push_back(area); // 将面积保存到向量中
    }

//    // 根据面积对轮廓进行排序，从大到小
//    std::sort(contourAreas.begin(), contourAreas.end(), std::greater<double>());
//    if (contourAreas.size() >= 2) {
//        double secondLargestArea = contourAreas[1]; // 获取面积第二大的轮廓面积
//        for (size_t i = 0; i < contours.size(); i++) {
//            if (cv::contourArea(contours[i]) == secondLargestArea) {
//                // 获取面积第二大的轮廓
//                // 绘制轮廓
//                cv::Mat result = img.clone();
//                cv::drawContours(result, contours, i, cv::Scalar(0, 0, 255), 2);
//                // 显示结果
//                cv::imshow("Result", result);
//                cv::waitKey(0);

//                break;
//            }
//        }
//    }

    // 绘制轮廓
    cv::Mat result = img.clone();
    for (size_t i = 0; i < contours.size(); i++)
    {
        cv::Scalar color = cv::Scalar(0, 255, 0); // 轮廓线的颜色
        cv::drawContours(result, contours, i, color, 2, cv::LINE_8, hierarchy, 0);
    }
    // 显示结果
    cv::imshow("Result", result);
    cv::waitKey(0);

    return 0;
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
    //test(image);
    // 文字区域检测
    //cv::Rect textRect = DetectTextRegion(image);

    // 文字识别
//    cv::Mat textImage = image(textRect).clone();
//    cv::cvtColor(textImage, textImage, cv::COLOR_BGR2GRAY);

    // 将 OpenCV 图像转换为 Leptonica 的 Pix 格式
//    Pix *pix = pixCreate(image.size().width, image.size().height, 8);
//    int wpl = pixGetWpl(pix);
//    for (int y = 0; y < image.rows; y++) {
//        l_uint32 *line = pixGetData(pix) + y * wpl;
//        for (int x = 0; x < image.cols; x++) {
//            l_uint8 *pixel = reinterpret_cast<l_uint8*>(image.data + y * image.step + x * image.elemSize());
//            SET_DATA_BYTE(line, x, pixel[0]);
//        }
//    }

//    // 设置要识别的图像
//    ocr->SetImage(pix);

//    // 进行识别
//    char *text = ocr->GetUTF8Text();
//    QString result = QString::fromUtf8(text);

//    // 释放内存
//    pixDestroy(&pix);
//    delete[] text;

    return "result";
}

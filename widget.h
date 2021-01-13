#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QFileDialog>
#include <QPixmap>
#include <QImage>
#include <complex>
#include <QChartView>
#include <QLineSeries>
#include <QMargins>
#include <QFileInfo>

namespace Ui {
class Widget;
}
QT_CHARTS_USE_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void on_pushButton_open_pic_clicked();
    void on_pushButton_operate_clicked();

private:
    Ui::Widget *ui;

    QImage *sourceimage;
    QVector<QRgb>  colorTable;
    QVector<int> brightColArray;//分割图像的各个边界位置，不区分起始和终止位置
    QVector<QImage> imglist;//分割得到的8张图片

    QImage* Gray(QImage* img);
    QImage* Binary(QImage* img);
    void Split(QImage *img);
    void Match();
    QImage* Scale(QImage* img);
    QList<int> HashValue(QImage* img);
    int Distance(QList<int> image,QList<int> tem);
};

#endif // WIDGET_H

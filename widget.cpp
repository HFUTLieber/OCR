#include "widget.h"
#include "ui_widget.h"
#include <QDebug>

#define Threshold 160

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    for(int k=0;k<256;++k)
    {
        colorTable.push_back( qRgb(k,k,k) );
    }
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_pushButton_open_pic_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this,"选择图片","","*.jpg *.png *bmp *.jpeg");
    if(filename.isEmpty())
    {
        return;
    }
    sourceimage = new QImage(filename);
    QPixmap sourcemap = QPixmap::fromImage(*sourceimage);
    ui->label_source->setPixmap(sourcemap.scaled(ui->label_source->size()));
}

void Widget::on_pushButton_operate_clicked()
{
    QImage *tmp = Gray(sourceimage);
    tmp = Binary(tmp);
    Split(tmp);
    Match();
}

QImage *Widget::Gray(QImage *img)
{
    QImage * binaryimg = new QImage(img->width(), img->height(), QImage::Format_Indexed8);

    binaryimg->setColorTable(colorTable);

    QColor oldColor;

    for(int x = 0; x<binaryimg->width(); x++)
    {
        for(int y = 0; y<binaryimg->height(); y++)
        {
            oldColor = QColor(img->pixel(x,y));
            uint average = (oldColor.red()+oldColor.green()+oldColor.blue())/3;
            binaryimg->setPixel(x,y,average);
        }
    }
    return binaryimg;
}

QImage *Widget::Binary(QImage *img)
{
    QLineSeries *series = new QLineSeries();

    for(int x = 0; x<img->width(); x++)
    {
        int brightCount=0;
        for(int y = 0; y<img->height(); y++)
        {
            uint average = img->pixelIndex(x,y);
            (average>Threshold)?(average=255):(average=0);//二值化
            if(average==255)
                brightCount++;

            img->setPixel(x,y,average);
        }
        series->append(x,brightCount);
        brightColArray.append(brightCount);//列统计亮度分布，横向分割图片
    }

    QChart *chart = new QChart();
    chart->legend()->hide();
    chart->addSeries(series);
    chart->createDefaultAxes();
    chart->setTitle("纵向灰度直方图");
    chart->setMargins(QMargins(0,0,0,0));

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setParent(ui->frame_brightHistogram);
    chartView->setGeometry(0,0,595,305);
    chartView->show();

    QPixmap tarmap = QPixmap::fromImage(*img);
    ui->label_source->setPixmap(tarmap.scaled(ui->label_source->size()));

    return img;
}

void Widget::Split(QImage *img)
{
    //分割图片，理论上应有8张图片
    QVector<int> startArray;//8个分割起点的位置
    QVector<int> endArray;//8个分割终点的位置
    //获得每张图边界
    for(int i=1;i<img->width();i++)
    {
        if((brightColArray.at(i-1)==0)&&(brightColArray.at(i)>0))
        {
            startArray.append(i);
        }
        else if((brightColArray.at(i-1)>0)&&(brightColArray.at(i)==0))
        {
            endArray.append(i-1);
        }
    }

    //分割
    imglist.clear();
    for(int k=0;k<startArray.size();k++)
    {
        imglist.append(img->copy(startArray.at(k),0,endArray.at(k)-startArray.at(k)+1,img->height()));
    }
    imglist.at(1).save("D:/F.jpg");
//    ui->label->setPixmap(QPixmap::fromImage(imglist.at(0)).scaled(ui->label->size()));
//    ui->label_2->setPixmap(QPixmap::fromImage(imglist.at(1)).scaled(ui->label->size()));
//    ui->label_3->setPixmap(QPixmap::fromImage(imglist.at(2)).scaled(ui->label->size()));
//    ui->label_4->setPixmap(QPixmap::fromImage(imglist.at(3)).scaled(ui->label->size()));
//    ui->label_5->setPixmap(QPixmap::fromImage(imglist.at(4)).scaled(ui->label->size()));
//    ui->label_6->setPixmap(QPixmap::fromImage(imglist.at(5)).scaled(ui->label->size()));
//    ui->label_7->setPixmap(QPixmap::fromImage(imglist.at(6)).scaled(ui->label->size()));
//    ui->label_8->setPixmap(QPixmap::fromImage(imglist.at(7)).scaled(ui->label->size()));
    ui->label->setPixmap(QPixmap::fromImage(imglist.at(0)));
    ui->label_2->setPixmap(QPixmap::fromImage(imglist.at(1)));
    ui->label_3->setPixmap(QPixmap::fromImage(imglist.at(2)));
    ui->label_4->setPixmap(QPixmap::fromImage(imglist.at(3)));
    ui->label_5->setPixmap(QPixmap::fromImage(imglist.at(4)));
    ui->label_6->setPixmap(QPixmap::fromImage(imglist.at(5)));
    ui->label_7->setPixmap(QPixmap::fromImage(imglist.at(6)));
    ui->label_8->setPixmap(QPixmap::fromImage(imglist.at(7)));
}

void Widget::Match()
{
    ui->lineEdit->clear();

    for(int n=0;n<imglist.size();++n)
    {
        if(n==2)//车牌里的•
        {
            ui->lineEdit->setText(ui->lineEdit->text().append("•"));
            continue;
        }

        QImage * aImg = new QImage(imglist.at(n));

        QDir templateDir;
        if(n==0)
            templateDir.setPath("D:/temp/Location");
        else if(n==1)
            templateDir.setPath("D:/temp/Letter");
        else
            templateDir.setPath("D:/temp/Nb&Letter");


        QStringList theDirFilter;
        theDirFilter<<"*.bmp"<<"*.png"<<"*.jpg";
        QFileInfoList aInfoList = templateDir.entryInfoList(theDirFilter,QDir::Files);

        QString tarName;
        int distance = 257;
        for(int m=0;m<aInfoList.size();++m)
        {
            QString aFileName = templateDir.absolutePath()+"/"+aInfoList.at(m).fileName();
            QImage * bImg = new QImage(aFileName);
            *bImg = bImg->convertToFormat(QImage::Format_Indexed8);
            bImg->setColorTable(colorTable);
            QColor oldColor;
            for(int x = 0; x<bImg->width(); x++)
            {
                for(int y = 0; y<bImg->height(); y++)
                {
                    oldColor = QColor(bImg->pixel(x,y));
                    uint average = oldColor.red();
                    (average>Threshold)?(average=255):(average=0);

                    bImg->setPixel(x,y,average);
                }
            }
            int tmp = Distance(HashValue(Scale(aImg)),HashValue(Scale(bImg)));
            if(tmp<distance)
            {
                distance = tmp;
                tarName = aInfoList.at(m).baseName();
            }
        }
        ui->lineEdit->setText(ui->lineEdit->text().append(tarName));
    }
}

QImage *Widget::Scale(QImage *img)
{
    *img = img->scaled(16,16);
    return img;
}

QList<int> Widget::HashValue(QImage *img)
{
    QList<int> value;
    int averageValue = 0;
    for(int i=0;i<img->width();++i)
    {
        for(int j=0;j<img->height();++j)
        {
            QColor aColor(img->pixel(i,j));
            averageValue += aColor.red();
        }
    }
    averageValue /= img->width()*img->height();
    for(int i=0;i<img->width();++i)
    {
        for(int j=0;j<img->height();++j)
        {
            QColor aColor(img->pixel(i,j));
            if(aColor.red()>averageValue)
                value.append(1);
            else
                value.append(0);
        }
    }
    return value;
}

int Widget::Distance(QList<int> image, QList<int> tem)
{
    Q_ASSERT(image.size()==tem.size());

    int distance=0;
    for(int s=0;s<image.size();++s)
    {
        if(image[s]!=tem[s])
            distance++;
    }
    return distance;
}

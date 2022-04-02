#ifndef IMAGEWIDGET_H
#define IMAGEWIDGET_H

#include <QWidget>
#include <QSplitter>
#include <QToolButton>
#include <QPushButton>
#include <QLabel>
#include <QProgressBar>
#include <QPainter>
#include <QPen>
#include <QHBoxLayout>
#include <QEvent>
#include <QVBoxLayout>
#include "ui_image_widget.h"
#include "stateTool.h"
#include "myimageshowitem.h"

#pragma once 
class ImageWidget : public QWidget
{
	Q_OBJECT

public:
	ImageWidget(QWidget *parent = 0);
	~ImageWidget();
	
	void initDialog();
	bool checkCamera();
	void showErrorCheck(int nItemID);
	void showMaxImage(int);
	Ui::ImageWidget ui;
protected:
	void resizeEvent(QResizeEvent *event);
public:
	bool bIsCarveWidgetShow;
signals:
	void signals_SetCameraStatus(int,int);
	void signals_showCarve();
	void signals_hideCarve();
public slots:
	void slots_showErrorImage(QImage*, int, int,double, int, int, QList<QRect>, int);
	void slots_imageItemDoubleClick(int );
	void slots_addError(int nCamSN,int nSignalNo,int nErrorType);
	void slots_turnImage();
	void slots_showCarve();

	void slots_showStartRefresh(int);
	void slots_showPrevious(int);
	void slots_showFollowing(int);

	void slots_showCheck(int nItemID);
	void slots_stopCheck(int nItemID);
	void slots_stopAllStressCheck();
	void slots_startCheck(int nItemID);
	//void slots_startAllStressCheck();
	void slots_startShow(int nItemID);
	void slots_stopShow(int nItemID);
	void slots_startShowAll();
	void slots_showOnlyCamera(int cameraId);

public:
	QImage *ImageError[CAMERA_MAX_COUNT];
	s_AlgImageLocInfo  sAlgImageLocInfo[CAMERA_MAX_COUNT];	

	QList<MyImageShowItem*> listImageShowItem;

	bool bIsShow[CAMERA_MAX_COUNT];//指示对应相机是否刷新图像，目前无作用
	bool bIsShowErrorImage[CAMERA_MAX_COUNT];//指示对应相机是否刷新图像，目前无作用
	bool bIsStopAllStessCheck;
	QPushButton *buttonTurnImage;
	QPushButton *buttonShowCarve;
private:
	QWidget *widgetContent;
	QWidget *widgetContentStess;
	QGridLayout *gridLayoutImage;
	QGridLayout *gridLayoutStressImage;
	int minwidgetContentWidth;
	int iSpacing;
	int iImagePage;
 	int iShownMode;			//显示模式：-1:全部显示。1~n：显示对应的相机。
	int iBmpItemWidth;
	int iBmpItemHeight;
	int widgetWidth;
	int widgetHeight;
	int iCamCount;
	int iImagePosition[CAMERA_MAX_COUNT];
	int nRow;
	QGridLayout *gridLayoutLeft;
	QGridLayout *gridLayoutMiddle;
	QGridLayout *gridLayoutRight;
};

#endif //IMAGEWIDGET_H
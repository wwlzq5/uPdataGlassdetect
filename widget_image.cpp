#include "widget_image.h"
#include "glasswaredetectsystem.h"

extern GlasswareDetectSystem *pMainFrm;
ImageWidget::ImageWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	iSpacing = 10;
	iShownMode = -1;
	iImagePage = 0;
	bIsCarveWidgetShow = false;
	bIsStopAllStessCheck = false;
	for (int i = 0; i < CAMERA_MAX_COUNT; i++)
	{
		bIsShow[i] = true;
		bIsShowErrorImage[i] = false;
		iImagePosition[i] = -1;
		ImageError[i] = NULL;
		sAlgImageLocInfo[i].sXldPoint.nColsAry = new int[BOTTLEXLD_POINTNUM];
		sAlgImageLocInfo[i].sXldPoint.nRowsAry = new int[BOTTLEXLD_POINTNUM];
	}
	if(pMainFrm->m_sSystemInfo.iCamCount==4)
	{
		nRow = 2;
	}else if(pMainFrm->m_sSystemInfo.iCamCount == 27 || pMainFrm->m_sSystemInfo.iCamCount == 24)
	{
		nRow = 3;
	}else{
		nRow = 2;
	}
	initDialog();
	checkCamera();
}
ImageWidget::~ImageWidget()
{
	for (int i = 0; i < CAMERA_MAX_COUNT; i++)
	{
		delete[] sAlgImageLocInfo[i].sXldPoint.nColsAry;
		delete[] sAlgImageLocInfo[i].sXldPoint.nRowsAry;
		if (ImageError[i] != NULL)
		{
			delete ImageError[i];
			ImageError[i] = NULL;
		}
	}
}
void ImageWidget::resizeEvent(QResizeEvent *event)
{
	widgetWidth = ui.scrollArea->geometry().width();
	widgetHeight = ui.scrollArea->geometry().height();

	int minItemHeight = (widgetHeight - nRow*iSpacing)/nRow;
	minwidgetContentWidth = 0;
	for (int i = 0; i < pMainFrm->m_sSystemInfo.iCamCount; i++)
	{
		int minItemWidth;
		if (pMainFrm->m_sCarvedCamInfo[i].m_iImageType == 0) //瓶身
		{
			minItemWidth = minItemHeight/4;
		}
		else if (pMainFrm->m_sCarvedCamInfo[i].m_iImageType == 1)
		{
			minItemWidth = minItemHeight/2;
		}
		else if (pMainFrm->m_sCarvedCamInfo[i].m_iImageType == 2)
		{
			minItemWidth = minItemHeight/4;
		}
		MyImageShowItem *imageShowItem = listImageShowItem.at(i);
		imageShowItem->setMinimumSize(minItemWidth,minItemHeight);
		if (0 == i%2 && i < pMainFrm->m_sSystemInfo.iRealCamCount)
		{
			minwidgetContentWidth += (iSpacing + minItemWidth);
		}
	}
	widgetContent->setMinimumSize(minwidgetContentWidth + iSpacing, widgetHeight);
	widgetContentStess->setMinimumSize(minwidgetContentWidth + iSpacing, widgetHeight);
}

void ImageWidget::initDialog()
{
	iCamCount = pMainFrm->m_sSystemInfo.iCamCount;
	buttonTurnImage = new QPushButton;
	QPixmap iconTurnImage(":/sysButton/turnImage");
	buttonTurnImage->setIcon(iconTurnImage);
	buttonTurnImage->setFixedSize(iconTurnImage.size());
	connect(buttonTurnImage, SIGNAL(clicked()), this, SLOT(slots_turnImage()));
	buttonShowCarve = new QPushButton;
	QPixmap iconShowCarve(":/sysButton/arrowright");
	buttonShowCarve->setIcon(iconShowCarve);
	buttonShowCarve->setFixedSize(iconTurnImage.size());
	connect(buttonShowCarve, SIGNAL(clicked()), this, SLOT(slots_showCarve()));
	ui.widget->setWidgetName(tr("Image"));
	ui.LayoutName->addWidget(buttonShowCarve);
	ui.LayoutName->addWidget(ui.widget->widgetName);
	ui.LayoutName->addStretch();
	ui.LayoutName->addWidget(buttonTurnImage);

	QWidget *WidgetScrollArea = new QWidget();
	widgetContent = new QWidget(WidgetScrollArea);
	widgetContentStess = new QWidget(WidgetScrollArea);
	ui.scrollArea->setWidget(WidgetScrollArea);

	QHBoxLayout *Contentlayout = new QHBoxLayout(WidgetScrollArea);
	Contentlayout->addWidget(widgetContent);
	Contentlayout->addWidget(widgetContentStess);
	Contentlayout->setSpacing(0);
	Contentlayout->setContentsMargins(0,0,0,0);
	gridLayoutImage = new QGridLayout(widgetContent);
	gridLayoutStressImage = new QGridLayout(widgetContentStess);
	int nNormalRow = 0;
	int nStressRow = 0;
	for (int i = 0; i < pMainFrm->m_sSystemInfo.iCamCount; i++)
	{
		MyImageShowItem *imageShowItem = new MyImageShowItem(this);
		imageShowItem->inital(i);
		listImageShowItem.append(imageShowItem);
		if(pMainFrm->m_sCarvedCamInfo[i].m_iStress)
		{
			if(pMainFrm->m_sSystemInfo.iCamCount > 24)
			{
				gridLayoutStressImage->addWidget(listImageShowItem[i],nStressRow%3,nStressRow/3,1,1);
			}else{
				gridLayoutStressImage->addWidget(listImageShowItem[i],nStressRow%2,nStressRow/2,1,1);
			}
			//gridLayoutImage->addWidget(listImageShowItem[i],nNormalRow%nRow,nNormalRow/nRow,1,1);//0.0  
			nStressRow++;
		}else{
			gridLayoutImage->addWidget(listImageShowItem[i],nNormalRow%nRow,nNormalRow/nRow,1,1);//0.0  
			nNormalRow++;
		}
	}
	/*gridLayoutImage->setSpacing(iSpacing);
	gridLayoutImage->setContentsMargins(iSpacing,iSpacing,iSpacing,iSpacing);

	gridLayoutStressImage->setSpacing(iSpacing);
	gridLayoutStressImage->setContentsMargins(iSpacing,iSpacing,iSpacing,iSpacing);*/

	widgetContentStess->setVisible(false);
	for (int i = 0;i<pMainFrm->m_sSystemInfo.iCamCount;i++)
	{
		MyImageShowItem *imageShowItem = listImageShowItem.at(i);
		connect(imageShowItem,SIGNAL(signals_imageItemDoubleClick(int ) ),this,SLOT(slots_imageItemDoubleClick(int )));
		connect(imageShowItem,SIGNAL(signals_showStartRefresh(int)),this,SLOT(slots_showStartRefresh(int)));
		connect(imageShowItem,SIGNAL(signals_showPrevious(int)),this,SLOT(slots_showPrevious(int)));
		connect(imageShowItem,SIGNAL(signals_showFollowing(int)),this,SLOT(slots_showFollowing(int)));
		connect(imageShowItem,SIGNAL(signals_showCheck(int)),this,SLOT(slots_showCheck(int)));
		connect(imageShowItem,SIGNAL(signals_stopCheck(int)),this,SLOT(slots_stopCheck(int)));
		connect(imageShowItem,SIGNAL(signals_stopAllStressCheck()),this,SLOT(slots_stopAllStressCheck()));
		connect(imageShowItem,SIGNAL(signals_startCheck(int)),this,SLOT(slots_startCheck(int)));
		connect(imageShowItem,SIGNAL(signals_startShow(int)),this,SLOT(slots_startShow(int)));
		connect(imageShowItem,SIGNAL(signals_stopShow(int)),this,SLOT(slots_stopShow(int)));
		connect(imageShowItem,SIGNAL(signals_startShowAll()),this,SLOT(slots_startShowAll()));
		connect(pMainFrm->pdetthread[i],SIGNAL(signals_updateImage(QImage*, QString, QString, QString ,QString, QString, QList<QRect>,int )),imageShowItem,SLOT(slots_updateImage(QImage*, QString , QString ,QString , QString, QString, QList<QRect>,int )));
		connect(pMainFrm->pdetthread[i], SIGNAL(signals_AddErrorTableView(int,int,int)), this, SLOT(slots_addError(int,int,int)));
	}
}
//检查对应相机是否正确初始化
bool ImageWidget::checkCamera()
{
	bool bRet = true;
	for (int i = 0; i < pMainFrm->m_sSystemInfo.iCamCount; i++)
	{
		MyImageShowItem *imageShowItem = listImageShowItem.at(i);
		if (!pMainFrm->m_sRealCamInfo[i].m_bCameraInitSuccess)
		{
			imageShowItem->slots_showErrorInfo(pMainFrm->m_sRealCamInfo[i].m_strErrorInfo);
			bRet = false;
		}
		else
		{
			imageShowItem->slots_clearErrorInfo();
			emit signals_SetCameraStatus(i,1);
		}
	}
	return true;
}
void ImageWidget::slots_turnImage()
{
	if (iShownMode != -1) //显示全部图像
	{
		slots_imageItemDoubleClick(iShownMode);
	}
	if (iImagePage == 0)
	{
		widgetContent->setVisible(false);
		widgetContentStess->setVisible(true);
		iImagePage = 1;
	}
	else
	{
		widgetContentStess->setVisible(false);
		widgetContent->setVisible(true);
		iImagePage = 0;
	}
}
void ImageWidget::slots_showCarve()
{
	if (!bIsCarveWidgetShow)
	{
		QPixmap iconShowCarve(":/sysButton/arrowLeft");
		buttonShowCarve->setIcon(iconShowCarve);
		emit signals_showCarve();
		bIsCarveWidgetShow = true;
	}
	else
	{
		QPixmap iconHideCarve(":/sysButton/arrowright");
		buttonShowCarve->setIcon(iconHideCarve);
		emit signals_hideCarve();
		bIsCarveWidgetShow = false;
	}
}

void ImageWidget::slots_imageItemDoubleClick(int iCameraNo)
{	
	if (iShownMode != -1) //显示全部图像
	{
		widgetContent->setMinimumWidth(minwidgetContentWidth);
		for (int i = 0; i < pMainFrm->m_sSystemInfo.iCamCount; i++)
		{
			MyImageShowItem *imageShowItem = listImageShowItem.at(i);
			imageShowItem->setVisible(true);
			imageShowItem->setMaxShow(false);
		}
		iShownMode = -1;
	}else //只显示双击图像
	{
		widgetContent->setMinimumWidth(0);
		for (int i = 0; i < pMainFrm->m_sSystemInfo.iCamCount; i++)
		{
			MyImageShowItem *imageShowItem = listImageShowItem.at(i);
			if (iCameraNo == i)
			{
				imageShowItem->setVisible(true); 
				imageShowItem->setMaxShow(true);
			}
			else
			{
				if (iCameraNo != -1)
				{
					imageShowItem->setVisible(false);
				}
			}
		}
		iShownMode = iCameraNo;
	}
}
void ImageWidget::showMaxImage(int iCameraNo)
{
	if(pMainFrm->m_sCarvedCamInfo[iCameraNo].m_iStress)
	{
		widgetContent->setVisible(false);
		widgetContentStess->setVisible(true);
		iImagePage = 1;
	}else{
		widgetContentStess->setVisible(false);
		widgetContent->setVisible(true);
		iImagePage = 0;
	}

	widgetContent->setMinimumWidth(0);
	for (int i = 0; i < pMainFrm->m_sSystemInfo.iCamCount; i++)
	{
		MyImageShowItem *imageShowItem = listImageShowItem.at(i);
		if (iCameraNo == i)
		{
			imageShowItem->setVisible(true); 
			imageShowItem->setMaxShow(true);
		}
		else
		{
			if (iCameraNo != -1)
			{
				imageShowItem->setVisible(false);
			}
		}
	}
}
void ImageWidget::slots_showPrevious(int nItemID)
{
	bool bFound = false;
	CGrabElement* pElement;

	pMainFrm->m_ErrorList.m_mutexmErrorList.lock();

	for (int i = iImagePosition[nItemID]+1; i < pMainFrm->m_ErrorList.listError.length(); i++)
	{
		pElement = pMainFrm->m_ErrorList.listError.at(i);
		if (pElement->nCamSN == nItemID)
		{
			iImagePosition[nItemID] = i;
			bFound = true;
			pMainFrm->m_SavePicture[nItemID].pThat=pElement->myImage;
			pMainFrm->m_SavePicture[nItemID].m_Picture=pElement->myImage->copy();
			break;
		}
	}
	pMainFrm->m_ErrorList.m_mutexmErrorList.unlock();

	if (bFound)
	{
		slots_stopShow(nItemID);
		QString camera = QString::number(pElement->nCamSN+1);
		QString imageSN = QString::number(pElement->nSignalNo);
		QString time = QString::number(pElement->dCostTime,'f',2);
		QString result = pMainFrm->m_sErrorInfo.m_vstrErrorType.at(pElement->nCheckRet);
		QString mouldID = QString::number(pElement->nMouldID);
		Q_ASSERT(pElement->myImage->byteCount() == pElement->myImage->height() * pElement->myImage->width());
		listImageShowItem.at(nItemID)->updateImage(pElement->myImage, camera, imageSN, time, result, mouldID, pElement->cErrorRectList);
		sAlgImageLocInfo[nItemID].sLocOri = pElement->sImgLocInfo.sLocOri;
		sAlgImageLocInfo[nItemID].sXldPoint.nCount = pElement->sImgLocInfo.sXldPoint.nCount;
		memcpy(sAlgImageLocInfo[nItemID].sXldPoint.nColsAry,pElement->sImgLocInfo.sXldPoint.nColsAry,4*BOTTLEXLD_POINTNUM);
		memcpy(sAlgImageLocInfo[nItemID].sXldPoint.nRowsAry,pElement->sImgLocInfo.sXldPoint.nRowsAry,4*BOTTLEXLD_POINTNUM);
		//new一个设置算法用的QImage
		if (ImageError[nItemID] != NULL)
		{
			delete ImageError[nItemID];
			ImageError[nItemID] = NULL;
		}
		ImageError[nItemID] = new QImage(*pElement->myImage);

	}
	else if (iImagePosition[nItemID] == -1)
	{
		QString strError = QString(tr("No error Image! "));
		listImageShowItem.at(nItemID)->slots_showWarningInfo(strError);
		return;
	}
	else
	{
		QString strError = QString(tr("Alreay the final Image! "));
		listImageShowItem.at(nItemID)->slots_showWarningInfo(strError);
		return;
	}
}
void ImageWidget::slots_showFollowing(int nItemID)
{
	bool bFound = false;
	CGrabElement* pElement;
	if (iImagePosition[nItemID] == -1)
	{
		iImagePosition[nItemID] = ERROR_IMAGE_COUNT;
	}
	for (int i = iImagePosition[nItemID]-1; i >=0; i--)
	{
		if (i < 0)
		{
			continue;
		}
		pElement = pMainFrm->m_ErrorList.listError.at(i);
		if (pElement->nCamSN == nItemID)
		{
			iImagePosition[nItemID] = i;
			bFound = true;
			pMainFrm->m_SavePicture[nItemID].pThat=pElement->myImage;
			pMainFrm->m_SavePicture[nItemID].m_Picture=pElement->myImage->copy();;
			break;
		}
	}
	if (bFound)
	{
		slots_stopShow(nItemID);

		QString camera = QString::number(pElement->nCamSN+1);
		QString imageSN = QString::number(pElement->nSignalNo);
		QString time = QString::number(pElement->dCostTime,'f',2);
		QString result = pMainFrm->m_sErrorInfo.m_vstrErrorType.at(pElement->nCheckRet);
		QString mouldID = QString::number(pElement->nMouldID);

		Q_ASSERT(pElement->myImage->byteCount() == pElement->myImage->height() * pElement->myImage->width());

		listImageShowItem.at(nItemID)->updateImage(pElement->myImage, camera, imageSN, time, result, mouldID, pElement->cErrorRectList);
		sAlgImageLocInfo[nItemID].sLocOri = pElement->sImgLocInfo.sLocOri;
		sAlgImageLocInfo[nItemID].sXldPoint.nCount = pElement->sImgLocInfo.sXldPoint.nCount;
		memcpy(sAlgImageLocInfo[nItemID].sXldPoint.nColsAry,pElement->sImgLocInfo.sXldPoint.nColsAry,4*BOTTLEXLD_POINTNUM);
		memcpy(sAlgImageLocInfo[nItemID].sXldPoint.nRowsAry,pElement->sImgLocInfo.sXldPoint.nRowsAry,4*BOTTLEXLD_POINTNUM);
		//new一个设置算法用的QImage
		if (ImageError[nItemID] != NULL)
		{
			delete ImageError[nItemID];
			ImageError[nItemID] = NULL;
		}
		ImageError[nItemID] = new QImage(*pElement->myImage);


	}else if (iImagePosition[nItemID] == ERROR_IMAGE_COUNT)
	{
		QString strError = QString(tr("No error Image! "));
		listImageShowItem.at(nItemID)->slots_showWarningInfo(strError);
		iImagePosition[nItemID] = -1;
		return;
	}
	else
	{
		QString strError = QString(tr("Alreay the first Image! "));
		listImageShowItem.at(nItemID)->slots_showWarningInfo(strError);
		return;
	}
}
void ImageWidget::slots_showStartRefresh(int nItemID)
{
	bIsShow[nItemID] = true;
	iImagePosition[nItemID] = -1;
	bIsShowErrorImage[nItemID] = false;
}
void ImageWidget::slots_showErrorImage(QImage* ImageShown, int iCamera, int iSignalNo,double iCostTime, int nMouldID, int iErrorType, QList<QRect> listRect, int listNo)
{
	slots_stopShow(iCamera);
	QString camera = QString::number(iCamera+1);
	QString imageSN = QString::number(iSignalNo);
	QString time = QString::number(iCostTime,'f',2);
	QString result = pMainFrm->m_sErrorInfo.m_vstrErrorType.at(iErrorType);
	QString mouldID = QString::number(nMouldID);
	iImagePosition[iCamera] = listNo;
	listImageShowItem.at(iCamera)->updateImage(ImageShown, camera, imageSN, time, result, mouldID, listRect);
	if (ImageError[iCamera] != NULL)
	{
		delete ImageError[iCamera];
		ImageError[iCamera] = NULL;
	}
	ImageError[iCamera] = new QImage(*ImageShown);
}
void ImageWidget::slots_showCheck(int nItemID)
{
	pMainFrm->slots_turnPage(3, nItemID);
}

void ImageWidget::showErrorCheck(int nItemID)
{
	if (nItemID >= pMainFrm->m_sSystemInfo.iCamCount)
	{
		return;
	}

	try
	{
		s_Status  sReturnStatus;
		QImage* tempIamge;
		if(pMainFrm->m_SavePicture[nItemID].pThat==NULL)
		{
			return;
		}
		tempIamge=pMainFrm->m_SavePicture[nItemID].pThat;
		sReturnStatus = pMainFrm->m_cBottleModel.CloseModelDlg();
		if (sReturnStatus.nErrorID != RETURN_OK)
		{
			pMainFrm->Logfile.write(tr("Abnormal in close model "),AbnormityLog);
			return;
		}

		s_AlgModelPara  sAlgModelPara;	
		sAlgModelPara.sImgPara.nChannel = 1;
		sAlgModelPara.sImgPara.nHeight = tempIamge->height();
		sAlgModelPara.sImgPara.nWidth = tempIamge->width();
		sAlgModelPara.sImgPara.pcData = (char*)tempIamge->bits();
		sAlgModelPara.sImgLocInfo = sAlgImageLocInfo[nItemID];

		if (sAlgModelPara.sImgPara.nHeight != pMainFrm->m_sCarvedCamInfo[nItemID].m_iImageHeight)
		{
			pMainFrm->Logfile.write(tr("Image height:%1 not fit camera height:%2").arg(sAlgModelPara.sImgPara.nHeight).arg(pMainFrm->m_sCarvedCamInfo[nItemID].m_iImageHeight),AbnormityLog);
			return;
		}
		if (sAlgModelPara.sImgPara.nWidth != pMainFrm->m_sCarvedCamInfo[nItemID].m_iImageWidth)
		{
			pMainFrm->Logfile.write(tr("Image Width:%1 not fit camera Width:%2").arg(sAlgModelPara.sImgPara.nWidth).arg(pMainFrm->m_sCarvedCamInfo[nItemID].m_iImageWidth),AbnormityLog);
			return;
		}		
		for (int i=0;i<pMainFrm->m_sSystemInfo.iCamCount;i++)
		{
			pMainFrm->CherkerAry.pCheckerlist[i].nID = i;
			pMainFrm->CherkerAry.pCheckerlist[i].pChecker = &pMainFrm->m_cBottleCheck[i];
		}	
		int widthd = pMainFrm->widget_alg->geometry().width();
		int heightd	= pMainFrm->widget_alg->geometry().height();
		if (widthd < 150 || heightd < 150)
		{
			pMainFrm->statked_widget->setCurrentWidget(pMainFrm->widget_alg);
			pMainFrm->statked_widget->setCurrentWidget(pMainFrm->widget_carveSetting);
			widthd = pMainFrm->widget_alg->geometry().width();
			heightd	= pMainFrm->widget_alg->geometry().height();

			if (widthd < 150 || heightd < 150)
			{
				pMainFrm->Logfile.write(tr("widget_alg size is too small width:%1 height:%2").arg(widthd).arg(heightd),AbnormityLog);
				return;
			}
		}	
		sReturnStatus = pMainFrm->m_cBottleModel.SetModelDlg(sAlgModelPara,
			&pMainFrm->m_cBottleCheck[nItemID],pMainFrm->CherkerAry,pMainFrm->widget_alg);
		if (sReturnStatus.nErrorID != RETURN_OK)
		{
			pMainFrm->Logfile.write(tr("Abnormal in set Model"),AbnormityLog);
			return;
		}
		pMainFrm->statked_widget->setCurrentWidget(pMainFrm->widget_alg);
		pMainFrm->m_eCurrentMainPage = AlgPage;
		pMainFrm->m_eLastMainPage = AlgPage;
		pMainFrm->iLastPage = 3;

	}
	catch (...)
	{
		pMainFrm->Logfile.write(tr("Abnormal in set Model "),AbnormityLog);

	}

	pMainFrm->Logfile.write(QString("Into Alg page(From failure image)")+QString("CameraNo:%1").arg(nItemID),OperationLog,0);

}
void ImageWidget::slots_stopCheck(int nItemID)
{
	//pMainFrm->Logfile.write(tr("Stop camera%1 Check").arg(nItemID+1),OperationLog);
	pMainFrm->m_sRunningInfo.m_bIsCheck[nItemID] = 0;
	CameraStatusLabel *cameraStatus = pMainFrm->cameraStatus_list.at(nItemID);
	pMainFrm->m_cCombine.SetCombineCamera(nItemID,false);
	emit signals_SetCameraStatus(nItemID,2);
}
void ImageWidget::slots_stopAllStressCheck()
{
	if (!bIsStopAllStessCheck)
	{
		bIsStopAllStessCheck = true;
		for (int i=0; i<pMainFrm->m_sSystemInfo.iCamCount; i++)
		{
			if (1 == pMainFrm->m_sCarvedCamInfo[i].m_iStress)
			{
				slots_stopCheck(i);
				listImageShowItem.at(i)->bIsCheck = false;
				emit signals_SetCameraStatus(i,2);
			}
		}
	}
	else
	{
		bIsStopAllStessCheck = false;
		for (int i=0; i<pMainFrm->m_sSystemInfo.iCamCount; i++)
		{
			if (1 == pMainFrm->m_sCarvedCamInfo[i].m_iStress)
			{
				slots_startCheck(i);
				listImageShowItem.at(i)->bIsCheck = true;
				emit signals_SetCameraStatus(i,0);
			}
		}
	}
}
void ImageWidget::slots_startCheck(int nItemID)
{
	pMainFrm->m_sRunningInfo.m_bIsCheck[nItemID] = 1;
	pMainFrm->m_cCombine.SetCombineCamera(nItemID,true);
	emit signals_SetCameraStatus(nItemID,0);
}

void ImageWidget::slots_startShowAll()
{
	for (int i = 0; i < pMainFrm->m_sSystemInfo.iCamCount;i++)
	{
		if (!bIsShow[i])	
		{
			bIsShow[i] = true;
			listImageShowItem.at(i)->slots_startShow();
			MyImageShowItem *imageShowItem = listImageShowItem.at(i);
		}
		bIsShowErrorImage[i] = false;
	}
}

void ImageWidget::slots_startShow(int nItemID)
{
	bIsShow[nItemID] = true;
	bIsShowErrorImage[nItemID] = false;
	listImageShowItem.at(nItemID)->slots_startShow();
	MyImageShowItem *imageShowItem = listImageShowItem.at(nItemID);
}
void ImageWidget::slots_stopShow(int nItemID)
{
	bIsShowErrorImage[nItemID] = true;
	bIsShow[nItemID] = false;
	listImageShowItem.at(nItemID)->slots_stopShow();
	MyImageShowItem *imageShowItem = listImageShowItem.at(nItemID);
	if (ImageError[nItemID] != NULL)
	{
		delete ImageError[nItemID];
		ImageError[nItemID] = NULL;
	}
	if (pMainFrm->nQueue[nItemID].listGrab.size() <= 0)
	{
		return;
	}
	CGrabElement *pElement = pMainFrm->nQueue[nItemID].listGrab.last();
	ImageError[nItemID] = new QImage(*pElement->myImage);
}
//加入新错误图像时，当前错误图像位置加1。
void ImageWidget::slots_addError(int nItemID,int nSignalNo,int nErrorType)
{
	for (int i=0; i < pMainFrm->m_sSystemInfo.iCamCount; i++)
	{
		if (iImagePosition[i] != -1)
		{
			if (iImagePosition[i] < ERROR_IMAGE_COUNT - 1)
			{
				iImagePosition[i]++;
			}
		}
	}
}

void ImageWidget::slots_showOnlyCamera(int cameraId)
{
	if(pMainFrm->m_sSystemInfo.iCamCount == pMainFrm->m_sSystemInfo.iRealCamCount)
	{
		widgetContentStess->setVisible(false);
		widgetContent->setVisible(true);
		iImagePage = 0;
	}else{
		if(pMainFrm->m_sCarvedCamInfo[cameraId].m_iStress)
		{
			widgetContent->setVisible(false);
			widgetContentStess->setVisible(true);
			iImagePage = 1;
		}else{
			widgetContentStess->setVisible(false);
			widgetContent->setVisible(true);
			iImagePage = 0;
		}
	}
}

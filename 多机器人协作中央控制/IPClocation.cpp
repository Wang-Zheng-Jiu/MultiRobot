/*
IPClocation
˵  ����һ����RTSP��ַIP����ͷ�����ж�λ�Ŀ⣬�ṩ�˶�λ�ӿ�
�����ṩ��һ����Ϣ��IPCobj��������Ϊ��Ϣ������洢���ڳ������ж���
���岢�Ҹ���������λ�á�
�����ˣ�����ǿ
��  ����beta 0.3
��  �ģ�
	1��ɾ���˻������ģ���࣬�Լ��������
	2���޸�setWorld��������Ϊfalse������
	3�������delayTime�������洢��λϵͳ����ʱʱ�䡣���Ҽ���xml
	4����ӻ������ĺ������ܣ���������������Ҫ���ƣ��Ӵ���ӻ���
	5��ΪIPCmsg�������һ������������Ҫ������Ƶ����ͬʱ������̷߳��ʡ�
	6�����л�UpdateXMLfile�Ⱥ�����
*/

#include "stdafx.h"
#include "IPClocation.h"
#include "zfun.h"
#include "Eigen\Eigen"

using namespace std;





IPCobj::IPCobj()
{
}

/*-----------------------------------------------------------------------------
						IPCmsg�����ӿڶ���
							class
------------------------------------------------------------------------------*/

//��ʼ������Ҫ��ʼ�������С
IPCmsg::IPCmsg()
{
	cameraMatrix= Mat(3, 3, CV_32FC1, Scalar::all(0));
	distCoeffs = Mat(1, 5, CV_32FC1, Scalar::all(0));
	RwMatrix = Mat(3, 3, CV_32FC1, Scalar::all(0));
	TwVec = Mat(3, 1, CV_32FC1, Scalar::all(0));
	Status = NOSTAR;
}

IPCmsg::IPCmsg(String addr)
{
	cameraMatrix = Mat(3, 3, CV_32FC1, Scalar::all(0));
	distCoeffs = Mat(1, 5, CV_32FC1, Scalar::all(0));
	RwMatrix = Mat(3, 3, CV_32FC1, Scalar::all(0));
	TwVec = Mat(3, 1, CV_32FC1, Scalar::all(0));
	Status = NOSTAR;
	rtsp = addr;
}
bool IPCmsg::Open()
{
	bool ret=cap.open(rtsp);
	if (ret == false)
	{
		Status = CANNOT_OPEN;
	}
	else
	{
		Status = NORMAL;
	}
	return ret;
}
IPCmsg::StatusEnum IPCmsg::checkStatus()
{
	return Status;
}
void IPCmsg::updateRwMatrixI()
{
	invert(RwMatrix, RwMatrixI);
}


/*-----------------------------------------------------------------------------
					IPClocation�����ӿڶ���
							class
------------------------------------------------------------------------------*/
IPClocation::IPClocation()
{

}


bool IPClocation::AddIPC(string filedir, cv::Size board_size, cv::Size square_size, cv::String rtsp,double err_th)
{
	//�鿴�Ƿ���rtsp�ظ������򷵻ش���
	vector<string> allipcrtsp=getIPCrtsp();
	if (zfun::findVecterElm(allipcrtsp, rtsp) != -1)
	{
		printf("error:IPC�д�����ͬrtsp��ַ��IP���");
		return false;
	}

	//�궨���
	Mat cameraMatrix;
	Mat distCoeffs; 
	vector<Mat> rvecsMat; 
	vector<Mat> tvecsMat;
	
	int image_count = 0;                                            // ͼ������ 
	Size image_size;                                                // ͼ��ĳߴ� 

	std::vector<Point2f> image_points;                              // ����ÿ��ͼ���ϼ�⵽�Ľǵ�
	std::vector<std::vector<Point2f>> image_points_seq;             // �����⵽�����нǵ�

	//����ļ����µ�����jpgͼƬ
	std::vector<std::string> FilesName;//�洢����ͼƬ�ļ�
	string FileType = ".jpg";
	std::string buffer = filedir + "\\*" + FileType;
	_finddata_t c_file;   // ����ļ����Ľṹ��
	long long hFile;
	hFile = _findfirst(buffer.c_str(), &c_file);   //�ҵ�һ���ļ���
	if (hFile == -1L)   // ����ļ���Ŀ¼�´�����Ҫ���ҵ��ļ�
		printf("No %s files in current directory!\n", FileType);
	else
	{
		string fullFilePath;
		do
		{
			fullFilePath.clear();
			//����
			fullFilePath = filedir + "\\" + c_file.name;
			FilesName.push_back(fullFilePath);
		} while (_findnext(hFile, &c_file) == 0);  //����ҵ��¸��ļ������ֳɹ��Ļ��ͷ���0,���򷵻�-1  
		_findclose(hFile);
	}

	//���ǵ�
	for (int i = 0; i < FilesName.size(); i++)
	{
		image_count++;

		// ���ڹ۲�������
		cout << "image_count = " << image_count << endl;
		Mat imageInput = imread(FilesName[i]);
		if (image_count == 1)  //�����һ��ͼƬʱ��ȡͼ������Ϣ
		{
			image_size.width = imageInput.cols;
			image_size.height = imageInput.rows;
			cout << "image_size.width = " << image_size.width << endl;
			cout << "image_size.height = " << image_size.height << endl;
		}

		/* ��ȡ�ǵ� */
		bool ok = findChessboardCorners(imageInput, board_size, image_points, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_NORMALIZE_IMAGE);
		if (0 == ok)
		{
			cout << "��" << image_count << "����Ƭ��ȡ�ǵ�ʧ�ܣ���ɾ�������±궨��" << endl; //�Ҳ����ǵ�
			
			return false;
		}
		else
		{
			Mat view_gray;
			cout << "imageInput.channels()=" << imageInput.channels() << endl;
			cvtColor(imageInput, view_gray, CV_RGB2GRAY);

			/* �����ؾ�ȷ�� */
			//find4QuadCornerSubpix(view_gray, image_points, Size(5, 5)); //�Դ���ȡ�Ľǵ���о�ȷ��
			cv::cornerSubPix(view_gray, image_points, cv::Size(11, 11), cv::Size(-1, -1), cv::TermCriteria(CV_TERMCRIT_ITER + CV_TERMCRIT_EPS, 20, 0.01));

			image_points_seq.push_back(image_points);  //���������ؽǵ�

													   /* ��ͼ������ʾ�ǵ�λ�� */
			drawChessboardCorners(view_gray, board_size, image_points, true);

			//imshow("Camera Calibration", view_gray);//��ʾͼƬ
			//waitKey(100);//��ͣ0.1S     
		}
	}
	cout << "�ǵ���ȡ��ɣ�����" << endl;


	/*������ά��Ϣ*/
	vector<vector<Point3f>> object_points_seq;                     // ����궨���Ͻǵ����ά����
	for (int t = 0; t < image_count; t++)
	{
		vector<Point3f> object_points;
		for (int i = 0; i < board_size.height; i++)
		{
			for (int j = 0; j < board_size.width; j++)
			{
				Point3f realPoint;
				/* ����궨�������������ϵ��z=0��ƽ���� */
				realPoint.x = i*square_size.width;
				realPoint.y = j*square_size.height;
				realPoint.z = 0;
				object_points.push_back(realPoint);
			}
		}
		object_points_seq.push_back(object_points);
	}

	/* ���б궨���� */

	double err_first = calibrateCamera(object_points_seq, image_points_seq, image_size, cameraMatrix, distCoeffs, rvecsMat, tvecsMat, CV_CALIB_FIX_K3);
	cout << "��ͶӰ���1��" << err_first << "����" << endl << endl;
	cout << "�궨��ɣ�����" << endl;

	//������̫���򷵻ش���
	if (err_first > err_th)
	{
		printf("error:���̫��");
		return false;
	}
	
	
	//���IPC
	IPCmsg newIPC(rtsp);
	newIPC.cameraMatrix = cameraMatrix;
	newIPC.distCoeffs = distCoeffs;
	IPC.push_back(newIPC);

	//���涨����    
	UpdateXMLfile();

	return true;
}

//��IPC�����¸��µ�xmlFILE�У�˽�к�����
void IPClocation::UpdateXMLfile()
{
	if (xmlfileName.empty() == true) return;
	FileStorage xml(xmlfileName, cv::FileStorage::WRITE);
	//rdxml << "IPC" << IPC;
	xml << "delayTime" << delayTime;
	xml << "IPCNum" << (int)IPC.size();
	for (size_t i = 0; i < IPC.size(); i++)
	{
		std::string indexc=std::to_string(i);
		xml << "r" + indexc << IPC[i].rtsp;
		xml << "M" + indexc << IPC[i].cameraMatrix;
		xml << "D" + indexc << IPC[i].distCoeffs;
		xml << "R" + indexc << IPC[i].RwMatrix;
		xml << "T" + indexc << IPC[i].TwVec;
	}
	xml.release();
}

void IPClocation::UpdateIPC()
{
	if (xmlfileName.empty() == true) return;
	FileStorage xml(xmlfileName, cv::FileStorage::READ);
	//���IPC���ر�����ͷ
	for (size_t i = 0; i < IPC.size(); i++)
	{
		IPC[i].cap.release();
	}
	IPC.clear();
	//��ȡIPCxml
	int IPCNum;
	xml["IPCNum"] >> IPCNum;
	xml["delayTime"] >> delayTime;
	for (size_t i = 0; i < IPCNum; i++)
	{
		IPCmsg newipc;
		std::string indexc = std::to_string(i);
		xml["r" + indexc] >> newipc.rtsp;
		xml["M" + indexc] >> newipc.cameraMatrix;
		xml["D" + indexc] >> newipc.distCoeffs;
		xml["R" + indexc] >> newipc.RwMatrix;
		xml["T" + indexc] >> newipc.TwVec;
		newipc.updateRwMatrixI();
		IPC.push_back(newipc);
	}

	xml.release();

}
  
//�õ�����IPC��rtsp
std::vector<std::string> IPClocation::getIPCrtsp()
{
	vector<string> outrtsp;
	for (int i = 0; i < IPC.size(); i++)
	{
		outrtsp.push_back(IPC[i].rtsp);
	}
	return outrtsp;
}

/*
*@brief����һ��xml�ļ�ͬ����ʵ����flash����,ֻ��û��xmlfileNameʱ������Ч��
*/
void IPClocation::bindxml(std::string xmlfile)
{
	xmlfileName = xmlfile;
	//�鿴�Ƿ����xml
	FileStorage tsxml(xmlfileName, cv::FileStorage::READ);
	if (tsxml.isOpened() == true)
	{
		tsxml.release();
		UpdateIPC();
	}
	else
	{
		tsxml.release();
		UpdateXMLfile();
	}
}

/*
*@brief��������������ϵ���������е�IPC������Ρ��������������IPC����ʱ�ų���
���д˺���ʱ��Ӧ�ñ�֤����IPC�ֲ������ܣ��ҳ�������һ���궨ARtag��������IPC��
�ܿ����˱궨ARtag��
*/
bool IPClocation::setWorld()
{
	//�������IP����Ƿ��ܴ򿪡�
	for (size_t ipci = 0; ipci < IPC.size(); ipci++)
	{
		if (IPC[ipci].Open() == false) return false;
	}
	//������IPC�ֱ���в����궨
	for (size_t ipci = 0; ipci < IPC.size(); ipci++)
	{
		//�������ϵ��λ����Ϣ
		vector< Vec3d > rvecs, tvecs;
		vector<int> ids;
		//�ֵ�
		Ptr<aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(ARtag_dict);
		//ͼƬ
		Mat image, imageCopy;
		//���������
		IPC[ipci].cap >> image;
		resize(image, imageCopy, Size(1280, 720));

		//��ʼ����
		vector<vector<Point2f> > corners;
		cv::aruco::detectMarkers(imageCopy, dictionary, corners, ids);
		// �����AR�룬�������̬����
		if (ids.size() > 0)
		{
			cv::aruco::estimatePoseSingleMarkers(corners, worldAR_size, IPC[ipci].cameraMatrix, IPC[ipci].distCoeffs, rvecs, tvecs);
		}
		else
		{
			return false;
		}
		//�����Ƿ��б궨AR��
		int orignum = zfun::findVecterElm(ids, worldAR_ID);//�궨tag�����ڵ�����
		//����б궨AR��
		if (orignum >= 0)
		{
			//�õ�����������ϵ��RT��
			Rodrigues(rvecs[orignum], IPC[ipci].RwMatrix);
			IPC[ipci].TwVec = Mat(tvecs[orignum], true);
			IPC[ipci].updateRwMatrixI();
		}
		else
		{
			return false;
		}

	}
	UpdateXMLfile();
	return true;
}

/*
*@brief���õ�ָ��IPC��ŵľ���AR�����ͼƬ
*/
Mat IPClocation::getIPCARPimage(int index)
{
	Mat img;
	IPC[index].cap >> img;
	resize(img, img, Size(1280, 720));
	//�������ϵ��λ����Ϣ
	vector<int> ids;
	double starttime = cv::getTickCount();//��ʱ

	vector<vector<Point2f> > corners;
	Ptr<aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(aruco::DICT_6X6_250);
	cv::aruco::detectMarkers(img, dictionary, corners, ids);
	// if at least one marker detected 
	if (ids.size() > 0)
	{
		
		vector< Vec3d > rvecsx, tvecsx;		//�洢С�Ż����˵���̬
		vector<int> idsx;					//����С�Ż����˵�ID
		vector<vector<Point2f> > cornersx;	//����С�Ż����˵Ľǵ�

		//ѡȡС�Ż����˵�id�ͽǵ�λ��
		for (size_t i = 0; i < ids.size(); i++)
		{
			if (ids[i] >= AR_ID_distribList[1][0] && ids[i] <= AR_ID_distribList[1][1])
			{
				//�ҵ���С�Ż�����id
				idsx.push_back(ids[i]);
				cornersx.push_back(corners[i]);
			}
		}
		if (idsx.size() > 0)
		{
			//����ЩС�Ż����˽�����̬����
			cv::aruco::estimatePoseSingleMarkers(cornersx, (float)AR_ID_distribList[1][2] / 1000.0, IPC[index].cameraMatrix, IPC[index].distCoeffs, rvecsx, tvecsx);
			//����
			cv::aruco::drawDetectedMarkers(img, cornersx, idsx);

			for (int i = 0; i < idsx.size(); i++)
				cv::aruco::drawAxis(img, IPC[index].cameraMatrix, IPC[index].distCoeffs, rvecsx[i], tvecsx[i], 0.1);
		}

		rvecsx.clear(); tvecsx.clear(); idsx.clear(); cornersx.clear();

		//ѡȡ��Ż����˵�id�ͽǵ�λ��
		for (size_t i = 0; i < ids.size(); i++)
		{
			if (ids[i] >= AR_ID_distribList[0][0] && ids[i] <= AR_ID_distribList[0][1])
			{
				//�ҵ���С�Ż�����id
				idsx.push_back(ids[i]);
				cornersx.push_back(corners[i]);
			}
		}
		if (idsx.size() > 0)
		{
			//����ЩС�Ż����˽�����̬����
			cv::aruco::estimatePoseSingleMarkers(cornersx, (float)AR_ID_distribList[0][2] / 1000.0, IPC[index].cameraMatrix, IPC[index].distCoeffs, rvecsx, tvecsx);
			//����
			cv::aruco::drawDetectedMarkers(img, cornersx, idsx);

			for (int i = 0; i < idsx.size(); i++)
				cv::aruco::drawAxis(img, IPC[index].cameraMatrix, IPC[index].distCoeffs, rvecsx[i], tvecsx[i], 0.1);
		}

		rvecsx.clear(); tvecsx.clear(); idsx.clear(); cornersx.clear();
		//�궨ARtag
		for (size_t i = 0; i < ids.size(); i++)
		{
			if (ids[i]== worldAR_ID)
			{
				//�ҵ���С�Ż�����id
				idsx.push_back(ids[i]);
				cornersx.push_back(corners[i]);
			}
		}
		if (idsx.size() > 0)
		{
			//����̬����
			cv::aruco::estimatePoseSingleMarkers(cornersx, worldAR_size, IPC[index].cameraMatrix, IPC[index].distCoeffs, rvecsx, tvecsx);
			//����
			cv::aruco::drawDetectedMarkers(img, cornersx, idsx);

			for (int i = 0; i < idsx.size(); i++)
				cv::aruco::drawAxis(img, IPC[index].cameraMatrix, IPC[index].distCoeffs, rvecsx[i], tvecsx[i], 0.5);
		}


	}
	return img;
}

/*
*@brief���õ�IPC����
*/
int IPClocation::getIPCNum()
{
	return IPC.size();
}

/*
*@brief��������ɾ��IPC
*/
bool IPClocation::DeleteIPC(int index)
{
	if (index >= IPC.size() || index < 0)
	{
		return false;
	}

	std::vector<IPCmsg>::iterator iter=IPC.begin();
	IPC.erase(iter + index);
	return true;
}

/*
*@brief����RTSPɾ��IPC
*/
bool IPClocation::DeleteIPC(String rtsp)
{
	std::vector<IPCmsg>::iterator iter;

	for ( iter = IPC.begin(); iter != IPC.end();)
	{
		if (iter->rtsp == rtsp)
		{
			iter=IPC.erase(iter);
		}
		else
		{
			iter++;
		}
	}
}

/*
@brief:��һ������IPC��ͼƬ����λͼ�е�AR�룬��������3D����
*/
std::vector<IPCobj> IPClocation::location(Mat img, int IPCindex)
{
	std::vector<IPCobj> retobj;

	//��ʼ��λ
	//�洢����id��
	vector<int> ids;
	//�洢���нǵ�
	vector<vector<Point2f> > corners;

	Mat imageCopy;
	Ptr<aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(ARtag_dict);

	//resize(img, imageCopy, Size(1280, 720));

	
	cv::aruco::detectMarkers(img, dictionary, corners, ids);
	// if at least one marker detected 
	if (ids.size() > 0)
	{
		
		vector< Vec3d > rvecsx, tvecsx;		//�洢С�Ż����˵���̬
		vector<int> idsx;					//����С�Ż����˵�ID
		vector<vector<Point2f> > cornersx;	//����С�Ż����˵Ľǵ�

		//ѡȡС�Ż����˵�id�ͽǵ�λ��
		for (size_t i = 0; i < ids.size(); i++)
		{
			if (ids[i] >= AR_ID_distribList[1][0] && ids[i] <= AR_ID_distribList[1][1])
			{
				//�ҵ���С�Ż�����id
				idsx.push_back(ids[i]);
				cornersx.push_back(corners[i]);
			}
		}
		if (idsx.size() > 0)
		{
			//����ЩС�Ż����˽�����̬����
			cv::aruco::estimatePoseSingleMarkers(cornersx, (float)AR_ID_distribList[1][2] / 1000.0, IPC[IPCindex].cameraMatrix, IPC[IPCindex].distCoeffs, rvecsx, tvecsx);
		}
		//�õ���ЩС�Ż�������������ϵ����
		for (size_t i = 0; i < idsx.size(); i++)
		{
			//����һ��IPCobj ���ڼ�¼������Ϣ
			IPCobj newobj;
			newobj.cls = IPCobj::Robot;
			newobj.ID = idsx[i];
			newobj.dimension = 3;
			//��ʼ��������ת��
			Mat Pw; //�洢������������
			Mat TsVec = Mat(tvecsx[i], true);
			Pw = IPC[IPCindex].RwMatrixI*(TsVec - IPC[IPCindex].TwVec);//��ʽ
			if (Pw.rows==3&&Pw.cols==1)//�����ȷ��ת����coordinate3D
			{
				newobj.coordinate3D = Pw;//?????
			}
			Mat rw; //�洢���巽��
			Mat Rs;//�洢������ת����
			Mat re = Mat(Vec3d(1, 0, 0), true);;//
			Rodrigues(rvecsx[i], Rs);
			rw = IPC[IPCindex].RwMatrixI*Rs*re;//��ʽ
			if (rw.rows == 3 && rw.cols == 1)//�����ȷ��ת����direction3D
			{
				newobj.direction3D = rw;//?????
			}
			retobj.push_back(newobj);
		}

		rvecsx.clear(); tvecsx.clear(); idsx.clear(); cornersx.clear();
		//ѡȡ��Ż����˵�id�ͽǵ�λ��
		for (size_t i = 0; i < ids.size(); i++)
		{
			if (ids[i] >= AR_ID_distribList[0][0] && ids[i] <= AR_ID_distribList[0][1])
			{
				//�ҵ��˴�Ż�����id
				idsx.push_back(ids[i]);
				cornersx.push_back(corners[i]);
			}
		}
		if (idsx.size() > 0)
		{
			//����Щ�����˽�����̬����
			cv::aruco::estimatePoseSingleMarkers(cornersx, (float)AR_ID_distribList[0][2] / 1000.0, IPC[IPCindex].cameraMatrix, IPC[IPCindex].distCoeffs, rvecsx, tvecsx);

			//�õ���Щ��������������ϵ����
			for (size_t i = 0; i < idsx.size(); i++)
			{
				//����һ��IPCobj ���ڼ�¼������Ϣ
				IPCobj newobj;
				newobj.cls = IPCobj::Robot;
				newobj.ID = idsx[i];
				newobj.dimension = 3;
				//��ʼ��������ת��
				Mat Pw; //�洢������������
				Mat TsVec = Mat(tvecsx[i], true);
				Pw = IPC[IPCindex].RwMatrixI*(TsVec - IPC[IPCindex].TwVec);//��ʽ
				if (Pw.rows == 3 && Pw.cols == 1)//�����ȷ��ת����coordinate3D
				{
					newobj.coordinate3D = Pw;//?????
				}
				Mat rw; //�洢���巽��
				Mat Rs;//�洢������ת����
				Mat re = Mat(Vec3d(1, 0, 0), true);;//
				Rodrigues(rvecsx[i], Rs);
				rw = IPC[IPCindex].RwMatrixI*Rs*re;//��ʽ
				if (rw.rows == 3 && rw.cols == 1)//�����ȷ��ת����direction3D
				{
					newobj.direction3D = rw;//?????
				}
				retobj.push_back(newobj);
			}
		}
	}

	return retobj;

}


/*
@brief:��һ������IPC��ͼƬ����λͼ�е�AR�룬��������3D����,ͬʱ����һ������ͼƬ
*/
std::vector<IPCobj> IPClocation::location(Mat img, int IPCindex, Mat &outimg)
{
	std::vector<IPCobj> retobj;

	//��ʼ��λ
	//�洢����id��
	vector<int> ids;
	//�洢���нǵ�
	vector<vector<Point2f> > corners;

	Mat imageCopy;
	Ptr<aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(ARtag_dict);
	outimg = img.clone();
	//resize(img, imageCopy, Size(1280, 720));


	cv::aruco::detectMarkers(img, dictionary, corners, ids);
	// if at least one marker detected 
	if (ids.size() > 0)
	{

		vector< Vec3d > rvecsx, tvecsx;		//�洢С�Ż����˵���̬
		vector<int> idsx;					//����С�Ż����˵�ID
		vector<vector<Point2f> > cornersx;	//����С�Ż����˵Ľǵ�

		//**ѡȡС�Ż����˵�id�ͽǵ�λ��
		for (size_t i = 0; i < ids.size(); i++)
		{
			if (ids[i] >= AR_ID_distribList[1][0] && ids[i] <= AR_ID_distribList[1][1])
			{
				//�ҵ���С�Ż�����id
				idsx.push_back(ids[i]);
				cornersx.push_back(corners[i]);
			}
		}
		if (idsx.size() > 0)
		{
			//����ЩС�Ż����˽�����̬����
			cv::aruco::estimatePoseSingleMarkers(cornersx, (float)AR_ID_distribList[1][2] / 1000.0, IPC[IPCindex].cameraMatrix, IPC[IPCindex].distCoeffs, rvecsx, tvecsx);
		
			//����
			cv::aruco::drawDetectedMarkers(outimg, cornersx, idsx);
			for (int j = 0; j < idsx.size(); j++)
				cv::aruco::drawAxis(outimg, IPC[IPCindex].cameraMatrix, IPC[IPCindex].distCoeffs, rvecsx[j], tvecsx[j], 0.1);
			//�õ���ЩС�Ż�������������ϵ����
			for (size_t i = 0; i < idsx.size(); i++)
			{
				//����һ��IPCobj ���ڼ�¼������Ϣ
				IPCobj newobj;
				newobj.cls = IPCobj::Robot;
				newobj.ID = idsx[i];
				newobj.dimension = 3;
				//��ʼ��������ת��
				Mat Pw; //�洢������������
				Mat TsVec = Mat(tvecsx[i], true);
				Pw = IPC[IPCindex].RwMatrixI*(TsVec - IPC[IPCindex].TwVec);//��ʽ
				if (Pw.rows == 3 && Pw.cols == 1)//�����ȷ��ת����coordinate3D
				{
					newobj.coordinate3D = Pw;//?????
				}
				Mat rw; //�洢���巽��
				Mat Rs;//�洢������ת����
				Mat re = Mat(Vec3d(1, 0, 0), true);;//
				Rodrigues(rvecsx[i], Rs);
				rw = IPC[IPCindex].RwMatrixI*Rs*re;//��ʽ
				if (rw.rows == 3 && rw.cols == 1)//�����ȷ��ת����direction3D
				{
					newobj.direction3D = rw;//?????
				}
				retobj.push_back(newobj);
			}
		}

		rvecsx.clear(); tvecsx.clear(); idsx.clear(); cornersx.clear();
		//**ѡȡ��Ż����˵�id�ͽǵ�λ��
		for (size_t i = 0; i < ids.size(); i++)
		{
			if (ids[i] >= AR_ID_distribList[0][0] && ids[i] <= AR_ID_distribList[0][1])
			{
				//�ҵ��˴�Ż�����id
				idsx.push_back(ids[i]);
				cornersx.push_back(corners[i]);
			}
		}
		if (idsx.size() > 0)
		{
			//����Щ�����˽�����̬����
			cv::aruco::estimatePoseSingleMarkers(cornersx, (float)AR_ID_distribList[0][2] / 1000.0, IPC[IPCindex].cameraMatrix, IPC[IPCindex].distCoeffs, rvecsx, tvecsx);

			//����
			cv::aruco::drawDetectedMarkers(outimg, cornersx, idsx);
			for (int i = 0; i < idsx.size(); i++)
				cv::aruco::drawAxis(outimg, IPC[IPCindex].cameraMatrix, IPC[IPCindex].distCoeffs, rvecsx[i], tvecsx[i], 0.1);
			//�õ���ЩС�Ż�������������ϵ����
			for (size_t i = 0; i < idsx.size(); i++)
			{
				//����һ��IPCobj ���ڼ�¼������Ϣ
				IPCobj newobj;
				newobj.cls = IPCobj::Robot;
				newobj.ID = idsx[i];
				newobj.dimension = 3;
				//��ʼ��������ת��
				Mat Pw; //�洢������������
				Mat TsVec = Mat(tvecsx[i], true);
				Pw = IPC[IPCindex].RwMatrixI*(TsVec - IPC[IPCindex].TwVec);//��ʽ
				if (Pw.rows == 3 && Pw.cols == 1)//�����ȷ��ת����coordinate3D
				{
					newobj.coordinate3D = Pw;//?????
				}
				Mat rw; //�洢���巽��
				Mat Rs;//�洢������ת����
				Mat re = Mat(Vec3d(1, 0, 0), true);;//
				Rodrigues(rvecsx[i], Rs);
				rw = IPC[IPCindex].RwMatrixI*Rs*re;//��ʽ
				if (rw.rows == 3 && rw.cols == 1)//�����ȷ��ת����direction3D
				{
					newobj.direction3D = rw;//?????
				}
				retobj.push_back(newobj);
			}
		}


		rvecsx.clear(); tvecsx.clear(); idsx.clear(); cornersx.clear();
		//ѡȡARTAG
		for (size_t i = 0; i < ids.size(); i++)
		{
			if (ids[i]== worldAR_ID)
			{
				//�ҵ��˴�Ż�����id
				idsx.push_back(ids[i]);
				cornersx.push_back(corners[i]);
			}
		}
		if (idsx.size() > 0)
		{
			//����Щ�����˽�����̬����
			cv::aruco::estimatePoseSingleMarkers(cornersx, worldAR_size, IPC[IPCindex].cameraMatrix, IPC[IPCindex].distCoeffs, rvecsx, tvecsx);

			//����
			cv::aruco::drawDetectedMarkers(outimg, cornersx, idsx);
			for (int i = 0; i < idsx.size(); i++)
				cv::aruco::drawAxis(outimg, IPC[IPCindex].cameraMatrix, IPC[IPCindex].distCoeffs, rvecsx[i], tvecsx[i], 0.5);
		}
		
	}

	return retobj;
}

/*
@brief:��IPCindex��IP������㵽�Ļ�������λAR�룬��������3D����
*/
std::vector<IPCobj> IPClocation::location(int IPCindex)
{
	Mat img;
	IPC[IPCindex].cap >> img;
	resize(img, img, Size(1280, 720));
	vector<IPCobj> retobj = location(img, IPCindex);
	return retobj;
}

/*
*@brief���õ�ָ��IPC��ŵ�ͼ��
*/
Mat IPClocation::getIPCimage(int index)
{
	Mat img;
	IPC[index].cap >> img;
	return img;
}

/*
@brief:��һ������IPC��ͼƬ����λͼ�е�AR�룬�����������
*/
std::vector<IPCobj> IPClocation::locationMat(Mat img, int IPCindex)
{
	std::vector<IPCobj> retobj;

	//��ʼ��λ
	//�洢����id��
	vector<int> ids;
	//�洢���нǵ�
	vector<vector<Point2f> > corners;

	Mat imageCopy;
	Ptr<aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(ARtag_dict);

	//resize(img, imageCopy, Size(1280, 720));


	cv::aruco::detectMarkers(img, dictionary, corners, ids);
	// if at least one marker detected 
	if (ids.size() > 0)
	{

		vector<int> idsx;					//����С�Ż����˵�ID
		vector<vector<Point2f> > cornersx;	//����С�Ż����˵Ľǵ�

		//ѡȡС�Ż����˵�id�ͽǵ�λ��
		for (size_t i = 0; i < ids.size(); i++)
		{
			if (ids[i] >= AR_ID_distribList[1][0] && ids[i] <= AR_ID_distribList[1][1])
			{
				//�ҵ���С�Ż�����id
				idsx.push_back(ids[i]);
				cornersx.push_back(corners[i]);
			}
		}
		//�õ���ЩС�Ż������������
		for (size_t i = 0; i < idsx.size(); i++)
		{
			//����һ��IPCobj ���ڼ�¼������Ϣ
			IPCobj newobj;
			newobj.cls = IPCobj::Robot;
			newobj.ID = idsx[i];
			newobj.dimension = 2;
			//��ʼ��������ת��
			newobj.coordinate2D = calculateCentre(cornersx[i]);

			retobj.push_back(newobj);
		}



	}

	return retobj;

}

Point2i IPClocation::calculateCentre(vector<Point2f> corner)
{
	float dy13 = corner[0].y - corner[2].y;
	float dx13 = corner[0].x - corner[2].x;
	float dy24 = corner[1].y - corner[3].y;
	float dx24 = corner[1].x - corner[3].x;
	
	Eigen::Matrix2f K;
	K << dy13, -dx13,
		 dy24, -dx24;
	Eigen::Vector2f B;
	B << dy13*corner[0].x - dx13*corner[0].y, 
		 dy24*corner[1].x - dx24*corner[1].y;
	Eigen::Vector2f retxy;
	retxy = K.inverse()*B;
	Point2f ret;
	ret.x = retxy[0]; ret.y = retxy[1];
	return ret;
}

bool IPClocation::AddIPC(std::vector<Mat> img, cv::Size board_size, cv::Size square_size, cv::String rtsp, double err_th )
{
	//�鿴�Ƿ���rtsp�ظ������򷵻ش���
	vector<string> allipcrtsp = getIPCrtsp();
	if (zfun::findVecterElm(allipcrtsp, rtsp) != -1)
	{
		printf("error:IPC�д�����ͬrtsp��ַ��IP���");
		return false;
	}

	//�궨���
	Mat cameraMatrix;
	Mat distCoeffs;
	vector<Mat> rvecsMat;
	vector<Mat> tvecsMat;

	int image_count = 0;                                            // ͼ������ 
	Size image_size;                                                // ͼ��ĳߴ� 

	std::vector<Point2f> image_points;                              // ����ÿ��ͼ���ϼ�⵽�Ľǵ�
	std::vector<std::vector<Point2f>> image_points_seq;             // �����⵽�����нǵ�



	//���ǵ�
	for (int i = 0; i < img.size(); i++)
	{
		image_count++;

		// ���ڹ۲�������
		cout << "image_count = " << image_count << endl;
		Mat imageInput = img[i];
		if (image_count == 1)  //�����һ��ͼƬʱ��ȡͼ������Ϣ
		{
			image_size.width = imageInput.cols;
			image_size.height = imageInput.rows;
			cout << "image_size.width = " << image_size.width << endl;
			cout << "image_size.height = " << image_size.height << endl;
		}

		/* ��ȡ�ǵ� */
		bool ok = findChessboardCorners(imageInput, board_size, image_points, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_NORMALIZE_IMAGE);
		if (0 == ok)
		{
			cout << "��" << image_count << "����Ƭ��ȡ�ǵ�ʧ�ܣ���ɾ�������±궨��" << endl; //�Ҳ����ǵ�

			return false;
		}
		else
		{
			Mat view_gray;
			cout << "imageInput.channels()=" << imageInput.channels() << endl;
			cvtColor(imageInput, view_gray, CV_RGB2GRAY);

			/* �����ؾ�ȷ�� */
			//find4QuadCornerSubpix(view_gray, image_points, Size(5, 5)); //�Դ���ȡ�Ľǵ���о�ȷ��
			cv::cornerSubPix(view_gray, image_points, cv::Size(11, 11), cv::Size(-1, -1), cv::TermCriteria(CV_TERMCRIT_ITER + CV_TERMCRIT_EPS, 20, 0.01));

			image_points_seq.push_back(image_points);  //���������ؽǵ�

													   /* ��ͼ������ʾ�ǵ�λ�� */
			drawChessboardCorners(view_gray, board_size, image_points, true);

			//imshow("Camera Calibration", view_gray);//��ʾͼƬ
			//waitKey(100);//��ͣ0.1S     
		}
	}
	cout << "�ǵ���ȡ��ɣ�����" << endl;


	/*������ά��Ϣ*/
	vector<vector<Point3f>> object_points_seq;                     // ����궨���Ͻǵ����ά����
	for (int t = 0; t < image_count; t++)
	{
		vector<Point3f> object_points;
		for (int i = 0; i < board_size.height; i++)
		{
			for (int j = 0; j < board_size.width; j++)
			{
				Point3f realPoint;
				/* ����궨�������������ϵ��z=0��ƽ���� */
				realPoint.x = i*square_size.width;
				realPoint.y = j*square_size.height;
				realPoint.z = 0;
				object_points.push_back(realPoint);
			}
		}
		object_points_seq.push_back(object_points);
	}

	/* ���б궨���� */

	double err_first = calibrateCamera(object_points_seq, image_points_seq, image_size, cameraMatrix, distCoeffs, rvecsMat, tvecsMat, CV_CALIB_FIX_K3);
	cout << "��ͶӰ���1��" << err_first << "����" << endl << endl;
	cout << "�궨��ɣ�����" << endl;

	//������̫���򷵻ش���
	if (err_first > err_th)
	{
		printf("error:���̫��");
		return false;
	}


	//���IPC
	IPCmsg newIPC(rtsp);
	newIPC.cameraMatrix = cameraMatrix;
	newIPC.distCoeffs = distCoeffs;
	IPC.push_back(newIPC);

	//���涨����    
	UpdateXMLfile();

	return true;
}

std::vector<IPCobj> IPClocation::calculateAllObjection(std::vector<std::vector<IPCobj>> eobj)
{
	//���������������ԣ�ex��ͬһ��IPC�¿����˶��ID��ͬ�Ļ����ˣ�

	//��robot����������
	vector<uint8_t> robotID;//�ҵ�����ЩrobotID
	vector<vector<Vec2i>> fIPCindex;//��ЩrobotID���������ĸ�IPC��,�����������IPC��ĵڼ������

	//����eobj
	for (size_t i = 0; i < eobj.size(); i++)
	{
		for (size_t j = 0; j < eobj[i].size(); j++)
		{
			//�ж����IPCOBJ�ǲ���robot
			if (eobj[i][j].cls == IPCobj::Robot)
			{
				//�鿴���robotid�Ƿ�����ջ��
				int samIDindex = zfun::findVecterElm(robotID, eobj[i][j].ID);	
				if (samIDindex == -1)//��
				{
					robotID.push_back(eobj[i][j].ID);
					Vec2i x; x[0] = i; x[1] = j;
					vector<Vec2i> newindex; newindex.push_back(x);//��i��IPC��������robotID
					fIPCindex.push_back(newindex);
				}
				else
				{
					Vec2i x; x[0] = i; x[1] = j;
					fIPCindex[samIDindex].push_back(x);
				}
			}
		}
	}

	//��Ի����˿�ʼ�����µ�IPCobj
	vector<IPCobj> retobj;

	if (Algorithm == 0)//ȷ���㷨
	{
		for (size_t i = 0; i < robotID.size(); i++)
		{
			IPCobj newobj;
			newobj.dimension = 3;
			newobj.ID = robotID[i];
			newobj.cls = IPCobj::Robot;
			newobj.coordinate3D[0] = 0; newobj.coordinate3D[1] = 0; newobj.coordinate3D[2] = 0;
			newobj.direction3D[0] = 0; newobj.direction3D[1] = 0; newobj.direction3D[2] = 0;
			//������robotID���е�IPC,����ƽ������
			for (size_t j = 0; j < fIPCindex[i].size(); j++)
			{
				
				newobj.coordinate3D[0] += eobj[fIPCindex[i][j][0]][fIPCindex[i][j][1]].coordinate3D[0];
				newobj.coordinate3D[1] += eobj[fIPCindex[i][j][0]][fIPCindex[i][j][1]].coordinate3D[1];
				newobj.coordinate3D[2] += eobj[fIPCindex[i][j][0]][fIPCindex[i][j][1]].coordinate3D[2];
				newobj.direction3D[0] += eobj[fIPCindex[i][j][0]][fIPCindex[i][j][1]].direction3D[0];
				newobj.direction3D[1] += eobj[fIPCindex[i][j][0]][fIPCindex[i][j][1]].direction3D[1];
				newobj.direction3D[2] += eobj[fIPCindex[i][j][0]][fIPCindex[i][j][1]].direction3D[2];
			}
			newobj.coordinate3D[0] /= fIPCindex[i].size();
			newobj.coordinate3D[1] /= fIPCindex[i].size();
			newobj.coordinate3D[2] /= fIPCindex[i].size();
			//push
			retobj.push_back(newobj);
		}
	}
	return retobj;

}


bool IPClocation::cmp(Point2d &s1, Point2d &s2)
{
	return s1.y > s2.y;
}

//����ʾ���巽��ļ�ͷ
void IPClocation::drawArrow(cv::Mat& img, cv::Point2d pLocation, cv::Point2d pDirection, Point2d oPoint, int len, int alpha,
	cv::Scalar color, int thickness, int lineType)
{
	const double PI = 3.1415926;
	Point2d arrow;

	//���� �� ��
	double angle = atan2((double)(oPoint.y - pDirection.y), (double)(oPoint.x - pDirection.x));

	pLocation.x = pLocation.x - sqrt(2) * 4 * cos(angle);
	pLocation.y = pLocation.y - sqrt(2) * 4 * sin(angle);
	//������Ǳߵ���һ�˵Ķ˵�λ��
	arrow.x = pLocation.x + len * cos(angle + PI * alpha / 180);
	arrow.y = pLocation.y + len * sin(angle + PI * alpha / 180);
	line(img, pLocation, arrow, color, thickness, lineType);
	arrow.x = pLocation.x + len * cos(angle - PI * alpha / 180);
	arrow.y = pLocation.y + len * sin(angle - PI * alpha / 180);
	line(img, pLocation, arrow, color, thickness, lineType);
}

//��������ļ�ͷ
void IPClocation::drawCoorArrow(cv::Mat& img, cv::Point2d pLocation, cv::Point2d pDirection, Point2d oPoint, int len, int alpha,
	cv::Scalar color, int thickness , int lineType )
{
	const double PI = 3.1415926;
	Point2d arrow;

	//���� �� ��
	double angle = atan2((double)(oPoint.y - pDirection.y), (double)(oPoint.x - pDirection.x));

	//������Ǳߵ���һ�˵Ķ˵�λ��
	arrow.x = pLocation.x + len * cos(angle + PI * alpha / 180);
	arrow.y = pLocation.y + len * sin(angle + PI * alpha / 180);
	line(img, pLocation, arrow, color, thickness, lineType);
	arrow.x = pLocation.x + len * cos(angle - PI * alpha / 180);
	arrow.y = pLocation.y + len * sin(angle - PI * alpha / 180);
	line(img, pLocation, arrow, color, thickness, lineType);
}


int IPClocation::findVecterElm(vector<IPCobj> vec, uint8_t robotidid)
{
	int ret;
	std::vector<IPCobj>::iterator iter;

	for (iter = vec.begin(); iter < vec.end(); iter++)
	{
		if (iter->ID == robotidid)
		{
			ret = &*iter - &vec[0];
			return ret;
		}
	}
	return -1;

}




Mat IPClocation::paintObject(vector<IPCobj> input, Point2d center, float scale)
{
	vector<Point2d> points;
	vector<Point2d>	directions;
	for (int i = 0; i < input.size(); i++)
	{
		points.push_back(Point2d(input[i].coordinate3D[0], input[i].coordinate3D[1]));
		directions.push_back(Point2d(input[i].direction3D[0], input[i].direction3D[1]));
	}
	double rows = center.y * 2; //ͼƬ����
	double cols = center.x * 2;

	/*if (min(rows, cols) < 10 * scale)
	{
		cout << "ͼƬ�ĸ߿�̫С,�����˳�" << endl;
		exit(0);
	}*/
	Mat back(rows, cols, CV_8UC3, Scalar(255, 255, 255));
	Point2d oPoint = center; //���ĵ�����
	vector<Point2d> xPoint; //x������
	vector<Point2d> yPoint;
	for (int i = -5; i <= 5; i++)
	{
		xPoint.push_back(Point2d(oPoint.x + i * scale, oPoint.y));
	}

	for (int i = -5; i <= 5; i++)
	{
		yPoint.push_back(Point2d(oPoint.x, oPoint.y + i * scale));
	}
	//sort(yPoint.begin(), yPoint.end(), cmp); //��������

	Point2d xEnd = Point2d(xPoint[10].x + 20, xPoint[10].y); //x���յ�
	Point2d yEnd = Point2d(yPoint[10].x, yPoint[10].y - 20); //y���յ�
	line(back, xPoint[0], xEnd, 0); //����x��
	drawCoorArrow(back, xEnd, Point2d(scale * 1 + cols / 2, -scale * 0 + rows / 2), oPoint, 5, 45, Scalar(0, 0, 0), 1);//��x��ļ�ͷ

	line(back, yPoint[0], yEnd, 0);//����y��
	drawCoorArrow(back, yEnd, Point2d(scale * 0 + cols / 2, -scale * 1 + rows / 2), oPoint, 5, 45, Scalar(0, 0, 0), 1);//��y��ļ�ͷ

																													   //��ʾx��y�������ֵ
	char text[11];
	for (int i = -5; i <= 5; i++)
	{
		if (i == 0)
		{
			continue;
		}
		sprintf_s(text, "%d", i); //��ʽ�����
		putText(back, text, xPoint[i + 5], CV_FONT_HERSHEY_COMPLEX, 0.4, Scalar(0, 0, 255));
	}
	for (int i = -5; i <= 5; i++)
	{
		sprintf_s(text, "%d", i);
		putText(back, text, yPoint[i + 5], CV_FONT_HERSHEY_COMPLEX, 0.4, Scalar(0, 0, 255));
	}

	for (int i = 0; i < points.size(); i++)
	{
		int id = input[i].ID;
		//��ʾ�����˵���Ϣ
		if (input[i].cls == 1)
		{
			circle(back, Point2d(scale * points[i].x + cols / 2, -scale * points[i].y + rows / 2), 4, Scalar(0, 0, 255), -1); //��ʾλ��
			drawArrow(back, Point2d(scale * points[i].x + cols / 2, -scale * points[i].y + rows / 2), Point2d(scale * directions[i].x + cols / 2, -scale * directions[i].y + rows / 2),
				oPoint, 10, 45, Scalar(255, 0, 0)); //��ʾ�����ü�ͷ��ʾ
													//��ʾid
			string str;
			stringstream ss;
			ss << id;
			ss >> str;
			putText(back, str, Point2d(scale * points[i].x + cols / 2 - 8, -scale * points[i].y + rows / 2 - 10), CV_FONT_HERSHEY_COMPLEX, 0.4, Scalar(0, 0, 255));
		}
		//��ʾ���������λ��
		else
		{
			rectangle(back, Point2d(scale * points[i].x + cols / 2 - 5, -scale * points[i].y + rows / 2 - 5),
				Point2d(scale * points[i].x + cols / 2 + 5, -scale * points[i].y + rows / 2 + 5), Scalar(0, 255, 0), -1); //��ʾλ��
		}
	}

	//imshow("back", back);
	//imwrite("2.jpg", back);
	//waitKey(0);
	return back;
}

/*
IPClocation
˵  ����һ����RTSP��ַIP����ͷ�����ж�λ�Ŀ⣬�ṩ�˶�λ�ӿ�
�����ṩ��һ����Ϣ��IPCobj��������Ϊ��Ϣ������洢���ڳ������ж���
���岢�Ҹ���������λ�á�
�����ˣ�����ǿ
��  ����beta 0.1
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

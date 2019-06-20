/*
robotAPI
˵  ����ʵ���˻���ͨѶ���ܣ�����������
�����ˣ�����ǿ
��  ����beta 0.9
��  �£�
1������˶�uArm��е�۵�֧��
*/


#pragma once
#include <opencv2\opencv.hpp>
#include <stdio.h>
#include <Winsock2.h>
#include <cstring>
#include <iostream>
#include <WS2tcpip.h>
#include <time.h>
#include <windows.h>
#include <vector>



#pragma comment(lib,"ws2_32.lib")
#define ServerIPport 6000
#define SEND_LENGHT 128 //һ�����ݰ��İ���
#define ROBOT_MAXCONNECT_NUM 5 //������������Ӹ���
#define ROBOT_CONNECTloseNUM 3 //�������Ĵ���
//-------------------------------------------------------
//		���壺ROBOTServer::is_Open()�ķ���ֵ��ֵ��		-
//-------------------------------------------------------
enum ServerStatus_ret
{
	ROBOTServer_BIND_SUCCESS=1, //�ոճ�ʼ��bind�ɹ�

	ROBOTServer_WSAERROR=-3, //��ʼ��sock��汾�Ŵ���
	ROBOTServer_INVALID_SOCKET=-2, //�����׽���ʧ��
	ROBOTServer_BINDERROR=-4, //bindʧ��
};

//-------------------------------------------------------
//		���壺getVoltage�ķ���ֵ��ֵ��		            -
//-------------------------------------------------------
#define getVoltage_SOCKET_ERROR -3 //��ʾ�׽����д���
#define getVoltage_SOCKET_CLOSE -2 //��ʾ�׽����Ѿ��ر�
#define getVoltage_FORMAT_ERROR -1 //��ʾ���յ������ݸ�ʽ���󣬻��߲��ǵ�ѹֵ


//-------------------------------------------------------
//		���壺robot::connectStatus��ֵ��,��������״̬	-
//-------------------------------------------------------
enum robot_connectStatus_ret
{
	ROBOT_connectStatus_CONNECTERROR=-1, //�쳣��һ��ʼ�������쳣������˵�ڴ���robot��ʱ���ٴμ��ID�ţ����ִ���
	ROBOT_connectStatus_CHECKERROR =-2,   //���ʱ�����жϣ�getVoltage�����з����쳣��
	ROBOT_connectStatus_COMMUNICATEERROR= -3, //��ִ��ͨѶ�����У������ж�

	ROBOT_connectStatus_1Level= 1, //��������������ͨѶ�����Ƚϲ
	ROBOT_connectStatus_2Level= 2, //��������������ͨѶ����һ�㡣
	ROBOT_connectStatus_3Level= 3, //��������������ͨѶ�����ܺá�
};

 

/*******************************************************************************
* kobukiЭ��ָ�����ݵĽṹ������
*******************************************************************************/

typedef struct _StruRunControl
{
	unsigned char identifier;
	unsigned char lenth;
	unsigned short speed;
	unsigned short radius;
}StruRunControl;

typedef struct _StruSound
{
	unsigned char identifier;
	unsigned char lenth;
	unsigned short note;
	unsigned char duration;

}StruSound;


typedef struct _StruSoundSeq
{
	unsigned char identifier;
	unsigned char lenth;
	unsigned char SequenceNumber;

}StruSoundSeq;

typedef struct _StruPower
{
	unsigned char identifier;
	unsigned char lenth;
	unsigned short PowerFlags;

}StruPower;

typedef struct _StruExtraReq
{
	unsigned char identifier;
	unsigned char lenth;
	unsigned short ReqFlags;

}StruExtraReq;

typedef struct _StruNormolInput
{
	unsigned char identifier;
	unsigned char lenth;
	unsigned short Flags;

}StruNormolInput;

typedef struct _StruNetModel
{
	unsigned char identifier;
	unsigned char lenth;
	unsigned char STA[3];

}StruNetModel;

typedef struct _StruWifID
{
	unsigned char identifier;
	unsigned char lenth;
	unsigned char ids[100];

}StruWifID;

typedef struct _StruWifPWD
{
	unsigned char identifier;
	unsigned char lenth;
	unsigned char ids[100];

}StruWifPWD;

typedef struct _StruCommincateModel
{
	unsigned char identifier;
	unsigned char lenth;
	unsigned char date[4];

}StruCommincateModel;

typedef struct _StruFeedBackIndentifer
{
	unsigned short Timestamp;
	unsigned char Bumper;
	unsigned char WheelDrop;
	unsigned char Cliff;
	unsigned short LeftEncoder;
	unsigned short RightEncode;
	unsigned char LeftPWM;
	unsigned char RightPWM;
	unsigned char Button;
	unsigned char Charge;
	unsigned short int Battery;
	unsigned char OverCurrentFlags;

}StruFeedBackIndentifer;

typedef struct _StruInertialSensor
{
	short Angle;
	short AngleRate;
	unsigned char Unused[3];

}StruInertialSensor;

typedef struct _StruCliffSensor
{
	unsigned short RightCliff;
	unsigned short CentralCliff;
	unsigned short LeftCliff;

}StruCliffSensor;

typedef  struct _StruMotor
{
	short LeftMotor;
	short RightMotor;
}StruMotor;

typedef struct _StruHardVer
{
	unsigned char Patch;
	unsigned char Minor;
	unsigned char Major;
	unsigned char unused;
}StruHardVer;

typedef struct _StruFirmWare
{
	unsigned char Patch;
	unsigned char Minor;
	unsigned char Major;
	unsigned char unused;

}StruFirmWare;

typedef struct _Stru3DgyroScope
{
	unsigned char FrameID;
	unsigned char FollowLenth;
	struct {
		unsigned short x_axis;
		unsigned short y_axis;
		unsigned short z_axis;
	}RawGyro[100];
}Stru3DgyroScope;



typedef struct _StruUDID
{
	unsigned int UDID0;
	unsigned int UDID1;
	unsigned int UDID2;

}StruUDID;



typedef struct _StruUltrasonicData
{
	unsigned short DISL1;
	unsigned short DISL2;
	unsigned short DISL3;
	unsigned short DISL4;
	unsigned short DISL5;

}StruUltrasonicData;

typedef struct _StruRobotID
{
	unsigned char  id;
}StruRobotID;





//using namespace std;
/*******************************************************************************
* ���������ָ�����ݵĽṹ��
*******************************************************************************/
struct rebotATmsg
{
	/* data */
	uint8_t header;//��ͷ��ʶ
	int8_t ret;//������ȷֵ��
	uint8_t cmd;
	uint8_t parmNum;
	uint8_t callret;
	uint8_t parmdata[8][64];
};
struct imu_msg
{
	//���ٶ�
	float angular_velocity_x;
	float angular_velocity_y;
	float angular_velocity_z;
	//���Լ��ٶ�
	float linear_acceleration_x;
	float linear_acceleration_y;
	float linear_acceleration_z;
	//��̬ ��Ԫ��
	float orientation_w;
	float orientation_x;
	float orientation_y;
	float orientation_z;
};
/*******************************************************************************
* ��������ת���Ĺ�����,��������ת��
*******************************************************************************/
union float_to_char
{
	/* data */
	float fval;
	uint8_t bval[4];
};
union int32_to_char
{
	/* data */
	int32_t intval;
	uint8_t bval[4];
};

union imumsg_to_char
{
	/* data */
	imu_msg imuval;
	uint8_t bval[40];
};


/******************************************************************************
* //���ڻ������ٶ��Լ����ٶȵ��࣬�����ṩ���ֽӿڡ�
*******************************************************************************/
#define CasheQueue_MAXSIZE 20
class CasheQueuevw
{
public:
	/*
	@brief:��ʼ�����л��档
	*/
	void init();

private:
	int lenth;
public://��Ա����
	std::vector<float>  v;
	std::vector<float>  w;

public://�ӿ�
	   /*
	   @brief:���ٶ�push��
	   */
	void vpush(float input);

	/*
	@brief:���ٶ�push��
	*/
	void wpush(float input);

	/*
	@brief:�������ٶȶ��д�С
	*/
	int vsize();

	/*
	@brief:���ؽ��ٶȶ��д�С
	*/
	int wsize();

	/*
	@brief:�������ٶȻ��֣���������Ϊ0.
	@param	detaT��ʱ��΢�֣�����Ts/lenth
	@param	UPn���������ޡ�
	*/
	float integralV(float detaT, int UPn = CasheQueue_MAXSIZE);

	/*
	@brief:���ؽ��ٶȻ��֣���������Ϊ0.
	@param	detaT��ʱ��΢�֣�����Ts/lenth
	@param	UPn���������ޡ�
	*/
	float integralW(float detaT, int UPn = CasheQueue_MAXSIZE);

	/*
	@brief:������ʱʱ���ڣ�����λ�� ��x
	@param	Ts��ԭ��λϵͳ��ʱʱ��
	@param	theta:n=0ʱ�� ���ԽǶȡ�
	@returns:���ز���λ��������
	*/
	cv::Point2f displace(float Ts, float theta);



};
/******************************************************************************
* ���׼һ����˷���������
*******************************************************************************/
class ClsRecvRobotInfo
{
public:
	StruFeedBackIndentifer feedBackIndentifer;
	StruInertialSensor InertialSensor;
	StruCliffSensor CliffSensor;
	StruMotor Motor;
	StruHardVer HardVer;
	StruFirmWare irmWare;
	Stru3DgyroScope DgyroScope;
	StruUDID UDID;
	StruUltrasonicData  UltrasonicData;
	StruRobotID RobotID;

	void init(char *buf, int length);
};
/*******************************************************************************
* �����������
*******************************************************************************/
class robot
{
public:
	robot(SOCKET socket, struct sockaddr_in sin);

	
private:
	//��ص���
	int batteryVolt;
	//����������г���10�η���-1�����connectStatusz��Ϊ�쳣
	int connectstatus_cout=0;

	/*  func   */
	rebotATmsg resolverMsg(char rbuf[SEND_LENGHT]);
	uint8_t getID(void);

public:
	//��������״̬
	robot_connectStatus_ret connectStatus;
	//�����˵�ID��
	uint8_t robotID;
	//�����˵�sock��Ϣ
	struct sockaddr_in socksin;
	//�û����˵�ͨѶ�׽���
	SOCKET robotsock;
	//
	float Voltage;
	float v = 0;//���������ٶȴ�С
	float w = 0;//�����˽��ٶȴ�С

	//��λ�������ƵĲ���
	CasheQueuevw pvw;//���ٶȺͽ��ٶȣ�����Ҫʵʱ���µĶ��л��档

	/*  func   */
	
	INT8 move(float lin_val, float ang_val);
	void setTorque(char sta);
	float getVoltage(void);
	imu_msg getIMU();
	float getIllumination(void);
	uint8_t setLED(uint8_t lednum, uint8_t onoff);
	uint8_t getTorque();
	void initIMU(void);

};
/*******************************************************************************
* ����aimijia��������
*******************************************************************************/
class AimiRobot
{
public:
	bool init(const char* ip, int port);
	int Connect();//��������

private:
	//��ص���
	int batteryVolt;



public:
	//ר��Ϊ���׼һ����˿��ٵ��߳�
	HANDLE hThread;
	DWORD hThreadID;
	HANDLE hMutex;

	//��������״̬
	robot_connectStatus_ret connectStatus;
	//�����˵�ID��
	uint8_t robotID;
	//�����˵�sock��Ϣ
	struct sockaddr_in socksin;
	//�û����˵�ͨѶ�׽���
	SOCKET robotsock;
	//������״̬������ID��
	ClsRecvRobotInfo robotInfo;


	//�ı�������˶�
	float v = 0;//���������ٶȴ�С,��������Ϊһ��״ֵ̬���ǿ���ͨ���ⲿ�ı�ġ�
	float w = 0;//�����˽��ٶȴ�С

	//��λ�������ƵĲ���
	CasheQueuevw pvw;//���ٶȺͽ��ٶȣ�����Ҫʵʱ���µĶ��л��档

	/*  func   */

	INT8 move();//ͨ��vw���������ı�����˵��˶��ٶȡ�
	INT8 updateInfo();
	

};


/*******************************************************************************
* ����ͨѶ�׽�����:����������
* ����������У��������ͨѶ����
*******************************************************************************/
class ROBOTServer
{
public:
	//ROBOTServer();
	void init(int port);


private:
	ServerStatus_ret sock_Status; //�洢�����Ƿ񴴽��ɹ�
	

public:
	SOCKET ServerSock; //���������׽��֣����ı���
	std::vector<robot> robotlist;//���ڴ洢�Ѿ���ͨѶ�б��еĻ����ˣ����Զ���Щ�����˽���ͨѶ����
	AimiRobot aimirobot;//aimiplus�����������׽���
	//���ڶ��߳� �����߳�
	HANDLE hListenThread;
	DWORD ListenThreadID;
	//���ڶ��߳� ������
	HANDLE hMutex;

	//�鿴ROBOTServer�Ƿ��ʼ���ɹ���
	ServerStatus_ret is_Open();
	//����
	int Listen(int maxacp);
	//����
	int Accept();
	//�õ�robotlist�����Ӹ���
	int getRobotListNum();
	//�õ�robotlist�еڼ���robot��IP��ַ
	sockaddr_in getRobotIP(int robotlistCont);
	//��robotlist�в���ID�Ų�����������-1��ʾû��
	int findID(uint8_t robotid);
	//��ȡ����IP��ַ
	bool GetLocalAddress(std::string& strAddress);

	
};



//-------------------------------------------------------
//		���壺uArmSocket::is_Open()�ķ���ֵ��ֵ��		-
//-------------------------------------------------------
enum uArmServerStatus_ret
{
	uArmSocket_BIND_SUCCESS = 1, //�ոճ�ʼ��bind�ɹ�

	uArmSocket_WSAERROR = -3, //��ʼ��sock��汾�Ŵ���
	uArmSocket_INVALID_SOCKET = -2, //�����׽���ʧ��
	uArmSocket_BINDERROR = -4, //bindʧ��
};


/*******************************************************************************
* ����ͨѶ�׽�����:��е�۷�����
*******************************************************************************/
class uArmSocket
{
public:
	void init(int port);
	//�鿴ROBOTServer�Ƿ��ʼ���ɹ���
	uArmServerStatus_ret is_Open();
	//����
	int Listen(int maxacp);
	//����
	int Accept();
	//��ȡ����IP��ַ
	bool GetLocalAddress(std::string& strAddress);
	//��е�ۿ���
	INT8 uArmControl(int x, int y, int z, int f);
	INT8 uArmControlsu();
	INT8 uArmControlsu(int x, int y, int z, int f);
	INT8 uArmqi(bool open);

private:
	uArmServerStatus_ret sock_Status; //�洢�����Ƿ񴴽��ɹ�

public:
	SOCKET ServerSock; //���������׽��֣����ı���
	SOCKET uArmSock; //���������׽��֣����ı���

	int uArmSock_status;


	//���ڶ��߳� �����߳�
	HANDLE hListenThread;
	DWORD ListenThreadID;
	//���ڶ��߳� ������
	HANDLE hMutex;

	//�ٶ�״̬
	int su_x = 0, su_y = 0, su_z = 0;
	int su_f = 0;


};




/*
robotAPI
˵  ����ʵ���˻���ͨѶ���ܣ�����������
�����ˣ�����ǿ
��  ����beta 0.2
��  �£�
1������ˣ�socket�����׽��ֵĳ�ʱ����
2������ˣ��õ����ط�����IP��ַ�Ľӿ�
*/


#pragma once
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

 






using namespace std;
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

	/*  func   */
	
	INT8 move(float lin_val, float ang_val);
	void setTorque(char sta);
	float getVoltage(void);
	imu_msg getIMU();
	float getIllumination(void);
	uint8_t setLED(uint8_t lednum, uint8_t onoff);
	uint8_t getTorque();


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
	vector<robot> robotlist;//���ڴ洢�Ѿ���ͨѶ�б��еĻ����ˣ����Զ���Щ�����˽���ͨѶ����
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






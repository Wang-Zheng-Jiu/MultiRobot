//�汾�ţ�beta 0.1


#include "stdafx.h"
#include "robotAPI.h"


/*-----------------------------------------------------------------------------
						robot�����ӿڶ���
							 class
------------------------------------------------------------------------------*/
robot::robot(SOCKET socket, struct sockaddr_in sin)
{ 
	robotsock = socket;
	socksin = sin;
	//ˢ�»����˹̼���Ϣ
	robotID = getID();
	if (robotID == 0)
	{
		connectStatus = ROBOT_connectStatus_CONNECTERROR;
	}
	else
	{
		connectStatus = ROBOT_connectStatus_3Level;
	}

}



/*******************************************************************************
* ˵�������ػ�����ID��
*******************************************************************************/
uint8_t robot::getID(void)
{
	char outbuf[SEND_LENGHT] = { 0xD8, 0x00, 0x00, 0x04, 0xC7 };
	send(robotsock, outbuf, sizeof(outbuf), 0);

	char recvbuf[SEND_LENGHT];
	int recvLen=recv(robotsock, recvbuf, sizeof(recvbuf), 0);
	if (recvLen == 0)
	{
		return 0;
	}
	else if (recvLen == SOCKET_ERROR)
	{
		return 0;
	}

	rebotATmsg atmsg = resolverMsg(recvbuf);

	if (atmsg.ret > 0 && atmsg.callret == 1 && atmsg.cmd == 0x04)
	{
		return atmsg.parmdata[0][0];
	}
	else
	{
		return 0;
	}

}
/*******************************************************************************
* �������ƶ�����
˵���������������ƶ��ٶ���ת��Ƕ������ƻ����˵��ƶ���
����	lin_val		�ƶ��ٶȣ�0-0.2Ϊ����ֵ����������Ի����������ת��
		ang_val		�������������������һ������ֵһ��Ϊ0-1.8
���	1/-1		1��ȷ  -1����ֵ
*******************************************************************************/
INT8 robot::move(float lin_val, float ang_val)
{
	float_to_char lin;
	float_to_char ang;
	
	lin.fval = lin_val;
	ang.fval = ang_val;

	char outbuf[SEND_LENGHT] = { 0xd8,0x02,0x00,0x02,0x04,0,0,0,0,0x04,0,0,0,0 ,0xc7};
	for (int i = 0; i < 4; i++)
	{
		outbuf[5 + i] = lin.bval[i];
		outbuf[10 + i] = ang.bval[i];
	}
	int sendret=send(robotsock, outbuf, sizeof(outbuf), 0);

	if (sendret <= 0)
	{
		connectStatus = ROBOT_connectStatus_COMMUNICATEERROR;
		return -1;
	}
	else
	{
		return 1;
	}

	
}

/*******************************************************************************
* ���ǿ��ͣת
˵������������رգ�1�ǿ�����0�ǹرգ�ͬʱ���ʼ�������ر�֮�����Ҫ�����˶��Ļ���
	  Ӧ����1������ִ��move
����	sta	1�ǿ�����0�ǹر�
*******************************************************************************/
void robot::setTorque(char sta)
{
	char outbuf[SEND_LENGHT] = { 0xd8,0x01,0x00,0x03,0x01,0x01,0xc7 };
	outbuf[5] = sta;
	int sendret = send(robotsock, outbuf, sizeof(outbuf), 0);

	if (sendret <= 0)
	{
		connectStatus = ROBOT_connectStatus_COMMUNICATEERROR;
		
	}

}

/*******************************************************************************
* ���ͽ��յ��ĺ�����������ͷ��ڴ档
*******************************************************************************/
rebotATmsg robot::resolverMsg(char rbuf[SEND_LENGHT])
{
	rebotATmsg retmsgz;
	int readi = 0;
	uint8_t header0 = rbuf[readi]; readi++;
	if (header0 == 0xD8 || header0 == 0xD9)
	{
		retmsgz.header = header0;
		retmsgz.parmNum = rbuf[readi]; readi++;
		retmsgz.callret = rbuf[readi]; readi++;
		retmsgz.cmd = rbuf[readi]; readi++;
		//����������
		for (int i = 0; i<retmsgz.parmNum; i++)
		{
			uint8_t inum = rbuf[readi]; readi++;
			for (int j = 0; j < inum; j++)
			{
				retmsgz.parmdata[i][j]= rbuf[readi]; readi++;
			}
		}
		//�жϰ�β�Ƿ���ȷ
		if ((UINT8)rbuf[readi] == 0xc7) { retmsgz.ret = 1; }
		else retmsgz.ret = -1;

		return retmsgz;
	}
	else
	{
		retmsgz.ret = -1;
		return retmsgz;
	}
}

/*******************************************************************************
* �õ���ص�ѹ��������(��������updata)
*******************************************************************************/
imu_msg robot::getIMU()
{
	
	imumsg_to_char imubuf;
	char outbuf[SEND_LENGHT] = { 0xD8, 0x00, 0x00, 0x07, 0xC7 };
	send(robotsock, outbuf, sizeof(outbuf), 0);

	
	char recvbuf[SEND_LENGHT];
	int recvLen = recv(robotsock, recvbuf, sizeof(recvbuf), 0);
	if (recvLen<=0)
	{
		connectStatus = ROBOT_connectStatus_COMMUNICATEERROR;
	}
	
	
	rebotATmsg atmsg = resolverMsg(recvbuf);
	if (atmsg.ret > 0 && atmsg.callret == 1 && atmsg.cmd == 0x07)
	{
		for (int i = 0; i<40; i++)
		{
			imubuf.bval[i] = atmsg.parmdata[0][i];
		}
	}
	
	return imubuf.imuval;
}

/*******************************************************************************
* �õ���ص�ѹ��������(��������updata),�����������
*******************************************************************************/
float robot::getVoltage(void)
{
	DWORD starttime = GetTickCount();
	char outbuf[SEND_LENGHT] = { 0xD8, 0x00, 0x00, 0x05, 0xC7 };
	send(robotsock, outbuf, sizeof(outbuf), 0);

	char recvbuf[SEND_LENGHT];
	int recvLen=recv(robotsock, recvbuf, sizeof(recvbuf), 0);
	//�����������
	if (recvLen == 0)
	{
		connectStatus = ROBOT_connectStatus_CHECKERROR;
		return -2;
	}
	else if(recvLen==SOCKET_ERROR)
	{
		connectStatus = ROBOT_connectStatus_CHECKERROR;
		return -3;
	}
	else
	{
		DWORD pingms = GetTickCount()- starttime;
		if(pingms<1) connectStatus = ROBOT_connectStatus_3Level;
		else if(pingms < 10) connectStatus = ROBOT_connectStatus_2Level;
		else connectStatus = ROBOT_connectStatus_1Level;
	}
	

	rebotATmsg atmsg = resolverMsg(recvbuf);
	if (atmsg.ret > 0 && atmsg.callret == 1 && atmsg.cmd == 0x05)
	{
		//��ȡ����
		float_to_char volt;
		for (int i = 0; i<4; i++)
		{
			volt.bval[i] = atmsg.parmdata[0][i];
		}
		return volt.fval;
	}
	else
	{
		return -1;
	}

}
/*******************************************************************************
* �õ�����ǿ�ȣ�������(��������updata)����getVoltage���÷�����
*******************************************************************************/
float robot::getIllumination(void)
{
	char outbuf[SEND_LENGHT] = { 0xD8, 0x00, 0x00, 0x09, 0xC7 };
	send(robotsock, outbuf, sizeof(outbuf), 0);

	char recvbuf[SEND_LENGHT];
	int recvLen = recv(robotsock, recvbuf, sizeof(recvbuf), 0);
	if (recvLen == 0)
	{
		connectStatus = ROBOT_connectStatus_CHECKERROR;
		return -2;
	}
	else if (recvLen == SOCKET_ERROR)
	{
		connectStatus = ROBOT_connectStatus_CHECKERROR;
		return -3;
	}


	rebotATmsg atmsg = resolverMsg(recvbuf);
	if (atmsg.ret > 0 && atmsg.callret == 1 && atmsg.cmd == 0x09)
	{
		//��ȡ����
		float_to_char Illumi;
		for (int i = 0; i<4; i++)
		{
			Illumi.bval[i] = atmsg.parmdata[0][i];
		}
		return Illumi.fval;
	}
	else
	{
		return -1;
	}
}


/*******************************************************************************
* ����LED��
����	lednum	led��ţ�1-4��
		Onoff	����/�صƣ�1/0��
����	Ret	ͨѶ�Ƿ�ɹ���send����ֵ��
*******************************************************************************/
uint8_t robot::setLED(uint8_t lednum, uint8_t onoff)
{
	char outbuf[SEND_LENGHT] = { 0xD8,0x02,0x00,0x08,0x01,0,0x01,0,0xC7 };
	outbuf[5] = lednum;
	outbuf[7] = onoff;

	int sendret=send(robotsock, outbuf, sizeof(outbuf), 0);
	if (sendret <= 0)
	{
		connectStatus = ROBOT_connectStatus_COMMUNICATEERROR;
		return -1;
	}
	else
	{
		return 1;
	}
}

/*******************************************************************************
* ���ػ����˵��ʹ����Ϣ
����	
����	ʹ����Ϣ,1�ǿ��� 0�ǹض�
*******************************************************************************/
uint8_t robot::getTorque()
{
	char outbuf[SEND_LENGHT] = { 0xD8,0x00,0x00,0x10,0xC7 };
	send(robotsock, outbuf, sizeof(outbuf), 0);

	char recvbuf[SEND_LENGHT];
	int recvLen = recv(robotsock, recvbuf, sizeof(recvbuf), 0);
	if (recvLen <= 0)
	{
		connectStatus = ROBOT_connectStatus_COMMUNICATEERROR;
	}


	rebotATmsg atmsg = resolverMsg(recvbuf);
	if (atmsg.ret > 0 && atmsg.callret == 1 && atmsg.cmd == 0x10)
	{
		return atmsg.parmdata[0][0];
	}
	else
	{
		return -1;
	}

	 

}


/*-----------------------------------------------------------------------------
						 ROBOTServer�����ӿڶ���
								class
------------------------------------------------------------------------------*/

void ROBOTServer::init(int port)
{

	//��ʼ��Socket
	WORD socket_version = MAKEWORD(2, 2);
	WSADATA wsadata;
	if (WSAStartup(socket_version, &wsadata) != 0)
	{
		sock_Status = ROBOTServer_WSAERROR;
		return;
	}


	ServerSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	//����socket ���ж��Ƿ񴴽��ɹ�
	if (ServerSock == INVALID_SOCKET)
	{
		sock_Status = ROBOTServer_INVALID_SOCKET;
		return;
	}


	//���÷�����Socket��ַ
	struct sockaddr_in s_sin; //���ڴ洢���ش���socket�Ļ�����Ϣ
	s_sin.sin_family = AF_INET;  //����Э����ΪIPV4
	s_sin.sin_port = htons(port);//�涨�˿ں�
	s_sin.sin_addr.S_un.S_addr = INADDR_ANY;

	//��Socket Server�����ص�ַ
	if (bind(ServerSock, (LPSOCKADDR)&s_sin, sizeof(s_sin)) == SOCKET_ERROR)//��
	{
		sock_Status = ROBOTServer_BINDERROR;
		return;
	}
	//����bind�ɹ�
	sock_Status = ROBOTServer_BIND_SUCCESS;
	return;
}




ServerStatus_ret ROBOTServer::is_Open()
{
	return sock_Status;
}
int ROBOTServer::Listen(int maxacp)
{
	return listen(ServerSock, maxacp);
}

//����ʽ���ջ����˵�����
int ROBOTServer::Accept()
{
	SOCKET socket_of_client;  //�ͻ��ˣ�Զ�̣���socket
	struct sockaddr_in c_sin; //���ڴ洢�����ӵĿͻ��˵�socket������Ϣ
	int    c_sin_len;         //����accept�ĵ�����������c_sin�Ĵ�С��

	c_sin_len = sizeof(c_sin);

	socket_of_client = accept(ServerSock, (SOCKADDR *)&c_sin, &c_sin_len);

	//�������û�
	if (socket_of_client == INVALID_SOCKET)
	{
		return -1;//Accept error
	}
	else
	{
		robot robotnew(socket_of_client, c_sin);
		robotlist.push_back(robotnew);

	}
}

//�õ�robotlist�����Ӹ���
int ROBOTServer::getRobotListNum()
{
	return robotlist.size();
}
//�õ�robotlist�еڼ���robot��IP��ַ
sockaddr_in ROBOTServer::getRobotIP(int robotlistCont)
{
	sockaddr_in ipport;
	if (robotlistCont >= getRobotListNum())
	{
		perror("getRobotIP�����ڱ��");
	}

	return robotlist[robotlistCont].socksin;
}

int ROBOTServer::findID(uint8_t robotid)
{
	for (size_t i = 0; i < getRobotListNum(); i++)
	{
		if (robotid == robotlist[i].robotID)
		{
			return i;
		}
	}
	return -1;
}



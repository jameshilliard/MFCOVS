#pragma once

enum OVSSTATUS
{
	// ���������ɹ�
	SUCCESS = 0,
	// ��������ʧ��
	ERR = -1,
	// SDKδ��ʼ��
	ERR_SDK_UNINITIALIZED = -10,
	// SDK�Ѿ���ʼ��
	ERR_SDK_ALREADY_INITIALIZED = -11,
	// SDK��ʼ��ʧ��
	ERR_SDK_INIT_FAIL = -12,
	// �豸������
	ERR_NO_DEVICE = -100,
	// �豸��Ų�����
	ERR_INVALID_DEVICE_INDEX = -101,
	// ������Ч
	ERR_INVALID_PARAM = -102,
	// δע��ص�����
	ERR_NOT_REGISTERD_CALLBACK = -103,
	// ͼ��ɼ�δ��ʹ��
	ERR_CAPTURE_DISABLED = -200,
	// ͼ��ɼ�����ֹ
	ERR_CAPTURE_ABORTED = -201,
	// ͼ��ɼ���������
	ERR_CAPTURE_IS_RUNNING = -202,
	// ͼ��ɼ���ʱ
	ERR_CAPTURE_TIMEOUT = -203,
	// ��ȡ������ʧ��
	ERR_GET_FEATURE = -300,
	// �û�����Ϊ0
	ERR_USER_COUNT_ZERO = -400,
	// ����û�ʧ��
	ERR_FAILED_TO_ADD_USER = -401,
	// �����û�ʧ��
	ERR_FAILED_TO_UPDATE_USER = -402,
	// ɾ���û�ʧ��
	ERR_FAILED_TO_DELETE_USER = -403
};


struct Image
{
	unsigned char* data; //ͼ������
	int width;  
	int height;
};


struct Information
{
	enum PalmPosition
	{
		Suitable,
		NoPalm,
		TooClose,
		TooFar,
		TooLeft,
		TooRight,
		TooUp,
		TooDown
	};

	PalmPosition palm_position; //����λ����Ϣ
	int palm_cx;                //��������X����
	int palm_cy;                //��������Y����
	bool is_feature_ok;         //�ж��Ƿ������ȡ������
	int enroll_stage;           //¼��׶Σ���¼�������Ч
	int progress_enroll;        //¼����ȣ���¼�������Ч
};

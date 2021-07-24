#pragma once

enum OVSSTATUS
{
	// 函数操作成功
	SUCCESS = 0,
	// 函数运行失败
	ERR = -1,
	// SDK未初始化
	ERR_SDK_UNINITIALIZED = -10,
	// SDK已经初始化
	ERR_SDK_ALREADY_INITIALIZED = -11,
	// SDK初始化失败
	ERR_SDK_INIT_FAIL = -12,
	// 设备不存在
	ERR_NO_DEVICE = -100,
	// 设备编号不存在
	ERR_INVALID_DEVICE_INDEX = -101,
	// 参数无效
	ERR_INVALID_PARAM = -102,
	// 未注册回调函数
	ERR_NOT_REGISTERD_CALLBACK = -103,
	// 图像采集未被使能
	ERR_CAPTURE_DISABLED = -200,
	// 图像采集被终止
	ERR_CAPTURE_ABORTED = -201,
	// 图像采集正在运行
	ERR_CAPTURE_IS_RUNNING = -202,
	// 图像采集超时
	ERR_CAPTURE_TIMEOUT = -203,
	// 提取特征点失败
	ERR_GET_FEATURE = -300,
	// 用户数量为0
	ERR_USER_COUNT_ZERO = -400,
	// 添加用户失败
	ERR_FAILED_TO_ADD_USER = -401,
	// 更新用户失败
	ERR_FAILED_TO_UPDATE_USER = -402,
	// 删除用户失败
	ERR_FAILED_TO_DELETE_USER = -403
};


struct Image
{
	unsigned char* data; //图像数据
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

	PalmPosition palm_position; //手掌位置信息
	int palm_cx;                //手掌中心X坐标
	int palm_cy;                //手掌中心Y坐标
	bool is_feature_ok;         //判断是否可以提取特征点
	int enroll_stage;           //录入阶段，仅录入过程有效
	int progress_enroll;        //录入进度，仅录入过程有效
};

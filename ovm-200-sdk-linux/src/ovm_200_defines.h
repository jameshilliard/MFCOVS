#ifndef OVS_DEINFES_H
#define OVS_DEINFES_H

namespace OVS {
/**
 * @brief The OVS STATUS
 */
enum STATUS
{
    SUCCESS                     = 0,      //函数操作成功
    ERR                         = -1,     //函数运行失败
    ERR_INVALID_PARAM           = -2,     //参数无效

    ERR_DEV_OPEN_FAILED         = -99,    //设备打开失败
    ERR_NO_DEVICE               = -100,   //设备不存在
    ERR_INVALID_DEVICE_INDEX    = -101,   //设备编号不存在
    ERR_NOT_REGISTERD_CALLBACK  = -103,   //未注册回调函数

    ERR_SDK_UNINITIALIZED       = -10,    //SDK未初始化
    ERR_SDK_ALREADY_INITIALIZED = -11,    //SDK已经初始化
    ERR_SDK_INIT_FAIL           = -12,    //SDK初始化失败
    ERR_SDK_UNINIT_FAIL         = -13,    //SDK uninit失败
    
    ERR_PROCESS_NOT_START       = -50,     //操作未启动
    ERR_ENROLL_PROCESS_NOT_START= -53,     //录入过程未启动
    ERR_ENROLL_PROCESS_NOT_STOP = -54,     //录入过程未停止
    ERR_MATCH_PROCESS_NOT_START = -55,     //比对过程未启动
    ERR_MATCH_PROCESS_NOT_STOP  = -56,     //比对过程未停止

    ERR_FUNC_MODE_CALL_BACK     = -199,   //已启用回调函数方式获取图像
    ERR_CAPTURE_DISABLED        = -200,   //图像采集未被使能
    ERR_CAPTURE_ABORTED         = -201,   //图像采集被终止
    ERR_CAPTURE_IS_RUNNING      = -202,   //图像采集正在运行
    ERR_CAPTURE_TIMEOUT         = -203,   //图像采集超时
    ERR_PREVIEW_IS_EMPTY        = -204,   //预览图像为空

    ERR_GET_FEATURE             = -300,   //提取特征点失败

    ERR_MATCH_FAILED            = -400,   //比对失败
    //match
    ERR_INSUFFICIENT_MEMORY     = -450,   //内存不足
    ERR_USER_ID_NOT_EXIST       = -451,   //用户ID未添加
    ERR_USER_ID_EXISTED         = -452,   //用户ID已经存在
    ERR_USER_LEFT_FEATURE_EXISTED      = -453,   //该用户左手特征数据已经存在
    ERR_USER_RIGHT_FEATURE_EXISTED     = -454,   //该用户右手特征数据已经存在

    ERR_USER_COUNT_ZERO         = -500,   //用户数量为0
    ERR_FAILED_TO_ADD_USER      = -501,   //添加用户失败
    ERR_FAILED_TO_UPDATE_USER   = -502,   //更新用户失败
    ERR_FAILED_TO_DELETE_USER   = -503    //删除用户失败

};

}

#endif

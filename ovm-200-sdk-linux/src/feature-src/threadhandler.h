#ifndef THREAD_HANDLER_H
#define THREAD_HANDLER_H

#include <thread>
#include <condition_variable>
#include <functional>
#include <algorithm>

class ThreadHandler{
public:
    ThreadHandler();
    static ThreadHandler* instance();
    ~ThreadHandler();

    void notifyDisThread();
    void notifyImageThread();
    void notifyAlgoThread();

    void blockDisThread();
    void blockImageThread();
    void blockAlgoThread();

    void blockAllThd();
private:
    static void disFunc();
    static void imageFunc();
    static void algoFunc();

    static bool checkUpdateEnrollProgress();
    static void updateStageProgress();
    static void updateAlgoStage();
    static void updateProgress(const int featureCount,const int max);
private:
    std::thread _thdDis,_thdImage,_thdAlgo;

    static bool
    _thdDisRunFlag,
    _thdImageRunFlag,
    _thdAlgoRunFlag;

    static bool
    _thdDisBlockFlag,
    _thdImageBlockFlag,
    _thdAlgoBlockFlag;

    static std::condition_variable
    _condDis,_condImage,_condAlgo;

    static std::mutex
    _mtxDis,_mtxImage,_mtxAlgo;

    static bool _enrollStageUpdateFlag;

};

#endif


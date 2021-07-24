#include "threadhandler.h"
#include "logger.h"
#include "global.h"
#include "devhandler.h"
#include "algorithmhandler.h"

#include <iostream>
#include <chrono>

using namespace std;

bool ThreadHandler::_thdDisRunFlag      = true;
bool ThreadHandler::_thdImageRunFlag    = true;
bool ThreadHandler::_thdAlgoRunFlag     = true;

bool ThreadHandler::_thdDisBlockFlag    = true;
bool ThreadHandler::_thdImageBlockFlag  = true;
bool ThreadHandler::_thdAlgoBlockFlag   = true;

std::condition_variable ThreadHandler::_condDis;
std::condition_variable ThreadHandler::_condImage;
std::condition_variable ThreadHandler::_condAlgo;

std::mutex ThreadHandler::_mtxDis;
std::mutex ThreadHandler::_mtxImage;
std::mutex ThreadHandler::_mtxAlgo;

bool ThreadHandler::_enrollStageUpdateFlag = false;

ThreadHandler::ThreadHandler()
{
    _thdDis     = thread(disFunc);
    _thdImage   = thread(imageFunc);
    _thdAlgo    = thread(algoFunc);

    _thdDis.detach();
    _thdImage.detach();
    _thdAlgo.detach();
}

ThreadHandler *ThreadHandler::instance()
{
    static ThreadHandler ins;
    return &ins;
}

ThreadHandler::~ThreadHandler()
{
    _thdDisRunFlag      = false;
    _thdImageRunFlag    = false;
    _thdAlgoRunFlag     = false;

    notifyDisThread();
    notifyImageThread();
    notifyAlgoThread();
}

void ThreadHandler::notifyDisThread()
{
    _thdDisBlockFlag = false;
    _condDis.notify_one();
}

void ThreadHandler::notifyImageThread()
{
    _thdImageBlockFlag = false;
    _condImage.notify_one();
}

void ThreadHandler::notifyAlgoThread()
{
    _thdAlgoBlockFlag = false;
    _condAlgo.notify_one();
}

void ThreadHandler::blockDisThread()
{
    _thdDisBlockFlag = true;
}

void ThreadHandler::blockImageThread()
{
    //    _thdImageBlockFlag = true;
}

void ThreadHandler::blockAlgoThread()
{
    _thdAlgoBlockFlag = true;
}

void ThreadHandler::blockAllThd()
{
    blockDisThread();
    blockImageThread();
    blockAlgoThread();
}

void ThreadHandler::disFunc()
{
    while(_thdDisRunFlag){
        if(_thdDisBlockFlag){
            logde<<"dis wait,before";
            std::unique_lock<std::mutex> lkd(_mtxDis);
            _condDis.wait(lkd);
            logde<<"dis wait,after";
        }

        DevHandler::instance()->getDistance(g_sensorDis);

        std::this_thread::sleep_for(
                    std::chrono::milliseconds(
                        conDisIntervaleMs));
    }
}

void ThreadHandler::imageFunc()
{
    while(_thdImageRunFlag){

        if(_thdImageBlockFlag){
            logde<<"image wait before.";
            std::unique_lock<std::mutex> lkd(_mtxImage);
            _condImage.wait(lkd);
            logde<<"image wait after.";
        }
        //check device work status
        if(false == Global::instance()->isDeviceWorking()){
            ThreadHandler::instance()->blockAllThd();
            _thdImageBlockFlag = true;
            g_cvCap.release();
        }

        //
        if(g_cvCap.read(g_matRaw)){
            if(g_matRaw.empty()){
                continue;
            }

            {
                std::lock_guard<std::mutex> lkd(g_mtxMatRaw);
#if 0
                g_matRaw.copyTo(g_matPreview);
#endif

#if 1

                Mat ok_dst;
                g_matRaw(cv::Range(0, 1080),
                         cv::Range(420, 1500)).copyTo(ok_dst);

                Mat tmpMat;
                cv::resize(ok_dst, tmpMat,
                           cv::Size(100,100),
                           0, 0, cv::INTER_NEAREST);

                //                // add --
                //                Mat img1;
                //                Mat MM_ = getRotationMatrix2D(Point(tmpMat.cols / 2, tmpMat.rows / 2),
                //                                              -90.0, 1);
                //                warpAffine(tmpMat, img1, MM_, tmpMat.size());

                //                Point pt(g_ovsInfo.palm_cx/10.8,g_ovsInfo.palm_cy/10.8);
                //                circle(img1, pt, 2, Scalar(0, 0, 255));


                //                img1.copyTo(g_matPreview);
                //                imwrite("1.jpg",img1);
                //                imwrite("2.jpg",g_matPreview);


                //                g_ovsInfo.palm_cx = (1080-g_ovsInfo.palm_cx)/10.8;
                //                g_ovsInfo.palm_cy = g_ovsInfo.palm_cy/10.8;

                //                Point pt((1080-g_ovsInfo.palm_cx)/10.8,g_ovsInfo.palm_cy/10.8);
                //circle(tmpMat, pt, 2, Scalar(0, 0, 255));
                transpose(tmpMat, g_matPreview);
                //imwrite("2.jpg",g_matPreview);


#endif
            }

            Global::instance()->callBackGetPreviewImage();
        }

        std::this_thread::sleep_for(
                    std::chrono::milliseconds(
                        conImageIntervalMs));

    }
}

void ThreadHandler::algoFunc()
{
    while(_thdAlgoRunFlag){
        if(_thdAlgoBlockFlag){
            logde<<"algo thread wait before";
            std::unique_lock<std::mutex> lkd(_mtxAlgo);
            _condAlgo.wait(lkd);
            logde<<"algo thread wait after";
        }

        static int preDisEnableTimes = 0;

        int dis = g_sensorDis;

        if((dis > g_iDisSensorLow)
                && (dis < g_iDisSensorHigh))
        {
            //
            if(_enrollStageUpdateFlag){
                if(preDisEnableTimes < conPreDisEnableTimes){
                    continue;
                }else{
                    _enrollStageUpdateFlag = false;
                    preDisEnableTimes = 0;
                }
            }

            //
            cv::Mat matTmp;
            std::string feature;

            {
                std::lock_guard<std::mutex> lkd(g_mtxMatRaw);
                if(g_matRaw.empty()) {
                    continue;
                }
                g_matRaw.copyTo(matTmp);
            }

            if(AlgorithmHandler::instance()->
                    execute(matTmp,g_palmImageInfo,feature))
            {
                logde<<"exec true +++++++++++++++++";
                //
                g_ovsInfo.palm_cx = (1080-g_palmImageInfo.center_x)/10.8;
                g_ovsInfo.palm_cy = g_palmImageInfo.center_y/10.8;

                // g_ovsInfo.palm_cx = g_palmImageInfo.center_x;
                // g_ovsInfo.palm_cy = g_palmImageInfo.center_y;

                //
                if(g_sdkStatus & E_SDK_STATUS::e_enroll){
                    //
#if 0
                    updateStageProgress();
#endif
                    //
                    logde<<"sdk feature,algo func,feature size:"
                        <<feature.size();
                    if(feature.size() ==
                            sizeof(float) * conTemplateCellFloatSize)
                    {
                        g_featureVtr.push_back(feature);

                        if(checkUpdateEnrollProgress()){
                            Global::instance()->stopProcessEnroll();
                        }
                    }
                }else if(g_sdkStatus & E_SDK_STATUS::e_match){
                    g_ovsInfo.is_feature_ok = true;

                    string disStr = Global::instance()->getMatchDisMarkStr();
                    feature.append(disStr);

                    //
                    g_featureVtr.push_back(feature);
                }


            }else{
                logde<<"exec false ------------------";
                g_ovsInfo.palm_position = OVS::Information::NoPalm;
                g_ovsInfo.is_feature_ok = false;

                if(g_sdkStatus & E_SDK_STATUS::e_match){
                    g_featureVtr.clear();
                }
            }
            //endof dis enable
        }else{
            //
            g_iEnrollFeatureCount = 0;
            //
            ++g_iMatchDisMark;
            //
            if(_enrollStageUpdateFlag){
                preDisEnableTimes++;
            }

            g_ovsInfo.is_feature_ok = false;
            g_ovsInfo.palm_cx = -1;
            g_ovsInfo.palm_cy = -1;

            if(g_sdkStatus & E_SDK_STATUS::e_match){
                g_featureVtr.clear();
            }

            //sleep
            std::this_thread::sleep_for(
                        std::chrono::milliseconds(
                            conAlgorithmIntervalMs));
            //
        }

    }

}

bool ThreadHandler::checkUpdateEnrollProgress()
{
    logde<<"check enroll progress:"<<g_iEnrollFeatureCount;

    if(g_iEnrollFeatureCount < conIEnrollFeatureCount - 1){
        g_iEnrollFeatureCount++;

        int tmpProgress = g_iEnrollFeatureCount * (100 / conIEnrollFeatureCount);

        if(tmpProgress > g_ovsInfo.enroll_progress){
            if(tmpProgress == 100){
                tmpProgress = 95;
            }

            g_ovsInfo.enroll_progress = tmpProgress;
        }
        return false;
    }else{
        g_ovsInfo.enroll_progress = 0;
        g_iEnrollFeatureCount = 0;
        return true;
    }

}

void ThreadHandler::updateAlgoStage()
{
    switch (g_ovsInfo.enroll_stage) {
    case e_algo_stage_1:{
        g_ovsInfo.enroll_stage = e_algo_stage_2;
        g_ovsInfo.enroll_progress = 0;

        _enrollStageUpdateFlag = true;
    }
        break;
    case e_algo_stage_2:{
        g_ovsInfo.enroll_stage = e_algo_stage_3;
        g_ovsInfo.enroll_progress = 0;

        _enrollStageUpdateFlag = true;
    }
        break;
    case e_algo_stage_3:{
        Global::instance()->stopProcessEnroll();
    }
        break;
    default:
        break;
    }
}

void ThreadHandler::updateProgress(const int featureCount, const int max)
{
    int maxTmp = max - 1;
    if(featureCount == maxTmp){
        if(g_ovsInfo.enroll_stage == 3){
            g_ovsInfo.enroll_progress = 100;
        }else{
            g_ovsInfo.enroll_progress = 95;
        }
    }else if(featureCount < maxTmp){
        double rate = (double)featureCount / maxTmp * 100;
        g_ovsInfo.enroll_progress = (int)rate / 10 * 10;
    }
}

void ThreadHandler::updateStageProgress()
{
    //
    g_ovsInfo.palm_position = OVS::Information::Suitable;
    //
    static int featureCount = 0;

    int stageFeatureMax = g_iFeatureMax / 3;
    int featurePlusTimes = g_iFeatureMax % 3;

    if(g_ovsInfo.enroll_stage == 3){
        stageFeatureMax += featurePlusTimes;
    }

    if(++featureCount < stageFeatureMax){
        updateProgress(featureCount,stageFeatureMax);
    }else{
        featureCount = 0;

        updateAlgoStage();
    }
}


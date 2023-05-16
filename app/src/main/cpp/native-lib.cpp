//
// Created by Huili Yu on 3/30/23.
//

#include <jni.h>
#include <string>
#include <android/bitmap.h>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <android/log.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include "inference.h"

/**
 * @brief Convert input image from bitmap to OpenCV Mat.
 * @tparam env: JNIEnv pointer.
 * @tparam bitmap: input image in bitmap format.
 * @param bst: output input image in OpenCV format.
 * @param needUnPremultiplyAlpha: boolean variable to convert image color space
 */
void Bitmap2Mat(JNIEnv * env, jobject bitmap, cv::Mat& dst, jboolean needUnPremultiplyAlpha)
{
    AndroidBitmapInfo  info;    // uses jnigraphics
    void*              pixels = 0;

    CV_Assert( AndroidBitmap_getInfo(env, bitmap, &info) >= 0 );
    CV_Assert( info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
               info.format == ANDROID_BITMAP_FORMAT_RGB_565 );
    CV_Assert( AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0 );
    CV_Assert( pixels );
    dst.create(info.height, info.width, CV_8UC4);
    if( info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 )
    {
        cv::Mat tmp(info.height, info.width, CV_8UC4, pixels);
        if(needUnPremultiplyAlpha) cvtColor(tmp, dst, cv::COLOR_mRGBA2RGBA);
        else tmp.copyTo(dst);
    } else {
        cv::Mat tmp(info.height, info.width, CV_8UC2, pixels);
        cvtColor(tmp, dst, cv::COLOR_BGR5652RGBA);
    }
    AndroidBitmap_unlockPixels(env, bitmap);
    return;
}

/**
 * @brief Entry point of C++ code and MainActivity.java will call this function.
 * @tparam env: JNIEnv pointer.
 * @tparam bitmapIn: input image in bitmap format.
 * @param assetManager: AssetManager object for loading NCNN model files in assets folder.
 * @Return predicted class.
 */
extern "C" JNIEXPORT jstring JNICALL
Java_com_example_imageclassification_MainActivity_ImageClassification(
        JNIEnv* env,
        jobject,
        jobject bitmapIn,
        jobject assetManager) {
    cv::Mat src;
    Bitmap2Mat(env, bitmapIn, src, false);
    cv::Mat resized_src;
    cv::resize(src, resized_src, cv::Size(32, 32), cv::INTER_LINEAR);
    cv::cvtColor(resized_src, resized_src, cv::COLOR_RGBA2RGB);

    AAssetManager* mgr = AAssetManager_fromJava(env, assetManager);
    std::string pred_class = Inference(resized_src, mgr);
    return env->NewStringUTF(pred_class.c_str());
}
//
// Created by Huili Yu on 3/30/23.
//
#include <algorithm>
#include <vector>
#include <string>

#include <opencv2/imgproc.hpp>

#include "net.h"
#include "inference.h"

// Perform inference for classifying a given image using the NCNN model
std::string Inference(cv::Mat& src, AAssetManager* mgr) {
    std::vector<std::string> classes = {"plane", "car",  "bird", "cat",
                                        "deer",  "dog",  "frog", "horse",
                                        "ship",  "truck"};

    // Load model
    ncnn::Net net;
    int ret = net.load_param(mgr, "image_classifier_opt.param");
    if (ret) {
        __android_log_print(ANDROID_LOG_ERROR, "load_param_error",
                            "Failed to load the model parameters");
    }
    ret = net.load_model(mgr,"image_classifier_opt.bin");
    if (ret) {
        __android_log_print(ANDROID_LOG_ERROR, "load_weight_error",
                            "Failed to load the model weights");
    }

    // Convert image data to ncnn format
    // opencv image in bgr, model needs rgb
    ncnn::Mat input = ncnn::Mat::from_pixels(src.data,
                                             ncnn::Mat::PIXEL_BGR2RGB,
                                             src.cols, src.rows);

    // Data preprocessing (normalization)
    const float mean_vals[3] = {0.4914f*255.f, 0.4822f*255.f, 0.4465f*255.f};
    const float norm_vals[3] = {1/0.2470f/255.f, 1/0.2435f/255.f, 1/0.2616f/255.f};
    // In ncnn, substract_mean_normalize needs input pixels in [0, 255]
    input.substract_mean_normalize(mean_vals, norm_vals);

    // Inference
    ncnn::Extractor extractor = net.create_extractor();
    extractor.input("input", input);
    ncnn::Mat C2, C3, C4, C5, output;
    extractor.extract("output", output);

    // Flatten
    ncnn::Mat out_flatterned = output.reshape(output.w * output.h * output.c);
    std::vector<float> scores;
    scores.resize(out_flatterned.w);
    for (int j=0; j<out_flatterned.w; j++) {
        scores[j] = out_flatterned[j];
    }

    // Get predicted class
    std::string pred_class =
            classes[std::max_element(scores.begin(), scores.end()) - scores.begin()];
    return pred_class;
}

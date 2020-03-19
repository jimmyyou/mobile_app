/* Copyright 2019 The MLPerf Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/
#include <jni.h>

#include <memory>
#include <string>

#include "cpp/backend.h"
#include "cpp/datasets/coco.h"
#include "cpp/datasets/dummy_dataset.h"
#include "cpp/datasets/imagenet.h"
#include "tensorflow/lite/java/src/main/native/jni_utils.h"

using mlperf::mobile::Backend;
using mlperf::mobile::Dataset;

Backend* convertLongToBackend(JNIEnv* env, jlong handle) {
  if (handle == 0) {
    tflite::jni::ThrowException(env, kIllegalArgumentException,
                                "Internal error: Invalid handle to Backend.");
    return nullptr;
  }
  return reinterpret_cast<Backend*>(handle);
}

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

JNIEXPORT jlong JNICALL Java_org_mlperf_inference_MLPerfDriverWrapper_imagenet(
    JNIEnv* env, jclass clazz, jlong backend_handle, jstring jimage_dir,
    jstring jgroundtruth_file, jint offset, jint image_width,
    jint image_height) {
  // Convert parameters to C++.
  Backend* backend = convertLongToBackend(env, backend_handle);
  std::string image_dir = env->GetStringUTFChars(jimage_dir, nullptr);
  std::string gt_file = env->GetStringUTFChars(jgroundtruth_file, nullptr);

  // Create a new Imagenet object.
  std::unique_ptr<mlperf::mobile::Imagenet> imagenet_ptr(
      new mlperf::mobile::Imagenet(backend->GetInputFormat(),
                                   backend->GetOutputFormat(), image_dir,
                                   gt_file, offset, image_width, image_height));
  return reinterpret_cast<jlong>(imagenet_ptr.release());
}

JNIEXPORT jlong JNICALL Java_org_mlperf_inference_MLPerfDriverWrapper_coco(
    JNIEnv* env, jclass clazz, jlong backend_handle, jstring jimage_dir,
    jstring groundtruth_file, jint offset, jint num_classes, jint image_width,
    jint image_height) {
  // Convert parameters to C++.
  Backend* backend = convertLongToBackend(env, backend_handle);
  std::string image_dir = env->GetStringUTFChars(jimage_dir, nullptr);
  std::string gt_file = env->GetStringUTFChars(groundtruth_file, nullptr);

  // Create a new Coco object.
  std::unique_ptr<mlperf::mobile::Coco> coco_ptr(new mlperf::mobile::Coco(
      backend->GetInputFormat(), backend->GetOutputFormat(), image_dir, gt_file,
      offset, num_classes, image_width, image_height));
  return reinterpret_cast<jlong>(coco_ptr.release());
}

JNIEXPORT jlong JNICALL
Java_org_mlperf_inference_MLPerfDriverWrapper_dummyDataset(JNIEnv* env,
                                                           jclass clazz,
                                                           jlong backend_handle,
                                                           jint dataset_type) {
  Backend* backend = convertLongToBackend(env, backend_handle);
  // Create a new DummyDataset object.
  std::unique_ptr<mlperf::mobile::DummyDataset> dummy_dataset_ptr(
      new mlperf::mobile::DummyDataset(
          backend->GetInputFormat(), backend->GetOutputFormat(),
          static_cast<mlperf::mobile::DatasetConfig::DatasetType>(
              dataset_type)));
  return reinterpret_cast<jlong>(dummy_dataset_ptr.release());
}

JNIEXPORT void JNICALL
Java_org_mlperf_inference_MLPerfDriverWrapper_nativeDeleteDataset(
    JNIEnv* env, jclass clazz, jlong handle) {
  if (handle != 0) {
    delete reinterpret_cast<Dataset*>(handle);
  }
}

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

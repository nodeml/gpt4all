#pragma once

#include <napi.h>
#include "llmodel_c.h"
#include <memory>
#include <string>

namespace nodeml_gpt4all {

    
    class Model : public Napi::ObjectWrap<Model>
    {

    public:
        static Napi::FunctionReference constructor;

        llmodel_model model;

        std::mutex mutex;

        static Napi::Object Init(Napi::Env env, Napi::Object exports);

        static bool IsInstance(Napi::Object &obj);

        Model(const Napi::CallbackInfo &info);

        static Napi::Object FromLLModel(Napi::Env env, const llmodel_model &targetLLModel);

        static Model *FromObject(Napi::Value value);

        static Napi::Value Create(const Napi::CallbackInfo &info);


        Napi::Value Predict(const Napi::CallbackInfo &info);

        Napi::Value Destroy(const Napi::CallbackInfo &info);
    };

}
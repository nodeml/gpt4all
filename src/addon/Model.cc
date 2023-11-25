#include "Model.h"
#include <addon/FunctionWorker.h>
#include <addon/PredictWorker.h>
#include <addon/utils.h>

namespace nodeml_gpt4all
{

    Napi::FunctionReference Model::constructor;

    Napi::Object Model::Init(Napi::Env env, Napi::Object exports)
    {
        auto func = DefineClass(env, "Model",
                                {Model::InstanceMethod("destroy", &Model::Destroy),
                                 Model::InstanceMethod("predict", &Model::Predict)});

        constructor = Napi::Persistent(func);
        constructor.SuppressDestruct();
        exports.Set("Model", func);
        exports.Set("create", Napi::Function::New(env,&Model::Create));
        return exports;
    }

    Model::Model(const Napi::CallbackInfo &info) : ObjectWrap(info)
    {
        model = NULL;
    }

    Napi::Object Model::FromLLModel(Napi::Env env, const llmodel_model &targetLLModel)
    {
        try
        {
            Napi::EscapableHandleScope scope(env);
            auto newModel = Model::constructor.New({});
            Napi::ObjectWrap<Model>::Unwrap(newModel)->model = targetLLModel;
            return scope.Escape(newModel).ToObject();
        }
        catch (const std::exception &e)
        {
            throw Napi::Error::New(env, e.what());
        }
    }

    Model *Model::FromObject(Napi::Value value)
    {
        return Napi::ObjectWrap<Model>::Unwrap(value.ToObject());
    }
    Napi::Value Model::Create(const Napi::CallbackInfo &info)
    {
        auto env = info.Env();

        try
        {
            auto modelPath = info[0].As<Napi::String>().Utf8Value();
            auto implementation = "auto";

            auto worker = new FunctionWorker<llmodel_model>(
                info.Env(),
                [=]() -> llmodel_model
                {
                    llmodel_error *error = NULL;

                    llmodel_error e;
                    e.code = 0;
                    e.message = "No Errors";

                    auto created = llmodel_model_create2(modelPath.c_str(), implementation, &e);

                    if (e.code != 0)
                    {
                        throw Napi::Error::New(env, e.message);
                    }

                    if (created == NULL)
                    {
                        throw Napi::Error::New(env, "Failed to load model");
                    }

                    if (llmodel_loadModel(created, modelPath.c_str()))
                    {
                        return created;
                    }

                    throw Napi::Error::New(env, "Failed to load model");
                },
                [=](Napi::Env env, llmodel_model value) -> Napi::Value
                {
                    return Model::FromLLModel(env, value);
                });

            worker->Queue();
            return worker->GetPromise();
        }
        catch (const std::exception &e)
        {
            throw Napi::Error::New(env, e.what());
        }
    }
    Napi::Value Model::Predict(const Napi::CallbackInfo &info)
    {
        auto env = info.Env();

        try
        {
            auto prompt = info[0].As<Napi::String>().Utf8Value();
            auto tokenCallback = info[1].As<Napi::Function>();

            auto config = PredictWorkerConfig();

            llmodel_prompt_context context;

            utils::getPromptContext(info.Length() >= 3 && info[2].IsObject() ? info[2].ToObject() : Napi::Object::New(env),context);

            config.context = context;
            config.model = model;
            config.prompt = prompt;
            config.tokenCallback = tokenCallback;
            config.mutex = &mutex;

            auto worker = new PredictWorker(env, config);

            worker->Queue();

            return worker->GetPromise();
        }
        catch (const std::exception &e)
        {
            throw Napi::Error::New(env, e.what());
        }
    }
    Napi::Value Model::Destroy(const Napi::CallbackInfo &info)
    {
        if (model != NULL)
        {
            llmodel_model_destroy(model);
            model = NULL;
        }
        return Napi::Value();
    }
}

#include "Model.h"
#include <nodeml_gpt4all/ModelPromptContext.h>
#include <nodeml_gpt4all/FunctionWorker.h>
#include <nodeml_gpt4all/PredictWorker.h>

namespace nodeml_gpt4all
{

    Napi::FunctionReference Model::constructor;

    Napi::Object Model::Init(Napi::Env env, Napi::Object exports)
    {
        auto func = DefineClass(env, "Model",
                                {Model::StaticMethod("load", &Model::Load),
                                 Model::InstanceMethod("destroy", &Model::Destroy),
                                 Model::InstanceMethod("predict", &Model::Predict)});

        constructor = Napi::Persistent(func);
        constructor.SuppressDestruct();
        exports.Set("Model", func);
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
    Napi::Value Model::Load(const Napi::CallbackInfo &info)
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

            context.logits = nullptr;
            context.tokens = nullptr;
            context.n_past = 0;
            context.n_ctx = 1024;
            context.n_predict = 128;
            context.top_k = 40;
            context.top_p = 0.9f;
            context.temp = 0.72f;
            context.n_batch = 8;
            context.repeat_penalty = 1.0f;
            context.repeat_last_n = 10;
            context.context_erase = 0.5;

            config.context = context;

            config.model = model;
            config.prompt = prompt;
            config.tokenCallback = tokenCallback;
            config.mutex = &mutex;

            auto worker = new PredictWorker(env, config);

            worker->Queue();

            return worker->GetPromise();

            // // llmodel_prompt_context copiedContext = promptContext;

            // // auto threadSafeContext = new TsfnContext(env,model,prompt,promptContext);

            // std::thread nativeThread;

            // auto tokenCallbackThreadSafe = Napi::ThreadSafeFunction::New(
            //     env, tokenCallback, "TokenCallback", 0, 1, config, [=, &nativeThread](Napi::Env env, void *finalizeData, PredictWorkerConfig *ctx)
            //     {
            //         Napi::Error::Fatal("THREAD","FATAL THREAD ERROR");
            //         // Resolve the Promise previously returned to JS
            //         promise.Resolve(Napi::String::New(env, ctx->result));
            //         // Wait for the thread to finish executing before proceeding.
            //         nativeThread.join();

            //         delete config; },
            //     (void *)nullptr);

            // config->tsfn = tokenCallbackThreadSafe;

            // nativeThread = std::thread([promise](PredictWorkerConfig *config)
            //                            {
            //                             // Napi::Error::Fatal("THREAD","FATAL THREAD ERROR");
            //                             // std::this_thread::sleep_for(std::chrono::milliseconds(2000));
            //                             //    try
            //                             //    {
            //                             //        LLModelWrapper *wrapper = reinterpret_cast<LLModelWrapper *>(config->model);

            //                             //        auto ctx = &config->context;

            //                             //        if (size_t(ctx->n_past) < wrapper->promptContext.tokens.size())
            //                             //            wrapper->promptContext.tokens.resize(ctx->n_past);

            //                             //        // Copy the C prompt context
            //                             //        wrapper->promptContext.n_past = ctx->n_past;
            //                             //        wrapper->promptContext.n_ctx = ctx->n_ctx;
            //                             //        wrapper->promptContext.n_predict = ctx->n_predict;
            //                             //        wrapper->promptContext.top_k = ctx->top_k;
            //                             //        wrapper->promptContext.top_p = ctx->top_p;
            //                             //        wrapper->promptContext.temp = ctx->temp;
            //                             //        wrapper->promptContext.n_batch = ctx->n_batch;
            //                             //        wrapper->promptContext.repeat_penalty = ctx->repeat_penalty;
            //                             //        wrapper->promptContext.repeat_last_n = ctx->repeat_last_n;
            //                             //        wrapper->promptContext.contextErase = ctx->context_erase;

            //                             //        // Call the C++ prompt method
            //                             //        wrapper->llModel->prompt(
            //                             //            config->prompt, [](int32_t tid)
            //                             //            { return true; },
            //                             //            [=](int32_t token_id, const std::string tok)
            //                             //            {
            //                             //                if (token_id == -1)
            //                             //                {
            //                             //                    return false;
            //                             //                }

            //                             //                return true;

            //                             //             //    config->result += tok;

            //                             //             //    bool shouldContinue = false;

            //                             //             //    auto callback = [&shouldContinue](Napi::Env env, Napi::Function jsCallback, TokenCallbackInfo *value)
            //                             //             //    {
            //                             //             //        // Transform native data into JS data, passing it to the provided
            //                             //             //        // `jsCallback` -- the TSFN's JavaScript function.
            //                             //             //        shouldContinue = jsCallback.Call({Napi::Number::New(env, value->tokenId), Napi::String::New(env, value->token)}).ToBoolean();

            //                             //             //        // We're finished with the data.
            //                             //             //        delete value;
            //                             //             //    };

            //                             //             //    auto info = new TokenCallbackInfo();
            //                             //             //    info->tokenId = token_id;
            //                             //             //    info->token = tok;
            //                             //             //    info->total = config->result;

            //                             //             //    config->tsfn.BlockingCall(info, callback);

            //                             //             //    return shouldContinue;
            //                             //            },
            //                             //            [](bool isRecalculating)
            //                             //            {
            //                             //                return isRecalculating;
            //                             //            },
            //                             //            wrapper->promptContext);

            //                             //        // Update the C context by giving access to the wrappers raw pointers to std::vector data
            //                             //        // which involves no copies
            //                             //        ctx->logits = wrapper->promptContext.logits.data();
            //                             //        ctx->logits_size = wrapper->promptContext.logits.size();
            //                             //        ctx->tokens = wrapper->promptContext.tokens.data();
            //                             //        ctx->tokens_size = wrapper->promptContext.tokens.size();

            //                             //        // Update the rest of the C prompt context
            //                             //        ctx->n_past = wrapper->promptContext.n_past;
            //                             //        ctx->n_ctx = wrapper->promptContext.n_ctx;
            //                             //        ctx->n_predict = wrapper->promptContext.n_predict;
            //                             //        ctx->top_k = wrapper->promptContext.top_k;
            //                             //        ctx->top_p = wrapper->promptContext.top_p;
            //                             //        ctx->temp = wrapper->promptContext.temp;
            //                             //        ctx->n_batch = wrapper->promptContext.n_batch;
            //                             //        ctx->repeat_penalty = wrapper->promptContext.repeat_penalty;
            //                             //        ctx->repeat_last_n = wrapper->promptContext.repeat_last_n;
            //                             //        ctx->context_erase = wrapper->promptContext.contextErase;
            //                             //    }
            //                             //    catch (const std::exception &e)
            //                             //    {
            //                             //        std::cerr << e.what() << '\n';
            //                             //    } 
            //                                },
            //                            config);

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

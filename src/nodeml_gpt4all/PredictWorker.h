#pragma once

#include <napi.h>
#include <functional>
#include <llmodel_c.h>
#include <llmodel.h>
#include <future>

namespace nodeml_gpt4all
{

    
    struct TokenCallbackInfo
    {
        int32_t tokenId;
        std::string total;
        std::string token;
    };

    struct LLModelWrapper
    {
        LLModel *llModel = nullptr;
        LLModel::PromptContext promptContext;
        ~LLModelWrapper() { delete llModel; }
    };

    struct PredictWorkerConfig
    {
        Napi::Function tokenCallback;
        llmodel_model model;
        std::string prompt;
        llmodel_prompt_context context;
        std::string result;
        std::mutex * mutex;
    };

    class PredictWorker : public Napi::AsyncWorker
    {
    public:
        PredictWorker(Napi::Env env, PredictWorkerConfig config);
        ~PredictWorker();
        void Execute() override;
        void OnOK() override;
        void OnError(const Napi::Error &e) override;
        Napi::Promise GetPromise();

        bool ResponseCallback(int32_t token_id, const std::string token);
        bool RecalculateCallback(bool isrecalculating);
        bool PromptCallback(int32_t tid);

    private:
        Napi::Promise::Deferred promise;
        std::string result;
        PredictWorkerConfig _config;
        Napi::ThreadSafeFunction _tsfn;
    };

    PredictWorker::PredictWorker(Napi::Env env, PredictWorkerConfig config)
        : promise(Napi::Promise::Deferred::New(env)), _config(config), AsyncWorker(env) {
            _tsfn = Napi::ThreadSafeFunction::New(config.tokenCallback.Env(),config.tokenCallback,"SomeName",0,1,this);
        }

    PredictWorker::~PredictWorker()
    {
        _tsfn.Release();
    }

    void PredictWorker::Execute()
    {
        _config.mutex->lock();

        LLModelWrapper *wrapper = reinterpret_cast<LLModelWrapper *>(_config.model);

        auto ctx = &_config.context;

        if (size_t(ctx->n_past) < wrapper->promptContext.tokens.size())
            wrapper->promptContext.tokens.resize(ctx->n_past);

        // Copy the C prompt context
        wrapper->promptContext.n_past = ctx->n_past;
        wrapper->promptContext.n_ctx = ctx->n_ctx;
        wrapper->promptContext.n_predict = ctx->n_predict;
        wrapper->promptContext.top_k = ctx->top_k;
        wrapper->promptContext.top_p = ctx->top_p;
        wrapper->promptContext.temp = ctx->temp;
        wrapper->promptContext.n_batch = ctx->n_batch;
        wrapper->promptContext.repeat_penalty = ctx->repeat_penalty;
        wrapper->promptContext.repeat_last_n = ctx->repeat_last_n;
        wrapper->promptContext.contextErase = ctx->context_erase;

        // Call the C++ prompt method
        wrapper->llModel->prompt(
            _config.prompt, [](int32_t tid)
            { return true; },
            [=](int32_t token_id, const std::string tok)
            {
                return ResponseCallback(token_id,tok);
            },
            [](bool isRecalculating)
            {
                return isRecalculating;
            },
            wrapper->promptContext);

        // Update the C context by giving access to the wrappers raw pointers to std::vector data
        // which involves no copies
        ctx->logits = wrapper->promptContext.logits.data();
        ctx->logits_size = wrapper->promptContext.logits.size();
        ctx->tokens = wrapper->promptContext.tokens.data();
        ctx->tokens_size = wrapper->promptContext.tokens.size();

        // Update the rest of the C prompt context
        ctx->n_past = wrapper->promptContext.n_past;
        ctx->n_ctx = wrapper->promptContext.n_ctx;
        ctx->n_predict = wrapper->promptContext.n_predict;
        ctx->top_k = wrapper->promptContext.top_k;
        ctx->top_p = wrapper->promptContext.top_p;
        ctx->temp = wrapper->promptContext.temp;
        ctx->n_batch = wrapper->promptContext.n_batch;
        ctx->repeat_penalty = wrapper->promptContext.repeat_penalty;
        ctx->repeat_last_n = wrapper->promptContext.repeat_last_n;
        ctx->context_erase = wrapper->promptContext.contextErase;
    }

    void PredictWorker::OnOK()
    {
        _config.mutex->unlock();
        promise.Resolve(Napi::String::New(Env(), result));
    }

    void PredictWorker::OnError(const Napi::Error &e)
    {
        _config.mutex->unlock();
        promise.Reject(e.Value());
    }

    Napi::Promise PredictWorker::GetPromise()
    {
        return promise.Promise();
    }

    bool PredictWorker::ResponseCallback(int32_t token_id, const std::string token)
    {
        if (token_id == -1)
        {
            return false;
        }

        result += token;

       std::promise<bool> promise;

       auto info = new TokenCallbackInfo();
       info->tokenId = token_id;
       info->token = token;
       info->total = result;

       auto future = promise.get_future();

       _tsfn.BlockingCall(info, [&promise](Napi::Env env, Napi::Function jsCallback, TokenCallbackInfo *value)
       {
           // Transform native data into JS data, passing it to the provided
           // `jsCallback` -- the TSFN's JavaScript function.

           auto jsResult = jsCallback.Call({Napi::Number::New(env, value->tokenId), Napi::String::New(env, value->token), Napi::String::New(env,value->total)}).ToBoolean();

           promise.set_value(jsResult);
           // We're finished with the data.
           delete value;
       });

       return future.get();
    }

    bool PredictWorker::RecalculateCallback(bool isRecalculating)
    {
        return isRecalculating;
    }

    bool PredictWorker::PromptCallback(int32_t tid)
    {
        return true;
    }
}
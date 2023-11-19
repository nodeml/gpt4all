#pragma once

#include <napi.h>
#include "llmodel_c.h"
#include <thread>
#include <mutex>
#include <iostream>
#include <atomic>
#include <memory>

namespace nodeml_gpt4all
{

  

  struct TsfnContext
  {
    public:
      TsfnContext(Napi::Env env, llmodel_model model, std::string prompt, llmodel_prompt_context context);
      std::thread nativeThread;
      Napi::Env env_;
      Napi::Promise::Deferred deferred_;
      llmodel_model model_;
      std::string prompt_;
      llmodel_prompt_context context_;
      std::string result;
      Napi::ThreadSafeFunction tsfn;

    // Some data to pass around
    // int ints[ARRAY_LENGTH];
  };

  // The thread entry point. This takes as its arguments the specific
  // threadsafe-function context created inside the main thread.
  void threadEntry(TsfnContext *ctx);

  // The thread-safe function finalizer callback. This callback executes
  // at destruction of thread-safe function, taking as arguments the finalizer
  // data and threadsafe-function context.
  void FinalizerCallback(Napi::Env env, void *finalizeData, TsfnContext *ctx);

  bool response_callback(int32_t token_id, const char *response);
  bool recalculate_callback(bool isrecalculating);
  bool prompt_callback(int32_t tid);


  namespace
    {
        static TsfnContext *globalContext = nullptr;
    }

    TsfnContext::TsfnContext(Napi::Env env, llmodel_model model, std::string prompt, llmodel_prompt_context context) : env_(env), model_(model), prompt_(prompt), context_(context), result(""), deferred_(Napi::Promise::Deferred::New(env))
    {
        /***/
    }

    bool response_callback(int32_t token_id, const char *response)
    {
        if (token_id == -1)
        {
            return false;
        }

        // globalContext->result += std::string(response);

        // TokenCallbackInfo info;
        // info.tokenId = token_id;
        // info.total = globalContext->result;
        // info.token = std::string(response);

        auto shouldContinue = true;

        // globalContext->tsfn.BlockingCall(&info, [&shouldContinue](Napi::Env env, Napi::Function jsCallback, TokenCallbackInfo *callInfo)
        //                                  {
        //             auto jsResult = jsCallback.Call({Napi::Number::New(env,callInfo->tokenId), Napi::String::New(env,callInfo->token),Napi::String::New(env,callInfo->total)});
        //             shouldContinue = jsResult.ToBoolean(); });

        return shouldContinue;
    }

    bool recalculate_callback(bool isrecalculating)
    {
        return isrecalculating;
    }

    bool prompt_callback(int32_t tid)
    {
        return true;
    }

    void threadEntry(TsfnContext *ctx)
    {
        Napi::Error::Fatal("ThreadEntry","IN DA THREAD");
        globalContext = ctx;
        try
        {

            llmodel_prompt(globalContext->model_,
                           globalContext->prompt_.c_str(),
                           &prompt_callback,
                           &response_callback,
                           &recalculate_callback,
                           &globalContext->context_);
        }
        catch (const std::exception &e)
        {
          Napi::Error::Fatal("ThreadEntry",e.what());
        }

        ctx->tsfn.Release();
    }

    void FinalizerCallback(Napi::Env env, void *finalizeData, TsfnContext *ctx)
    {

        // Resolve the Promise previously returned to JS
        ctx->deferred_.Resolve(Napi::String::New(env, ctx->result));
        // Wait for the thread to finish executing before proceeding.
        ctx->nativeThread.join();
        globalContext = nullptr;
        delete ctx;
    }

}
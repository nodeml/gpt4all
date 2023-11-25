#include "utils.h"
#include <iostream>

namespace nodeml_gpt4all
{
    namespace utils
    {
        
        void getPromptContext(Napi::Object obj,llmodel_prompt_context &context)
        {
            context.logits = nullptr;
            context.tokens = nullptr;
            context.n_past = 0;
            context.n_ctx = 1024;
            context.n_predict = 128;
            context.top_k = 40;
            context.top_p = 0.9f;
            context.temp = 0.72f;
            context.n_batch = 1;
            context.repeat_penalty = 1.0f;
            context.repeat_last_n = 10;
            context.context_erase = 0.5;

            if(obj.Has("numPast")){
                context.n_past = obj.Get("numPast").ToNumber().FloatValue();
            }

            if(obj.Has("numContext")){
                context.n_ctx = obj.Get("numContext").ToNumber().FloatValue();
            }

            if(obj.Has("numPredict")){
                context.n_predict = obj.Get("numPredict").ToNumber().FloatValue();
            }

            if(obj.Has("topK")){
                context.top_k = obj.Get("topK").ToNumber().FloatValue();
            }

            if(obj.Has("topP")){
                context.top_p = obj.Get("topP").ToNumber().FloatValue();
            }

            if(obj.Has("temperature")){
                context.temp = obj.Get("temperature").ToNumber().FloatValue();
            }

            if(obj.Has("repeatPenalty")){
                context.repeat_penalty = obj.Get("repeatPenalty").ToNumber().FloatValue();
            }

            if(obj.Has("repeatLastN")){
                context.repeat_last_n = obj.Get("repeatLastN").ToNumber().FloatValue();
            }

            if(obj.Has("contextErase")){
                context.context_erase = obj.Get("contextErase").ToNumber().FloatValue();
            }
        }
    }
}

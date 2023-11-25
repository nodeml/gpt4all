#pragma once

#include <napi.h>
#include <functional>
#include <llmodel_c.h>

namespace nodeml_gpt4all
{

    namespace utils
    {
        void getPromptContext(Napi::Object obj,llmodel_prompt_context &context);

        //Napi::Object Init(Napi::Env env, Napi::Object exports);
    }
}

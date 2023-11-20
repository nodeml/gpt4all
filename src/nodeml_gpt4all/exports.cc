#include <napi.h>
#include "llmodel_c.h"
#include <nodeml_gpt4all/Model.h>


Napi::Value SetLibraryPath(const Napi::CallbackInfo &info){
    auto env = info.Env();
    try
    {
        llmodel_set_implementation_search_path(info[0].As<Napi::String>().Utf8Value().c_str());
    }
    catch (const std::exception &e)
    {
        throw Napi::Error::New(env, e.what());
    }

    return env.Undefined();
}

Napi::Object InitModule(Napi::Env env, Napi::Object exports)
{
    nodeml_gpt4all::Model::Init(env,exports);
    exports.Set("setLibraryPath",Napi::Function::New(env,SetLibraryPath));
    
    return exports;
}


NODE_API_MODULE(nodeml_gpt4all, InitModule);

// if(obj.Has("numPast")){
//   context.n_past = obj.Get("numPast").ToNumber().FloatValue();
// }

// if(obj.Has("numContext")){
//   context.n_ctx = obj.Get("numContext").ToNumber().FloatValue();
// }

// if(obj.Has("numPredict")){
//   context.n_predict = obj.Get("numPredict").ToNumber().FloatValue();
// }

// if(obj.Has("topK")){
//   context.top_k = obj.Get("topK").ToNumber().FloatValue();
// }

// if(obj.Has("topP")){
//   context.top_p = obj.Get("topP").ToNumber().FloatValue();
// }

// if(obj.Has("repeatPenalty")){
//   context.repeat_penalty = obj.Get("repeatPenalty").ToNumber().FloatValue();
// }

// if(obj.Has("repeatLastN")){
//   context.repeat_last_n = obj.Get("repeatLastN").ToNumber().FloatValue();
// }

// if(obj.Has("contextErase")){
//   context.context_erase = obj.Get("contextErase").ToNumber().FloatValue();
// }

export interface IModelInferenceContext {
  numPast: number;
  numContext: number;
  numPredict: number;
  topK: number;
  topP: number;
  temperature: number;
  repeatPenalty: number;
  repeatLastN: number;
  contextErase: number;
}

export declare class Model {
  destroy: () => void;

  predict: (prompt: string,tokenCallback: (tokenId: number, token: string, total: string) => boolean,context?: Partial<IModelInferenceContext>) => Promise<string>

}

export declare function create(modelPath: string): Promise<Model>;

export declare function setLibraryPath(path: string): void;
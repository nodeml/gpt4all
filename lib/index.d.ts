
export declare class Model {

  static load: (modelPath: string,device: 'gpu' | 'cpu') => Promise<Model>;

  destroy: () => void;

  predict: (prompt: string,tokenCallback: (tokenId: number, token: string, total: string) => boolean) => Promise<string>

}

export declare function setLibraryPath(path: string): void;
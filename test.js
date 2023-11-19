//C++ Tensor Slicing https://stackoverflow.com/questions/56908893/copy-a-chunk-of-one-tensor-into-another-one-in-c-api?rq=4
const gpt4all = require('./lib')
const path = require('node:path')
// let a = torch.tensor(new Int32Array([1, 2, 3, 4, 5, 6])).reshape([2, 3]);
// let b = torch
//   .tensor(new Int32Array([1, 2, 3, 4, 5, 6].reverse()))
//   .reshape([2, 3]);
// console.log(a.toMultiArray());
// // a = a.add(b);
// a.set(b.get(1, [0, 3]), 1, [0, 3]);
// console.log(a.toMultiArray());
// // torch.jit
// //   .load("./segmentation.torchscript")
// //   .forward(torch.rand([1, 3, 640, 640]))
// //   .then((h) => {
// //     console.log("FORWARD RESULT", h);
// //   });
// console.log(
//   a.get(1, [0, 3]).toMultiArray(),
//   torch.arange(0, 20, torch.types.double).toArray(),
//   torch.arange(0, 20, torch.types.uint8).toArray(),
//   a.amax(0).toMultiArray(),
//   b.toMultiArray(),
//   torch.cat([a, b]).toMultiArray(),
//   torch.equal(a, b).toMultiArray()
// );

// let g = torch.zeros([4, 3]);

// "binary": {
//   "napi_versions": [
//     7
//   ]
// }

// const bytes = torch.vision.io.readFile("./test.jpg");
// let img = torch.vision.io.decodeImage(bytes);
// img = torch.nn.functional.interpolate(
//   torch.nn.functional.pad(img, [0, Math.floor(1400 - 994)]).unsqueeze(0),
//   [640, 640]
// );
const formatMemoryUsage = (data) => `${Math.round(data / 1024 / 1024 * 100) / 100} MB`;
// console.log(img.shape, img.dtype);
// const x = torch.tensor([true,false,true])
// console.log(x.dtype,x.get([null,2]).toArray())
async function main() {
    
    // const modelPath = path.resolve('wizardLM-13B-Uncensored.ggmlv3.q4_0.bin')
    const modelPath = path.resolve('mistral-7b-openorca.Q4_0.gguf')
    

    const model = await gpt4all.Model.load(modelPath)

    console.log("Loaded",modelPath)

    console.log("Starting predict")
    console.time("predict")
    const predictResult = await model.predict(`A chat between a user and an Ai Assistant
    USER: how are you ?
    ASSISTANT: `,(tokenId,token,total) => {
        console.log("Args",total)
        return !total.includes('USER:');
    })
    console.timeEnd("predict")
    console.log("RESULT",predictResult,predictResult.length)
}

main()


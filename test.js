
const gpt4all = require('./lib')
const path = require('node:path')

async function main() {
    
    // const modelPath = path.resolve('wizardLM-13B-Uncensored.ggmlv3.q4_0.bin')
    const modelPath = path.resolve('mistral-7b-openorca.Q4_0.gguf')
    

    const model = await gpt4all.create(modelPath)

    console.log("Loaded",modelPath)

    console.log("Starting predict")
    console.time("predict")
    const predictResult = await model.predict(`A chat between a user and an Ai Assistant
    USER: how are you ?
    ASSISTANT: `,(tokenId,token,total) => {
        console.log("Args",total)
        return !total.includes('USER:');
    },{
        temperature: 1
    })
    console.timeEnd("predict")
    console.log("RESULT",predictResult,predictResult.length)
}

main()


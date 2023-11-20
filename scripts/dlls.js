const fs = require('fs');
const path = require('path');

const workingDir = process.cwd();
const buildType = fs.readdirSync(path.join(workingDir,'bin'))[0]

const binDir = path.join(workingDir,'bin',buildType)

const filesToCopy = fs.readdirSync(binDir).map(c => [path.join(binDir,c),c])

filesToCopy.forEach(c => fs.copyFileSync(c[0],path.join(workingDir,buildType,c[1])))

"use strict";
const path = require('node:path')
const libraryPath = path.resolve(path.join(__dirname,'..','build','Release'))
const mod = require("bindings")("nodeml_gpt4all");
mod.setLibraryPath(libraryPath)
module.exports = mod

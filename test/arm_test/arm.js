const path = require('path');
const sljs = require('../../build/Release/sljs'); // Adjust if your .node file name differs

const soPath = path.resolve(__dirname, 'game_arm.so');
const symbol = 'game_logic';

const result = sljs.runARMFunc(soPath, symbol);
console.log(result);

// test-mode7.js
const sljs = require('../build/Release/sljs');

const path = './libgreet.so';
const symbol = 'hello_func';

const result = sljs.runStringReturn(path, symbol);
console.log('Result:', result);

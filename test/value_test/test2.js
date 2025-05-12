const sljs = require('../build/Release/sljs.node');

const value = sljs.runValue('./math.so', 'give_number');
console.log('Returned:', value);

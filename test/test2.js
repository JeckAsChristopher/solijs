const sljs = require('../build/sljs.node');

const value = sljs.runValue('./math.so', 'give_number');
console.log('Returned:', value);

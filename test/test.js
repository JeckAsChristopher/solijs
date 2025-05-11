const sljs = require('../build/sljs.node');
console.log(sljs.runText('./texttest.so', 'hello_func'));

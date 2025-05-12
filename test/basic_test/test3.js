const sljs = require('../build/Release/sljs.node'); // Path may vary

// Logger for C++ stdout
sljs.setStdoutLogger((msg) => {
  console.log('[Native Log]:', msg);
});

const soPath = './libtest.so';

// Inspect symbols
console.log('Symbols:', sljs.inspect(soPath));

// Test runStringReturn (calls greet)
const result1 = sljs.runStringReturn(soPath, 'greet');
console.log('greet() =>', result1);

// Test runArgsString (calls echo with 2 args)
const result2 = sljs.runArgsString(soPath, 'echo', ['foo', 'bar']);
console.log('echo() =>', result2);

// Test runText (calls say_hello which prints to stdout)
const result3 = sljs.runText(soPath, 'say_hello');
console.log('say_hello() =>', result3);

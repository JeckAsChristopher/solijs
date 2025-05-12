const sljs = require('../build/Release/sljs'); // Your native module

// Create a test buffer with some bytes
const buffer = Buffer.from([0x13, 0x37, 0x42, 0x99]);

// Call the C++ function that handles the raw buffer
const result = sljs.runBufferFunc('./test.so', 'handle_buffer', buffer);

console.log("runBufferFunc result:", result);

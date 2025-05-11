const sljs = require('../build/Release/sljs');

// Set stdout logger if necessary
sljs.setStdoutLogger((msg) => {
    process.stdout.write(msg);
});

// Run the function from the shared object file
sljs.runText('./prompt.so', 'hello_prompt');

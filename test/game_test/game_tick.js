const sljs = require('../build/Release/sljs'); // Your native module

// Define a deltaTime for the game tick (e.g., 0.016 for ~60 FPS)
const deltaTime = 0.016;

// Call the Game Tick function from C++ code in test.so
const result = sljs.runGameTick('./game_tick.so', 'game_tick', deltaTime);

console.log(result);  // Should print: "Game tick executed"

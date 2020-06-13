# tetrominic
tetromino game


## Installation

Try it out with `npx`:

    npx tetrominic

Install globally using node (downloads prebuilt WebAssembly binary):

    npm install -g tetrominic
    
You can also build it yourself (native binary):

    cd src
    make

## Usage

Start the game as single player:

    tetrominic

Use the arrow keys to rotate and move the falling tetromino piece:
  - ← or "j" to move left
  - ↑ or "k" to rotate clockwise, "i" to rotate anticlockwise 
  - → or "l" to move right
  - ↓ or "m" to move down
  - space to drop and lock the piece instantly

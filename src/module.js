var keystrokes = [];

Module.onkeypress = function (s, key) {
  keystrokes.push((key && key.name) || s);
};

Module.print = function (s) {
  process.stdout.write(s + '\n');
};

var tetField = null;
var changed = null;

async function enterTetField(piece) {
  if (!tetField) {
    const {TetField} = await import('./game/tetfield.js');
    const {Tetmino} = await import('./game/tetmino.js');

    tetField = new TetField();
    tetField.piece = new Tetmino();
  }

  tetField.enter(new tetField.piece.constructor(piece));
  tetField.piece.col += 3;
}

function runTetField(action) {
  changed = tetField.run(action);
  return changed;
}
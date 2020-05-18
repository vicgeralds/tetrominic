var keystrokes = [];

Module.onkeypress = function (s, key) {
  keystrokes.push((key && key.name) || s);
};

Module.print = function (s) {
  process.stdout.write(s + '\n');
};

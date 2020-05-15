const readline = require('readline')

const keystrokes = []

readline.emitKeypressEvents(process.stdin)

process.stdin.setRawMode(true)

process.stdin.on('keypress', onkeypress)

function onkeypress (s, key) {
  keystrokes.push((key && key.name) || s)
}

module.exports = keystrokes

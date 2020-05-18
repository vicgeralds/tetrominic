const readline = require('readline')
const { onkeypress } = require('./lib/tetrominic.js')

readline.emitKeypressEvents(process.stdin)

process.stdin.on('keypress', onkeypress)

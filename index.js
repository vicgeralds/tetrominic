import process from 'node:process'
import { TETFIELD_FPS } from './lib/game/tetfield.js'
import { Terminal } from './lib/terminal.js'
import tetrominic from './lib/tetrominic.js'

const {
  cwrap,
  _restore_terminal: restoreTerminal
} = await tetrominic()

const resizeTerminal = cwrap('resize_terminal', null, ['number', 'number'])

/** @type {(keypress: string) => number} */
const runGame = cwrap('run_game', 'number', ['string'])

export function start () {
  const terminal = new Terminal()

  terminal.on('resize', resizeTerminal)
  terminal.on('restore', restoreTerminal)
  terminal.on('close', process.exit)

  terminal.resize()

  setInterval(() => {
    if (!runGame(terminal.keystrokes.shift() || '')) {
      process.exit()
    }
  }, 1000 / TETFIELD_FPS)
}

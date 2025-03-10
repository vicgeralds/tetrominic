import { EventEmitter } from 'node:events'
import process from 'node:process'
import * as readline from 'node:readline'

const ESC = '\x1b'
const CSI = ESC + '['

const ansiEscapes = {
  cursorHide: CSI + '?25l',
  cursorShow: CSI + '?25h'
}

export class Terminal extends EventEmitter {
  input = process.stdin
  output = process.stdout
  keystrokes = ['']

  constructor () {
    super()
    readline.emitKeypressEvents(this.input)
    this.setup()
    this.input.resume()
  }

  setup () {
    this.setRawMode()
    this.resize()

    this.input.on('keypress', this.onKeypress)
    this.output.on('resize', this.resize)
    process.on('exit', this.restore)
  }

  setRawMode (mode = true, hideCursor = mode) {
    if (this.input.isTTY) {
      this.input.setRawMode(mode)
      this.output.write(hideCursor
        ? ansiEscapes.cursorHide
        : ansiEscapes.cursorShow)
    }
  }

  /**
   * @param {string | undefined} _data
   * @param {import('node:readline').Key} key
   */
  onKeypress = (_data, key) => {
    this.keystrokes.push(key.name)

    if (key.ctrl) {
      switch (key.name) {
        case 'z':
          return this.pause()
        case 'd':
        case 'c':
          this.quit()
      }
    }
  }

  pause () {
    this.restore()
    process.kill(process.pid, 'SIGTSTP')
    this.setup()
  }

  quit () {
    this.restore()
    this.input.destroy()
    this.emit('close')
  }

  resize = () => {
    if (this.output.isTTY) {
      this.emit('resize', this.output.columns, this.output.rows)
    }
  }

  restore = () => {
    this.emit('restore')
    this.setRawMode(false)
    this.keystrokes = []
    this.input.removeListener('keypress', this.onKeypress)
    this.output.removeListener('resize', this.resize)
    process.removeListener('exit', this.restore)
  }
}

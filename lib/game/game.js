import { runAtFramesPerSecond } from './animate.js'
import { TETFIELD_FPS, TETFIELD_SPAWN, TETFIELD_TOP_OUT, TetField } from './tetfield.js'
import { Tetmino } from './tetmino.js'
import { renderBlocks, renderClearedBlocks } from './view.js'

const randomPiece = () => Math.floor(Math.random() * 7)

export class Game {
  tetField = new TetField()
  nextPiece = new Tetmino(randomPiece(), 0, GAME_SPAWN_COL)
  blocks = [' ']

  constructor () {
    this.spawnPiece()
    this.changed = this.tetField.run()
    this.updateCallback = this.update.bind(this)
  }

  start () {
    this.update = runAtFramesPerSecond(this.updateCallback, TETFIELD_FPS)
  }

  spawnPiece () {
    const piece = this.nextPiece

    this.tetField.enter(piece)

    this.nextPiece = new Tetmino(
      randomPiece(),
      piece.row + piece.height,
      GAME_SPAWN_COL,
      this.tetField.gravity
    )
  }

  getAction (input = '') {
    if (input === 'j' || input === 'left') {
      return 'move left'
    }
    if (input === 'k' || input === 'up') {
      return 'rotate cw'
    }
    if (input === 'i') {
      return 'rotate ccw'
    }
    if (input === 'l' || input === 'right') {
      return 'move right'
    }
    if (input === 'm' || input === 'down') {
      return 'softdrop'
    }
    if (input === 'space') {
      return 'harddrop'
    }

    return ''
  }

  update (input = '') {
    const grid = this.tetField.grid
    const cleared = grid.updateLineClears()

    if (cleared > 0) {
      let row = cleared
      while (row > 0) {
        const mask = grid.shiftClearedBlocks(row)
        renderClearedBlocks(this.blocks, row, mask)
        row = grid.nextClearedRow(row)
      }
    }

    if (input === 'q') {
      this.onQuit()
      return false
    }

    const action = input && this.getAction(input)
    this.changed = this.tetField.run(action, this.changed)

    if (this.tetField.state === TETFIELD_TOP_OUT) {
      this.onGameOver()
      return false
    }

    if (this.tetField.isLockable) {
      this.tetField.lock()
      renderBlocks(this.blocks, this.tetField.piece)

      this.spawnPiece()
    } else {
      this.updateNextPiece()
    }

    this.onRender()
    return true
  }

  updateNextPiece () {
    if (this.tetField.state === TETFIELD_SPAWN) {
      return false
    }

    if (this.nextPiece.row > this.tetField.spawnRow + 1) {
      this.nextPiece.falling--
      if (this.nextPiece.falling > 0) {
        return false
      }
      this.nextPiece.row--
      this.nextPiece.falling = this.tetField.gravity
      return true
    }

    return false
  }

  onRender () {}

  onQuit () {}

  onGameOver () {}
}

const GAME_SPAWN_COL = 3

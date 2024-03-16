import { TetGrid } from './tetgrid.js'
import { Piece, actions } from './piece.js'

/**
 * Tetromino playing field
 */
export class TetField {
  gravity = 20
  state = TETFIELD_SPAWN
  /** @type {typeof actions[number]} */
  lastAction = ''
  /** @type {Record<typeof actions[number], number>} */
  timeout = {}

  constructor (grid = new TetGrid()) {
    this.grid = grid
    for (const a of actions) {
      this.timeout[a] = 0
    }
  }

  /** prepare next tetromino */
  enter (piece = new Piece()) {
    this.piece = piece
    const spawnRow = this.grid.blocks.length - piece.height
    piece.row = spawnRow + 1
    piece.falling = SPAWN_DELAY + 1

    this.state = TETFIELD_SPAWN

    // prevent last action from cancelling spawn delay
    this.stopRetryingAction()
  }

  stopRetryingAction () {
    if (this.lastAction === HARDDROP) {
      this.lastAction = ''
      this.timeout[RETRY_ACTION] = 0
      this.timeout.harddrop += SPAWN_DELAY - AUTOREPEAT_FRAMES
    }
    if (this.lastAction) {
      this.lastAction = ''
      this.timeout[RETRY_ACTION] += SPAWN_DELAY - AUTOREPEAT_FRAMES
    }
  }

  makeMove (action = this.lastAction) {
    if (this.timeout[action] === 0 && this.piece.control(this.grid, action)) {
      if (!this.lastAction || !this.timeout[this.lastAction]) {
        this.lastAction = action
      }
      this.timeout[RETRY_ACTION] = AUTOREPEAT_FRAMES
      this.timeout[action] = AUTOREPEAT_FRAMES
      return action
    }
    return ''
  }

  /**
   * advance one frame
   *
   * @param {typeof actions[number]} action
   */
  run (action = '', out = {
    moved: action,
    dropped: 0
  }) {
    if (typeof action === 'number') {
      action = actions[action]
    }
    for (const a of actions) {
      if (this.timeout[a]) {
        this.timeout[a]--
      }
    }

    out.moved = ''
    out.dropped = 0

    switch (this.state) {
      case TETFIELD_SPAWN:
        this.updatePrespawn(action)
        if (this.piece.falling > 1) {
          // stop spawn timer during line clear animation
          if (this.grid.isClearing) return out

          this.piece.falling--
          return out
        }
        out.dropped = this.piece.update(this.grid, this.gravity)
        if (!out.dropped) {
          this.state = TETFIELD_TOP_OUT
          return out
        }
        this.timeout[RETRY_ACTION] = 1
        this.state = TETFIELD_MOVE
        out.moved = this.updateMove(action)
        break
      case TETFIELD_MOVE:
        out.moved = this.updateMove(action)
        out.dropped = this.piece.update(this.grid, this.gravity)
        break
      case TETFIELD_PLACED:
        // accepting no more actions
        out.dropped = +this.piece.control(this.grid, HARDDROP)
        break
      case TETFIELD_TOP_OUT:
        return out
      default:
        break
    }

    // set timer to block actions for next tetromino during spawn delay
    if (this.timeout[RETRY_ACTION] === 1) {
      this.stopRetryingAction()
    }

    this.updateRemovableRows()

    if (this.piece.isMovable) {
      return out
    }

    this.state = TETFIELD_PLACED
    return out
  }

  /** process moves before the piece is spawned */
  updatePrespawn (action) {
    if (!this.timeout[RETRY_ACTION]) {
      switch (action) {
        case 'rotate cw':
        case 'rotate ccw':
          this.lastAction = action
          break
        default:
          if (action && !this.timeout[action]) {
            this.piece.falling = 1
            this.timeout[action] = 1
          }
          break
      }
    }
  }

  updateMove (action) {
    let moved = ''

    if (this.timeout[action]) {
      if (this.timeout[RETRY_ACTION]) {
        moved = this.makeMove()
      }
      if (action) {
        this.lastAction = action
        this.timeout[RETRY_ACTION] = AUTOREPEAT_FRAMES
      }
    } else if (!(moved = this.makeMove(action))) {
      if (action) {
        this.lastAction = action
        this.timeout[RETRY_ACTION] = WALL_CHARGE_FRAMES
      }
    }

    return moved
  }

  /** check for collision with tetromino when removing cleared rows */
  updateRemovableRows () {
    if (this.grid.delay > 0) {
      return
    }

    let row = this.grid.getClearedRow()
    let grid2

    if (row > 0) {
      grid2 = new TetGrid(this.grid.cols, this.grid.blocks.slice())
    }

    // find row that causes collision with tetmino
    while (row > 0 && (grid2.shiftClearedBlocks(row) || this.piece.canMoveTo(grid2))) {
      row = this.grid.nextClearedRow(row)
    }

    this.grid.preventLineClears(row)
  }

  get isLockable () {
    return this.state === TETFIELD_PLACED &&
      this.grid.isLockable(this.piece)
  }

  /**
   * lock tetromino into place and init line clear
   *
   * @return number of lines cleared
   */
  lock () {
    // add blocks
    this.grid.xorBlocks(this.piece)

    const numLinesCleared = this.grid.updateCompletedLines(this.piece.row)

    return numLinesCleared
  }
}

export const TETFIELD_SPAWN = 0
export const TETFIELD_MOVE = 1
export const TETFIELD_PLACED = 2
export const TETFIELD_TOP_OUT = 3

/**
 * Time constants are in frames assuming that the game will be run at this
 * speed
 */
export const TETFIELD_FPS = 60

/** auto repeat rate. This limits how fast an action can be repeated */
const AUTOREPEAT_FRAMES = 3

/** how many times a blocked move is retried */
const WALL_CHARGE_FRAMES = 4

/** time until appearance of next tetromino */
const SPAWN_DELAY = 25

/** `timeout[RETRY_ACTION]` is number of times to retry last action */
const RETRY_ACTION = ''

const HARDDROP = 'harddrop'
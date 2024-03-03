/**
 * @typedef {import('./tetgrid.js').TetGrid} TetGrid
 */

export const actions = Object.freeze(/** @type {const} */ ([
  '',
  'rotate cw',
  'rotate ccw',
  'move right',
  'move left',
  'harddrop',
  'softdrop'
]))

/**
 * A moving shape that can be rotated and stacked in a grid
 */
export class Piece {
  /** width of shape image */
  width = PIECE_WIDTH

  /** height of shape image */
  height = PIECE_HEIGHT

  constructor (shape = 0, row = 0, col = 0, delay = 1) {
    /** bitmap representation of the piece in one of its orientations */
    this.shape = shape
    this.row = row
    this.col = col
    /** gravity timer */
    this.falling = delay
    /** move reset timer */
    this.lockDelayMove = 0
    /** step reset timer */
    this.lockDelayStep = 0
    /** floor kick state */
    this.climbed = 2
  }

  get isMovable () {
    return (
      this.falling || (this.lockDelayMove && this.lockDelayStep)
    ) > 0
  }

  /** get range of rows occupied by shape */
  getOccupiedRows () {
    let shape = this.shape
    let begin = this.row
    const rowMask = (1 << this.width) - 1

    while (shape && !(shape & rowMask)) {
      begin++
      shape >>>= this.width
    }

    let end = begin
    while (shape & rowMask) {
      end++
      shape >>>= this.width
    }

    return [begin, end]
  }

  /**
   * check collision when moving piece inside grid
   *
   * @param {TetGrid} grid
   */
  canMoveTo (grid, col = this.col, shape = this.shape) {
    return !(shape & grid.getBlocksAt(
      this.row, col, this.width, this.height
    ))
  }

  /**
   * get distance to the blocks below
   *
   * @param {TetGrid} grid
   * @param {number} max height to return when reached without detecting
   * collision
   */
  dropHeight (grid, max) {
    let h = 0

    if (max > this.row) {
      max = this.row
    }

    if (max > 0) {
      let box = grid.getBlocksAt(
        this.row - 1, this.col, this.width, this.height
      )

      while (!(this.shape & box)) {
        h++
        if (h === max) {
          break
        }
        box = (box << this.width) | grid.getBlocksAt(
          this.row - h - 1, this.col, this.width
        )
      }
    }
    return h
  }

  /**
   * try to floor kick a rotated shape, moving it up to resolve collision
   *
   * @param {TetGrid} grid
   * @param {number} rotated shape
   */
  floorKick (grid, rotated) {
    if (this.climbed > 0) {
      return false
    }

    const overlapBottomRow = rotated & grid.getBlocksAt(
      this.row, this.col, this.width
    )
    if (!overlapBottomRow) {
      return false
    }

    const overlapMovingUp = rotated & grid.getBlocksAt(
      this.row + 1, this.col, this.width, this.height
    )
    if (overlapMovingUp) {
      return false
    }

    this.climbed = 1
    this.row++
    if (this.falling) {
      this.falling = 1
    }

    this.onRotate(rotated)
    return true
  }

  drop (grid, n = 1) {
    const h = this.dropHeight(grid, n)
    this.row -= h
    this.climbed -= h
    if (this.climbed < 0) {
      this.climbed = 0
    }
    return h
  }

  /**
   * process moves
   *
   * @param {TetGrid} grid
   * @param {typeof actions[number]} action
   *
   * @return true if position or orientation changed or if a locking drop was
   * made, otherwise false
   */
  control (grid, action = '') {
    switch (action) {
      case 'rotate cw':
        return this.rotateTo(grid, this.rotate(CLOCKWISE))
      case 'rotate ccw':
        return this.rotateTo(grid, this.rotate(COUNTERCLOCKWISE))
      case 'move right':
        return this.moveTo(grid, this.col + 1)
      case 'move left':
        return this.moveTo(grid, this.col - 1)
      case 'harddrop':
      case 'softdrop':
        this[action](grid)
        return true

      default: return false
    }
  }

  rotate (_n) {
    return this.shape
  }

  rotateTo (grid, rotated, col = this.col) {
    if (rotated !== this.shape && this.canMoveTo(grid, col, rotated)) {
      this.onRotate(rotated, col)
      return true
    }

    return false
  }

  onRotate (rotated, col = this.col) {
    this.shape = rotated
    this.onMove(col)
  }

  moveTo (grid, col) {
    if (this.canMoveTo(grid, col)) {
      this.onMove(col)
      return true
    }

    return false
  }

  harddrop (grid) {
    this.drop(grid, this.row)
    this.onLock()
  }

  softdrop (grid) {
    const dropped = this.drop(grid, this.row)
    if (!dropped) {
      this.onLock()
    }
  }

  onLock () {
    this.falling = 0
    this.lockDelayMove = 0
    this.lockDelayStep = 0
  }

  /** called after any successful shifting or rotation */
  onMove (col) {
    this.col = col
    this.lockDelayMove = LOCK_DELAY_MOVE + 1
  }

  /**
   * process gravity
   *
   * @param {TetGrid} grid
   * @param {number} gravity number of frames until drop
   * @return number of rows dropped
   */
  update (grid, gravity = 1) {
    this.unfloat(grid)
    if (!this.falling) {
      if (this.lockDelayMove > 0) {
        this.lockDelayMove--
      }
      if (this.lockDelayStep > 0) {
        this.lockDelayStep--
      }
      return 0
    }

    if (this.falling > gravity) {
      this.falling = gravity
    }

    // count down frames until drop
    this.falling--
    if (this.falling > 0) {
      return 0
    }

    if (this.drop(grid)) {
      this.falling = gravity
      return 1
    } else {
      this.falling = 0
      this.startLockDelay(gravity)
      return 0
    }
  }

  /** prevent floating piece */
  unfloat (grid) {
    if (!this.falling) {
      // handle floor kicked
      this.drop(grid, this.climbed)

      this.falling = this.dropHeight(grid, 1)
    }
  }

  startLockDelay (gravity) {
    // shorter initial lock delay at slower speeds
    const lockDelay = LOCK_DELAY_MOVE - gravity
    this.lockDelayStep = LOCK_DELAY_STEP
    if (lockDelay > this.lockDelayMove) {
      this.lockDelayMove = lockDelay
    }
  }
}

const PIECE_WIDTH = 4
const PIECE_HEIGHT = 4

/* timer presets */
const LOCK_DELAY_MOVE = 20
const LOCK_DELAY_STEP = 30

export const CLOCKWISE = 1
export const COUNTERCLOCKWISE = 3
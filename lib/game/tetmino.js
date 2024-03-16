import { CLOCKWISE, COUNTERCLOCKWISE, Piece } from './piece.js'

/**
 * A moving tetromino that can be rotated and stacked in a grid
 */
export class Tetmino extends Piece {
  /**
   * create a tetromino piece
   *
   * @param {'i' | 'j' | 'l' | 'o' | 's' | 't' | 'z' | number} piece
   * letter name of the tetromino, or a number 0-6
   */
  constructor (piece = 0, row = 1, col = 0, delay = 1) {
    if (typeof piece === 'string') {
      piece = tetminoTiles.indexOf(piece)
    }
    super(tetminoShapes[piece][0], row, col, delay)
    this.tile = tetminoTiles.charAt(piece)
    this.piece = piece
    this.col += Boolean(tetminoShapes[piece][1] & 0x1111)
  }

  rotate (n = CLOCKWISE) {
    if (n === COUNTERCLOCKWISE) {
      return this.rotateRightHanded()
    }
    const rotationStates = tetminoShapes[this.piece]
    const i = rotationStates.indexOf(this.shape)
    return rotationStates[(i + n) % rotationStates.length]
  }

  rotateRightHanded () {
    const rotated = this.rotate()
    return (leftHandedRotationStates.includes(rotated))
      ? (rotated << 1)
      : rotated
  }

  rotateTo (grid, rotated, col = this.col) {
    if (super.rotateTo(grid, rotated, col)) {
      return true
    }

    if (col !== this.col) {
      return false
    }

    switch (this.shape) {
      case TETMINO_O:
        return false
      case TETMINO_I:
      case TETMINO_S:
      case TETMINO_Z:
        // try both vertical orientations
        if (leftHandedRotationStates.includes(rotated)) {
          return super.rotateTo(grid, this.rotateRightHanded())
        }
        return super.rotateTo(grid, this.rotate())
      case TETMINO_J3:
      case TETMINO_L3:
      case TETMINO_T3:
        if (this.floorKick(grid, rotated)) {
          return true
        }
        break
      default:
        break
    }

    return this.wallKick(grid, rotated)
  }

  wallKick (grid, rotated) {
    if (rotated === TETMINO_I) {
      return super.rotateTo(grid, rotated,
        this.shape === TETMINO_I2
          ? this.col - 1
          : this.col + 1)
    }

    const overlap = rotated & grid.getBlocksAt(
      this.row, this.col, this.width, this.height
    )
    // left column
    const mask1 = TETMINO_I2 >> 1
    // right column
    const mask3 = TETMINO_I2 << 1

    // kick if exactly one of the columns overlaps
    if (overlap === (overlap & mask1) && !(this.shape & mask1)) {
      return super.rotateTo(grid, rotated, this.col + 1)
    }
    if (overlap === (overlap & mask3) && !(this.shape & mask3)) {
      return super.rotateTo(grid, rotated, this.col - 1)
    }

    return false
  }
}

/* tetromino orientations as 4x4 bitmaps */

const TETMINO_I = 0x00F0
const TETMINO_I2 = 0x2222

const TETMINO_O = 0x0066

const TETMINO_J = 0x0074
const TETMINO_J2 = 0x0223
const TETMINO_J3 = 0x0170
const TETMINO_J4 = 0x0622

const TETMINO_L = 0x0071
const TETMINO_L2 = 0x0322
const TETMINO_L3 = 0x0470
const TETMINO_L4 = 0x0226

const TETMINO_S = 0x0063
const TETMINO_S2 = 0x0132

const TETMINO_Z = 0x0036
const TETMINO_Z2 = 0x0231

const TETMINO_T = 0x0072
const TETMINO_T2 = 0x0232
const TETMINO_T3 = 0x0270
const TETMINO_T4 = 0x0262

const tetminoTiles = 'istolzj'

const tetminoShapes = [
  [TETMINO_I, TETMINO_I2],
  [TETMINO_S, TETMINO_S2],
  [TETMINO_T, TETMINO_T2, TETMINO_T3, TETMINO_T4],
  [TETMINO_O],
  [TETMINO_L, TETMINO_L2, TETMINO_L3, TETMINO_L4],
  [TETMINO_Z, TETMINO_Z2],
  [TETMINO_J, TETMINO_J2, TETMINO_J3, TETMINO_J4]
]

const leftHandedRotationStates = /** @type {const} */ ([
  TETMINO_I2, TETMINO_S2, TETMINO_Z2
])

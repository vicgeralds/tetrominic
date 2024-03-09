import { Piece } from './piece.js'

/**
 * Grid representing the set of occupied cells. It also has state for line
 * clear animation
 */
export class TetGrid {
  /** time until next update */
  delay = 0

  /** initialize grid of width cols (excluding walls) */
  constructor (cols = 10, blocksOrPlayfieldHeight = 24) {
    this.cols = cols

    /** rows of occupied grid cells and walls */
    this.blocks = blocksOrPlayfieldHeight instanceof Uint32Array
      ? blocksOrPlayfieldHeight
      : new Uint32Array(blocksOrPlayfieldHeight)

    if (typeof blocksOrPlayfieldHeight === 'number') {
      this.blocks[0] = ((1 << cols) - 1) << LEFT_WALL_WIDTH
      setWalls(this.blocks)
    }
  }

  /** are lines being cleared? */
  get isClearing () {
    return this.getClearedRow() > 0
  }

  /** get mask to test collision */
  getBlocksAt (row = 0, col = 0, width = this.cols, height = 1) {
    let result = 0
    let bits = 0
    const rowMask = (1 << width) - 1

    col += LEFT_WALL_WIDTH

    for (const b of this.blocks.subarray(row, row + height)) {
      result |= ((b >> col) & rowMask) << bits
      bits += width
    }

    return result
  }

  /**
   * add or remove blocks
   *
   * @param {number | Piece} shape
   */
  xorBlocks (shape, row = 0, col = 0, width = this.cols) {
    if (shape instanceof Piece) {
      const piece = shape
      shape = piece.shape
      row = piece.row
      col = piece.col
      width = piece.width
    }
    const rowMask = (1 << width) - 1

    col += LEFT_WALL_WIDTH

    while (shape) {
      this.blocks[row] ^= (shape & rowMask) << col
      row++
      shape >>>= width
    }
  }

  /**
   * find completed lines and mark blocks to clear.
   *
   * @return number of lines cleared
   */
  updateCompletedLines (row) {
    row = this.blocks.indexOf(0xffffffff, row)
    if (row > 0) {
      // init line clear
      this.blocks[row] ^= LINE_CLEAR_MARK
      this.delay += BLOCK_CLEAR_DELAY

      return 1 + this.updateCompletedLines(row + 1)
    }

    return 0
  }

  /** find cleared row to start with from the top */
  getClearedRow () {
    return this.blocks.findLastIndex(isMarkedAsCleared)
  }

  /**
   * tick line clear timer.
   *
   * @return row number > 0 when there are cleared blocks to process
   */
  updateLineClears () {
    if (this.delay > 0) {
      this.delay--
      return 0
    }

    const cleared = this.getClearedRow()
    if (cleared > 0) {
      this.delay = BLOCK_CLEAR_DELAY
    }
    const gridBlocks = this.blocks

    // find empty rows to remove on next update
    for (let i = cleared + 1; i + 1 < gridBlocks.length; i++) {
      if (isEmptyRow(gridBlocks, i) && !isEmptyRow(gridBlocks, i + 1)) {
        gridBlocks[i] ^= LINE_CLEAR_MARK
      }
    }
    return cleared
  }

  /**
   * remove cleared blocks from row (two at a time).
   *
   * if the row is empty: remove it from the grid by moving all rows above it
   * down by 1.
   *
   * @return blocks that have been cleared as set bits, with the leftmost visible
   * column in bit 0
   */
  shiftClearedBlocks (row) {
    const gridBlocks = this.blocks
    if (isEmptyRow(gridBlocks, row)) {
      // remove cleared row
      gridBlocks.set(gridBlocks.subarray(row + 1), row)
      setWalls(gridBlocks, -1)
      return 0
    }

    const b = invertBlocks(gridBlocks, row)
    if (b) {
      gridBlocks[row] &= ~((b << 1) | (b >> 1))
    } else {
      // start clearing from middle
      gridBlocks[row] ^= middleBlocks(this.cols)
    }
    return (b ^ invertBlocks(gridBlocks, row)) >> LEFT_WALL_WIDTH
  }

  /**
   * get next row of cleared blocks.
   *
   * @return row number > 0 until done
   */
  nextClearedRow (row) {
    // assumes row 0 always has the LINE_CLEAR_MARK bit reversed
    do row--
    while (this.blocks[row] & LINE_CLEAR_MARK)

    return row
  }

  /** stop remaining empty rows from being cleared */
  preventLineClears (row) {
    while (row > 0) {
      if (isEmptyRow(this.blocks, row)) {
        this.blocks[row] |= LINE_CLEAR_MARK
      }
      row = this.nextClearedRow(row)
    }
  }

  /**
   * check that there are no unfinished line clears at occupied rows
   *
   * @param {Piece} piece
   */
  isLockable (piece) {
    return !this.blocks.subarray(...piece.getOccupiedRows()).some(
      isMarkedAsCleared
    )
  }
}

/** time between line clear animation steps */
const BLOCK_CLEAR_DELAY = 5

const LEFT_WALL_WIDTH = 4

/** inverted bit used to mark blocks being cleared */
const LINE_CLEAR_MARK = 2

const isMarkedAsCleared = b => !(b & LINE_CLEAR_MARK)

function setWalls (gridBlocks, startRow = 1) {
  const floor = gridBlocks[0]
  const walls = (-1) ^ floor

  gridBlocks.fill(walls, startRow)
}

function isEmptyRow (gridBlocks, row) {
  // this works because floor has all bits set between the walls
  return (gridBlocks[row] & gridBlocks[0]) === 0
}

/** return empty columns as set bits */
function invertBlocks (gridBlocks, row) {
  const b = -1
  return b ^ (gridBlocks[row] | LINE_CLEAR_MARK)
}

function middleBlocks (cols) {
  const colsDividedBy2 = cols >> 1
  // odd or even width
  const odd = (cols & 1)

  return (3 - odd) << (colsDividedBy2 + LEFT_WALL_WIDTH - 1)
}

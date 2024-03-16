/**
 * @typedef {import('./piece.js').Piece} Piece
 */

/**
 * Render a piece as text blocks
 *
 * Row 1 is rendered as the last row in the blocks array.
 *
 * @param {string[]} blocks previously rendered blocks to add the piece to
 * @param {string | Piece} tile a character for rendering one block, or a piece object
 * @param {shape} shape mask with bits set for the blocks to add
 * @param {number} row vertical position of the piece, counting from bottom
 * @param {number} col horizontal position of the piece, counting from left
 * @param {number} width width of shape image
 *
 */
export function renderBlocks (
  blocks = [' '], tile = 'x', shape = 1, row = 1, col = 0, width = 32
) {
  if (typeof tile === 'object') {
    const piece = tile

    return renderBlocks(blocks, piece.tile, piece.shape, piece.row, piece.col, piece.width)
  }

  while (row < 1) {
    shape >>>= width
    row++
  }

  while (shape) {
    while (row > blocks.length) {
      blocks.unshift(' ')
    }

    const i = blocks.length - row
    let blocksAtRow = blocks[i].padEnd(col + width)

    for (let j = 0; j < width; j++) {
      if (shape & 1) {
        blocksAtRow = blocksAtRow.slice(0, col + j) +
            tile + blocksAtRow.slice(col + j + 1)
      }
      shape >>>= 1
    }

    blocks[i] = blocksAtRow
    row++
  }

  return blocks
}

export function renderClearedBlocks(blocks = [' '], row = 1, mask = 0) {
    if (mask) {
        return renderBlocks(blocks, ' ', mask, row)
    }

    blocks.splice(blocks.length - row, 1)
    return blocks
}
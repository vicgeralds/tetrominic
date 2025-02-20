import { render, html } from 'uhtml'
import { renderPiece } from './game/view.js'

/**
 * @typedef {import('./game/piece.js').Piece} Piece
 */

/**
 * Render text blocks as HTML
 *
 * @param {Element} where an element to render inside
 * @param {string[]} blocks lines of text blocks
 * @param {Piece[]} pieces piece objects
 */
export const renderHtmlBlocks = (
  where,
  blocks,
  ...pieces
) => render(where, () => html`
  <div class="blocks">${
    blocks.map(blocksTemplate)
  }</div>
  ${pieces.map(pieceTemplate)}
`)

/** @param {Piece} piece */
const pieceTemplate = piece => html`
  <div class="piece blocks" style=${
    'bottom: ' + piece.bottom + 'em; ' +
    'left: ' + piece.col + 'em'
  }>${
    renderPiece(piece).map(blocksTemplate)
  }</div>
`

const blocksTemplate = (text = '') => html`<div
    class=${text.endsWith('-') ? 'cleared' : null}
  >${
    text.match(spacesOrBlock)?.map(spanTemplate)
  }</div>`

const spacesOrBlock = /\s+|\w/g

const spanTemplate = (content = '') => html`<span
    class=${content.trim() ? 'block ' + content : 'space'}
    style=${'width: ' + content.length + 'em'}
  >${content}</span>`

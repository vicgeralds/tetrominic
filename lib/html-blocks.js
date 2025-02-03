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
 * @param {Piece} piece a piece object
 */
export const renderHtmlBlocks = (
  where,
  blocks,
  piece
) => render(where, () => html`
  <div class="blocks">${
    blocks.map(blocksTemplate)
  }</div>
  <div class="piece blocks" style=${
    'bottom: ' + (piece.bottom ?? piece.row - 1) + 'em; ' +
    'left: ' + piece.col + 'em'
  }>${
    renderPiece(piece).map(blocksTemplate)
  }</div>
`)

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

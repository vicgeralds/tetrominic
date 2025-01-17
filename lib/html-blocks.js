import { render, html } from 'uhtml'

/**
 * Render text blocks as HTML
 *
 * @param {Element} where an element to render inside
 * @param {string[]} blocks lines of text blocks
 */
export const renderHtmlBlocks = (
  where,
  blocks
) => render(where, () => html`${
  blocks.map(blocksTemplate)}`)

const blocksTemplate = (text = '') => html`<div>${
  text.match(spacesOrBlock)?.map(spanTemplate)}</div>`

const spacesOrBlock = /\s+|\S/g

const spanTemplate = (content = '') => html`<span
    class=${content.trim() ? 'block ' + content : 'space'}
    style=${'width: ' + content.length + 'em'}
  >${content}</span>`

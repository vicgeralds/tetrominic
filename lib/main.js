import { Game } from './game/game.js'
import { renderBlocks } from './game/view.js'
import { renderHtmlBlocks } from './html-blocks.js'

const game = new Game()

setupGame()

document.addEventListener('keydown', handleKeyDown)

function setupGame () {
  const gridElement = document.querySelector('.tetgrid > .blocks')
  const pieceElement = document.querySelector('.piece.blocks')

  if (!gridElement || !pieceElement) return

  game.onRender = () => {
    renderHtmlBlocks(gridElement, game.blocks)
    renderHtmlBlocks(pieceElement, renderBlocks([' '], game.tetField.piece))
  }

  game.start()
}

/** @param {KeyboardEvent} event */
function handleKeyDown (event) {
  game.update(keyToGameInput(event.key))
}

function keyToGameInput (key = '') {
  if (key === ' ') return 'space'

  if (key.startsWith('Arrow')) {
    key = key.replace('Arrow', '')
  }
  return key.toLowerCase()
}

import { Game } from './game/game.js'
import { renderHtmlBlocks } from './html-blocks.js'

const game = new Game()

setupGame()

document.addEventListener('keydown', handleKeyDown)

function setupGame () {
  const gridElement = document.querySelector('.tetgrid')

  if (!gridElement) return

  game.onRender = () => {
    renderHtmlBlocks(
      gridElement, game.blocks, game.tetField.piece, game.nextPiece
    )
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

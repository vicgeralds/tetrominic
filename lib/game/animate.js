/* global requestAnimationFrame */

/**
 * Runs animation at a fixed rate
 *
 * @template T
 * @param {(input?: T) => boolean} callback a function to run at given frame
 * rate until it returns false
 * @return {(input?: T) => boolean}
 */
export function runAtFramesPerSecond (callback, fps = 60) {
  const frameDuration = Math.floor(1000 / fps)
  let start = 0
  let stopped = false

  requestAnimationFrame(function (time) {
    start = time
    run(time)
  })

  return function (input) {
    start += frameDuration
    stopped ||= callback(input) === false
    return !stopped
  }

  /** @type {FrameRequestCallback} */
  function run (time) {
    const elapsed = time - start

    if (elapsed >= frameDuration) {
      start = time - (Math.floor(elapsed) % frameDuration)
      stopped ||= callback() === false
    }

    if (elapsed >= frameDuration * 2) {
      stopped ||= callback() === false
    }

    if (elapsed >= frameDuration * 3) {
      stopped ||= callback() === false
    }

    if (!stopped) {
      requestAnimationFrame(run)
    }
  }
}

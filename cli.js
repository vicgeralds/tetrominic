#!/usr/bin/env node

import * as readline from 'node:readline'
import tetrominic from './lib/tetrominic.cjs'
const { onkeypress } = tetrominic

readline.emitKeypressEvents(process.stdin)

process.stdin.on('keypress', onkeypress)

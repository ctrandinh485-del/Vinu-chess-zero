# Vinu Chess Zero

A chess engine built from scratch in C++ on Android (Termux).

## About

Vinu Chess Zero is a chess engine I built from scratch in **4 days** on my Android phone using Termux.

## Features

- **Search**: Alpha-Beta + Iterative Deepening
- **Quiescence Search**: Avoids horizon effect
- **Transposition Table**: Zobrist hashing (1M entries)
- **Move Ordering**: MVV-LVA + Killer + History
- **Evaluation**: Material + Piece-Square Tables
- **Opening Book**: 50+ common opening lines

## Strength

| Opponent | Result |
|---|---|
| Stockfish level 5 | Win (2x) |
| Stockfish level 6 | Loss |
| Estimated ELO | ~2000-2050 |

## How to Build

```bash
git clone https://github.com/ctrandinh485-del/Vinu-chess-zero.git
cd Vinu-chess-zero
clang++ -O2 main.cpp -o chess
./chess

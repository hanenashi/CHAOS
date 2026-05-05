# CHAOS — Web Prototype

CHAOS is a browser-based prototype of a turn-based tactical game inspired by *Chaos*.

Current scope:
- Local hotseat play on a grid board.
- Basic 2-player online play via WebSockets.
- Prototype movement/turn loop with minimal rules.

No original *Chaos* assets are included.

---

## Current Status

The project currently includes:

- **Canvas client** (`index.html`, `main.js`, `src/game.js`) for rendering and input.
- **Minimal rules loop** with two wizards, one-tile orthogonal movement, and end-turn logic.
- **Simple WebSocket room server** (`server/server.js`) for host/join and authoritative action application.

### Implemented gameplay behavior

- Board size: **15 x 10** tiles.
- Starting units: 2 wizards (one per player).
- Legal move: one orthogonal tile into an unoccupied destination.
- End turn: swaps current player and increments turn counter.

### Not yet implemented

- Spell casting/resolution (UI button is a stub).
- Combat/win-condition system.
- Reconnect/replay/protocol hardening.

---

## Run Locally

### 1) Start the client (static server)

From the repo root:

```bash
python -m http.server 8000
```

Open:

- http://localhost:8000

### 2) Start the WebSocket server

In a second terminal:

```bash
cd server
npm init -y
npm i ws
node server.js
```

Server default port: `8787`.

If testing across devices on LAN, update the WebSocket URL in `main.js` from localhost to your host IP.

---

## Technical Architecture

### Client

- `main.js`
  - Wires UI buttons and mode switching (hotseat/online).
  - Sends network actions in online mode.
- `src/game.js`
  - Owns local game state representation.
  - Handles rendering + input.
  - Applies local action validation/mutation.
- `src/net.js`
  - Lightweight WebSocket client.
  - Host/join room requests.
  - Action send and state receive.

### Server

- `server/server.js`
  - Manages rooms (`host`, `join`).
  - Applies incoming actions to room state.
  - Broadcasts updated state snapshots.

---

## Roadmap

This roadmap is designed to move the prototype to a robust, deterministic, multiplayer-capable game.

## Phase A — Stabilize Prototype Core

1. **Fix correctness bugs and cleanup**
   - Resolve touch-input handling issues.
   - Add state invariants/assertions in development mode.
2. **Define canonical action envelope**
   - Add action IDs, actor identity, turn metadata.
3. **Extract pure reducer**
   - Convert rule application into pure functions.
   - Reuse same logic on client and server.
4. **Add baseline tests**
   - Unit tests for move legality and turn transitions.

**Deliverable:** deterministic, tested core for existing rules.

## Phase B — Rules Engine Expansion

1. **Formal turn-phase state machine**
   - e.g. `StartTurn -> Move -> Cast -> Resolve -> EndTurn`.
2. **Spell system v1**
   - Data-driven spell definitions.
   - Cast validation, RNG success checks, effect resolution.
3. **Combat and win conditions**
   - Unit interaction/combat resolution.
   - Defeat/victory rules and end-game flow.
4. **State model hardening**
   - Explicit player metadata, effects, and board abstractions.

**Deliverable:** fully playable match loop.

## Phase C — Multiplayer Reliability

1. **Protocol versioning**
   - Introduce message schema/version fields.
2. **Action ACK/reject flow**
   - Server responses with reason codes and state hashes.
3. **Action log + replay**
   - Reconstruct state from initial snapshot + action history.
4. **Reconnect/resync support**
   - Session tokens and catch-up synchronization.
5. **Validation and abuse controls**
   - Rate limiting and strict server-side action checks.

**Deliverable:** resilient online play with replay/debug support.

## Phase D — UX, Tooling, and Production Readiness

1. **UX improvements**
   - Legal action highlighting, turn/phase indicators, action log.
2. **Developer tooling**
   - Linting/formatting, CI checks, core test automation.
3. **Observability**
   - Structured logs and runtime metrics.
4. **Performance tuning**
   - Render/input optimization and state sync efficiency.

**Deliverable:** maintainable, scalable foundation.

---

## Suggested Near-Term Priorities (Next 2–3 PRs)

1. Fix touch input bug and centralize action dispatch.
2. Extract shared pure rule reducer used by both client and server.
3. Add unit tests for movement/end-turn legality.
4. Implement phase gating + minimal cast pipeline.

---

## Project Notes

- This repository is intentionally a prototype scaffold.
- Keep rule logic deterministic and side-effect free where possible.
- Prefer shared client/server game logic to avoid desync.

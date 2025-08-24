#!/data/data/com.termux/files/usr/bin/sh
# CHAOS web prototype scaffold (directly in /storage/emulated/0/GIT/CHAOS)

BASE="/storage/emulated/0/GIT/CHAOS"

echo "[*] Creating folder structure at $BASE ..."
mkdir -p "$BASE/src"
mkdir -p "$BASE/server"

# --- top-level files ---
cat > "$BASE/index.html" <<'EOF'
<!doctype html>
<html lang="en">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0"/>
  <title>Chaos — Web Prototype</title>
  <link rel="stylesheet" href="styles.css">
</head>
<body>
  <div id="topbar">
    <div class="left">
      <strong>Chaos (web)</strong>
      <span id="modeLabel">HOTSEAT</span>
    </div>
    <div class="right">
      <input id="roomId" placeholder="room id (e.g. mygame)">
      <button id="hostBtn">Host</button>
      <button id="joinBtn">Join</button>
      <button id="leaveBtn" disabled>Leave</button>
    </div>
  </div>

  <canvas id="game" width="960" height="640"></canvas>

  <div id="bottombar">
    <div class="left">
      <button id="endTurnBtn">End Turn</button>
      <button id="castBtn">Cast (stub)</button>
      <span id="turnInfo"></span>
    </div>
    <div class="right">
      <span id="hint">Tap/click tiles to move; drag to pan; pinch/scroll to zoom.</span>
    </div>
  </div>

  <script type="module" src="main.js"></script>
</body>
</html>
EOF

cat > "$BASE/styles.css" <<'EOF'
*{box-sizing:border-box}
html,body{margin:0;height:100%;background:#111;color:#ddd;font-family:system-ui,Segoe UI,Roboto,Helvetica,Arial,sans-serif}
#topbar,#bottombar{display:flex;justify-content:space-between;align-items:center;padding:.5rem .75rem;background:#1b1b1b;border-bottom:1px solid #2a2a2a}
#bottombar{position:fixed;bottom:0;left:0;right:0;border-top:1px solid #2a2a2a;border-bottom:none}
#topbar{position:fixed;top:0;left:0;right:0;z-index:2}
#game{position:fixed;top:48px;bottom:48px;left:0;right:0;margin:auto;display:block;background:#0b0b0b}
button{background:#2c2c2c;border:1px solid #3a3a3a;color:#ddd;padding:.4rem .7rem;border-radius:.5rem;cursor:pointer}
button:disabled{opacity:.5;cursor:not-allowed}
#modeLabel{margin-left:.5rem;color:#7ad}
input{background:#151515;color:#ddd;border:1px solid #333;border-radius:.4rem;padding:.35rem .5rem}
EOF

cat > "$BASE/main.js" <<'EOF'
import {Game} from './src/game.js';
import {NetClient} from './src/net.js';

const canvas = document.getElementById('game');
const endTurnBtn = document.getElementById('endTurnBtn');
const castBtn = document.getElementById('castBtn');
const turnInfo = document.getElementById('turnInfo');
const modeLabel = document.getElementById('modeLabel');

const hostBtn = document.getElementById('hostBtn');
const joinBtn = document.getElementById('joinBtn');
const leaveBtn = document.getElementById('leaveBtn');
const roomInput = document.getElementById('roomId');

const game = new Game(canvas, updateUI);
let net = null;

function updateUI() {
  turnInfo.textContent = `Turn: P${game.state.currentPlayer+1} · Phase: ${game.state.phase}`;
}

endTurnBtn.addEventListener('click', () => {
  if (net && net.connected) {
    net.sendAction({type:'END_TURN'});
  } else {
    game.endTurn();
    updateUI();
  }
});

castBtn.addEventListener('click', () => alert('Spell casting is a stub in this prototype.'));

// --- Networking hooks ---
hostBtn.addEventListener('click', async () => {
  const room = roomInput.value.trim() || 'test';
  net = new NetClient(`ws://localhost:8787`);
  await net.connect();
  const ok = await net.host(room);
  if (!ok) return alert('Room already exists');
  bindNet();
  modeLabel.textContent = 'ONLINE (host)';
  leaveBtn.disabled = false;
});

joinBtn.addEventListener('click', async () => {
  const room = roomInput.value.trim() || 'test';
  net = new NetClient(`ws://localhost:8787`);
  await net.connect();
  const ok = await net.join(room);
  if (!ok) return alert('Join failed (room missing or full)');
  bindNet();
  modeLabel.textContent = 'ONLINE (client)';
  leaveBtn.disabled = false;
});

leaveBtn.addEventListener('click', () => {
  if (net) net.close();
  net = null;
  modeLabel.textContent = 'HOTSEAT';
  leaveBtn.disabled = true;
});

function bindNet(){
  net.on('state', s => {
    game.loadState(s);
    updateUI();
  });
  game.onLocalAction = (a) => {
    if (net && net.connected) net.sendAction(a);
  };
}
updateUI();
EOF

cat > "$BASE/README.md" <<'EOF'
# CHAOS — web prototype (direct)

This is the starter scaffold for a Chaos web remake with hotseat + basic online.
Run a static server for the client; run the Node ws server for online.

## Run
- Client: in this folder, `python -m http.server 8000` then open http://localhost:8000
- Server: `cd server && npm init -y && npm i ws && node server.js`
  (Edit main.js WS URL to your LAN IP to join from phone.)

No original Chaos assets are included.
EOF

# --- src files ---
cat > "$BASE/src/game.js" <<'EOF'
import {seededRng} from './rng.js';

/** Minimal board+movement prototype.
 * Board: 15x10 grid; two wizards (P1 at (1,1), P2 at (13,8)).
 * Click/tap to select/move (1 tile orthogonal).
 * End Turn advances current player.
 */
export class Game {
  constructor(canvas, onUpdate){
    this.canvas = canvas;
    this.ctx = canvas.getContext('2d');
    this.onUpdate = onUpdate || (()=>{});

    this.state = Game.initialState();
    this.scale = 48; // px/tile
    this.offset = {x: 0, y: 0};
    this.drag = null;
    this.hover = null;
    this.selectedId = null;
    this.onLocalAction = null;

    this._bindInput();
    this._loop();
  }

  static initialState(){
    const width=15, height=10;
    const tiles = new Array(width*height).fill(0);
    const units = [
      {id:1, owner:0, type:'wizard', x:1, y:1, moved:false, mp:1, hp:5},
      {id:2, owner:1, type:'wizard', x:13, y:8, moved:false, mp:1, hp:5},
    ];
    return {
      seed: 123456789,
      turn: 1,
      phase: 'move',
      width, height, tiles, units,
      currentPlayer: 0
    };
  }

  rng(){ return seededRng(this.state.seed + this.state.turn); }

  draw(){
    const {ctx, state} = this;
    ctx.clearRect(0,0,this.canvas.width,this.canvas.height);
    const s = this.scale;
    const ox = Math.floor(this.offset.x), oy = Math.floor(this.offset.y);

    // grid
    for (let y=0;y<state.height;y++){
      for (let x=0;x<state.width;x++){
        ctx.strokeStyle = '#222';
        ctx.lineWidth = 1;
        ctx.strokeRect(ox + x*s, oy + y*s, s, s);
      }
    }

    // units
    for (const u of state.units){
      const gx = ox + u.x*s, gy = oy + u.y*s;
      ctx.fillStyle = u.owner===0 ? '#4ad' : '#d64';
      ctx.fillRect(gx+4, gy+4, s-8, s-8);
      ctx.fillStyle = '#111';
      ctx.font = `${Math.floor(s*0.35)}px system-ui`;
      ctx.textAlign = 'center';
      ctx.textBaseline = 'middle';
      ctx.fillText(u.type==='wizard'?'W':'C', gx+s/2, gy+s/2);
      if (u.moved){
        ctx.strokeStyle = '#aaa'; ctx.lineWidth = 2;
        ctx.beginPath(); ctx.moveTo(gx+6,gy+6); ctx.lineTo(gx+s-6,gy+s-6); ctx.stroke();
      }
      if (this.selectedId===u.id){
        ctx.strokeStyle = '#ff0'; ctx.lineWidth = 2;
        ctx.strokeRect(gx+2, gy+2, s-4, s-4);
      }
    }

    // hover
    if (this.hover){
      const {x,y} = this.hover;
      ctx.strokeStyle = '#555'; ctx.lineWidth = 2;
      ctx.strokeRect(ox + x*s+1, oy + y*s+1, s-2, s-2);
    }
  }

  _loop(){
    this.draw();
    requestAnimationFrame(()=>this._loop());
  }

  _bindInput(){
    const c = this.canvas;
    let last = null;
    const toGrid = (px,py)=>{
      const x = Math.floor((px - this.offset.x)/this.scale);
      const y = Math.floor((py - this.offset.y)/this.scale);
      if (x<0||y<0||x>=this.state.width||y>=this.state.height) return null;
      return {x,y};
    };

    const pickUnitAt = (x,y)=> this.state.units.find(u=>u.x===x && u.y===y);

    const onDown = (e)=>{
      const rect = c.getBoundingClientRect();
      const px = (e.touches?e.touches[0].clientX:e.clientX) - rect.left;
      const py = (e.touches?e.touches[0].clientY:e.clientY) - rect.top;
      last = {px,py};
      this.drag = {px,py, ox:this.offset.x, oy:this.offset.y};
    };

    const onMove = (e)=>{
      const rect = c.getBoundingClientRect();
      const px = (e.touches?e.touches[0].clientX:e.clientX) - rect.left;
      const py = (e.touches?e.touches[0].clientY)e.touches[0].clientY - rect.top;
      if (this.drag){
        const dx = px - this.drag.px;
        const dy = py - this.drag.py;
        this.offset.x = this.drag.ox + dx;
        this.offset.y = this.drag.oy + dy;
      }
      const g = toGrid(px,py);
      this.hover = g;
    };

    const onUp = (e)=>{
      const rect = c.getBoundingClientRect();
      const px = (e.changedTouches?e.changedTouches[0].clientX:e.clientX) - rect.left;
      const py = (e.changedTouches?e.changedTouches[0].clientY:e.clientY) - rect.top;
      const g = toGrid(px,py);
      if (!g){ this.drag=null; return; }

      const u = this.state.units.find(u=>u.id===this.selectedId);
      const there = pickUnitAt(g.x,g.y);
      const isClick = last && Math.hypot(px-last.px, py-last.py) < 6;

      if (isClick){
        if (there){
          if (there.owner===this.state.currentPlayer && !there.moved){
            this.selectedId = there.id;
          } else {
            this.selectedId = null;
          }
        } else if (u){
          const dist = Math.abs(u.x-g.x) + Math.abs(u.y-g.y);
          if (dist===1 && !u.moved){
            this._applyLocalAction({type:'MOVE', id:u.id, x:g.x, y:g.y});
          }
        }
      }
      this.drag = null;
    };

    c.addEventListener('mousedown', onDown);
    c.addEventListener('mousemove', onMove);
    window.addEventListener('mouseup', onUp);
    c.addEventListener('touchstart', onDown, {passive:false});
    c.addEventListener('touchmove', onMove, {passive:false});
    c.addEventListener('touchend', onUp);

    c.addEventListener('wheel', (e)=>{
      e.preventDefault();
      const s0 = this.scale;
      const direction = Math.sign(e.deltaY);
      const s1 = Math.min(96, Math.max(24, s0 + (-direction)*4));
      if (s1!==s0){
        const rect = c.getBoundingClientRect();
        const cx = e.clientX - rect.left;
        const cy = e.clientY - rect.top;
        const k = s1/s0;
        this.offset.x = cx - (cx - this.offset.x)*k;
        this.offset.y = cy - (cy - this.offset.y)*k;
        this.scale = s1;
      }
    }, {passive:false});
  }

  _applyLocalAction(a){
    if (this.onLocalAction){
      this.onLocalAction(a);
      return;
    }
    this.applyAction(a);
  }

  applyAction(a){
    switch(a.type){
      case 'MOVE': {
        const u = this.state.units.find(x=>x.id===a.id);
        if (!u) return;
        if (u.owner!==this.state.currentPlayer || u.moved) return;
        const occupied = this.state.units.some(x=>x.x===a.x && x.y===a.y);
        const dist = Math.abs(u.x-a.x) + Math.abs(u.y-a.y);
        if (!occupied && dist===1){
          u.x=a.x; u.y=a.y; u.moved=true;
        }
        break;
      }
      case 'END_TURN': {
        this.endTurn();
        break;
      }
    }
    this.onUpdate();
  }

  endTurn(){
    for (const u of this.state.units){
      if (u.owner===this.state.currentPlayer) u.moved=false;
    }
    this.state.currentPlayer = (this.state.currentPlayer+1)%2;
    this.state.turn+=1;
    this.selectedId=null;
  }

  getState(){ return JSON.parse(JSON.stringify(this.state)); }
  loadState(s){ this.state = s; this.selectedId = null; }
}
EOF

cat > "$BASE/src/rng.js" <<'EOF'
// Small deterministic RNG (mulberry32-ish)
export function seededRng(seed){
  let t = (seed|0) + 0x6D2B79F5;
  return function(){
    t |= 0; t = (t + 0x6D2B79F5) | 0;
    let r = Math.imul(t ^ (t >>> 15), 1 | t);
    r ^= r + Math.imul(r ^ (r >>> 7), 61 | r);
    return ((r ^ (r >>> 14)) >>> 0) / 4294967296;
  };
}
EOF

cat > "$BASE/src/net.js" <<'EOF'
// Minimal WebSocket client with room/lockstep actions
export class NetClient{
  constructor(url){
    this.url = url;
    this.ws = null;
    this.handlers = {};
    this.connected = false;
    this.room = null;
  }
  on(type, fn){ this.handlers[type]=fn; }
  _emit(type, payload){ if (this.handlers[type]) this.handlers[type](payload); }

  async connect(){
    this.ws = new WebSocket(this.url);
    await new Promise((res,rej)=>{
      this.ws.onopen = ()=>res();
      this.ws.onerror = (e)=>rej(e);
    });
    this.connected = true;
    this.ws.onmessage = (ev)=>{
      const msg = JSON.parse(ev.data);
      if (msg.type==='state') this._emit('state', msg.state);
      if (msg.type==='action') this._emit('action', msg.action);
      if (msg.type==='error') alert('Server: '+msg.message);
    };
  }
  async host(room){
    this.room = room;
    this.ws.send(JSON.stringify({type:'host', room}));
    const ok = await this._awaitOnce('hosted');
    return ok && ok.ok;
  }
  async join(room){
    this.room = room;
    this.ws.send(JSON.stringify({type:'join', room}));
    const ok = await this._awaitOnce('joined');
    return ok && ok.ok;
  }
  close(){ this.ws?.close(); this.connected=false; }
  sendAction(action){
    this.ws.send(JSON.stringify({type:'action', room:this.room, action}));
  }
  _awaitOnce(expect){
    return new Promise((resolve)=>{
      const h = (ev)=>{
        try{
          const msg = JSON.parse(ev.data);
          if (msg.type===expect){
            this.ws.removeEventListener('message', h);
            resolve(msg);
          }
        }catch{}
      };
      this.ws.addEventListener('message', h);
      setTimeout(()=>{ 
        this.ws.removeEventListener('message', h); 
        resolve(null);
      }, 2000);
    });
  }
}
EOF

# --- server ---
cat > "$BASE/server/server.js" <<'EOF'
// Simple WS server for Chaos (prototype).
// Run: `npm init -y && npm i ws && node server.js`
const WebSocket = require('ws');
const http = require('http');

const PORT = 8787;
const wss = new WebSocket.Server({ noServer: true });

const rooms = new Map();

function initialState(){
  return {
    seed: 123456789,
    turn: 1,
    phase: 'move',
    width: 15,
    height: 10,
    tiles: Array(15*10).fill(0),
    units: [
      {id:1, owner:0, type:'wizard', x:1, y:1, moved:false, mp:1, hp:5},
      {id:2, owner:1, type:'wizard', x:13, y:8, moved:false, mp:1, hp:5},
    ],
    currentPlayer: 0
  };
}

function broadcast(room, msg){
  for (const p of room.players){
    if (p.ws.readyState===WebSocket.OPEN){
      p.ws.send(JSON.stringify(msg));
    }
  }
}

const server = http.createServer();
server.on('upgrade', (req, socket, head) => {
  wss.handleUpgrade(req, socket, head, (ws) => {
    wss.emit('connection', ws, req);
  });
});

wss.on('connection', (ws) => {
  let currentRoom = null;
  ws.on('message', (data) => {
    let msg;
    try { msg = JSON.parse(data); } catch { return; }

    if (msg.type==='host'){
      const {room} = msg;
      if (rooms.has(room)){
        ws.send(JSON.stringify({type:'hosted', ok:false, reason:'exists'}));
        return;
      }
      const r = { id: room, players: [{ws, id:0}], state: initialState() };
      rooms.set(room, r);
      currentRoom = r;
      ws.send(JSON.stringify({type:'hosted', ok:true}));
      ws.send(JSON.stringify({type:'state', state: r.state}));
      return;
    }

    if (msg.type==='join'){
      const {room} = msg;
      const r = rooms.get(room);
      if (!r || r.players.length>=2){
        ws.send(JSON.stringify({type:'joined', ok:false}));
        return;
      }
      r.players.push({ws, id:1});
      currentRoom = r;
      ws.send(JSON.stringify({type:'joined', ok:true}));
      ws.send(JSON.stringify({type:'state', state: r.state}));
      return;
    }

    if (msg.type==='action'){
      const r = rooms.get(msg.room);
      if (!r) return;
      applyAction(r.state, msg.action);
      broadcast(r, {type:'state', state: r.state});
      return;
    }
  });

  ws.on('close', ()=>{
    if (currentRoom){
      currentRoom.players = currentRoom.players.filter(p=>p.ws!==ws);
      if (currentRoom.players.length===0){
        rooms.delete(currentRoom.id);
      }
    }
  });
});

function applyAction(state, a){
  switch(a.type){
    case 'MOVE': {
      const u = state.units.find(x=>x.id===a.id);
      if (!u) return;
      if (u.owner!==state.currentPlayer || u.moved) return;
      const occupied = state.units.some(x=>x.x===a.x && x.y===a.y);
      const dist = Math.abs(u.x-a.x) + Math.abs(u.y-a.y);
      if (!occupied && dist===1){
        u.x=a.x; u.y=a.y; u.moved=true;
      }
      break;
    }
    case 'END_TURN': {
      for (const u of state.units){
        if (u.owner===state.currentPlayer) u.moved=false;
      }
      state.currentPlayer = (state.currentPlayer+1)%2;
      state.turn+=1;
      break;
    }
  }
}

server.listen(PORT, () => {
  console.log('WS server on', PORT);
});
EOF

echo "[*] Done. Project created under $BASE"
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

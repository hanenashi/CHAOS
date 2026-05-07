// Simple WS server for Chaos (prototype).
// Run: `npm init -y && npm i ws && node server.js`
const WebSocket = require('ws');
const http = require('http');
const {createLogger} = require('./logger');

const PORT = 8787;
const logger = createLogger({name:'ws-server'});
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
      {id:1, owner:0, type:'wizard', spriteId:'wiz1', x:1, y:1, moved:false, mp:1, hp:5},
      {id:2, owner:1, type:'wizard', spriteId:'wiz2', x:13, y:8, moved:false, mp:1, hp:5},
    ],
    currentPlayer: 0
  };
}

function broadcast(room, msg){
  logger.debug('room.broadcast', {room: room.id, type: msg.type, players: room.players.length});
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
  logger.info('ws.connection.open');
  let currentRoom = null;
  ws.on('message', (data) => {
    let msg;
    try { msg = JSON.parse(data); } catch { logger.warn('ws.message.invalid_json'); return; }
    logger.debug('ws.message.received', {type: msg.type});

    if (msg.type==='host'){
      const {room} = msg;
      if (rooms.has(room)){
        logger.warn('room.host.rejected_exists', {room});
        ws.send(JSON.stringify({type:'hosted', ok:false, reason:'exists'}));
        return;
      }
      const r = { id: room, players: [{ws, id:0}], state: initialState() };
      logger.info('room.host.created', {room});
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
        logger.warn('room.join.rejected', {room, reason: !r ? 'missing' : 'full'});
        ws.send(JSON.stringify({type:'joined', ok:false}));
        return;
      }
      r.players.push({ws, id:1});
      logger.info('room.join.accepted', {room, players: r.players.length});
      currentRoom = r;
      ws.send(JSON.stringify({type:'joined', ok:true}));
      ws.send(JSON.stringify({type:'state', state: r.state}));
      return;
    }

    if (msg.type==='action'){
      const r = rooms.get(msg.room);
      if (!r){ logger.warn('room.action.rejected_missing_room', {room: msg.room}); return; }
      logger.debug('room.action.apply.start', {room: msg.room, action: msg.action});
      applyAction(r.state, msg.action);
      logger.debug('room.action.apply.done', {room: msg.room, turn: r.state.turn, currentPlayer: r.state.currentPlayer});
      broadcast(r, {type:'state', state: r.state});
      return;
    }
  });

  ws.on('close', ()=>{
    logger.info('ws.connection.close');
    if (currentRoom){
      currentRoom.players = currentRoom.players.filter(p=>p.ws!==ws);
      if (currentRoom.players.length===0){
        rooms.delete(currentRoom.id);
        logger.info('room.deleted_empty', {room: currentRoom.id});
      }
    }
  });
});

function applyAction(state, a){
  switch(a.type){
    case 'MOVE': {
      const u = state.units.find(x=>x.id===a.id);
      if (!u){ logger.warn('rules.move.rejected', {reason:'unit_missing', action:a}); return; }
      if (u.owner!==state.currentPlayer || u.moved){ logger.warn('rules.move.rejected', {reason:'not_current_player_or_moved', action:a}); return; }
      const occupied = state.units.some(x=>x.x===a.x && x.y===a.y);
      const dist = Math.abs(u.x-a.x) + Math.abs(u.y-a.y);
      if (!occupied && dist===1){
        u.x=a.x; u.y=a.y; u.moved=true;
        logger.info('rules.move.applied', {id:u.id, to:{x:a.x,y:a.y}});
      } else {
        logger.warn('rules.move.rejected', {reason:'occupied_or_distance', occupied, dist, action:a});
      }
      break;
    }
    case 'END_TURN': {
      logger.info('rules.turn.end', {turn: state.turn, currentPlayer: state.currentPlayer});
      for (const u of state.units){
        if (u.owner===state.currentPlayer) u.moved=false;
      }
      state.currentPlayer = (state.currentPlayer+1)%2;
      state.turn+=1;
      break;
    }
    case 'SPAWN': {
      if (a.x < 0 || a.y < 0 || a.x >= state.width || a.y >= state.height){
        logger.warn('rules.spawn.rejected', {reason:'out_of_bounds', action:a});
        return;
      }
      const occupied = state.units.some(x=>x.x===a.x && x.y===a.y);
      if (occupied){
        logger.warn('rules.spawn.rejected', {reason:'occupied', action:a});
        return;
      }
      const id = Math.max(0, ...state.units.map(u=>u.id)) + 1;
      state.units.push({
        id,
        owner: a.owner ?? -1,
        type: a.unitType ?? 'summon',
        spriteId: a.spriteId,
        x: a.x,
        y: a.y,
        moved: false,
        hp: 1
      });
      logger.info('rules.spawn.applied', {id, spriteId: a.spriteId, to:{x:a.x,y:a.y}});
      break;
    }
  }
}

server.listen(PORT, () => {
  logger.info('server.started', {port: PORT});
});

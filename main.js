import {Game} from './src/game.js';
import {NetClient} from './src/net.js';
import {createLogger} from './src/log.js';

const canvas = document.getElementById('game');
const endTurnBtn = document.getElementById('endTurnBtn');
const castBtn = document.getElementById('castBtn');
const turnInfo = document.getElementById('turnInfo');
const copyLogsBtn = document.getElementById('copyLogsBtn');
const modeLabel = document.getElementById('modeLabel');

const hostBtn = document.getElementById('hostBtn');
const joinBtn = document.getElementById('joinBtn');
const leaveBtn = document.getElementById('leaveBtn');
const roomInput = document.getElementById('roomId');

const clientLogs = [];
const MAX_CLIENT_LOGS = 500;
const memorySink = (record) => {
  clientLogs.push(record);
  if (clientLogs.length > MAX_CLIENT_LOGS) clientLogs.shift();
  window.__chaosLogs = clientLogs;
};

const logger = createLogger({name:'client', level:'debug'});
logger.addSink(memorySink);
const game = new Game(canvas, updateUI, {logger: logger.child('game')});
let net = null;
let manifest = [];

async function getManifest(){
  if (manifest.length > 0) return manifest;
  const res = await fetch('./assets/manifest.json');
  if (!res.ok) throw new Error('assets/manifest.json missing');
  manifest = await res.json();
  return manifest;
}

function updateUI() {
  turnInfo.textContent = `Turn: P${game.state.currentPlayer+1} · Phase: ${game.state.phase}`;
}

endTurnBtn.addEventListener('click', () => {
  logger.debug('ui.end_turn_clicked', {online: Boolean(net && net.connected)});
  if (net && net.connected) {
    net.sendAction({type:'END_TURN'});
  } else {
    game.endTurn();
    updateUI();
  }
});

castBtn.addEventListener('click', async () => {
  logger.debug('ui.cast_clicked', {online: Boolean(net && net.connected)});
  let entries;
  try {
    entries = await getManifest();
  } catch (err) {
    logger.error('ui.cast_failed', {message: String(err)});
    alert('Sprite manifest missing. Expected assets/manifest.json.');
    return;
  }

  const skip = new Set([
    'cursor', 'cursor_engaged', 'cursor_fire', 'cursor_fly',
    'cursor_ground', 'cursor_spell', 'chaosfont', 'bg', 'spell'
  ]);
  const usable = entries.filter(e => !skip.has(e.id));
  let spawned = 0;
  let x = 0;
  let y = 0;

  for (const entry of usable) {
    while (y < game.state.height && game.state.units.some(u => u.x === x && u.y === y)) {
      x += 1;
      if (x >= game.state.width) { x = 0; y += 1; }
    }
    if (y >= game.state.height) break;

    const action = {type:'SPAWN', spriteId: entry.id, x, y, owner:-1, unitType:'test'};
    if (net && net.connected) net.sendAction(action);
    else game.applyAction(action);
    spawned += 1;
    x += 1;
    if (x >= game.state.width) { x = 0; y += 1; }
  }

  modeLabel.textContent = `SPAWNED ${spawned}`;
  logger.info('ui.cast_spawned_sprites', {spawned});
});

copyLogsBtn.addEventListener('click', async () => {
  const dump = JSON.stringify(clientLogs, null, 2);
  try {
    await navigator.clipboard.writeText(dump);
    logger.info('ui.copy_logs.success', {entries: clientLogs.length});
    alert(`Copied ${clientLogs.length} log entries to clipboard.`);
  } catch (err) {
    logger.error('ui.copy_logs.failed', {message: String(err)});
    alert('Failed to copy logs. Open DevTools and use window.__chaosLogs instead.');
  }
});

// --- Networking hooks ---
hostBtn.addEventListener('click', async () => {
  const room = roomInput.value.trim() || 'test';
  net = new NetClient(`ws://localhost:8787`, logger.child('net'));
  await net.connect();
  const ok = await net.host(room);
  if (!ok) { logger.warn('net.host_failed', {room}); return alert('Room already exists'); }
  bindNet();
  modeLabel.textContent = 'ONLINE (host)';
  leaveBtn.disabled = false;
});

joinBtn.addEventListener('click', async () => {
  const room = roomInput.value.trim() || 'test';
  net = new NetClient(`ws://localhost:8787`, logger.child('net'));
  await net.connect();
  const ok = await net.join(room);
  if (!ok) { logger.warn('net.join_failed', {room}); return alert('Join failed (room missing or full)'); }
  bindNet();
  modeLabel.textContent = 'ONLINE (client)';
  leaveBtn.disabled = false;
});

leaveBtn.addEventListener('click', () => {
  if (net) net.close();
  logger.info('net.left_room');
  net = null;
  modeLabel.textContent = 'HOTSEAT';
  leaveBtn.disabled = true;
});

function bindNet(){
  net.on('state', s => {
    logger.debug('net.state_received', {turn: s.turn, currentPlayer: s.currentPlayer});
    game.loadState(s);
    updateUI();
  });
  game.onLocalAction = (a) => {
    if (net && net.connected) net.sendAction(a);
  };
}
updateUI();

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

import {seededRng} from './rng.js';
import {createLogger} from './log.js';

async function loadImage(src) {
  return new Promise((resolve, reject) => {
    const img = new Image();
    img.onload = () => resolve(img);
    img.onerror = reject;
    img.src = src;
  });
}

async function loadManifest() {
  const res = await fetch('./assets/manifest.json');
  if (!res.ok) throw new Error('assets/manifest.json not found');
  return res.json();
}

function pickSprite(manifest, id) {
  return manifest.find(e => e.id === id) || null;
}

function defaultFpsFor(id, idleFrames) {
  const staticSet = new Set(['wall', 'magic_castle', 'shadow_wood', 'magic_wood', 'dark_citadel', 'bg', 'chaosfont']);
  if (staticSet.has(id)) return 0;
  return idleFrames > 1 ? 4 : 0;
}

/** Minimal board+movement prototype.
 * Board: 15x10 grid; two wizards (P1 at (1,1), P2 at (13,8)).
 * Click/tap to select/move (1 tile orthogonal).
 * End Turn advances current player.
 */
export class Game {
  constructor(canvas, onUpdate, options = {}){
    this.canvas = canvas;
    this.ctx = canvas.getContext('2d');
    this.ctx.imageSmoothingEnabled = false;
    this.onUpdate = onUpdate || (()=>{});
    this.logger = options.logger || createLogger({name:'game', level:'debug'});

    this.state = Game.initialState();
    this.scale = 48; // px/tile
    this.offset = {x: 0, y: 0};
    this.drag = null;
    this.hover = null;
    this.selectedId = null;
    this.onLocalAction = null;
    this.manifest = [];
    this.images = new Map();
    this._manifestPromise = null;
    this.time = 0;
    this._lastTs = performance.now();

    this.logger.info('game.init', {width: this.state.width, height: this.state.height});
    this._bindInput();
    this._boot();
  }

  static initialState(){
    const width=15, height=10;
    const tiles = new Array(width*height).fill(0);
    const units = [
      {id:1, owner:0, type:'wizard', spriteId:'wiz1', x:1, y:1, moved:false, mp:1, hp:5},
      {id:2, owner:1, type:'wizard', spriteId:'wiz2', x:13, y:8, moved:false, mp:1, hp:5},
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

  async _boot(){
    try {
      this._manifestPromise = loadManifest();
      this.manifest = await this._manifestPromise;
      await Promise.all(this.state.units.map(u => u.spriteId).filter(Boolean).map(id => this._ensureSpriteLoaded(id)));
      this.logger.info('game.assets.loaded', {sprites: this.manifest.length});
    } catch (err) {
      this.logger.warn('game.assets.failed', {message: String(err)});
    }
    this._loop();
  }

  async _ensureSpriteLoaded(id){
    if (this.images.has(id)) return;
    if (this.manifest.length === 0){
      this._manifestPromise ||= loadManifest();
      this.manifest = await this._manifestPromise;
    }
    const entry = pickSprite(this.manifest, id);
    if (!entry) return;
    const img = await loadImage(entry.path);
    this.images.set(id, {img, meta: entry});
  }

  draw(){
    const {ctx, state} = this;
    const now = performance.now();
    const dt = (now - this._lastTs) / 1000;
    this._lastTs = now;
    this.time += dt;

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
      const sprite = u.spriteId && this.images.get(u.spriteId);
      if (sprite){
        const {img, meta} = sprite;
        const totalFrames = Math.max(1, meta.frames || 1);
        const idleFrames = Math.max(1, meta.idleFrames || totalFrames);
        const fps = defaultFpsFor(u.spriteId, idleFrames);
        const frame = fps > 0 ? Math.floor(this.time * fps) % idleFrames : 0;
        const frameH = meta.frameH || meta.frameW;
        ctx.drawImage(img, 0, frame * frameH, meta.frameW, frameH, gx, gy, s, s);
      } else {
        ctx.fillStyle = u.owner===0 ? '#4ad' : '#d64';
        ctx.fillRect(gx+4, gy+4, s-8, s-8);
        ctx.fillStyle = '#111';
        ctx.font = `${Math.floor(s*0.35)}px system-ui`;
        ctx.textAlign = 'center';
        ctx.textBaseline = 'middle';
        ctx.fillText(u.type==='wizard'?'W':'C', gx+s/2, gy+s/2);
      }
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
    const getXY = (e)=>{
      const rect = c.getBoundingClientRect();
      const rx = c.width / rect.width;
      const ry = c.height / rect.height;
      const source = e.touches ? e.touches[0] : e;
      return {
        px: (source.clientX - rect.left) * rx,
        py: (source.clientY - rect.top) * ry,
      };
    };
    const toGrid = (px,py)=>{
      const x = Math.floor((px - this.offset.x)/this.scale);
      const y = Math.floor((py - this.offset.y)/this.scale);
      if (x<0||y<0||x>=this.state.width||y>=this.state.height) return null;
      return {x,y};
    };

    const pickUnitAt = (x,y)=> this.state.units.find(u=>u.x===x && u.y===y);

    const onDown = (e)=>{
      if (e.cancelable) e.preventDefault();
      const {px, py} = getXY(e);
      last = {px,py};
      this.drag = {px,py, ox:this.offset.x, oy:this.offset.y};
    };

    const onMove = (e)=>{
      if (e.cancelable) e.preventDefault();
      const {px, py} = getXY(e);
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
      if (e.cancelable) e.preventDefault();
      const event = e.changedTouches ? {touches: e.changedTouches} : e;
      const {px, py} = getXY(event);
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
            this.logger.debug('game.intent.move', {id:u.id, from:{x:u.x,y:u.y}, to:g});
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
        const rx = c.width / rect.width;
        const ry = c.height / rect.height;
        const cx = (e.clientX - rect.left) * rx;
        const cy = (e.clientY - rect.top) * ry;
        const k = s1/s0;
        this.offset.x = cx - (cx - this.offset.x)*k;
        this.offset.y = cy - (cy - this.offset.y)*k;
        this.scale = s1;
      }
    }, {passive:false});

    const resize = ()=>{
      const rect = c.getBoundingClientRect();
      const dpr = window.devicePixelRatio || 1;
      const width = Math.max(1, Math.floor(rect.width * dpr));
      const height = Math.max(1, Math.floor(rect.height * dpr));
      if (c.width !== width || c.height !== height){
        c.width = width;
        c.height = height;
        this.ctx.imageSmoothingEnabled = false;
      }
    };
    const ro = new ResizeObserver(resize);
    ro.observe(c);
    resize();
  }

  _applyLocalAction(a){
    this.logger.debug('game.action.dispatch', {action: a, remote: Boolean(this.onLocalAction)});
    if (this.onLocalAction){
      this.onLocalAction(a);
      return;
    }
    this.applyAction(a);
  }

  applyAction(a){
    const before = this.getState();
    switch(a.type){
      case 'MOVE': {
        const u = this.state.units.find(x=>x.id===a.id);
        if (!u){ this.logger.warn('game.move.rejected', {reason:'unit_missing', action:a}); return; }
        if (u.owner!==this.state.currentPlayer || u.moved){ this.logger.warn('game.move.rejected', {reason:'not_current_player_or_moved', action:a}); return; }
        const occupied = this.state.units.some(x=>x.x===a.x && x.y===a.y);
        const dist = Math.abs(u.x-a.x) + Math.abs(u.y-a.y);
        if (!occupied && dist===1){
          u.x=a.x; u.y=a.y; u.moved=true;
          this.logger.info('game.move.applied', {id:u.id, to:{x:a.x,y:a.y}});
        } else {
          this.logger.warn('game.move.rejected', {reason:'occupied_or_distance', occupied, dist, action:a});
        }
        break;
      }
      case 'END_TURN': {
        this.logger.info('game.turn.end_requested', {currentPlayer: this.state.currentPlayer, turn: this.state.turn});
        this.endTurn();
        break;
      }
      case 'SPAWN': {
        if (a.x < 0 || a.y < 0 || a.x >= this.state.width || a.y >= this.state.height){
          this.logger.warn('game.spawn.rejected', {reason:'out_of_bounds', action:a});
          return;
        }
        const occupied = this.state.units.some(x=>x.x===a.x && x.y===a.y);
        if (occupied){
          this.logger.warn('game.spawn.rejected', {reason:'occupied', action:a});
          return;
        }
        const id = Math.max(0, ...this.state.units.map(u=>u.id)) + 1;
        const unit = {
          id,
          owner: a.owner ?? -1,
          type: a.unitType ?? 'summon',
          spriteId: a.spriteId,
          x: a.x,
          y: a.y,
          moved: false,
          hp: 1
        };
        this.state.units.push(unit);
        this._ensureSpriteLoaded(unit.spriteId);
        this.logger.info('game.spawn.applied', {id, spriteId: unit.spriteId, to:{x:a.x,y:a.y}});
        break;
      }
    }
    this.logger.debug('game.state.changed', {action: a.type, before:{turn:before.turn,currentPlayer:before.currentPlayer}, after:{turn:this.state.turn,currentPlayer:this.state.currentPlayer}});
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
  loadState(s){
    this.state = s;
    this.selectedId = null;
    for (const u of this.state.units){
      if (u.spriteId) this._ensureSpriteLoaded(u.spriteId);
    }
    this.logger.debug('game.state.loaded', {turn:s.turn, currentPlayer:s.currentPlayer});
  }
}

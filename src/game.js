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

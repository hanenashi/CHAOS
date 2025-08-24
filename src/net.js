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

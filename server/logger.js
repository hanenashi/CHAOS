const LEVELS = {
  debug: 10,
  info: 20,
  warn: 30,
  error: 40,
  silent: 100,
};

function safeClone(payload){
  if (payload === undefined) return undefined;
  try { return JSON.parse(JSON.stringify(payload)); } catch { return {unserializable: true}; }
}

function createLogger({name='server', level=process.env.CHAOS_LOG_LEVEL || 'info', sinks=[]} = {}){
  let min = LEVELS[level] ?? LEVELS.info;
  let seq = 0;
  const listeners = [...sinks];

  const baseSink = (record)=>{
    const line = `[${record.ts}] [${record.level.toUpperCase()}] [${record.name}] ${record.event}`;
    if (record.level === 'error') console.error(line, record.data ?? '');
    else if (record.level === 'warn') console.warn(line, record.data ?? '');
    else console.log(line, record.data ?? '');
  };

  if (listeners.length===0) listeners.push(baseSink);

  function emit(levelName, event, data){
    if ((LEVELS[levelName] ?? LEVELS.info) < min) return;
    const record = {
      seq: ++seq,
      ts: new Date().toISOString(),
      name,
      level: levelName,
      event,
      data: safeClone(data),
    };
    for (const sink of listeners){
      try { sink(record); } catch {}
    }
  }

  return {
    setLevel(next){ min = LEVELS[next] ?? min; },
    addSink(sink){ listeners.push(sink); },
    debug(event, data){ emit('debug', event, data); },
    info(event, data){ emit('info', event, data); },
    warn(event, data){ emit('warn', event, data); },
    error(event, data){ emit('error', event, data); },
  };
}

module.exports = {createLogger};

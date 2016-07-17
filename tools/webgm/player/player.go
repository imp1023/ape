package player

import (
  "encoding/json"
  "sync"
)

const maxLogNum = 200 
const maxPlayerNum = 10

type History struct {
  Cmd string
  Detail string
  Time string
}

type player struct {
  uid string
  name string
  maxid int
  history []History 
}

func (p *player) addHistory(cmd string, detail string, time string){
  p.maxid = p.maxid + 1
  h := History{cmd, detail, time}
  p.history = append(p.history, h)

  if len(p.history) > maxLogNum {
    p.history = p.history[1:]
  }
}

type JLog struct {
  MaxId int
  Log []History
}

func (p *player) getHistory(maxid int) string {
  if maxid > p.maxid {
    maxid = p.maxid
  }

  num := p.maxid - maxid
  if num > maxLogNum {
    num = maxLogNum
  }

  start := len(p.history) - num
  if start < 0 {
    start = 0
  }

  var jl JLog
  jl.MaxId = p.maxid
  jl.Log = p.history[start:]
  jstr, err := json.Marshal(jl)
  if err != nil {
    return "failed when get log."
  }
  return string(jstr)
}

type Playerlist struct {
  players map[string]*player
  uids []string
  mutex *sync.Mutex
}

func NewPlayerlist() *Playerlist{
  return &Playerlist{
           players: make(map[string]*player),
           uids: []string{},
           mutex: new(sync.Mutex),
         }
}

func (pl *Playerlist) AddPlayer(uid string, name string){
  pl.mutex.Lock()
  defer pl.mutex.Unlock()

  p, find := pl.players[uid]
  if find {
    if name != string(p.uid) {
      p.name = name
    }
    return
  }

  p = &player{uid, name, 0, []History{}}
  pl.players[uid] = p
  pl.uids = append(pl.uids, uid)

  if len(pl.uids) > maxPlayerNum {
    delete(pl.players, pl.uids[0])
    pl.uids = pl.uids[1:]
  }
}

func (pl *Playerlist) AddLog(uid string, cmd string, detail string, time string) {
  pl.AddPlayer(uid, string(uid))

  pl.mutex.Lock()
  defer pl.mutex.Unlock()

  p, succ := pl.players[uid]
  if !succ {
    return
  }

  p.addHistory(cmd, detail, time)
}

func (pl *Playerlist) GetLog(uid string, maxid int) string {
  pl.mutex.Lock()
  defer pl.mutex.Unlock()

  p, succ := pl.players[uid]
  if !succ {
    return "player not found"
  }
  return p.getHistory(maxid)
}

type JPlayerInfo struct {
  Uid string
  Name string
}
type JUidList struct {
  Player []JPlayerInfo
}

func (pl *Playerlist) GetUidList() string {
  pl.mutex.Lock()
  defer pl.mutex.Unlock() 

  var juid JUidList
  for _, i := range pl.players {
    juid.Player = append(juid.Player, JPlayerInfo{i.uid, i.name})
  }

  jstr, err := json.Marshal(juid)
  if err != nil {
    return "failed when get uid list"
  }
  return string(jstr)
}


package main

import (
  "net"
  "net/http"
  "log"
  "fmt"
  "os"
  "io/ioutil"
  "encoding/json"
  "./player"
  "./gamewatch"
)

const serverFile1 = "/data/totalwar/bin_debug/gamed.log"
const serverFile2 = "/data/totalwar_release/bin_release/gamed.log"

var pl *player.Playerlist = player.NewPlayerlist()

func main(){
  gamewatch.NewServerWatch(serverFile1, pl)
  gamewatch.NewServerWatch(serverFile2, pl)

  http.HandleFunc("/send/", sendHandler)
  http.HandleFunc("/uidlist/", uidListHandler)
  http.HandleFunc("/log/", logHandler)

  http.Handle("/", http.FileServer(http.Dir("static")))
  http.ListenAndServe(":9090", nil)
}

func sendHandler(w http.ResponseWriter, r *http.Request) {
  body, _ := ioutil.ReadAll(r.Body)
  query := string(body)+"\n"
  log.Println("recv from client:", query)  

  tcpAddr, err := net.ResolveTCPAddr("tcp4", "192.168.0.125:12020")
  checkError(err)
  
  conn, err := net.DialTCP("tcp", nil, tcpAddr)
  checkError(err)
  
  conn.Write([]byte(query))
  log.Println("sent:"+query)

  result := make([]byte, 1024)
  conn.Read(result)
  log.Println(string(result))
  fmt.Fprintf(w, "recv:"+string(result))

  conn.Close()
}

func uidListHandler(w http.ResponseWriter, r *http.Request) {
  fmt.Fprintf(w, pl.GetUidList())
}

func logHandler(w http.ResponseWriter, r *http.Request) {
  body, _ := ioutil.ReadAll(r.Body)
  type JQuery struct {
    Uid string
    Maxid int
  }
  var j JQuery
  json.Unmarshal(body, &j)
  fmt.Fprintf(w, pl.GetLog(j.Uid, j.Maxid))
}

func checkError(err error) {
    if err != nil {
        fmt.Fprintf(os.Stderr, "Fatal error: %s", err.Error())
        os.Exit(1)
    }
}

package gamewatch

import (
  "os/exec"
  "bufio"
  "os"
  "fmt"
  "regexp"
)

type logStreamReciver interface {
  AddPlayer(uid string, name string)
  AddLog(uid string, cmd string, detail string, time string)
}

type ServerWatch struct {
  file string
  reciver logStreamReciver
}

func NewServerWatch(file string, recv logStreamReciver) *ServerWatch {
  s := &ServerWatch{file, recv}
  go s.run()
  return s
}

func (s *ServerWatch) run() {
  cmd := exec.Command("tail", "-f", s.file)
  cmdout, err := cmd.StdoutPipe()
  checkError(err)
  err = cmd.Start()
  checkError(err)
  reader := bufio.NewReader(cmdout)
  regexLogin, _ := regexp.Compile(`UserLogin\[u(\d+)\](.*)`)
  regexLog, _ := regexp.Compile(`([\d\s-:]+),.*(S[cs]e.*)\[u(\d+)\]`)

  for {
    line, err := reader.ReadString('\n')
    if err != nil {
      break
    }
    loginSub := regexLogin.FindStringSubmatch(line)
    if len(loginSub) >= 3 {
      s.reciver.AddPlayer(loginSub[1], loginSub[2])
    }

    logSub := regexLog.FindStringSubmatch(line)
    if len(logSub) >= 4 {
      line, err := reader.ReadString('\n')
      if err != nil {
        break
      }
      s.reciver.AddLog(logSub[3], logSub[2], line, logSub[1])
    }
  }
  cmd.Wait()
}

func checkError(err error) {
    if err != nil {
        fmt.Fprintf(os.Stderr, "Fatal error: %s", err.Error())
        os.Exit(1)
    }   
}

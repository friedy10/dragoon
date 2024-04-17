package client

import (
	"bytes"
	"fmt"
	"log"
    "os"
    "syscall"
    "strings"
	"dragoon/gdbmi"
	"golang.org/x/crypto/ssh"
	"golang.org/x/crypto/ssh/terminal"
)

type Client struct {
    hostKey ssh.PublicKey
    config *ssh.ClientConfig
    conn *ssh.Client
    buffer bytes.Buffer
    sess *ssh.Session
}

func (client Client) Connect(user string, ip string, pass string) int {
	client.config = &ssh.ClientConfig{
		User: user,
		Auth: []ssh.AuthMethod{
			ssh.Password(pass),
		},
		HostKeyCallback: ssh.FixedHostKey(client.hostKey),
	}
    
    var err error
	client.conn, err = ssh.Dial("tcp", ip + ":22", client.config)
	if err != nil {
		log.Fatal("Failed to dial: ", err)
        return -1
	}
	
	// Each ClientConn can support multiple interactive sessions,
	// represented by a Session.
	client.sess, err = client.conn.NewSession()
	if err != nil {
		log.Fatal("Failed to create session: ", err)
	}

	// Once a Session is created, you can execute a single command on
	// the remote side using the Run method.
	client.sess.Stdout = &client.buffer

    return 0
}

func (client Client) runCmd(cmd string){
    if err := client.sess.Run(cmd); err != nil {
        fmt.Println("Failed to start server.")
    }

    fmt.Println(client.buffer.String())
}

func (client Client) close(){
    fmt.Println("Closing connection")
    client.conn.Close()
    client.sess.Close()
}

func (client Client) breakpoint(){
    fmt.Println("Dragoon")

    	// start a new instance and pipe the target output to stdout
	gdb, _ := gdb.New(nil)
	go io.Copy(os.Stdout, gdb)

	// evaluate an expression
	gdb.Send("var-create", "x", "@", "40 + 2")
	fmt.Println(gdb.Send("var-evaluate-expression", "x"))

	// load and run a program
	gdb.Send("file-exec-file", "wc")
	gdb.Send("exec-arguments", "-w")
	gdb.Write([]byte("This sentence has five words.\n\x04")) // EOT
	gdb.Send("exec-run")

	gdb.Exit()

}

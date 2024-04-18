package client

import (
	"bytes"
	"fmt"
	"log"
    	"os"
	"io"
	"dragoon/gdbmi"
	"golang.org/x/crypto/ssh"
)

type Client struct {
	hostKey ssh.PublicKey
    	config *ssh.ClientConfig
    	conn *ssh.Client
    	buffer bytes.Buffer
    	sess *ssh.Session
}

func (client *Client) Connect(user string, ip string, pass string) error {
	client.config = &ssh.ClientConfig{
		User: user,
		Auth: []ssh.AuthMethod{
			ssh.Password(pass),
		},
		// TODO: include hostkey to prevent MITM
		// HostKeyCallback: ssh.FixedHostKey(client.hostKey),
		HostKeyCallback: ssh.InsecureIgnoreHostKey(),
	}
    
    	var err error
	client.conn, err = ssh.Dial("tcp", ip + ":22", client.config)
	if err != nil {
		log.Fatal("Failed to dial: ", err)
        	return err
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

    	return nil
}

func (client *Client) RunCmd(cmd string) error {
	if err := client.sess.Run(cmd); err != nil {
        	fmt.Println("Failed to start server.")
    	}

    	fmt.Println(client.buffer.String())

	return nil
}

func (client *Client) Close(){
	fmt.Println("Closing connection")
    	client.conn.Close()
    	client.sess.Close()
}

func (client *Client) Breakpoint(){
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

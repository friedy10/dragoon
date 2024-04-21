package client

import (
	"bytes"
	"fmt"
	"log"
	"golang.org/x/crypto/ssh"
	"dragoon/gdbmi"
)

type Client struct {
	gdbmi.Backend
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

func (client *Client) Exit() {
	fmt.Println("Closing connection")
    	client.conn.Close()
    	client.sess.Close()
}

func (client* Client) Exec(cmd string) error {
	if err := client.sess.Run(cmd); err != nil {
		log.Fatal("Failed to execute command: ", err)
	}
	
	return nil
}

func (client* Client) Wait() error {
	if err := client.sess.Wait(); err != nil {
		log.Fatal("Command failed: ", err)
	}

	return nil
}
	
// Read reads a number of bytes from the target program's output.
func (client* Client) Read() (out string, err error) {
	return client.buffer.String(), nil
}
	
// Write writes a number of bytes to the target program's input.
func (client* Client) Write(p []byte) (n int, err error) {
	var buf bytes.Buffer
	client.sess.Stdin = &buf
	
	_, err = buf.Write(p)

	if err != nil {
		log.Fatal("Failed to set program input: ", err)
	}

	return len(p), nil
}

// func (client *Client) Breakpoint(){
// 	fmt.Println("Dragoon")
// 
//     	// start a new instance and pipe the target output to stdout
// 	gdb, _ := gdb.New(nil)
// 	go io.Copy(os.Stdout, gdb)
// 
// 	// evaluate an expression
// 	gdb.Send("var-create", "x", "@", "40 + 2")
// 	fmt.Println(gdb.Send("var-evaluate-expression", "x"))
// 
// 	// load and run a program
// 	gdb.Send("file-exec-file", "wc")
// 	gdb.Send("exec-arguments", "-w")
// 	gdb.Write([]byte("This sentence has five words.\n\x04")) // EOT
// 	gdb.Send("exec-run")
// 
// 	gdb.Exit()
// }

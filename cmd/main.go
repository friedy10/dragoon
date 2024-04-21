package main

import (
	"fmt"
	"io"
	"os"
	"log"
	"syscall"
	"strings"
	"dragoon/gdbmi"
	"dragoon/client"
	"golang.org/x/crypto/ssh/terminal"
)

func main() {
    	if len(os.Args) != 2 {
		fmt.Println("Usage: ./client raspberry@ip_address")
		return
	}

    	sshConnection := os.Args[1]
	fmt.Printf("Enter password for %s: ", sshConnection)

	// Turn off echoing for password input
	bytePassword, err := terminal.ReadPassword(int(syscall.Stdin))
	if err != nil {
		fmt.Println("Error reading password:", err)
		return
	}
	
	password := string(bytePassword)
	user := sshConnection[:strings.IndexByte(sshConnection, '@')]
	ipaddr := sshConnection[strings.IndexByte(sshConnection, '@')+1:]
	
	fmt.Println("\n\nSSH Connection Details:")
	fmt.Println("User:", user)
	fmt.Println("IP Address:", ipaddr) 
	fmt.Println("Password:", password)


	{
		var c *client.Client
		var g *gdbmi.Gdb

		err := c.Connect("pi", "192.168.1.1", "raspberry")
		if err != nil {
			log.Fatal(err)	
		}


		g, _ = gdbmi.New(nil, (*c).sess.Stdin, (*c).sess.Stdout, c)
		
		// Do we need this?
		go io.Copy(os.Stdout, g)

		// evaluate an expression
		g.Send("var-create", "x", "@", "40 + 2")
		fmt.Println(g.Send("var-evaluate-expression", "x"))

		// load and run a program
		g.Send("file-exec-file", "wc")
		g.Send("exec-arguments", "-w")
		g.cmd.Write([]byte("This sentence has five words.\n\x04")) // EOT
		g.Send("exec-run")

		g.Exit()	
	}
}


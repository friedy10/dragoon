package main

import (
	"fmt"
	"io"
	"os"
	"gdbmi/gdb"
	"gdbmi/client"
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


	var client Client
	var gdb Gdb
	{

		err := client.Connect("pi", "192.168.1.1", "raspberry")
		if err != nil {
			t.Fatal(err)	
		}


		gdb, _ := gdb.New(nil, client, &client.sess.Stdin, &client.sess.Stdout)
		
		// Do we need this?
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
}


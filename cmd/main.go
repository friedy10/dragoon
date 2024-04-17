package main

import (
	"fmt"
    "io"
    "os"
)




func main() {
	// An SSH client is represented with a ClientConn.
	//
	// To authenticate with the remote server you must pass at least one
	// implementation of AuthMethod via the Auth field in ClientConfig,
	// and provide a HostKeyCallback.

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

	fmt.Println("\n\nSSH Connection Details:")
	fmt.Println("User:", sshConnection[:strings.IndexByte(sshConnection, '@')])
	fmt.Println("IP Address:", sshConnection[strings.IndexByte(sshConnection, '@')+1:])
	fmt.Println("Password:", password)

}


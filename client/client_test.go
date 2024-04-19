package client

import (
	"testing"
	"bytes"
)

var consoleRecordCount = 0

func onNotification(t *testing.T, notification map[string]interface{}) {
	if notification["type"] != "console" {
		consoleRecordCount++
	}
}

func TestSSHConnection(t *testing.T) {
	var client Client

	err := client.Connect("pi", "192.168.1.1", "raspberry")
	if err != nil {
		t.Fatal(err)	
	}
	
	var buf bytes.Buffer
	client.sess.Stdin = &buf
	
	input := []byte("dmesg")
    	_, err = buf.Write(input)

	if err := client.sess.Run("bash"); err != nil {
		t.Fatal(err)
	}

	
    	
	if err != nil {
    	    	t.Fatal(err)
    	}	

	t.Log(client.buffer.String())	

	client.Close()
}


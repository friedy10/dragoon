package client

import (
	"testing"
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

	client.Close()
}

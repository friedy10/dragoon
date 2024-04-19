package gdb

import (
	"io"
	"sync"
	"strings"
)

type Backend interface {
	Exec(cmd string) error

	Wait() error

	Exit()
	
	// Read reads a number of bytes from the target program's output.
	Read() (out string, err error)
	
	// Write writes a number of bytes to the target program's input.
	Write(p []byte) (n int, err error)
}

// Gdb represents a GDB instance. It implements the ReadWriter interface to
// read/write data from/to the target program's TTY.
type Gdb struct {
	// The backend contains the methods for executing commands, 
	// writing parameters, and reading input
	cmd Backend
	
	stdin  *io.Writer
	stdout *io.Reader

	mutex  sync.RWMutex

	sequence int64
	pending  map[string]chan map[string]interface{}

	onNotification NotificationCallback

	recordReaderDone chan bool
}

// New creates and starts a new GDB instance. onNotification if not nil is the
// callback used to deliver to the client the asynchronous notifications sent by
// GDB. It returns a pointer to the newly created instance handled or an error.
func New(onNotification NotificationCallback, stdin *io.Writer, 
			stdout *io.Reader, cmd Backend) (*Gdb, error) {
	return NewCustom([]string{"gdb"}, onNotification, stdin, stdout, cmd)
}

// Like New, but allows to specify the GDB executable path and arguments.
func NewCustom(gdbCmd []string, onNotification NotificationCallback,stdin *io.Writer, 
			stdout *io.Reader, cmd Backend) (*Gdb, error) {
	gdbCmd = append(gdbCmd, "--nx", "--quiet", "--interpreter=mi2")
	gdb, err := NewCmd(gdbCmd, onNotification, stdin, stdout, cmd)

	return gdb, err
}

// NewCmd creates a new GDB instance like New, but allows explicitely passing
// the gdb command to run (including all arguments). cmd is passed as-is to
// exec.Command, so the first element should be the command to run, and the
// remaining elements should each contain a single argument.
func NewCmd(cmd []string, onNotification NotificationCallback, stdin *io.Writer, 
			stdout *io.Reader, cmd Backend) (*Gdb, error) {
	gdb := Gdb{onNotification: onNotification}

	// TODO FIX ARRAY STUFF
	gdb.cmd.exec(strings.Join(cmd, " "))

	// prepare the command interface
	gdb.sequence = 1
	gdb.pending = make(map[string]chan map[string]interface{})

	gdb.recordReaderDone = make(chan bool)

	gdb.cmd = cmd
	gdb.stdin = stdin
	gdb.stdout = stdout
	
	// start the line reader
	go gdb.recordReader()

	return &gdb, nil
}

// Interrupt sends a signal (SIGINT) to GDB so it can stop the target program
// and resume the processing of commands.
func (gdb *Gdb) Interrupt() error {
	return nil	
//return gdb.cmd.Process.Signal(os.Interrupt)
}

// Exit sends the exit command to GDB and waits for the process to exit.
func (gdb *Gdb) Exit() error {
	// send the exit command and wait for the GDB process
	if _, err := gdb.cmd.Write([]byte("gdb-exit")); err != nil {
		return err
	}

	// Wait for the recordReader to finish (due to EOF on the pipe).
	// If we do not wait for this, Wait() might clean up stdout
	// while recordReader is still using it, leading to
	// panic: read |0: bad file descriptor
	// <-gdb.recordReaderDone

	if err := gdb.cmd.Wait(); err != nil {
		return err
	}

	// close the target program's terminal, since the lifetime of the terminal
	// is longer that the one of the targer program's instances reading from a
	// Gdb object ,i.e., the master side will never return EOF (at least on
	// Linux) so the only way to stop reading is to intercept the I/O error
	// caused by closing the terminal
	
	gdb.cmd.Exit()

	return nil
}

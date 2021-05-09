// Copyright © 2016 Alan A. A. Donovan & Brian W. Kernighan.
// License: https://creativecommons.org/licenses/by-nc-sa/4.0/

// See page 227.

// Netcat is a simple read/write client for TCP servers.
package main

import (
	"bufio"
	"flag"
	"fmt"
	"io"
	"log"
	"net"
	"os"
)

var user = flag.String("user", "user0", "Username to use in chat")
var server = flag.String("server", "localhost:8000", "Servet to connect")

//!+
func main() {
	flag.Parse()

	strUser := *user
	strServer := *server

	conn, err := net.Dial("tcp", strServer)
	if err != nil {
		log.Fatal(err)
	}
	done := make(chan struct{})

	_, e := io.WriteString(conn, strUser+"\n")
	if e != nil {
		log.Fatal(err)
	}

	go func() {
		input := bufio.NewScanner(conn)
		for input.Scan() {
			fmt.Print("\n" + input.Text())
			fmt.Print("\n" + *user + " > ")
		}

		io.Copy(os.Stdout, conn) // NOTE: ignoring errors
		log.Println("done")
		done <- struct{}{} // signal the main goroutine
	}()
	mustCopy(conn, os.Stdin)
	conn.Close()
	<-done // wait for background goroutine to finish
}

//!-

func mustCopy(dst io.Writer, src io.Reader) {
	output := bufio.NewScanner(src)
	for output.Scan() {
		if output.Text() == "" {
			fmt.Print(*user + " > ")
			continue
		}
		_, e := io.WriteString(dst, output.Text()+"\n")
		if e != nil {
			fmt.Printf("Connection closed\n")
			return
		}
		fmt.Print(*user + " > ")
	}
}

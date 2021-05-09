// Copyright © 2016 Alan A. A. Donovan & Brian W. Kernighan.
// License: https://creativecommons.org/licenses/by-nc-sa/4.0/

// See page 254.
//!+

// Chat is a server that lets clients chat with each other.
package main

import (
	"bufio"
	"flag"
	"fmt"
	"log"
	"net"
	"strings"
	"time"
)

//!+broadcaster
type client chan<- string // an outgoing message channel

var users = make(map[string]string)
var userConnection = make(map[string]net.Conn)
var userChannels = make(map[client]string)
var userConnectionTime = make(map[string]string)
var userAdmin = ""
var contUsers = 0

var (
	entering = make(chan client)
	leaving  = make(chan client)
	messages = make(chan string) // all incoming client messages
)

func broadcaster() {
	clients := make(map[client]bool) // all connected clients
	author := ""
	message := ""
	available := false

	for {
		available = false
		select {
		case msg := <-messages:

			values := []rune(msg)
			a := strings.Index(msg, ":")

			if a >= 0 {
				message = string(values[a+2:])
				author = string(values[:a])
				available = true
			}

			msgStatus := strings.SplitN(message, " ", 2)

			if len(msgStatus[0]) > 0 {
				if msgStatus[0][0] == '/' {
					switch msgStatus[0] {
					case "/users":
						// fmt.Print("Llamada del comando /users\n")
						for usr := range userConnectionTime {
							var data = usr + " - connected since " + userConnectionTime[usr]
							sendDirectMessage("irc-server", author, data)
						}

					case "/msg":
						//fmt.Print("Llamada del comando /msg\n")
						if len(msgStatus) < 2 {
							sendDirectMessage("irc-server", author, "ALERT: msg command badly formatted")
							break
						}

						msgComponents := strings.SplitN(msgStatus[1], " ", 2)
						sendDirectMessage(author, msgComponents[0], msgComponents[1])

					case "/time":
						//fmt.Print("Llamada del comando /time\n")
						location, err := time.LoadLocation("America/Mexico_City")
						if err != nil {
							log.Fatal(err)
						}

						time := time.Now().In(location)
						var responseTime = "Local Time: " + time.Location().String() + time.Format(" 15:04")

						sendDirectMessage("irc-server", author, responseTime)

					case "/user":
						//fmt.Print("Llamada del comando /user\n")
						var userDesired = msgStatus[1]
						var userIP = strings.Split(userConnection[userDesired].LocalAddr().String(), ":")[0]
						var serverResponse = "username: " + userDesired + ", IP: " + userIP + " Connected since: " + userConnectionTime[userDesired]
						sendDirectMessage("irc-server", author, serverResponse)

					case "/kick":
						//fmt.Print("Llamada del comando /kick\n")
						if userConnection[author].RemoteAddr().String() == userAdmin {
							kickUser(msgStatus[1])

						} else {
							sendDirectMessage("irc-server", author, "ALERT: Only admin can kick users")
						}

					}

				} else {
					for cli := range clients {
						if available {
							if author != userChannels[cli] {
								cli <- author + " > " + message
							} else {
								//cli <- author + " > "
							}
						} else {
							cli <- msg
						}
					}

				}
			} else {
				// Broadcast incoming message to all
				// clients' outgoing message channels.
				for cli := range clients {
					if available {
						if author != userChannels[cli] {
							cli <- author + " > " + message
						} else {
							//cli <- author + " > "
						}
					} else {
						cli <- msg
					}
				}

			}

		case cli := <-entering:
			clients[cli] = true

		case cli := <-leaving:
			delete(clients, cli)
			close(cli)
		}
	}
}

//!-broadcaster

//!+handleConn
func handleConn(conn net.Conn) {
	ch := make(chan string) // outgoing client messages
	time := time.Now()
	go clientWriter(conn, ch)

	who := conn.RemoteAddr().String()
	input := bufio.NewScanner(conn)
	input.Scan()
	userName := input.Text()
	contUsers++
	users[who] = userName
	userConnection[userName] = conn
	userChannels[ch] = userName
	userConnectionTime[userName] = time.Format("2006-01-02 15:04:05")

	fmt.Printf("irc-server > New connected user [%s]\n", users[who])
	//ch <- "You are " + who
	ch <- "irc-server > Welcome to the Simple IRC Server"
	ch <- "irc-server > Your user [" + users[who] + "] is successfully logged"
	if userAdmin == "" {
		userAdmin = who
		ch <- "irc-server > Congrats, you were the first user."
		ch <- "irc-server > You're the new IRC Server ADMIN"
		fmt.Printf("irc-server > [%s] was promoted as the channel ADMIN\n", users[who])
	}
	//messages <- who + " has arrived"
	messages <- "irc-server > New connected user [" + users[who] + "]"
	entering <- ch

	//input := bufio.NewScanner(conn)
	for input.Scan() {
		messages <- users[who] + ": " + input.Text()

	}
	// NOTE: ignoring potential errors from input.Err()

	leaving <- ch
	//messages <- who + " has left"

	messages <- "irc-server > [" + users[who] + "] left the channel"
	//Checar si el map tiene el usuario
	contains := users[who]

	if contains != "" {
		fmt.Printf("irc-server > [%s] left\n", users[who])
	}

	conecti := userConnection[users[who]]
	if conecti != nil {
		delete(userConnection, users[who])
		delete(users, who)
		delete(userChannels, ch)
		delete(userConnectionTime, users[who])

	}

	if who == userAdmin {
		if len(userConnection) == 0 {
			userAdmin = ""
		} else {

			for nam := range userConnection {

				sendDirectMessage("irc-server", nam, "You're the new IRC Server ADMIN")
				fmt.Printf("irc-server > [%s] was promoted as the channel ADMIN\n", users[userConnection[nam].RemoteAddr().String()])
				userAdmin = userConnection[nam].RemoteAddr().String()
				break
			}
		}

	}

	conn.Close()
}

func clientWriter(conn net.Conn, ch <-chan string) {
	for msg := range ch {
		fmt.Fprintln(conn, msg) // NOTE: ignoring network errors
	}
}

func sendDirectMessage(author, destination, message string) {
	ch := make(chan string)
	go clientWriter(userConnection[destination], ch)
	ch <- author + " > " + message
	close(ch)
	return

}

func kickUser(username string) {
	if userConnection[username] == nil {
		return
	}

	sendDirectMessage("irc-server", username, "You're kicked from this channel")
	userConnection[username].Close()
	delete(users, userConnection[username].RemoteAddr().String())
	delete(userConnection, username)
	delete(userConnectionTime, username)

	fmt.Print("irc-server > [", username, "] was kicked\n")

	for usr := range userConnectionTime {
		sendDirectMessage("irc-server", usr, "["+username+"] was kicked from channel")
	}

}

//!-handleConn

//!+main
func main() {
	host := flag.String("host", "localhost", "Host for IRC server")
	port := flag.String("port", "8000", "Port for IRC server")
	flag.Parse()

	strHost := *host
	strPort := *port
	var hostAndPort = strHost + ":" + strPort

	listener, err := net.Listen("tcp", hostAndPort)
	if err != nil {
		log.Fatal(err)
	}

	fmt.Print("irc-server > Simple IRC Server started at ", hostAndPort, "\n")
	fmt.Print("irc-server > Ready for receiving new clients\n")

	go broadcaster()
	for {
		conn, err := listener.Accept()
		if err != nil {
			log.Print(err)
			continue
		}
		go handleConn(conn)
	}
}

//!-main

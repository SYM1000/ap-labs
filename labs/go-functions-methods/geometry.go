// Copyright © 2016 Alan A. A. Donovan & Brian W. Kernighan.
// License: https://creativecommons.org/licenses/by-nc-sa/4.0/

// See page 156.

// Package geometry defines simple types for plane geometry.
//!+point
package main

import ("math"
		"math/rand"
		"strconv"
		"os"
		"fmt"
		"time")

type Point struct{ x, y float64 }

func (p Point) X() float64 {
	return p.x
}

func (p Point) Y() float64 {
	return p.y
}

// traditional function
func Distance(p, q Point) float64 {
	return math.Hypot(q.x - p.x, q.y - p.y)
}

// same thing, but as a method of the Point type
func (p Point) Distance(q Point) float64 {
	return math.Hypot(q.x - p.x, q.y - p.y)
}

func isIntersection(lineA, lineB, rectA, rectB Point) bool {
	return direction(lineA, lineB, rectA) == 0 && isInRange(lineA, rectA, lineB) || direction(lineA, lineB, rectB) == 0 && isInRange(lineA, rectB, lineB) || direction(rectA, rectB, lineA) == 0 && isInRange(rectA, lineA, rectB) || direction(rectA, rectB, lineB) == 0 && isInRange(rectA, lineB, rectB) || direction(lineA, lineB, rectA) != direction(lineA, lineB, rectB) && direction(rectA, rectB, lineA) != direction(rectA, rectB, lineB)
}

func direction(a, b, c Point) int {
	value := (b.y - a.y) * (c.x - b.x) - (b.x - a.x) * (c.y - b.y)

	if (value == 0) {
		return 0
	}
	return -1
	
}

//!-point

//!+path

// A Path is a journey connecting the points with straight lines.
type Path []Point

// Distance returns the distance traveled along the path.
func (path Path) Distance() float64 {
	cont := 0.0
	
	for i := range path {
		if (i > 0) {
			cont += path[i-1].Distance(path[i])
		}
	}

	return cont
}

func getRandomNumber(min, max float64) float64 {
	return min + rand.Float64() * (max + 1 - min)
}

func printPerimeter(vert Path){
	side:=make(Path, 2)

	fmt.Printf("  -")

	for i := range vert{
		side[0]=vert[i]
		side[1]=vert[(i+1)%len(vert)]

		fmt.Printf(" %.2f ", side.Distance()) 

		if (i<len(vert)-1) {
			fmt.Printf("+") 
		}

	}

	fmt.Printf("= %.2f \n", vert.Distance())
}


func main() {
	var args []string = os.Args[1:]
	sides, _ := strconv.Atoi(args[0])

	//Validaacion
	if (args == nil || sides < 3) {
		fmt.Println("- INPUT ERROR\n")
		return
	}

	vertices := make(Path, sides)
	esColision := false

	fmt.Printf("- Generating a [%d] sides figure\n", sides)
	
	for{
		rand.Seed(time.Now().UnixNano())
		esColision = false

		for i := 0; i < sides; i++{
			vertices[i] = Point{getRandomNumber(-100.0,100.0),getRandomNumber(-100.0,100.0)}
		}

		for i := 0; i < sides; i++{
			if (isIntersection(vertices[i%sides], vertices[(1+i)%sides], vertices[(2+i)%sides],vertices[(3+i)%sides])) {
				esColision = true
				break
			}

		}

		if (!esColision) {
			fmt.Println("- Figure's vertices")

			for i := range vertices {
				fmt.Printf("  - (%.1f, %.1f)\n", vertices[i].X(), vertices[i].Y())
			}

			fmt.Println("- Figure's perimeter: ")

			printPerimeter(vertices)
			fmt.Println("")
			break
		}
	}
}

func isInRange(a, b, c Point) bool {
	return  b.x <= math.Max(a.x, c.x) && b.x >= math.Min(a.x, c.x) && b.y <= math.Max(a.y, c.y) && b.y >= math.Min(a.y, c.y)
}

//!-path

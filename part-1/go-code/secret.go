package main

import (
	"fmt"
	"os"
	"strings"
)
import "C"

// the "import C" statement is needed for the compiler to produce a binary
// which is going to load libc.so when launched. This is needed
// for the side-loading the hook logic.

var SECRET string = "VALIDATEME"

func theGuessingGame(s string) bool {
	if s == SECRET {
		fmt.Println("Authorized")
		return true
	} else {
		fmt.Println("Unauthorised")
		return false
	}

}

func main() {

	var s string
	// necessarry for the injection process
	fmt.Println("PID : ", os.Getpid())
	// necessary to retrieve the address of the function
	fmt.Printf("Func address: %p\n", theGuessingGame)

	for {
		if _, err := fmt.Scanf("%s", &s); err != nil {
			panic(err)
		}
		s = strings.ToLower(s)
		if theGuessingGame(s) {
			os.Exit(0)
		}
	}
}

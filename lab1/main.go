package main

import (
	"bytes"
	"fmt"
	"log"
	"os"
	"unicode"
)

var fileText = ""
var template = ""
var alphabetArray []byte
var alphabet map[byte]int
var progArguments []string
var templateAppearance []int
var automatsAmount = 0

var transitionArray [][]int

func main() {
	getArguments()
	fmt.Println(progArguments[1])
	readFile(progArguments[2])
	template = progArguments[1]
	automatsAmount = len(template) + 1
	fillAlphabet()
	// println(mySufficFunctionfunc("abb", "abc"))
	fillTransitionArray()
	findTemplate()
	// for i := 0; i < len(fileText); i++ {
		// fmt.Println(fileText[i])
		// 	fmt.Println(rune('Щ'))
		// 	fmt.Println(rune(fileText[i]) == rune('Щ'))
	//}
}

func convert(input string) string {
	var buf bytes.Buffer
	for _, r := range input {
		if unicode.IsControl(r) {
			fmt.Fprintf(&buf, "\\u%04X", r)
		} else {
			fmt.Fprintf(&buf, "%c", r)
		}
	}
	return buf.String()
}

func contains(s []byte, e byte) bool {
	for _, a := range s {
		if a == e {
			return true
		}
	}
	return false
}

func min(a, b int) int {
	if a < b {
		return a
	}
	return b
}

func fillAlphabet() {
	tempAlphabet := make(map[byte]int)
	for i := 0; i < len(fileText); i++ {
		if !contains(alphabetArray, fileText[i]) {
			alphabetArray = append(alphabetArray, fileText[i])
			tempAlphabet[fileText[i]] = len(alphabetArray) - 1
		}
	}
	alphabet = tempAlphabet
}

func check(err error) {
	if err != nil {
		log.Fatal(err)
	}
}

func getArguments() {
	for i := 0; i < len(os.Args); i++ {
		progArguments = append(progArguments, os.Args[i])
	}
}

func readFile(fileName string) {

	content, err := os.ReadFile(fileName)
	check(err)

	fileText = string(content)
	// r := bufio.NewReader(strings.NewReader(fileText))
	// for {
	// 	if c, sz, err := r.ReadRune(); err != nil {
	// 		if err == io.EOF {
	// 			break
	// 		} else {
	// 			log.Fatal(err)
	// 		}
	// 	} else {
	// 		fmt.Printf("%q [%d]\n", string(c), sz)
	// 	}
	// }
}

// func readFile(fileName string) {
// 	content, err := ioutil.ReadFile(fileName)
// 	check(err)
// 	fmt.Println(content)
// 	fmt.Println(len(fileText))
// 	fileText = string(content)
// 	fmt.Println(fileText)
// 	fmt.Println(len(fileText))
// }

func fillTransitionArray() {
	var m = automatsAmount
	// for i := 0; i < automatsAmount; i++ {
	// for j := 0; j < len(alphabetArray); j++ {

	tempTransitionArray := make([][]int, automatsAmount)
	for i := range tempTransitionArray {
		tempTransitionArray[i] = make([]int, len(alphabetArray))
		for j := range tempTransitionArray[i] {
			tempTransitionArray[i][j] = 0
		}
	}

	for q := 0; q < m; q++ {
		for k := 0; k < len(alphabetArray); k++ {
			var value = min(m+1, q+2)
			for {
				value--
				if value == m {
					value--
				}
				if mySufficFunctionfunc(template[0:value], (template[0:q]+string(alphabetArray[k]))) > 0 {
					// fmt.Println(template[0:value])
					// fmt.Println(template[0:q] + string(alphabetArray[k]))
					break
				}
				if value == 0 {
					break
				}
			}
			tempTransitionArray[q][k] = value
			// fmt.Print(value)
			// fmt.Println(mySufficFunctionfunc((template[0:q] + string(alphabetArray[k])), template[0:value]))
		}
		// fmt.Println()
	}
	// }
	// }
	transitionArray = tempTransitionArray
}

func mySufficFunctionfunc(x string, temp string) int {
	var result = 0
	var templatePosition = len(temp) - 1
	for i := len(x) - 1; i >= 0; i-- {
		if x[i] == temp[templatePosition] {
			templatePosition--
			result++
		} else {
			result = 0
			break
		}
	}
	return result
}

func getNext(q int, text byte) int {
	return transitionArray[q][alphabet[text]]
}

func findTemplate() {
	var m = len(template)
	var n = len(fileText)
	var white = 0
	var q = 0
	for i := 0; i < n; i++ {
		if fileText[i] == '\n' {
			white++
		}
		q = getNext(q, fileText[i])
		if q == m {
			templateAppearance = append(templateAppearance, i-m+1-white)
			i -= len(template)
			i++
		}
	}
	fmt.Println(templateAppearance)
}

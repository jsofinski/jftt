package main

import (
	"bufio"
	"bytes"
	"fmt"
	"io"
	"log"
	"os"
	"strings"
	"unicode"
)

var textArray []string
var templateArray []string

var alphabetArray []string
var alphabet map[string]int
var progArguments []string
var templateAppearance []int
var automatsAmount = 0

var transitionArray [][]int

func main() {
	// println(mySufficFunctionfunc([]string{"b", "c"}, []string{"a", "b"}))

	getArguments()
	readFile(progArguments[3])
	readFileTemplate(progArguments[2])
	fmt.Println(templateArray)
	automatsAmount = len(templateArray) + 1

	switch progArguments[1] {
	case "FA":
		fmt.Println("FA")
		fillAlphabet()
		fillTransitionArray()
		FA()
	case "KMP":
		fmt.Println("KMP")
		computePrefix()
		KMP()
	default:
		fmt.Printf("Nie podano algorytmu")
	}

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

func contains(s []string, e string) bool {
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
	tempAlphabet := make(map[string]int)
	for i := 0; i < len(textArray); i++ {
		if !contains(alphabetArray, textArray[i]) {
			alphabetArray = append(alphabetArray, textArray[i])
			tempAlphabet[textArray[i]] = len(alphabetArray) - 1
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

	fileText := string(content)
	r := bufio.NewReader(strings.NewReader(fileText))
	for {
		if c, _, err := r.ReadRune(); err != nil {
			if err == io.EOF {
				break
			} else {
				log.Fatal(err)
			}
		} else {
			// fmt.Printf("%q [%d]\n", string(c), sz)
			textArray = append(textArray, string(c))
		}
	}
}
func readFileTemplate(fileName string) {
	content, err := os.ReadFile(fileName)
	check(err)

	tempText := string(content)

	r := bufio.NewReader(strings.NewReader(tempText))
	for {
		if c, _, err := r.ReadRune(); err != nil {
			if err == io.EOF {
				break
			} else {
				log.Fatal(err)
			}
		} else {
			// fmt.Printf("%q [%d]\n", string(c), sz)
			templateArray = append(templateArray, string(c))
		}
	}
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

				// syf bo golang zachowuje sie idiotycznie, copy po referencji
				tempArray := []string{}
				for i := 0; i < q; i++ {
					tempArray = append(tempArray, templateArray[i])
				}
				tempArray = append(tempArray, string(alphabetArray[k]))

				// fmt.Print("t:")
				// // fmt.Println(templateArray[0:value])
				// fmt.Println(tempArray)
				// fmt.Println(templateArray)
				// fmt.Println(template)
				// fmt.Println(value)

				if mySufficFunctionfunc(templateArray[0:value], tempArray) > 0 {
					// fmt.Println(mySufficFunctionfunc(templateArray[0:value], append(templateArray[0:q], string(alphabetArray[k]))))
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
	// fmt.Println(transitionArray)
}

func mySufficFunctionfunc(x []string, temp []string) int {
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

func getNext(q int, text string) int {
	return transitionArray[q][alphabet[text]]
}

var pi []int

func computePrefix() {
	var m = len(templateArray)

	for i := 0; i < m; i++ {
		pi = append(pi, 0)
	}

	var k = 0
	for q := 1; q < m; q++ {
		for k >= 0 && templateArray[k] != templateArray[q] {
			if k-1 >= 0 {
				k = pi[k-1]
			} else {
				k = -1
			}
		}
		k++
		pi[q] = k
	}

	// for i := 0; i < m; i++ {
	// 	fmt.Print(pi[i])
	// }
	// fmt.Println()
}

func KMP() {
	var m = len(templateArray)
	var n = len(textArray)
	var blank = 0
	var q = 0
	for i := 0; i < n; i++ {
		if textArray[i] == "\n" {
			blank++
		}
		for q > 0 && templateArray[q] != textArray[i] {
			q = pi[q-1]
		}
		if templateArray[q] == textArray[i] {
			q++
		}
		if q == m {
			q = pi[q-1]
			templateAppearance = append(templateAppearance, i-m+1-blank)
			// fmt.Println(i - m + 1 - blank)
		}
	}
	fmt.Println(templateAppearance)
}

func FA() {
	var m = len(templateArray)
	var n = len(textArray)
	var blank = 0
	var q = 0
	for i := 0; i < n; i++ {
		// fmt.Println(i)
		if textArray[i] == "\n" {
			blank++
		}
		q = getNext(q, textArray[i])
		if q == m {
			templateAppearance = append(templateAppearance, i-m+1-blank)
		}
	}
	fmt.Println(templateAppearance)
}

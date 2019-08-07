package main

import (
	"fmt"
	"time"

	lxr "github.com/pegnet/LXRHash"
)

func main() {
	var lx lxr.LXRHash
	lx.Init(lxr.Seed, 25, 256, 5)

	start := time.Now()
	lx.GenerateTable()
	fmt.Println("time to init table", time.Since(start))

	start = time.Now()
	hs := make([]byte, 32)
	for i := 0; i < 100000; i++ {
		hs = lx.Hash(hs)
	}
	fmt.Println("hash time: ", time.Since(start))
	fmt.Printf("%x\n", hs)
}

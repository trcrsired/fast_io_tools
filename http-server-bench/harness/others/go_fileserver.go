package main

import (
	"log"
	"net/http"
	"os"
	"path"
)

func main() {
	if len(os.Args) < 3 {
		os.Exit(2)
	}
	root := os.Args[2]
	http.HandleFunc("/", func(w http.ResponseWriter, r *http.Request) {
		p := path.Clean("/" + r.URL.Path)
		if p == "/" {
			p = "/index.html"
		}
		f, err := os.Open(root + p)
		if err != nil {
			http.NotFound(w, r)
			return
		}
		defer f.Close()
		fi, err := f.Stat()
		if err != nil || fi.IsDir() {
			http.NotFound(w, r)
			return
		}
		http.ServeContent(w, r, p, fi.ModTime(), f)
	})
	log.Fatal(http.ListenAndServe("127.0.0.1:"+os.Args[1], nil))
}

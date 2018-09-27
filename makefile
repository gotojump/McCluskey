	CC = g++
	main = Conversor
	gen = GeraMin
	flags = -Wall -Wextra -g
	in = Minterms.min

all: clean comp

debug: $(main).cpp
	$(CC) $(flags) -DDEBUG $(main).cpp -o $(main)

generator: $(gen).cpp
	$(CC) $(flags) $(gen).cpp -o $(gen)

generate: generator
	./$(gen)

comp: $(main).cpp
	$(CC) $(flags) $(main).cpp -o $(main)

exec:
	./$(main) $(in)

git_init:
	git init
	git add --all
	git commit -m "First commit!"
	git remote add origin https://github.com/gotojump/McCluskey.git
	git push -u origin master

git:
	git add --all
	git commit -m "Go!"
	git push -u origin master

clean:
	rm -rf $(main)

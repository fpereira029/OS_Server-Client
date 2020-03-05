programs:
	gcc -o articleManagement articleManagement.c parsers.c
	gcc -o agregator agregator.c parsers.c
	gcc -o server server.c parsers.c
	gcc -o client client.c parsers.c

clean:
	rm artigos.txt
	rm stocks.txt
	rm vendas.txt
	rm strings.txt

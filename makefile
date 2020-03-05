programas:
	gcc -o ma ma.c parsers.c
	gcc -o ag ag.c parsers.c
	gcc -o sv sv.c parsers.c
	gcc -o cv cv.c parsers.c

teste:
	gcc -o cvTester cvTester.c

clean:
	rm artigos.txt
	rm stocks.txt
	rm vendas.txt
	rm strings.txt
	rm cvOutput.txt

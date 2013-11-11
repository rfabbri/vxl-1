all:
	pdflatex paper.tex
	bibtex -min-crossrefs=1000 paper.aux
	pdflatex paper.tex
	pdflatex paper.tex
	evince paper.pdf &
clean:
	rm *.aux
	rm *.log
	rm *.blg
	rm *.out
	rm *.toc

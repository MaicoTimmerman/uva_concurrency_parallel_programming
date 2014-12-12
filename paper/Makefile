RM=rm -rf
BIBC=bibtex
LATEXC=pdflatex
ARGS=-shell-escape
PDFNAME=paper
BIBNAME=bib


latex: $(PDFNAME).aux
	$(BIBC) $(PDFNAME).aux
	$(LATEXC) $(ARGS) $(PDFNAME).tex
	$(LATEXC) $(ARGS) $(PDFNAME).tex

$(PDFNAME).aux: $(PDFNAME).tex $(BIBNAME).bib
	$(LATEXC) $(ARGS) $(PDFNAME).tex

remove:
	$(RM) $(BIBNAME).bbl
	$(RM) $(BIBNAME).blg
	$(RM) $(PDFNAME).log
	$(RM) $(PDFNAME).aux
	$(RM) $(PDFNAME).pdf

clean:
	$(RM) $(BIBNAME).bbl
	$(RM) $(BIBNAME).blg
	$(RM) $(PDFNAME).log
	$(RM) $(PDFNAME).aux

TEX=main.tex
PDF=tp03.pdf

# -shell-escape: lo necesita minted (igual que en guias/Makefile).
TEXFLAGS=-file-line-error -interaction=nonstopmode -shell-escape

.PHONY: all clean clean-all

all: $(PDF)

# Mismo esquema que teoricas/guia_tads/Makefile: pdflatex sale con status 1
# ante cualquier warning, por eso las pasadas van con "-".
$(PDF): $(TEX)
	-pdflatex $(TEXFLAGS) -jobname $(basename $(PDF)) $<
	-pdflatex $(TEXFLAGS) -jobname $(basename $(PDF)) $<

clean:
	rm -f *.aux *.log *.out *.toc *.synctex.gz *.fls *.fdb_latexmk
	rm -rf _minted-*

clean-all: clean
	rm -f $(PDF)

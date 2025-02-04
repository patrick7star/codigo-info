
all: debug otimizado

#=== === === === === === === === === === === === === === === ==== == === ==
VERSOES_DIR = $(CCODES)/versions
VERSAO = v0.1.2
CONTEUDO = lib/ src/ makefile
NOME = codigo-info
CAMINHO = $(VERSOES_DIR)/$(NOME).$(VERSAO)

salva:
	tar -cvf $(CAMINHO).tar $(CONTEUDO)

clean:
	@echo "removendo 'build'(construturos) e 'bin'(executaveis)..."
	rm -rv bin/ build/

backups:
	@echo -e "\nvisualizar todos backups deste projeto:"
	@ls --sort=time -1 ../versions/codigo-info*.tar

#=== === === === === === === === === === === === === === === ==== == === ==
DEBUG_FLAGS = -D_MODO_DEBUG \
				  -D_FILTRA_TODOS_ARQUIVOS \
				  -D_REDIMENSIONA \
				  -U_REDIMENSIONA
NOME_EXE = bin/codigo_info_debug
DEPS = build/arraylist.o build/legivel.o build/tempo.o 
SRCS = src/main.c src/hashtable.c

cria-diretorios:
	@mkdir --mode=0770 -p bin lib

unit-tests: cria-diretorios
	gcc -I ../utilitarios/include -D_UNIT_TESTS -Ofast -Wall \
		-o bin/unit-tests src/main.c -Llib -llegivel

debug: cria-diretorios
	@echo "criando diretórios 'bin' e 'build' ..."
	mkdir -pv bin/ build/
	@echo "criando objetos das dependências em 'build' ..."
	gcc -c -o build/legivel.o lib/utilitarios/legivel.c
	gcc -c -o build/arraylist.o src/arraylist.c
	gcc -c -o build/tempo.o lib/utilitarios/tempo.c
	# nome do executável e fonte do programa:
	@echo "compilando de fato o teste ..."
	#gcc $(DEBUG_FLAGS) -o $(NOME_EXE) src/main.c \
	#src/arraylist.c src/hashtable.c build/legivel.o build/tempo.o \
	#-Wno-unused-function -Wno-main -Wall -I ./lib/utilitarios
	gcc $(DEBUG_FLAGS) -o $(NOME_EXE) $(SRCS) $(DEPS) \
	-Wno-unused-function -Wno-main -Wall -I ./lib/utilitarios
	# teste do componente de lista do código:
	@echo "compilando o teste da 'array-lista' ..."
	gcc -o bin/ut_arraylist src/arraylist.c -D_UT_ARRAY_LISTA -Wall

#=== === === === === === === === === === === === === === === ==== == === ==
release: cria-diretorios
	gcc -Ofast -o bin/codigo_info src/main.c \
   src/arraylist.c lib/utilitarios/legivel.c src/hashtable.c lib/utilitarios/tempo.c \
   -I lib/utilitarios -Wall




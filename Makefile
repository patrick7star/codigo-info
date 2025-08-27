
all: debug otimizado

#=== === === === === === === === === === === === === === === ==== == === ==
VERSOES_DIR = $(CCODES)/versions
VERSAO = v0.1.2
CONTEUDO = lib/ src/ makefile
NOME = codigo-info
CAMINHO = $(VERSOES_DIR)/$(NOME).$(VERSAO)

# Tal biblioteca apenas funciona na máquina do desenvolvedor do projeto.
LIB_UTILS_HEADERS = $(C_CODES)/utilitarios-em-c/include
LIB_UTILS_BINS		= $(C_CODES)/utilitarios-em-c/bin/shared
LIB_UTILS_ST_BINS = $(CCODES)/utilitarios-em-c/bin/static

salva:
	tar -cvf $(CAMINHO).tar $(CONTEUDO)

clean:
	@echo "removendo 'build'(construturos) e 'bin'(executaveis)..."
	rm -rv bin/ build/

backups:
	@echo -e "\nvisualizar todos backups deste projeto:"
	@ls --sort=time -1 ../versions/codigo-info*.tar

# Apenas funciona na máquina do desenvolvedor do projeto. Você obviamente
# não precisa fazer isso, pois os binários necessários já vem junto.
importa-biblioteca-externas:
	@cp -uv	$(LIB_UTILS_ST_BINS)/liblegivel.a			\
				$(LIB_UTILS_ST_BINS)/libhtref.a				\
				$(LIB_UTILS_ST_BINS)/libmemoria.a			\
				$(LIB_UTILS_ST_BINS)/liblaref.a				\
				$(LIB_UTILS_ST_BINS)/libestringue.a			\
				$(LIB_UTILS_ST_BINS)/libteste.a				\
				$(LIB_UTILS_ST_BINS)/libtempo.a				\
				$(LIB_UTILS_ST_BINS)/libterminal.a			\
				./lib/
	@cp -uv	$(LIB_UTILS_HEADERS)/legivel.h						\
				$(LIB_UTILS_HEADERS)/hashtable_ref.h				\
				$(LIB_UTILS_HEADERS)/memoria.h						\
				$(LIB_UTILS_HEADERS)/listaarray_ref.h				\
				$(LIB_UTILS_HEADERS)/estringue.h						\
				$(LIB_UTILS_HEADERS)/teste.h							\
				$(LIB_UTILS_HEADERS)/tempo.h							\
				$(LIB_UTILS_HEADERS)/terminal.h						\
				./lib/include
	@echo "As bibliotecas estáticas liblegivel, libhtref, libmemoria, liblaref e libestringue foram copiadas com sucesso."

#=== === === === === === === === === === === === === === === ==== == === ==
DEBUG_FLAGS = -D_MODO_DEBUG \
				  -D_FILTRA_TODOS_ARQUIVOS \
				  -D_REDIMENSIONA \
				  -U_REDIMENSIONA
DEPS = build/arraylist.o build/legivel.o build/tempo.o 
SRCS = src/main.c src/hashtable.c

cria-diretorios:
	@mkdir --mode=0770 -p bin lib/include build/

unit-tests: cria-diretorios
	gcc -I ../utilitarios/include -D_UNIT_TESTS -Ofast -Wall \
		-o bin/unit-tests src/main.c -Llib -llegivel

debug:
	@gcc -I$(LIB_UTILS_HEADERS) -g3 -O0 -Wall -pedantic \
		-c -o build/variaveis_de_ambiente-debug.o src/variaveis_de_ambiente.c
	@echo "Compilado objeto 'variaveis_de_ambiente-debug'."
	@gcc -I$(LIB_UTILS_HEADERS) -g3 -O0 -Wall -pedantic \
		-c -o build/filtro-debug.o src/filtro.c 
	@echo "Compilado objeto 'filtro-debug'."
	@gcc -I$(LIB_UTILS_HEADERS) -g3 -O0 -Wall -pedantic \
		-c -o build/menu-debug.o src/menu.c 
	@echo "Compilado objeto 'menu-debug'."
	@gcc -I$(LIB_UTILS_HEADERS) -g3 -O0 -Wall -pedantic -D__debug__ \
		-c -o build/classificacao-debug.o src/classificacao.c 
	@echo "Compilado objeto 'classificação-debug'."
	@gcc -g3 -O0 -Wall -pedantic \
		-c -o build/linque-debug.o src/linque.c 
	@echo "Compilado objeto 'linque-debug'."
	@gcc -I$(LIB_UTILS_HEADERS) -g3 -O0 -D__unit_tests__ -D__debug__ \
		-Wall -pedantic -Wno-unused-function -Wno-main \
		-c -o ./build/main-debug.o ./src/main.c 
	@echo "Compilado objeto do 'main-debug'."
	@gcc -I$(LIB_UTILS_HEADERS) -o ./bin/$(NOME)-debug -D__debug__ \
		build/main-debug.o build/classificacao-debug.o \
		build/variaveis_de_ambiente-debug.o build/filtro-debug.o \
		build/menu-debug.o build/linque-debug.o \
		-L$(LIB_UTILS_BINS) -lhtref -llegivel -lmemoria -llaref -lestringue 
	@echo "Lincando ambos objetos, também verifica algum 'bad coding'."
	

#=== === === === === === === === === === === === === === === ==== == === ==
release:
	@gcc -I./lib/include -O3 -Wall -pedantic -Wextra \
		-c -o build/variaveis_de_ambiente.o src/variaveis_de_ambiente.c \
		-L./lib -lestringue
	@echo "Compilado objeto 'variaveis_de_ambiente'."
	@gcc -I./lib/include -O3 -Wall -Wextra -pedantic -D__release__ \
		-Wno-unused-function -Wno-main \
		-c -o ./build/main.o ./src/main.c 
	@echo "Compilado objeto do 'main'."
	@gcc -I./lib/include -O3 -Oz -Wall -pedantic -Wextra \
		-c -o build/menu.o src/menu.c 
	@echo "Compilado objeto do 'menu'."
	@gcc -I./lib/include -O3 -Oz -Wall -pedantic -Wextra \
		-c -o build/classificacao.o src/classificacao.c
	@echo "Compilado objeto do 'classificacao'."
	@gcc -I./lib/include -O3 -Oz -Wall -pedantic -Wextra \
		-c -o build/filtro.o src/filtro.c 
	@echo "Compilado objeto do 'filtro'."
	@gcc -I./lib/include -D__release__ \
		-o./bin/$(NOME) build/main.o build/variaveis_de_ambiente.o \
		build/menu.o build/classificacao.o build/filtro.o \
		-L./lib -lhtref -llegivel -lmemoria -llaref -lestringue -lm
	@echo "Lincando ambos objetos, também verifica algum 'bad coding'."


#=== === === === === === === === === === === === === === === ==== == === ==
variaveis_de_ambiente:
	gcc -g3 -O0 -I$(LIB_UTILS_HEADERS) -Wall -pedantic -D__unit_tests__ \
		-o ./bin/ut_$@ ./src/$@.c \
		-L$(LIB_UTILS_BINS) -lteste -lestringue -limpressao -llaref \
      -lterminal -llegivel -ltempo -lm

linque:
	gcc -g3 -O0 -I$(LIB_UTILS_HEADERS) -Wall -pedantic -D__unit_tests__ \
		-o ./bin/ut_$@ ./src/$@.c \

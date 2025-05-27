
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
	@mkdir --mode=0770 -p bin lib/include build/

unit-tests: cria-diretorios
	gcc -I ../utilitarios/include -D_UNIT_TESTS -Ofast -Wall \
		-o bin/unit-tests src/main.c -Llib -llegivel

debug:
	gcc -I$(LIB_UTILS_HEADERS) -g3 -O0 -D__unit_tests__ -Wall \
		-Wno-unused-function -Wno-main \
		-o $(NOME_EXE) ./src/main.c \
		-L$(LIB_UTILS_BINS) -lhtref -llegivel -lmemoria -llaref
	
LIB_UTILS_ST_BINS = $(CCODES)/utilitarios-em-c/bin/static
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
release: cria-diretorios
	gcc -Ofast -o bin/codigo_info src/main.c \
   src/arraylist.c lib/utilitarios/legivel.c src/hashtable.c lib/utilitarios/tempo.c \
   -I lib/utilitarios -Wall


variaveis_de_ambiente:
	gcc -g3 -O0 -I$(LIB_UTILS_HEADERS) -Wall -pedantic -D__unit_tests__ \
		-o ./bin/ut_$@ ./src/$@.c \
		-L$(LIB_UTILS_BINS) -lteste -lestringue -lm

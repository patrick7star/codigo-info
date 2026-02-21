
all: debug otimizado

#=== === === === === === === === === === === === === === === ==== == === ==
VERSOES_DIR = $(CCODES)/versions
VERSAO = v0.2.0
CONTEUDO = lib/ src/ makefile
NOME = codigo-info
CAMINHO = $(VERSOES_DIR)/$(NOME).$(VERSAO)

# Tal biblioteca apenas funciona na máquina do desenvolvedor do projeto.
LIB_UTILS_HEADERS = $(CCODES)/utilitarios-em-c/include
LIB_UTILS_BINS		= $(CCODES)/utilitarios-em-c/bin/shared
LIB_UTILS_ST_BINS = $(CCODES)/utilitarios-em-c/bin/static
# Bibliotecas que já vem no pacote baixado.
LOCAL_UTILS_HEADERS	= ./lib/include
LOCAL_UTILS_BINS		= ./lib/
# Compiladores utilizados:
CLANGCPP = g++
CARGO		= cargo
CLANG		= gcc

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
	@cp -uv	$(LIB_UTILS_ST_BINS)/libteste.a				\
				$(LIB_UTILS_ST_BINS)/libterminal.a			\
				$(LIB_UTILS_ST_BINS)/libvisualiza.a			\
				$(LIB_UTILS_ST_BINS)/libcomputa.a			\
				$(LIB_UTILS_ST_BINS)/libcolecoes.a			\
				./lib/
	@cp -uv	$(LIB_UTILS_HEADERS)/legivel.h						\
				$(LIB_UTILS_HEADERS)/hashtable_ref.h				\
				$(LIB_UTILS_HEADERS)/memoria.h						\
				$(LIB_UTILS_HEADERS)/listaarray_ref.h				\
				$(LIB_UTILS_HEADERS)/estringue.h						\
				$(LIB_UTILS_HEADERS)/teste.h							\
				$(LIB_UTILS_HEADERS)/tempo.h							\
				$(LIB_UTILS_HEADERS)/terminal.h						\
				$(LIB_UTILS_HEADERS)/impressao.h						\
				$(LIB_UTILS_HEADERS)/macros.h							\
				$(LIB_UTILS_HEADERS)/definicoes.h \
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
	@mkdir --mode=0770 -p bin/tests/ lib/include build/
	@echo "Todos diretórios estão estruturados."

unit-tests: cria-diretorios
	gcc -I ../utilitarios/include -D_UNIT_TESTS -Ofast -Wall \
		-o bin/unit-tests src/main.c -Llib -llegivel

	
#=== === === === === === === === === === === === === === === ==== == === ===
HEADERS_RLS = ./lib/include
LIB_RLS		= ./lib


#=== === === === === === === === === === === === === === === ==== == === ==
#						Testes Unitários de Cada Módulo
#=== === === === === === === === === === === === === === === ==== == === ==
variaveis_de_ambiente:
	gcc -g3 -O0 -I$(LIB_UTILS_HEADERS) -Wall -pedantic -D__unit_tests__ \
		-o ./bin/ut_$@ ./src/$@.c \
		-L$(LIB_UTILS_BINS) -lteste -lestringue -limpressao -llaref \
      -lterminal -llegivel -ltempo -lm

linque:
	gcc -g3 -O0 -I$(LIB_UTILS_HEADERS) -Wall -pedantic -D__unit_tests__ \
		-o ./bin/ut_$@ ./src/$@.c \

classificacao2:
	gcc -I/usr/include/libxml2 -o bin/tests/ut_classificacao2 src/classificacao2.c -lxml2

menu-test:
	gcc -I$(HEADERS_DBG) -D__unit_tests__ \
		-o bin/tests/ut_menu build/linque.o build/variaveis_de_ambiente.o \
			build/filtro.o build/classificacao.o src/menu.c \
		-L$(LIB_DBG) -lhtref -llegivel -lmemoria -llaref -lestringue \
			-limpressao -lterminal

funcionalidades-test:
	gcc -c -D__unit_tests__ -D__debug__ -I$(LIB_UTILS_HEADERS) -std=c11 \
		-o build/funcionalidades-test.obj src/funcionalidades.c
	gcc -I$(LIB_UTILS_HEADERS) -o \
		bin/tests/ut-fucionalidades build/funcionalidades-test.obj \
		-L$(LIB_UTILS_ST_BINS) -lteste -lm
#=== === === === === === === === === === === === === === === ==== == === ==

cria-raiz-programas:
	@mkdir -p build/cmd-frequencia build/ build/pacotes-externos/
	@mkdir -p bin/programs
	@echo "Diretório dos programas criados com sucesso."

cmd-frequencia-debug:
	$(CLANGCPP) -O0 -g3 -c -o \
		build/cmd-frequencia/main-debug.o cmd-frequencia/src/main.cpp
	$(CLANGCPP) -std=c++17 -O0 -g3 -c -I$(LIB_UTILS_HEADERS) -o \
		build/cmd-frequencia/lincagem-debug.o cmd-frequencia/src/lincagem.cpp
	$(CLANGCPP) -std=c++17 -O0 -g3 -I$(LIB_UTILS_HEADERS) \
		-c -o build/cmd-frequencia/extracao_comando-debug.o \
				cmd-frequencia/src/extracao_comando.cpp
	@echo "Objeto lincagem-debug" compilado com sucesso.
	$(CLANGCPP) -I$(LIB_UTILS_HEADERS) \
		-o bin/programs/cmd-frequencia-debug \
			build/cmd-frequencia/main-debug.o \
			build/cmd-frequencia/lincagem-debug.o \
			build/cmd-frequencia/extracao_comando-debug.o \
		-L$(LIB_UTILS_ST_BINS) -lteste

pacotes-externos-debug:
	@cd pacotes-externos && \
		$(CARGO) build --target-dir ../bin/programs/target-pe/
	@echo "Projeto Rust compilado com sucesso."
	@ln -T bin/programs/target-pe/debug/cargo-listagem \
			bin/programs/pacotes-externos-debug
	@echo "Hard linque ligando binário criado."
		
pacotes-externos-release: cria-raiz-programas
	@cd pacotes-externos && \
		cargo build -q --release --target-dir ../build/pacotes-externos/
	@echo "Projeto Rust compilado com sucesso."
	@ln -f -T build/pacotes-externos/release/cargo-listagem \
			bin/programs/pacotes-externos
	@echo "Hard linque ligando binário criado."

cmd-frequencia-release: cria-raiz-programas
	@$(CLANGCPP) -O3 -c -o \
		build/cmd-frequencia/main.o cmd-frequencia/src/main.cpp
	@echo "Objeto 'main' compilado com sucesso."
	@$(CLANGCPP) -std=c++17 -O3 -c -I$(LIB_UTILS_HEADERS) -o \
		build/cmd-frequencia/lincagem.o cmd-frequencia/src/lincagem.cpp
	@echo "Objeto 'lincagem' compilado com sucesso."
	@$(CLANGCPP) -std=c++17 -O3 -I$(LIB_UTILS_HEADERS) \
		-c -o build/cmd-frequencia/extracao-comando.o \
				cmd-frequencia/src/extracao_comando.cpp
	@echo "Objeto 'extração-comando' compilado com sucesso."
	@$(CLANGCPP) -Ilib/include \
		-o bin/programs/cmd-frequencia \
			build/cmd-frequencia/main.o \
			build/cmd-frequencia/lincagem.o \
			build/cmd-frequencia/extracao-comando.o \
		-Llib/ -lteste -lvisualiza
	@echo "Lincados num binário chamado 'cmd-frequencia'."

#=== === === === === === === === === === === === === === === ==== == === ===

debug:
	@gcc -I$(LIB_UTILS_HEADERS) -g3 -O0 -Wall -pedantic \
		-c -o build/variaveis_de_ambiente-debug.o src/variaveis_de_ambiente.c
	@echo "Compilado objeto 'variaveis_de_ambiente-debug'."
	@gcc -I$(LIB_UTILS_HEADERS) -g3 -O0 -Wall -pedantic -D__debug__ \
		-c -o build/filtro-debug.o src/filtro.c 
	@echo "Compilado objeto 'filtro-debug'."
	@gcc -I$(LIB_UTILS_HEADERS) -g3 -O0 -Wall -pedantic -D__debug__ \
		-c -o build/menu-debug.o src/menu.c 
	@echo "Compilado objeto 'menu-debug'."
	@gcc -I$(LIB_UTILS_HEADERS) -g3 -O0 -Wall -pedantic -D__debug__ \
		-c -o build/classificacao-debug.o src/classificacao.c 
	@echo "Compilado objeto 'classificação-debug'."
	@gcc -I$(LIB_UTILS_HEADERS) -g3 -O0 -Wall -pedantic \
		-c -o build/linque-debug.o src/linque.c 
	@echo "Compilado objeto 'linque-debug'."
	@gcc -I$(LIB_UTILS_HEADERS) -g3 -O0 -D__unit_tests__ -D__debug__ \
		-Wall -pedantic -Wno-unused-function -Wno-main \
		-c -o ./build/main-debug.o ./src/main.c 
	@echo "Compilado objeto do 'main-debug'."
	@$(CLANG) -O0 -g3 -D__debug__ -Wall \
		-c -o build/funcionalidades-debug.o src/funcionalidades.c
	@echo "Compilado objeto do 'funcionalidades-debug'."
	@gcc -I$(LIB_UTILS_HEADERS) -D__debug__ \
		-o ./bin/$(NOME)-debug \
			build/main-debug.o build/classificacao-debug.o \
			build/variaveis_de_ambiente-debug.o build/filtro-debug.o \
			build/menu-debug.o build/linque-debug.o \
			build/funcionalidades-debug.o \
		-L$(LIB_UTILS_ST_BINS) \
			-lcolecoes -lcomputa -lvisualiza -lm -lterminal
	@echo "Lincando ambos objetos, também verifica algum 'bad coding'."

release: pacotes-externos-release cmd-frequencia-release
	@gcc -I./lib/include -O3 -Wall -pedantic -Wextra \
		-c -o build/variaveis_de_ambiente.o src/variaveis_de_ambiente.c \
		-L./lib -lestringue
	@echo "Compilado objeto 'variaveis_de_ambiente'."
	@gcc -I./lib/include -O3 -Wall -Wextra -pedantic -D__release__ \
		-Wno-unused-function -Wno-main \
		-c -o ./build/main.o ./src/main.c 
	@echo "Compilado objeto do 'main'."
	@$(CLANG) -I$(LIB_UTILS_HEADERS) -O3 -Oz -Wall -pedantic -Wextra \
		-c -o build/funcionalidades.o src/funcionalidades.c 
	@gcc -I./lib/include -O3 -Oz -Wall -pedantic -Wextra \
		-c -o build/menu.o src/menu.c 
	@echo "Compilado objeto do 'menu'."
	@gcc -I./lib/include -O3 -Oz -Wall -pedantic -Wextra \
		-c -o build/classificacao.o src/classificacao.c
	@echo "Compilado objeto do 'classificacao'."
	@gcc -I./lib/include -O3 -Oz -Wall -pedantic -Wextra \
		-c -o build/filtro.o src/filtro.c 
	@echo "Compilado objeto do 'filtro'."
	@gcc -I$(LOCAL_UTILS_HEADERS) -O3 -Oz -Wall -pedantic -Wextra -Werror \
		-c -o build/linque.o src/linque.c
	@echo "Compilado objeto do 'linque'."
	@gcc -I$(LOCAL_UTILS_HEADERS) -D__release__ \
		-o./bin/$(NOME) \
			build/main.o build/variaveis_de_ambiente.o \
			build/menu.o build/classificacao.o build/filtro.o \
			build/linque.o build/funcionalidades.o \
		-L$(LOCAL_UTILS_BINS) -lcolecoes -lvisualiza -lcomputa -lm -lbasico
	@echo "Lincando ambos objetos, então, compilando binário."
	@echo "Todo o programa foi compilado com sucesso."

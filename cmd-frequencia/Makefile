

all: release debug

.PHONY: debug

# apenas defiinindo macros para desabilitar ou ativar partes do código a 
# compilar.
DEBUG_FLAGS = -D_NADA -D_DEBUG_VISUALIZACAO_DO_MAPA \
	-D_DEBUG_MODO_VISUALIZACAO -D_DEBUG_ORDENACAO_DECRESCENTE \
	-D_EXTRACAO_CMD -D_TESTES -D_CARREGA_CMDS
LIB_UTILS = $(CCODES)/utilitarios-em-c/include
SRC_UTILS = $(CCODES)/utilitarios-em-c/build
DEPS = $(SRC_UTILS)/teste.o $(SRC_UTILS)/tempo.o \
		$(SRC_UTILS)/legivel.o $(SRC_UTILS)/terminal.o
HEADERS = -I$(CCODES)/utilitarios-em-c/include/
TESTADOR = -L$(CCODES)/utilitarios-em-c/bin/shared/ \
			  -lteste -ltempo -lterminal -llegivel

EXE_DEBUG = bin/debug
FLAGS_DEBUG = -O0 -Wall -std=c++2a -Wno-unused-function

debug: 
	@echo "compilando os testes e debuggin' ..."
	g++ $(FLAGS_DEBUG) -I $(LIB_UTILS) $(DEBUG_FLAGS) -o $(EXE_DEBUG) src/main.cpp -L ../utilitarios/bin/static -lteste -llegivel -lterminal -ltempo


check-historico:
	clang++ -fsyntax-only -Wall -D__debug__ -std=c++17 src/historico.cpp

historico:
	clang++ -I $(LIB_UTILS) -O0 -Wall -D__debug__ -std=c++17 \
		-o bin/ut_historico src/historico.cpp

check-lincagem:
	clang++ $(HEADERS) -std=gnu++17 -fsyntax-only -Wall -O0 -D__unit_tests__ \
		-o bin/ut_lincagem src/lincagem.cpp

test-lincagem:
	clang++ $(HEADERS) -Werror -std=gnu++17 -Wall -O0 \
		-D__unit_tests__ -D__debug__\
		-o bin/ut_lincagem src/lincagem.cpp $(TESTADOR)

release: 
	@mkdir -p build/
	g++ $(HEADERS) -c -o build/extracao_comando.o src/extracao_comando.cpp
	g++ $(HEADERS) -c -o build/lincagem.o src/lincagem.cpp
	@echo "Objetos de 'lincagem' e 'extracao_comandos' compilados."
	g++ $(HEADERS) -Wall -std=c++17 -Wno-unused-function \
		-o bin/cmd_frequencia src/main.cpp build/*.o
	@echo "Lincagem realizada com sucesso(cmd_frequencia)."
	


EXE_UT = bin/extracao_cmd
FLAGS_UT = -D_EXTRACAO_CMD -D_TESTES 
testes-unitarios:
	g++ $(FLAGS_UT) -o $(EXE_UT) src/main.cpp -Wall -std=c++17


clean:
	rm --force -v *.o 
	rm --force --verbose bin/*


LOCAL = $(CCODES)/versions
VERSAO = v0.1.2
NOME = comandos-frequencia.$(VERSAO)
OPCAO = -cvf
CONTEUDO = src/*.cpp bin/cmd_frequencia Makefile

backup:
	tar $(OPCAO) $(LOCAL)/$(NOME).tar $(CONTEUDO)

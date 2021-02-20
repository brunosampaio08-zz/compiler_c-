Aluno:
	Bruno Sampaio Leite - 120213

Compilador de C- com base no compilador Tiny disponivel no livro "Compiladores: Principios e Práticas", Kenneth C. Louden.
Código de síntese para a máquina alvo disponível em https://github.com/brunosampaio08/acc_proccessor
Versão mais atualizada desse código disponível em https://github.com/brunosampaio08/compiler_c-

Para executar no terminal:
	cd compilador_c-
	make
	./compiler <arquivodeentrada.txt>

/****ATENÇÃO******/

Todos os arquivos de saída estão na pasta compilador_c-/output.
O arquivo de entrada DEVE estar na pasta compilador_c-/input.
Os códigos da fase de síntese só serão gerados caso NÃO EXISTAM erros sintáticos ou semânticos no arquivo de entrada.
    ->O item anterior pode ser alterado retirando o 'if' que checa por erros em main.c;
Após a compilação 'make clean' limpa os arquivos desnecessários.
    ->Após o make clean para execução deve-se compilar novamente.

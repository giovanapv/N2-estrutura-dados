Simulação de sensores de uma planta industrial inteligente em linguagem C.  
O projeto está dividido em 3 programas:

- "primeiro.c": Gera um arquivo com leituras aleatórias para múltiplos sensores, de diferentes tipos (int, float, bool, string), dentro de um intervalo de tempo definido pelo usuário.Cada sensor recebe 2000 medições.
  
- "segundo.c": Lê o arquivo bruto gerado no programa anterior, identifica os sensores, separa os dados por tipo e sensor, ordena as leituras por timestamp e salva tudo em arquivos separados.Esse processo garante que os dados estejam prontos para consultas rápidas e precisas.
  
- "terceiro.c": Permite ao usuário informar uma data e hora específica e realiza uma busca binária para encontrar a leitura mais próxima daquele momento para um sensor específico.

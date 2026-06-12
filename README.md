# Trabalho 2 - Transformacoes 3D

Visualizador de objetos 3D em wireframe com projecao ortografica, escrito em C
com SDL2. Permite aplicar transformacoes (rotacao, translacao, escala) no objeto
e orbitar a camera ao redor dele com o mouse.

## Bibliotecas necessarias

- **GCC** e **make** (compilacao)
- **SDL2** (biblioteca grafica)

Instalacao no Fedora:

```bash
sudo dnf install gcc make SDL2-devel
```

## Compilacao

A partir da raiz do projeto:

```bash
make          # compila e gera o executavel "trabalho2"
make clean    # remove os arquivos gerados
```

## Execucao

```bash
./trabalho2                      # abre objetos/cubo.dcg (padrao)
./trabalho2 objetos/casa.dcg     # abre outro objeto
```

Rode sempre a partir da raiz do projeto (o caminho padrao e relativo).

Objetos disponiveis em `objetos/`: cubo, piramide, casa.

## Controles

| Tecla / Mouse              | Acao                                  |
|----------------------------|---------------------------------------|
| Setas                      | Rotaciona o objeto em X (cima/baixo) e Y (esq/dir) |
| Q / E                      | Rotaciona o objeto em Z                |
| W A S D                    | Translada o objeto na tela             |
| M / N                      | Escala (aumenta / diminui)             |
| Arrastar mouse (botao esq) | Orbita a camera ao redor do objeto     |
| Roda do mouse              | Aproxima / afasta (zoom)               |
| R                          | Reseta a camera para a vista frontal   |
| P                          | Imprime os dados do objeto no terminal |
| ESC                        | Sai                                    |

## Estrutura do projeto

```
src/        codigo-fonte (.c)
include/    cabecalhos (.h)
objetos/    modelos 3D (.dcg)
build/      arquivos objeto (.o) gerados pelo make
docs/       PDF do trabalho e material de referencia
```

## Formato dos arquivos .dcg

```
<numero de vertices>
x y z          (uma linha por vertice)
<numero de arestas>
a b            (indices de dois vertices, uma linha por aresta)
```

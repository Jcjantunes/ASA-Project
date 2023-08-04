#include <stdlib.h>
#include <stdio.h>

/*--------------------------------------*/
/*
   Joao Antunes     87668
   Viviana Bernardo 87709
   Grupo 041
*/
/*--------------------------------------*/

#define MIN(a,b) ((a) < (b) ? a : b)
#define NIL -1

#define getNodeValue(n) n->value
#define getNext(n) n->next

#define getOrigin(l) l.origem
#define getDestiny(l) l.destino

#define getOrigem(l) l->origem
#define getDestino(l) l->destino

#define getVertexId(v) v->id
#define getVertexDiscovery(v) v->discovery
#define getVertexLow(v) v->low
#define getInstackFlag(v) v->inStack
#define getVertexSCCId(v) v->SCCId
#define getVertexLowId(v) v->SCClowId
#define getGraphRoot(v) v->root
#define getVertexHead(v) v->head

#define getNumScc(g) g->SCC
#define getNumVertix(g) g->V

/*------------- ESTRUTURAS -------------*/

typedef struct node {
	int value;
	struct node *next;
} *Node;

typedef struct lig {
	int origem;
	int destino;
} *Ligacoes;

typedef struct vertice {
	int id;
	int discovery;
	int low;
	int inStack;
	int SCCId;				/*identificador da SCC a que o vertice pertence*/
	int SCClowId;			/*identificador mais pequeno da SCC*/
	int root;				/*raiz da SCC*/
	Node head;  
}* Vertice;

typedef struct graph {
	int V; 
	int SCC;
	Vertice *adj; 
}* Graph;

/*------------------------------------------*/

static Node top;	/*topo da pilha*/

/*------------- INICIALIZACOES -------------*/

Node initNode(int value, Node next){
	Node node = (Node)malloc(sizeof(struct node));
	node->value = value;
	node->next = next;

	return node;
}

Vertice initVertix(int id, Node head) {
	Vertice vertice = (Vertice)malloc(sizeof(struct vertice));
  
	vertice->id = id;	
	vertice->discovery = vertice->low = vertice->inStack = NIL;
	vertice->head = head;

	return vertice;
}

Graph initGraph(int edges, int vertices) {
	Graph graph = (Graph)malloc(sizeof(struct graph));
	graph->V = vertices;
	graph->SCC = 0;
	graph->adj = malloc(sizeof(struct vertice)*vertices);
	
	int i;
	for (i = 0; i < vertices; ++i)
    	graph->adj[i] = initVertix(i,NULL);
  
  	return graph;   
}

/*--------------------------------------------*/

Graph addEdge(Graph graph, int origem, int destino) {		/*adiciona uma aresta ao vertice*/

  graph->adj[origem]->head = initNode(destino,graph->adj[origem]->head);
  return graph;
}

/*------------- FUNCOES DA PILHA -------------*/

void init() { 				/* inicializa a pilha */
 	top = NULL;
} 

void push(int value) { 		/* introduz novo elemento no topo */
 	Node new = initNode(value, top);
 	top = new;
}

int is_empty() { 			/* pergunta se está vazia */
 	return top == NULL;
}

int pop() { 				/* apaga o topo e retorna o valor apagado */
 	if (!is_empty()) {
 		int value = top->value;
 		Node old = top;
 		top = top->next;
 		free(old);
 		return value;
 	}
 	
 	else
 		return -1;
}

/*---------------------------------------------*/

/*---------PRIMITIVAS FUNCOES AUXILIARES ------*/

void Tarjan_Visit(Graph graph, Vertice u);
void SCC_Tarjan(Graph graph);

int cmpfunc(const void *a, const void *b);

int getConnections(Graph graph,Ligacoes ligacoes, int numVertices);
int removeDuplicatedConnections(Ligacoes ligacoes, int count);
void printConnections(Ligacoes ligacoes, int count);

/*----------------------------------------------*/

/*------------------- MAIN -------------------*/

int main() {
	
	int numVertices,numArestas;
	int vertice1,vertice2;
	int i, count, countLigacoes;

	if (!scanf("%d\n",&numVertices)) {
		return -1;
	}

	if (!scanf("%d\n",&numArestas)){
		return -1;
	}

	if (numVertices < 2 || numArestas < 1) {
		printf("Numero de vertices > 2 e numero de arestas < 1\n");
		return -1;
	}

	Graph graph = initGraph(numArestas,numVertices);

	if (graph == NULL) {
		printf("erro a criar grafo\n");
		return -1;
	}

	for (i = 0; i < numArestas; ++i) {
		if (!scanf("%d %d",&vertice1,&vertice2))
			return -1;

		addEdge(graph, vertice1 -1, vertice2 -1);		
	}

	SCC_Tarjan(graph);

	printf("%d\n", getNumScc(graph));				/*Imprime o numero de SCC*/

	Ligacoes ligacoes = (Ligacoes)malloc(sizeof(struct lig)*numArestas);

	count = getConnections(graph,ligacoes,numVertices);

	qsort(ligacoes, count, sizeof(struct lig), cmpfunc);

	countLigacoes = removeDuplicatedConnections(ligacoes,count);
	
	printf("%d\n", countLigacoes);			/*Imprime o numero de ligacoes entre SCC*/

	printConnections(ligacoes, count);	

	return 0;
}

/*-------------FUNCOES AUXILIARES--------------*/

/*------------ ALGORITMO DE TARJAN ------------*/

void SCC_Tarjan(Graph graph) {
	init();
  	int i;

  	for (i=0; i < getNumVertix(graph); ++i)
  		if (getVertexDiscovery(graph->adj[i]) == NIL)
  			Tarjan_Visit(graph, graph->adj[i]);
}

void Tarjan_Visit(Graph graph, Vertice u) {
	static int visited = 0;
	Vertice v;
	Node n;

	getVertexDiscovery(u) = getVertexLow(u) = visited++;

	push(getVertexId(u));
	getInstackFlag(u) = 1;

	for (n = getVertexHead(u); n != NULL; n = getNext(n)) {
		v = graph->adj[getNodeValue(n)];
		if (getVertexDiscovery(v) == NIL || getInstackFlag(v) == 1) {
			if (getVertexDiscovery(v) == NIL)
				Tarjan_Visit(graph, v);
			getVertexLow(u) = MIN(getVertexLow(u), getVertexLow(v));
		}
	}

	if (getVertexDiscovery(u) == getVertexLow(u)) {
		int out, lowId;
		getNumScc(graph)++;

		lowId = getVertexId(u);
		
		while (getNodeValue(top) != getVertexId(u)) {
			out = pop();
			v = graph->adj[out];
			
			getInstackFlag(v) = NIL;
			lowId = MIN(out,lowId);
			
			getGraphRoot(v) = getVertexId(u);
			getVertexSCCId(v) = getNumScc(graph);
		}

		out = pop();
		v = graph->adj[out];
		
		getInstackFlag(v) = NIL;
		getVertexSCCId(v) = getNumScc(graph);
		getGraphRoot(v) = getVertexId(u);
		getVertexLowId(v) = lowId;
	}
}

/*-----------------------------------------------------------*/

int cmpfunc(const void *a, const void *b) {		/*funcao de comparacao para usar no qsort*/
	Ligacoes first = (Ligacoes)a;
	Ligacoes last = (Ligacoes)b;

	if (getOrigem(first) - getOrigem(last) != 0)
		return getOrigem(first) - getOrigem(last);
	
	else
		return getDestino(first) - getDestino(last);
}

int getConnections(Graph graph,Ligacoes ligacoes, int numVertices) {
	
	int i,count = 0;
	Node n;
	Vertice v, u;

	for (i = 0; i < numVertices; ++i) {
		v = graph->adj[i];

		for (n = getVertexHead(v); n != NULL; n = getNext(n)) {
			u = graph->adj[getNodeValue(n)];

			if (getVertexSCCId(v) != getVertexSCCId(u)){
				getOrigin(ligacoes[count]) = graph->adj[getGraphRoot(v)]->SCClowId+1;
				getDestiny(ligacoes[count++]) = graph->adj[getGraphRoot(u)]->SCClowId+1;
			}
		}
	}

	return count;
}

int removeDuplicatedConnections(Ligacoes ligacoes, int count) {

	int i,j,countLigacoes = count;

	for (i = 1; i < count; ++i) {
		j = i-1;
		if (getOrigin(ligacoes[i]) == getOrigin(ligacoes[j]) && getDestiny(ligacoes[i]) == getDestiny(ligacoes[j])){
			getDestiny(ligacoes[j++]) = NIL;
			countLigacoes--;
		}
	}

	return countLigacoes;
}

void printConnections(Ligacoes ligacoes, int count) {
	int i;
	for (i = 0; i < count; ++i)
		if (getDestiny(ligacoes[i]) != NIL)
			printf("%d %d\n", getOrigin(ligacoes[i]) , getDestiny(ligacoes[i]));		/*Imprime cada ligacao entre as SCC*/
}
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

/*--------------------------------------*/
/*
   Joao Antunes     87668
   Viviana Bernardo 87709
   Grupo 041
*/
/*--------------------------------------*/

#define MIN(a,b) ((a) < (b) ? a : b)
#define SWITCH(a) ((a) == 0 ? 1 : 0)
#define NIL -1

#define getNodeValue(n) n->value
#define getFlow(n) n->flow
#define getResidualFlow(n) n->rGraphFlow
#define getPixelParent(n) n->parent
#define getVisitedDFS(n) n->visitedDFS
#define getVisitedBFS(n) n->visitedBFS
#define getPrimeiroPlano(n) n->primeiroPlano
#define getCenario(n) n->cenario
#define getNext(n) n->next
#define getReverse(n) n->reverse

#define getNumberPixels(g) g->V
#define getPesoFinal(g) g->pesoFinal
#define getGraphAdj(g,i) g->adj[i]


/*------------- ESTRUTURAS -------------*/

typedef struct node {
    int value;
    int flow;
    int rGraphFlow;
    int parent;
    int visitedDFS;
    int visitedBFS;
    int primeiroPlano;
    int cenario;
    struct node *next;
    struct node *reverse;
} *Node;

typedef struct graph {
    int V; 
    int pesoFinal; 
    Node *adj;
}* Graph;

typedef struct fila {
    int value;
    struct fila *next;
} *Fila;

typedef struct queue {
    Fila head;
    Fila tail;
} *Queue;

/*------------------------------------------*/

Queue queue;        /*Declaracao global da queue*/

/*------------- INICIALIZACOES -------------*/

Node initNode(int value, int flow, Node next){
    Node node = (Node)malloc(sizeof(struct node));
    
    node->value = value;
    node->flow = flow;
    node->rGraphFlow = flow;
    node->visitedDFS = 0;
    node->visitedBFS = 0;
    node->next = next;

    return node;
}

Graph initGraph(int vertices) {
    Graph graph = (Graph)malloc(sizeof(struct graph));
    
    graph->V = vertices;
    graph->pesoFinal = 0;
    graph->adj = (Node*)malloc(sizeof(Node)*vertices);

    int i;
    for (i = 0; i < vertices; i++)
        graph->adj[i] = (Node)malloc(sizeof(struct node));
  
    return graph;   
}

void initQueue() {
    queue = (Queue)malloc(sizeof(struct queue));
    queue->head = queue->tail = NULL;
}

/*---------------------------------------------*/

Graph addEdge(Graph graph, int origem, int destino, int flow) {

  graph->adj[origem] = initNode(destino, flow, graph->adj[origem]);
  graph->adj[destino] = initNode(origem, flow, graph->adj[destino]);

  graph->adj[origem]->reverse = graph->adj[destino];
  graph->adj[destino]->reverse = graph->adj[origem];

  return graph;
}

/*------------ FUNCOES DA QUEUE ------------*/

int isEmpty() {
   return queue->head == NULL;
}

void insertQueue(int data) {
    Fila n = (Fila)malloc(sizeof(struct fila));
    n->value = data;
    n->next = NULL;

    if (isEmpty())
        queue->head = queue->tail = n;

    else {
        queue->tail->next = n;
        queue->tail = n;
    }
}

int removeQueue() {
    Fila temp = queue->head;
    int data = temp->value;

    if (queue->head->next != NULL)
        queue->head = queue->head->next;
    else
        queue->head = queue->tail = NULL;

    free(temp);
    return data;
}

/*--------Primitivas Funcoes Auxiliares --------*/

void DFS(Graph rGraph, int s); 
int BFS(Graph rGraph, int s, int t, int visited);
void edmondsKarp(Graph g, int s, int t);
void vertixType(Graph graph, int t);



/*-------------------- MAIN --------------------*/

int main() {
    int i,j, count = 1;
    int linhas, colunas, value;
    
    if (!scanf("%d %d\n",&linhas,&colunas))
        return -1;
    
    Graph graph = initGraph(linhas*colunas+2);
    int t = linhas*colunas+1;

    /*Input -> Peso primeiro plano*/
    for (i=0; i<linhas; i++)
        for (j=0; j<colunas; j++) {
            if (!scanf("%d",&value))
                return -1;          
            getPrimeiroPlano(getGraphAdj(graph,count++)) = value;
            getchar();
        }

    getchar();    

    /*Input -> Peso cenario*/
    count = 1;
    for (i=0; i<linhas; i++)
        for (j=0; j<colunas; j++) {
            if (!scanf("%d",&value))
                return -1;
            getCenario(getGraphAdj(graph,count++)) = value;
            getchar();
        }

    vertixType(graph,t);

    /*Input -> Peso ligacoes horizontais*/
    if(colunas != 1) {
        getchar();
        for (i=0; i<linhas; i++)
            for (j=0; j<colunas-1; j++) {
                if (!scanf("%d",&value))
                    return -1;
                addEdge(graph,colunas*i+j+1,colunas*i+j+2,value);
                getchar();
            }
    }

    /*Input -> Peso ligacoes verticais*/
    if(linhas != 1) {
        getchar();
        for (i=0; i<linhas-1; i++)
            for (j=0; j<colunas; j++) {
                if (!scanf("%d",&value))
                    return -1;
                addEdge(graph,colunas*i+j+1,colunas*(i+1)+j+1,value);
                getchar();
            }
    }

    initQueue();
    edmondsKarp(graph, 0, t);

    /*Output*/
    printf("%d\n\n",getPesoFinal(graph));

    for (i=0; i<linhas; i++) {
        for (j=0; j<colunas; j++) {
            if (getVisitedDFS(getGraphAdj(graph,i*colunas+j+1)) == 1)
                printf("C ");
            else
                printf("P ");
        }
        printf("\n");
    }

    return 0;
}

/*-------------------FUNCOES AUXILIARES--------------------*/

/*-------------------- DFS --------------------*/

void DFS(Graph rGraph, int s) {
    getVisitedDFS(getGraphAdj(rGraph,s)) = 1;
    Node n;
   
    for (n = getGraphAdj(rGraph,s); n != NULL; n = getNext(n))
        if (getResidualFlow(n) && !getVisitedDFS(getGraphAdj(rGraph,getNodeValue(n))))
            DFS(rGraph, getNodeValue(n));
}

/*-------------------- BFS --------------------*/

int BFS(Graph rGraph, int s, int t, int visited) {
    Node n;
    int u;
    insertQueue(s);
    getVisitedBFS(getGraphAdj(rGraph,s)) = visited;
    getPixelParent(getGraphAdj(rGraph,s)) = NIL;

    while (!isEmpty()) {  
        u = removeQueue();
   
        for (n = getGraphAdj(rGraph,u); n != NULL; n = getNext(n))
            if (getVisitedBFS(getGraphAdj(rGraph,getNodeValue(n)))== SWITCH(visited) && getResidualFlow(n) > 0) {
                
                insertQueue(getNodeValue(n));
                getPixelParent(getGraphAdj(rGraph,getNodeValue(n))) = u;
                getVisitedBFS(getGraphAdj(rGraph,getNodeValue(n))) = visited;
            }
    }

    return getVisitedBFS(getGraphAdj(rGraph,t)) == visited;
}

/*------------ Algoritmo de Edmonds-Karp ------------*/

void edmondsKarp(Graph g, int s, int t){
    Node n;
    int i,u,v, visited = 1;

    while (BFS(g, s, t, visited)) {

        int path_flow = INT_MAX;
        
        for (v = t; v != s; v = getPixelParent(getGraphAdj(g,v))) {
            u = getPixelParent(getGraphAdj(g,v));
            for (n = getGraphAdj(g,u); getNodeValue(n) != v; n = getNext(n));
            path_flow = MIN(path_flow, getResidualFlow(n));
        }

        for (v = t; v != s; v = getGraphAdj(g,v)->parent) {
            u = getPixelParent(getGraphAdj(g,v));
            for (n = getGraphAdj(g,u); getNodeValue(n) != v; n = getNext(n));
            getResidualFlow(n) -= path_flow;
            getResidualFlow(getReverse(n)) += path_flow;     
        }

        visited = SWITCH(visited);
    }

    DFS(g,s);

    for (i = 0; i < getNumberPixels(g); i++)
        for (n = getGraphAdj(g,i); n != NULL; n = getNext(n))
            if (getVisitedDFS(getGraphAdj(g,i)) && !getVisitedDFS(getGraphAdj(g,getNodeValue(n))))
                getPesoFinal(g) += getFlow(n);
}

/*---------------------------------------------------*/

void vertixType(Graph graph, int t) {
    int i, fluxo;
    int primeiroPlano, cenario;
    for (i = 1; i < getNumberPixels(graph)-1; i++) {
        
        primeiroPlano = getPrimeiroPlano(getGraphAdj(graph,i));
        cenario = getCenario(getGraphAdj(graph,i));

        if (primeiroPlano != 0 && cenario != 0) {
            if (primeiroPlano > cenario) {

                fluxo = primeiroPlano - cenario;
                getPesoFinal(graph) += cenario;
                addEdge(graph,0,i,fluxo);
            }

            else {
                fluxo = cenario - primeiroPlano;
                getPesoFinal(graph) += primeiroPlano;
                addEdge(graph,i,t,fluxo);
            }
        }

        else if (primeiroPlano != 0)
            addEdge(graph, 0, i, primeiroPlano);

        else if (cenario != 0)
            addEdge(graph, i, t, cenario);
    }
}

/*----------------------------------------------*/
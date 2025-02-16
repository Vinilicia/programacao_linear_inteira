#include <bits/stdc++.h>
#include <ilcplex/ilocplex.h>

using namespace std;
ILOSTLBEGIN

#define CPLEX_TIME_LIM 3600  // Limite de tempo para o CPLEX (1 hora)

// Variáveis globais para o problema de fluxo com custo mínimo
int N; // Número de nós na rede
int E; // Número de arcos (arestas direcionadas)

// Vetor de balanço para cada nó: b[i] representa a oferta (se positiva) ou a demanda (se negativa)
// A soma de todos os b[i] deve ser zero (consumidores e produtores se equilibram)
vector<int> b;

// Estrutura para representar um arco da rede
struct Arc {
    int u;     // Nó de origem
    int v;     // Nó de destino
    int cap;   // Capacidade do arco
    int cost;  // Custo por unidade de fluxo
};

vector<Arc> arcs;

void cplex_min_cost_flow() {
    IloEnv env; // Ambiente do CPLEX

    int numberVar = 0; // Número total de variáveis
    int numberRes = 0; // Número total de restrições

    // Variáveis de decisão: fluxo em cada arco
    // f[i] representa o fluxo enviado pelo arco i (valor inteiro entre 0 e infinito)
    IloNumVarArray f(env, E, 0, IloInfinity, ILOINT);
    numberVar = E;

    // Criação do modelo
    IloModel model(env);
    IloExpr obj(env);

    // FUNÇÃO OBJETIVO: Minimizar o custo total do fluxo
    // Soma de (custo do arco) * (fluxo pelo arco) para cada arco
    for (int i = 0; i < E; i++) {
        obj += arcs[i].cost * f[i];
    }
    model.add(IloMinimize(env, obj));

    // RESTRIÇÕES DE CONSERVAÇÃO DE FLUXO para cada nó
    // Para cada nó i: (soma dos fluxos que saem de i) - (soma dos fluxos que entram em i) = b[i]
    // Se b[i] > 0, o nó é produtor; se b[i] < 0, o nó é consumidor
    for (int i = 0; i < N; i++) {
        IloExpr flowBalance(env);
        for (int j = 0; j < E; j++) {
            if (arcs[j].u == i) {
                flowBalance += f[j];  // fluxo saindo do nó i
            }
            if (arcs[j].v == i) {
                flowBalance -= f[j];  // fluxo entrando no nó i
            }
        }
        model.add(flowBalance == b[i]);
        flowBalance.end();
        numberRes++;
    }

    // RESTRIÇÕES DE CAPACIDADE para cada arco
    // Para cada arco i, o fluxo deve ser menor ou igual à sua capacidade
    for (int i = 0; i < E; i++) {
        model.add(f[i] <= arcs[i].cap);
        numberRes++;
    }

    // Informações de execução e criação do solver CPLEX
    IloCplex cplex(model);
    cplex.setParam(IloCplex::TiLim, CPLEX_TIME_LIM);

    // Solução do modelo
    if (cplex.solve()) {
        cout << "Status: " << cplex.getStatus() << "\n";
        cout << "Custo minimo: " << cplex.getObjValue() << "\n";
        cout << "Fluxo nos arcos:" << "\n";
        for (int i = 0; i < E; i++) {
            cout << "Arco de " << arcs[i].u << " para " << arcs[i].v
                 << " | Fluxo = " << IloRound(cplex.getValue(f[i])) << "\n";
        }
    } else {
        cout << "Nenhuma solucao encontrada." << "\n";
    }

    // Liberação dos recursos
    cplex.end();
    obj.end();
    env.end();
}

int main() {
    // Leitura dos dados de entrada:
    // Primeiro, lê o número de nós e o número de arcos
    cin >> N >> E;

    // Leitura do vetor de balanço b para cada nó
    // b[i] > 0 indica oferta (produção) e b[i] < 0 indica demanda (consumo)
    b.resize(N);
    for (int i = 0; i < N; i++) {
        cin >> b[i];
    }

    // Leitura dos dados de cada arco:
    // Para cada arco, lemos: nó de origem, nó de destino, capacidade e custo por unidade de fluxo
    arcs.resize(E);
    for (int i = 0; i < E; i++) {
        cin >> arcs[i].u >> arcs[i].v >> arcs[i].cap >> arcs[i].cost;
    }

    // (Opcional) Exibir os dados lidos para conferência
    cout << "Numero de nos: " << N << "\nNumero de arcos: " << E << "\n";
    cout << "Balanço dos nos: ";
    for (int i = 0; i < N; i++) {
        cout << b[i] << " ";
    }
    cout << "\nDados dos arcos (u, v, cap, cost):\n";
    for (int i = 0; i < E; i++) {
        cout << arcs[i].u << " " << arcs[i].v << " "
             << arcs[i].cap << " " << arcs[i].cost << "\n";
    }

    // Chamada da função que resolve o problema de fluxo com custo mínimo
    cplex_min_cost_flow();

    return 0;
}

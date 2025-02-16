#include <bits/stdc++.h>
#include <ilcplex/ilocplex.h>

using namespace std;
ILOSTLBEGIN

#define CPLEX_TIME_LIM 3600  // Limite de tempo para o CPLEX (1 hora)

int N, E;  // Número de nós e arcos
int source, sink;  // Origem e destino do fluxo máximo

struct Arc {
    int u, v, cap;  // Origem, destino e capacidade do arco
};

vector<Arc> arcs;

void cplex_max_flow() {
    IloEnv env; // Ambiente do CPLEX

    // Variáveis de decisão: fluxo em cada arco
    IloNumVarArray f(env, E, 0, IloInfinity, ILOINT);

    // Criando o modelo
    IloModel model(env);
    IloExpr obj(env);

    // FUNÇÃO OBJETIVO: Maximizar o fluxo total enviado da origem (source)
    for (int i = 0; i < E; i++) {
        if (arcs[i].u == source) {
            obj += f[i];  // Maximizamos o fluxo saindo da origem
        }
    }
    model.add(IloMaximize(env, obj));

    // RESTRIÇÕES DE CONSERVAÇÃO DE FLUXO para cada nó (exceto source e sink)
    for (int i = 0; i < N; i++) {
        if (i == source || i == sink) continue; // Origem e destino não precisam dessa restrição

        IloExpr flowBalance(env);
        for (int j = 0; j < E; j++) {
            if (arcs[j].u == i) {
                flowBalance += f[j];  // Fluxo saindo do nó i
            }
            if (arcs[j].v == i) {
                flowBalance -= f[j];  // Fluxo entrando no nó i
            }
        }
        model.add(flowBalance == 0); // O fluxo que entra deve ser igual ao que sai
        flowBalance.end();
    }

    // RESTRIÇÕES DE CAPACIDADE: fluxo em cada arco ≤ capacidade do arco
    for (int i = 0; i < E; i++) {
        model.add(f[i] <= arcs[i].cap);
    }

    // Criar solver CPLEX
    IloCplex cplex(model);
    cplex.setParam(IloCplex::TiLim, CPLEX_TIME_LIM);

    // Resolver o modelo
    if (cplex.solve()) {
        cout << "Status: " << cplex.getStatus() << "\n";
        cout << "Fluxo máximo: " << cplex.getObjValue() << "\n";
        cout << "Fluxo nos arcos:\n";
        for (int i = 0; i < E; i++) {
            cout << "Arco de " << arcs[i].u << " para " << arcs[i].v
                 << " | Fluxo = " << IloRound(cplex.getValue(f[i])) << "\n";
        }
    } else {
        cout << "Nenhuma solução encontrada.\n";
    }

    // Liberar recursos
    cplex.end();
    obj.end();
    env.end();
}

int main() {
    // Leitura dos dados de entrada
    cin >> N >> E;
    cin >> source >> sink;  // Lemos a origem e o destino do fluxo

    arcs.resize(E);
    for (int i = 0; i < E; i++) {
        int u, v, cap;
        cin >> u >> v >> cap;
        arcs[i] = {u, v, cap};
    }

    // Exibir os dados lidos
    cout << "Numero de nos: " << N << "\nNumero de arcos: " << E << "\n";
    cout << "Origem: " << source << ", Destino: " << sink << "\n";
    cout << "Arcos (u, v, capacidade):\n";
    for (int i = 0; i < E; i++) {
        cout << arcs[i].u << " " << arcs[i].v << " " << arcs[i].cap << "\n";
    }

    // Chamada da função que resolve o problema de fluxo máximo
    cplex_max_flow();

    return 0;
}

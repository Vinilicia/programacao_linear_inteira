#include <bits/stdc++.h>
#include <ilcplex/ilocplex.h>

using namespace std;
ILOSTLBEGIN

#define CPLEX_TIME_LIM 3600  // Limite de tempo para o CPLEX (1 hora)

// -------------------------------------------------------------
// Variáveis globais
// -------------------------------------------------------------
int N;
int E;

// b[i] > 0 => nó i oferece fluxo (oferta)
// b[i] < 0 => nó i consome fluxo (demanda)
vector<int> b;

struct Arc {
    int u;     // Nó de origem
    int v;     // Nó de destino
    int cap;   // Capacidade do arco
    int cost;  // Custo por unidade de fluxo
};

vector<Arc> arcs;


void cplex() {
    IloEnv env;

    int numberVar = 0;
    int numberRes = 0;

    cout << "Memory usage at start: "
         << env.getMemoryUsage() / (1024. * 1024.) << " MB" << endl;

    // x[i] = fluxo no arco i
    IloNumVarArray x(env, E, 0, IloInfinity, ILOINT); 
    numberVar = E;

    IloModel model(env);
    IloExpr sum(env);

    // FUNÇÃO OBJETIVO: Minimize( soma(cost[i] * x[i]) )
    for (int i = 0; i < E; i++) {
        sum += arcs[i].cost * x[i];
    }
    model.add(IloMinimize(env, sum));


    // RESTRIÇÂO Conservação de fluxo: (Fluxo que sai de i) - (Fluxo que entra em i) <= b[i]
    for (int i = 0; i < N; i++) {
        sum.clear();
        for (int j = 0; j < E; j++) {
            if (arcs[j].u == i) {
                sum += x[j];  // fluxo saindo em i
            }
            if (arcs[j].v == i) {
                sum -= x[j];  // fluxo entrando em i
            }
        }
        model.add(sum <= b[i]);

        numberRes++;
    }


    // RESTRIÇÂO Capacidade dos Arcos: x[i] <= cap[i]
    for (int i = 0; i < E; i++) {
        model.add(x[i] <= arcs[i].cap);
        numberRes++;
    }

    cout << "Memory usage after variable creation: "
         << env.getMemoryUsage() / (1024. * 1024.) << " MB" << endl;


    IloCplex cplex(model);
    cout << "Memory usage after cplex(Model): "
         << env.getMemoryUsage() / (1024. * 1024.) << " MB" << endl;

    cplex.setParam(IloCplex::TiLim, CPLEX_TIME_LIM);

    time_t timer, timer2;
    time(&timer);
    cplex.solve();
    time(&timer2);

    bool sol = true;
    string status;
    switch (cplex.getStatus()) {
        case IloAlgorithm::Optimal:
            status = "Optimal";
            break;
        case IloAlgorithm::Feasible:
            status = "Feasible";
            break;
        default:
            status = "No Solution";
            sol = false;
    }

	cout << endl << endl;
    cout << "Status da FO: " << status << endl;

    if (sol) {
        double runTime = difftime(timer2, timer);
        IloNum objValue = cplex.getObjValue();

        cout << "Funcao Objetivo Valor = " << objValue << endl;
        printf("Tempo de execucao: %.6lf seconds\n\n", runTime);

        cout << "Fluxo nos arcos:\n";
        for (int i = 0; i < E; i++) {
            IloNum value = IloRound(cplex.getValue(x[i]));
            cout << "Arco de " << arcs[i].u << " para " << arcs[i].v
                 << " | Fluxo = " << value << "\n";
        }
    } else {
        cout << "No Solution!\n";
    }

    cplex.end();
    sum.end();

    cout << "Memory usage before end: "
         << env.getMemoryUsage() / (1024. * 1024.) << " MB" << endl;

    env.end();
}



int main() {
    cin >> N >> E;
    b.resize(N);
    for (int i = 0; i < N; i++) {
        cin >> b[i];
    }

    arcs.resize(E);
    for (int i = 0; i < E; i++) {
        cin >> arcs[i].u >> arcs[i].v >> arcs[i].cap >> arcs[i].cost;
    }

	cout << "Verificacao da leitura dos dados:" << endl;
    cout << "Numero de nos: " << N << "\nNumero de arcos: " << E << "\n";
    cout << "Balanço dos nos (b[i]): ";
    for (int i = 0; i < N; i++) {
        cout << b[i] << " ";
    }
    cout << "\n\nArcos (u, v, cap, cost):\n";
    for (int i = 0; i < E; i++) {
        cout << arcs[i].u << " " << arcs[i].v << " "
             << arcs[i].cap << " " << arcs[i].cost << "\n";
    }
    cout << endl;

    cplex();

    return 0;
}

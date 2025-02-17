#include <bits/stdc++.h>
#include <ilcplex/ilocplex.h>

using namespace std;
ILOSTLBEGIN

#define CPLEX_TIME_LIM 3600


int N, E;       // Número de nós e de arcos
int origem, destino;  // Origem e destino do fluxo máximo

struct Arc {
    int u, v, cap;  // Nó de origem, nó de destino e capacidade do arco
};

vector<Arc> arcs;

void cplex_max_flow() {
    IloEnv env;
    
    int numberVar = 0;
    int numberRes = 0;

    cout << "Memory usage at start: " 
         << env.getMemoryUsage() / (1024. * 1024.) << " MB" << endl;

    // Criação das variáveis de decisão: x[i] representa o fluxo no arco i
    IloNumVarArray x(env, E, 0, IloInfinity, ILOINT);
    numberVar = E;

    cout << "Memory usage after variable creation: " 
         << env.getMemoryUsage() / (1024. * 1024.) << " MB" << endl;

    IloModel model(env);
    IloExpr sum(env);

    // FUNÇÃO OBJETIVO: Maximizar o fluxo total enviado da origem
    for (int i = 0; i < E; i++) {
        if (arcs[i].u == origem) {
            sum += x[i];
        }
    }
    model.add(IloMaximize(env, sum));

    // RESTRIÇÕES DE CONSERVAÇÃO DE FLUXO para cada nó (exceto origem e destino)
    for (int i = 0; i < N; i++) {
        if (i == origem || i == destino)
            continue;

		sum.clear();
        for (int j = 0; j < E; j++) {
            if (arcs[j].u == i) {
                sum += x[j];  // fluxo saindo do nó i
            }
            if (arcs[j].v == i) {
                sum -= x[j];  // fluxo entrando no nó i
            }
        }
        model.add(sum == 0);
        numberRes++;
    }

    // RESTRIÇÕES DE CAPACIDADE para cada arco: x[i] <= cap do arco
    for (int i = 0; i < E; i++) {
        model.add(x[i] <= arcs[i].cap);
        numberRes++;
    }

    cout << "Memory usage after cplex(Model): " 
         << env.getMemoryUsage() / (1024. * 1024.) << " MB" << endl;


    IloCplex cplex(model);
    cplex.setParam(IloCplex::TiLim, CPLEX_TIME_LIM);

    time_t timer, timer2;
    time(&timer);
    cplex.solve();
    time(&timer2);

    bool sol = true;
    string status;
    switch(cplex.getStatus()){
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
    cout << "Status da FO: " << status << "\n";

    if(sol){
        double runTime = difftime(timer2, timer);
        IloNum objValue = cplex.getObjValue();
        cout << "Fluxo maximo: " << objValue << "\n";
        printf("Tempo de execucao: %.6lf seconds\n\n", runTime);

        cout << "Fluxo nos arcos:\n";
        for (int i = 0; i < E; i++) {
            IloNum value = IloRound(cplex.getValue(x[i]));
            cout << "Arco de " << arcs[i].u << " para " << arcs[i].v
                 << " | Fluxo = " << value << "\n";
        }
    } else {
        cout << "Nenhuma solucao encontrada.\n";
    }

    cplex.end();
    sum.end();

    cout << "Memory usage before end: " 
         << env.getMemoryUsage() / (1024. * 1024.) << " MB" << endl;
    env.end();
}

int main() {

    cin >> N >> E;
    cin >> origem >> destino; 

    arcs.resize(E);
    for (int i = 0; i < E; i++) {
        int u, v, cap;
        cin >> u >> v >> cap;
        arcs[i] = {u, v, cap};
    }

    cout << "Verificacao da leitura dos dados:" << endl;
    cout << "Numero de nos: " << N << "\nNumero de arcos: " << E << "\n";
    cout << "Origem: " << origem << ", Destino: " << destino << "\n";
    cout << "Arcos (u, v, capacidade):\n";
    for (int i = 0; i < E; i++) {
        cout << arcs[i].u << " " << arcs[i].v << " " << arcs[i].cap << "\n";
    }
    cout << endl;

    cplex_max_flow();

    return 0;
}

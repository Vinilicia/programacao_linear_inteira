#include <bits/stdc++.h>
#include <ilcplex/ilocplex.h>

using namespace std;
ILOSTLBEGIN

#define CPLEX_TIME_LIM 3600

int A; // Número de Arestas
int V; // Número de Vértices
int s; // Vértice de origem
int d; // Vértice de destino
vector<vector<int>> c;

const int INF = 1e6;

void cplex(){
    IloEnv env;

    //Variaveis --------------------------------------------- 
	int numberVar = 0; //Total de Variaveis
	int numberRes = 0; //Total de Restricoes

    //---------- MODELAGEM ---------------
	IloArray<IloNumVarArray> x(env);
	for(int i = 0; i < V; i++){
		x.add(IloNumVarArray(env));
		for(int j = 0; j < V; j++){
			x[i].add(IloIntVar(env, 0, 1));
			numberVar++;
		}
	}

    // Proíbe o uso de arestas com valores infinitos
    for (int i = 0; i < V; i++) {
        for (int j = 0; j < V; j++) {
            if (c[i][j] == INF || i == j) {
                x[i][j].setBounds(0, 0);
            }
        }
    }


    //Definicao do ambiente modelo ------------------------------------------
    IloModel model(env);
    IloExpr sum(env);
    IloExpr sum2(env);

    //FUNCAO OBJETIVO ---------------------------------------------
    sum.clear();
    for (int i = 0; i < V; i++) {
        for (int j = 0; j < V; j++) {
            sum += c[i][j] * x[i][j];
        }
    }
    model.add(IloMinimize(env, sum)); //Minimizacao

    //RESTRICOES ---------------------------------------------
    // R1 - Saída do nó s
    sum.clear();
    for (int j = 0; j < V; j++) {
        sum += x[s][j];
    }
    model.add(sum == 1);
    numberRes++;

    // R2 - Chegada no nó d
    sum.clear();
    for (int i = 0; i < V; i++) {
        sum += x[i][d];
    }
    model.add(sum == 1);
    numberRes++;

    // R3 - Conservação de fluxo
    for (int i = 1; i < V-1; i++) {
        sum.clear();
        sum2.clear();
        for (int j = 0; j < V; j++) {
            sum += x[i][j];
            sum2 += x[j][i];
        }
        model.add(sum == sum2);
        numberRes++;
    }

    //------ EXECUCAO do MODELO ----------
	time_t timer, timer2;
	IloNum value, objValue;
	double runTime;
	string status;
	
	//Informacoes ---------------------------------------------	
	printf("--------Informacoes da Execucao:----------\n\n");
	printf("#Var: %d\n", numberVar);
	printf("#Restricoes: %d\n", numberRes);
	cout << "Memory usage after variable creation:  " << env.getMemoryUsage() / (1024. * 1024.) << " MB" << endl;
	
	IloCplex cplex(model);
	cout << "Memory usage after cplex(Model):  " << env.getMemoryUsage() / (1024. * 1024.) << " MB" << endl;

	//Setting CPLEX Parameters
	cplex.setParam(IloCplex::TiLim, CPLEX_TIME_LIM);

	time(&timer);
	cplex.solve();//COMANDO DE EXECUCAO
	time(&timer2);
	
	bool sol = true;

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
	cout << "Status da FO: " << status << endl;

	if(sol){ 
		objValue = cplex.getObjValue();
		runTime = difftime(timer2, timer);
		
        cout << "Variaveis de decisao: " << endl;
        for (int i = 0; i < V; i++) {
            for (int j = 0; j < V; j++) {
                value = IloRound(cplex.getValue(x[i][j]));
                printf("x[%d][%d]: %.0lf\n", i, j, value);
            }
        }
		printf("\n");
		
		cout << "Funcao Objetivo Valor = " << objValue << endl;
		printf("..(%.6lf seconds).\n\n", runTime);

	}else{
		printf("No Solution!\n");
	}

	//Free Memory
	cplex.end();
	sum.end();
    sum2.end();

	cout << "Memory usage before end:  " << env.getMemoryUsage() / (1024. * 1024.) << " MB" << endl;
	env.end();
}

int main(){
    // Leitura dos dados:
    cin >> V >> A;
    s = 0;
    d = V - 1;
    c.resize(V, vector<int>(V));

    // Inicializando a matriz de custos
    for (int i = 0; i < V; i++) {
        for (int j = 0; j < V; j++) {
            if (i == j){
                c[i][j] = 0;
            }
            else{
                c[i][j] = INF;
            }
        }
    }

    for (int i = 0; i < A; i++) {
        int s, d, custo;
        cin >> s >> d >> custo;
        c[s][d] = custo;
        c[d][s] = custo;
    }

    cout << "Verificacao da leitura dos dados:" << endl;
    cout << "Numero de vertices: " << V << endl << "Numero de arestas: " << A << endl;
    cout << "Matriz: " << endl;
    for (int i = 0; i < V; i++){
        for (int j = 0; j < V; j++) {
            cout << c[i][j] << " ";
        }
        cout << endl;
    } 

    cplex();

    return 0;
}
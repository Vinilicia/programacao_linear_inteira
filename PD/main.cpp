#include <bits/stdc++.h>
#include <ilcplex/ilocplex.h>

using namespace std;
ILOSTLBEGIN

#define CPLEX_TIME_LIM 3600

int N; // Pessoas e Tarefas
vector<int> s; // Oferta
vector<int> d; // Demanda
vector<vector<int>> c; // Custo de Designação

void cplex(){
    IloEnv env;

    //Variaveis --------------------------------------------- 
	int numberVar = 0; //Total de Variaveis
	int numberRes = 0; //Total de Restricoes

    //---------- MODELAGEM ---------------
	IloArray<IloNumVarArray> x(env);
	for(int i = 0; i < N; i++){
		x.add(IloNumVarArray(env));
		for(int j = 0; j < N; j++){
			x[i].add(IloIntVar(env, 0, 1));
			numberVar++;
		}
	}

    //Definicao do ambiente modelo ------------------------------------------
    IloModel model(env);
    IloExpr sum(env);

    //FUNCAO OBJETIVO ---------------------------------------------
    sum.clear();
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            sum += c[i][j] * x[i][j];
        }
    }
    model.add(IloMinimize(env, sum)); //Minimizacao

    //RESTRICOES ---------------------------------------------
    // R1 - Tarefa Designada
    for (int j = 0; j < N; j++) {
        sum.clear();
        for (int i = 0; i < N; i++) {
            sum += x[i][j];
        }
        model.add(sum == 1);
        numberRes++;
    }

    // R2 - Pessoa Designada
    for (int i = 0; i < N; i++) {
        sum.clear();
        for (int j = 0; j < N; j++) {
            sum += x[i][j];
        }
        model.add(sum == 1);
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
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
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

	cout << "Memory usage before end:  " << env.getMemoryUsage() / (1024. * 1024.) << " MB" << endl;
	env.end();
}

int main(){
    // Leitura dos dados:
    cin >> N;
    c.resize(N, vector<int>(N));
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            cin >> c[i][j];
        }
    }
    cout << "Verificacao da leitura dos dados:" << endl;
    cout << "Pessoas: " << N << "\nTarefas: " << N << "\n";
    cout << "Matriz de custos:" << endl;
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            cout << c[i][j] << " ";
        }
        cout << endl;
    }

    cplex();

    return 0;
}
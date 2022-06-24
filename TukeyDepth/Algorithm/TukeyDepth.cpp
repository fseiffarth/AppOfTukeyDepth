//
// Created by florian on 13.05.22.
//

#include "TukeyDepth.h"

#include "ortools/linear_solver/linear_solver.h"
#include "DataClasses.h"
#include "GraphFunctions.h"
#include <io.h>
#include <StaticFunctions.h>


using namespace operations_research;
    void TukeyDepth::run(int id, const GraphStruct& graph, std::vector<int>& depths, int geodesicDistance) {
        if (geodesicDistance == -1){
            geodesicDistance = graph.nodes();
        }
        depths.clear();
        for (int i = 0; i < graph.nodes(); ++i) {
            std::unique_ptr<MPSolver> solver(MPSolver::CreateSolver("SCIP"));
            if (!solver) {
                //LOG(WARNING) << "SCIP solver unavailable.";
                return;
            }

            for (int j = 0; j < graph.nodes(); ++j) {
                if (i == j) {
                    solver->MakeIntVar(1.0, 1.0, "x" + std::to_string(j));
                }
                else{
                    solver->MakeIntVar(0.0, 1.0, "x" + std::to_string(j));
                }
            }
            //LOG(INFO) << "Number of variables = " << solver->NumVariables();

            const double infinity = solver->infinity();

            std::vector<std::vector<int>> distance_matrix = std::vector<std::vector<int>>(graph.nodes(),std::vector<int>(graph.nodes(), 0));

            for (int j = 0; j < graph.nodes(); ++j) {
                GraphFunctions::BFSDistances(graph, j, distance_matrix[j]);
            }

            for (int u = 0; u < graph.nodes(); ++u) {
                for (int w = u + 1; w < graph.nodes(); ++w) {
                    if (distance_matrix[u][w] <= geodesicDistance) {
                        for (int s = 0; s < graph.nodes(); ++s) {
                            if (s != u && s != w) {
                                if (distance_matrix[u][s] + distance_matrix[s][w] == distance_matrix[u][w]) {
                                    // 0 <= x_u + x_w - x_s => if u and w are closed then also s
                                    auto *const c = solver->MakeRowConstraint(0.0, infinity);
                                    c->SetCoefficient(solver->variable(u), 1);
                                    c->SetCoefficient(solver->variable(w), 1);
                                    c->SetCoefficient(solver->variable(s), -1);
                                }
                            }
                        }
                    }
                }
            }
            //LOG(INFO) << "Number of constraints = " << solver->NumConstraints();

            //Objective
            MPObjective *const objective = solver->MutableObjective();
            for (auto const variable: solver->variables()) {
                objective->SetCoefficient(variable, 1);
            }
            objective->SetMinimization();






            const MPSolver::ResultStatus result_status = solver->Solve();
            // Check that the problem has an optimal solution.
            if (result_status != MPSolver::OPTIMAL) {
                //LOG(FATAL) << "The problem does not have an optimal solution!";
            }

            //LOG(INFO) << "Solution:";
            //LOG(INFO) << "Optimal objective value = " << objective->Value();
            for (auto *const var : solver->variables()) {
                //LOG(INFO) << var->name() << " = " << var->solution_value();
            }
            depths.emplace_back(objective->Value());
        }
        for (auto x: depths) {
            std::cout << x << " ";
        }
        std::cout << std::endl;
    }

void TukeyDepth::run_parallel(int id, const GraphStruct &graph, std::vector<int> &depths, int geodesicDistance) {
    if (geodesicDistance == -1){
        geodesicDistance = graph.nodes();
    }
    depths.clear();
    depths.resize(graph.nodes(),0);
#pragma omp parallel for firstprivate(geodesicDistance) shared(graph,depths) default(none)
    for (int i = 0; i < graph.nodes(); ++i) {
        std::unique_ptr<MPSolver> solver(MPSolver::CreateSolver("SCIP"));
        if (!solver) {
            //LOG(WARNING) << "SCIP solver unavailable.";
        } else {
            for (int j = 0; j < graph.nodes(); ++j) {
                if (i == j) {
                    solver->MakeIntVar(1.0, 1.0, "x" + std::to_string(j));
                } else {
                    solver->MakeIntVar(0.0, 1.0, "x" + std::to_string(j));
                }
            }
            //LOG(INFO) << "Number of variables = " << solver->NumVariables();

            const double infinity = solver->infinity();

            std::vector<std::vector<int>> distance_matrix = std::vector<std::vector<int>>(graph.nodes(),std::vector<int>(graph.nodes(), 0));

            for (int j = 0; j < graph.nodes(); ++j) {
                GraphFunctions::BFSDistances(graph, j, distance_matrix[j]);
            }

            for (int u = 0; u < graph.nodes(); ++u) {
                for (int w = u + 1; w < graph.nodes(); ++w) {
                    if (distance_matrix[u][w] <= geodesicDistance) {
                        for (int s = 0; s < graph.nodes(); ++s) {
                            if (s != u && s != w) {
                                if (distance_matrix[u][s] + distance_matrix[s][w] == distance_matrix[u][w]) {
                                    // 0 <= x_u + x_w - x_s => if u and w are closed then also s
                                    auto *const c = solver->MakeRowConstraint(0.0, infinity);
                                    c->SetCoefficient(solver->variable(u), 1);
                                    c->SetCoefficient(solver->variable(w), 1);
                                    c->SetCoefficient(solver->variable(s), -1);
                                }
                            }
                        }
                    }
                }
            }
            //LOG(INFO) << "Number of constraints = " << solver->NumConstraints();

            //Objective
            MPObjective *const objective = solver->MutableObjective();
            for (auto const variable: solver->variables()) {
                objective->SetCoefficient(variable, 1);
            }
            objective->SetMinimization();

            const MPSolver::ResultStatus result_status = solver->Solve();
            // Check that the problem has an optimal solution.
            if (result_status != MPSolver::OPTIMAL) {
                //LOG(FATAL) << "The problem does not have an optimal solution!";
            }

            //LOG(INFO) << "Solution:";
            //LOG(INFO) << "Optimal objective value = " << objective->Value();
            for (auto *const var: solver->variables()) {
                //LOG(INFO) << var->name() << " = " << var->solution_value();
            }
            depths[i] = (int) objective->Value();
        }
        std::stringstream ss;
        for (auto x: depths) {
            ss << x << " ";
        }
        ss << std::endl;
        //PrintStream(ss);
    }
}

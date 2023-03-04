#include <iostream>
#include <algorithm>
#include <vector>
#include <random>
#include <time.h>
#include <set>
#include <chrono>
#include <bits/stdc++.h>

using namespace std;

const int POPULATION_SIZE = 100;
const int NUM_GENERATIONS = 100;
const double MUTATION_RATE = 0.01;

const int tau = 99;
const int t = 5;


vector <vector<int>> calc_breaks_and_fit(vector < int >& m1, vector < int >& m2, vector<vector<int>> pop, vector<int>& fit) {
    fit.clear();
    int population_size = pop.size();
    int m_1_ending = 0;
    vector <int> m_2_ening;
    vector <vector<int>> breaks;   //pozycje przerw dla kazdego osobnika
    vector <int> breaks_ending_times;
    breaks_ending_times.push_back(0);
    for (int i = 0; i < population_size; i++) {
        vector <int> tmp = pop[i];
        vector<int> tmp_breaks;
        int fitt = 0;
        for (int j = 0; j < tmp.size(); j++) {
            int job = tmp[j];
            m_1_ending += m1[job];
            if (j == 0 && m_1_ending + m2[job] > tau) {                                          //przerwa na start
                tmp_breaks.push_back(0);
                breaks_ending_times.push_back(m_1_ending + t);
                m_2_ening.push_back(m_1_ending + t);
                fitt += m_2_ening.back();
            }
            else if (j == 0 && m_1_ending + m2[job] <= tau) {                                   // pierwsze zadanie ale nie przerwa
                m_2_ening.push_back(m_1_ending + m2[job]);
                fitt += m_2_ening.back();
                if (m_1_ending + m2[job] == tau) {
                    breaks_ending_times.push_back(m_2_ening.back() + t);
                    tmp_breaks.push_back(1);
                    m_2_ening.push_back(m_2_ening.back() + t);
                }
            }
            else {
                int dif = 0;
                if (m_1_ending > m_2_ening.back()) {
                    if (m_2_ening.back() - m_1_ending <= 0) {
                        dif = (m_1_ending - m_2_ening.back());
                    }
                    else {
                        dif = (m_2_ening.back() - m_1_ending);
                    }
                }
                //breaks_ending_times.back() gdy zerowe, wypierdala siê
                //przyporz¹dkowania niepierwsze i kiedy ostatnia jest przerwa
                if (m_2_ening.back() == breaks_ending_times.back()) {
                    m_2_ening.push_back(m_2_ening.back() + m2[job] + dif);
                    fitt += m_2_ening.back();
                    if (m2[job] == tau) {
                        breaks_ending_times.push_back(m_2_ening.back() + t + dif);
                        tmp_breaks.push_back(j + 1);
                        m_2_ening.push_back(m_2_ening.back() + t + dif);
                    }
                }
                else {                                                                                       //przyporzadkowanie niepierwsze kiedy ostatnia nie jest przerwa
                    if (m2[job] + dif < breaks_ending_times.back() + tau - m_2_ening.back()) {        //wiecej miejsca na zadanie
                        m_2_ening.push_back(m_2_ening.back() + m2[job] + dif);
                        fitt += m_2_ening.back();
                    }
                    else if (m2[job] + dif == breaks_ending_times.back() + tau - m_2_ening.back()) {  //idealnie tyle miejsca na zadanie
                        m_2_ening.push_back(m_2_ening.back() + m2[job] + dif);
                        fitt += m_2_ening.back();
                        breaks_ending_times.push_back(m_2_ening.back() + t);
                        tmp_breaks.push_back(j + 1);
                        m_2_ening.push_back(m_2_ening.back() + t);
                    }
                    else {                                                                  //mniej miejsca na zadanie
                        breaks_ending_times.push_back(m_2_ening.back() + t);
                        m_2_ening.push_back(m_2_ening.back() + t);
                        tmp_breaks.push_back(j + 1);
                        m_2_ening.push_back(m_2_ening.back() + m2[job] + dif);
                        fitt += m_2_ening.back();
                    }
                }
            }
        }
        breaks.push_back(tmp_breaks);
        fit.push_back(fitt);
    }
    return breaks;
}

// function to generate a random schedule
vector < int > generateRandomSchedule(int num_jobs) {
    vector < int > schedule(num_jobs);
    for (int i = 0; i < num_jobs; i++) {
        schedule[i] = i;
    }
    shuffle(schedule.begin(), schedule.end(), std::default_random_engine(time(0)));
    return schedule;
}

// function to mutate a schedule
vector < int > mutateSchedule(vector < int >& schedule) {
    int num_jobs = schedule.size();
    int job1 = rand() % num_jobs;
    int job2 = rand() % num_jobs;
    while (job1 == job2) {
        job2 = rand() % num_jobs;
    }
    swap(schedule[job1], schedule[job2]);
    return schedule;
}

// function to select a parent based on fitness
int selectParent(vector < vector < int >>& population, vector < int >& fitness_values) {
    int population_size = population.size();
    vector < double > cumulative_fitness(population_size);
    double total_fitness = 0;

    for (int i = 0; i < population_size; i++) {
        total_fitness += fitness_values[i];
        cumulative_fitness[i] = total_fitness;
    }

    double rand_val = (rand() / (double)RAND_MAX) * total_fitness;
    for (int i = 0; i < population_size; i++) {
        if (rand_val < cumulative_fitness[i]) {
            return i;
        }
    }
    return population_size - 1;

}

// function to perform crossover between two parents
vector < int > crossover(vector < int >& parent1, vector < int >& parent2) {
    int num_jobs = parent1.size();
    int crossover_point = rand() % num_jobs; //od 0 do 11
    if (crossover_point == 0){
        crossover_point = 1;
    }

    unordered_set<int> child;
    for (int i = 0; i < crossover_point; i++) {
        child.insert(parent1[i]);
    }
    for (int i = crossover_point; i < num_jobs; i++) {
        child.insert(parent2[i]);
    }
    int miss = 0;
    while(child.size() !=  parent1.size() ){
        child.insert(miss);
        miss += 1;
    }
    vector<int> vc(child.begin(), child.end());
    return vc;

}

// function to generate a new population
vector < vector < int >> generateNewPopulation(vector < vector < int >>& population, vector < int >& m1, vector < int >& m2, vector<int>& fitness) {
    int population_size = population.size();
    vector < vector < int >> new_population;
    // calculate fitness of each schedule in the current population
    vector<vector<int>> breakss = calc_breaks_and_fit(m1, m2, population, fitness);

    // create a new population by selecting parents based on fitness
    for (int i = 0; i < population_size; i++) {
        int parent1 = selectParent(population, fitness);
        int parent2 = selectParent(population, fitness);

        // perform crossover to generate a child
        vector < int > child = crossover(population[parent1], population[parent2]);

        // perform mutation on the child
        if (rand() / (double)RAND_MAX < MUTATION_RATE) {
            child = mutateSchedule(child);
        }

        new_population.push_back(child);
    }

    return new_population;

}

//function to calculate breaks on m2:


int main() {
    // initialize jobs with processing times, breaks, and tau

    vector <int> best_solution;
    vector <int> best_solution_breaks;
    int fitness_solution = 0;
    vector<int> machine_1 = { 10,19,40,35,80, 60, 40, 22,44,59, 88, 55};
    vector<int> machine_2 = { 77,18,49,50,79, 25, 33, 21,39,77, 88, 59};
    auto start = chrono::steady_clock::now();
    // generate initial population
    vector < vector < int >> population;
    for (int i = 0; i < POPULATION_SIZE; i++) {
        population.push_back(generateRandomSchedule(machine_1.size()));
    }
    vector <int> fitness;
    std::vector<int> allFitness;;
    // perform genetic algorithm
    for (int i = 0; i < NUM_GENERATIONS; i++) {

        population = generateNewPopulation(population, machine_1, machine_2, fitness);
        // find the best schedule in the current population
        int best_fitness = *min_element(fitness.begin(), fitness.end());
        int best_schedule = 0;
        for (int j = 0; j < POPULATION_SIZE; j++) {
            int fitnes = fitness[j];
            if (fitnes < best_fitness) {
                best_fitness = fitnes;
                best_schedule = j;
            }
        }
        vector<vector<int>> breaks_on_m2 = calc_breaks_and_fit(machine_1, machine_2, population, fitness);

        cout << "Generation " << i << ": Best schedule has fitness of " << best_fitness << endl;
        allFitness.push_back(best_fitness);
        // print out the best schedule
        cout << "Best schedule: [";
        for (int i = 0; i < population[best_schedule].size(); i++) {
            cout << population[best_schedule][i] + 1;
            if (i < population[best_schedule].size() - 1) {
                cout << ", ";
            }
        }
        cout << "]" << endl;

        cout << "Best schedule's breaks: [";
        for (int i = 0; i < breaks_on_m2[best_schedule].size(); i++) {
            cout << breaks_on_m2[best_schedule][i] + 1;
            if (i < breaks_on_m2[best_schedule].size() - 1) {
                cout << ", ";
            }
        }
        cout << "]" << endl;
        if(best_fitness < fitness_solution || fitness_solution == 0){
            fitness_solution = best_fitness;
            best_solution = population[best_schedule];
            best_solution_breaks = breaks_on_m2[best_schedule];
        }
    }
    auto end = chrono::steady_clock::now();
    int min_value = *std::min_element(allFitness.begin(), allFitness.end());
    std::cout << "******************************" << std::endl;
    std::cout << "The minimum Cj value is: " << min_value << std::endl;
    cout << "Czas w milisekundach: "
        << chrono::duration_cast<chrono::milliseconds>(end - start).count()
        << " milisec" << "\n";
    cout << "Solution: [";
        for (int i = 0; i < best_solution.size(); i++) {
            cout << best_solution[i] + 1;
            if (i < best_solution.size() - 1) {
                cout << ", ";
            }
        }
        cout << "]" << endl;
    cout << "Solution breaks: [";
        for (int i = 0; i < best_solution_breaks.size(); i++) {
            cout << best_solution_breaks[i] + 1;
            if (i < best_solution_breaks.size() - 1) {
                cout << ", ";
            }
        }
        cout << "]" << endl;


    return 0;
}

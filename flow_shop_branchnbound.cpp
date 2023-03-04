#include <iostream>
#include <vector>
#include <tuple>
#include <map>
#include <algorithm>
#include <numeric>
#include <bits/stdc++.h>
#include <chrono>

using namespace std;

bool Flag = false;
int tau = 10;
int t = 4;
int solution_cj = 0;
vector <tuple<int, int>> solution_jobs;


class Machine{
public:
    vector <int> jobs;
    int jobs_n;
    Machine(vector <int> j);
};

Machine::Machine(vector <int> j){
    jobs = j;
    jobs_n = jobs.size();
}

class Node{
public:
    vector <tuple<int, int>> jobs;
    int Cj;
    vector <tuple<int, int>> adjacjent;
    vector <int> m_1_ending_times;
    map <int, int> m_1_ending_times_dic;
    vector <int> m_2_ending_times;
    vector <int> breaks;
    Node(vector <tuple<int, int>> j, int C, vector <int> m_1_e, map <int, int> m_1_e_d, vector <int> m_2_e, vector<int> br);
    void find_adjacjent(Machine m1, Machine m2, int tau, int t);
};

Node::Node(vector <tuple<int, int>> j, int C, vector <int> m_1_e, map <int, int> m_1_e_d, vector <int> m_2_e, vector<int> br){
  jobs = j;
  Cj = C;
  adjacjent = vector <tuple<int, int>>();
  m_1_ending_times = m_1_e;
  m_1_ending_times_dic = m_1_e_d;
  m_2_ending_times = m_2_e;
  breaks = br;
};

void Node::find_adjacjent(Machine m1, Machine m2, int tau, int t){
    vector <int> m1_used;
    vector <int> m2_used;
    vector<tuple<int, int>>::iterator it;
    for (it = jobs.begin(); it != jobs.end(); ++it){
        if (!(std::count(m1_used.begin(), m1_used.end(), std::get<0>(*it)))){
            if(std::get<0>(*it) != -2)
                m1_used.push_back(get<0>(*it));
        }
        if (!(std::count(m2_used.begin(), m2_used.end(), get<1>(*it))))
            m2_used.push_back(get<1>(*it));
    }

    if(jobs.size() == 0 ){                   //Pierwszy poziom
        for(int i=0; i<m1.jobs_n; i++){
            if(!(m2.jobs[i] + m1.jobs[i] > tau))
                adjacjent.push_back(make_tuple(i, i));
            adjacjent.push_back(make_tuple(i, -1));
        }
    }
    else{
        if(m1_used.size() == m1.jobs_n){     //Jesli maszyna pierwsza jest pe³na
            for(int j = 0 ; j < m2.jobs_n ; j++){
                if (std::find(m2_used.begin(), m2_used.end(),j)!=m2_used.end())
                    continue;
                if(m_2_ending_times.back() + m2.jobs[j] > breaks.back() + tau){
                    if(get<0>(jobs.back()) == -2 && get<1>(jobs.back()) == -1){
                        adjacjent.push_back(make_tuple(-2, j));
                        continue;
                    }
                    adjacjent.push_back(make_tuple(-2, -1));
                    continue;
                }
                else if (m_1_ending_times_dic.find(j) != m_1_ending_times_dic.end() && m_1_ending_times_dic.find(j)->second > m_2_ending_times.back()){
                    if(get<0>(jobs.back()) == -2 && get<1>(jobs.back()) == -1){
                        adjacjent.push_back(make_tuple(-2, j));
                        continue;
                    }
                    adjacjent.push_back(make_tuple(-2, -1));
                    continue;
                }
                adjacjent.push_back(make_tuple(-2, j));
            }
        }
        else{
            for(int j = 0 ; j < m1.jobs_n ; j++){
                if (std::find(m1_used.begin(), m1_used.end(),j)!=m1_used.end())
                    continue;
                if(m_2_ending_times.size() != 0){
                    if(m_2_ending_times.back() == tau && breaks.size() == 0){
                        adjacjent.push_back(make_tuple(j, -1));
                        continue;
                    }
                }
                for(int i = 0 ; i < m2.jobs_n ; i++){
                    if (std::find(m2_used.begin(), m2_used.end(),i)!=m2_used.end())
                        continue;
                    if( i < j && m_1_ending_times_dic.find(i) == m_1_ending_times_dic.end())
                        continue;
                    if( i > j && m_1_ending_times_dic.find(i) == m_1_ending_times_dic.end())
                        continue;
                    if( i == j){
                        if(m_2_ending_times.size() != 0 && m_1_ending_times.size() != 0)
                            if(m_2_ending_times.back() + tau < m_1_ending_times.back() + m1.jobs[j] + m2.jobs[i])
                                continue;
                    }
                    if(m_2_ending_times.size() != 0){
                        if(breaks.size() != 0){
                            if(m_2_ending_times.back() + m2.jobs[i] > breaks.back() + tau){
                                adjacjent.push_back(make_tuple(j, -1));
                                continue;
                            }
                        }
                        else if(m_2_ending_times.back() + m2.jobs[i] > tau){
                            adjacjent.push_back(make_tuple(j, -1));
                            continue;
                        }
                    }
                    adjacjent.push_back(make_tuple(j, i));
                    //adjacjent.push_back(make_tuple(j, -1));
                }
            }
        }
    }
    set<tuple<int, int>> s( adjacjent.begin(), adjacjent.end());
    adjacjent.assign( s.begin(), s.end() );
}

template <typename T>
vector<size_t> sort_indexes(const vector<T> &v) {

  vector<size_t> idx(v.size());
  iota(idx.begin(), idx.end(), 0);

  stable_sort(idx.begin(), idx.end(),
       [&v](size_t i1, size_t i2) {return v[i1] < v[i2];});

  return idx;
}

int Count_bound(tuple<int, int> idx, Node node, Machine m1, Machine m2){
    int lowerbound = node.Cj;
    vector <int> jobs_done;
    vector<tuple<int, int>>::iterator it;
    for (it = node.jobs.begin(); it != node.jobs.end(); ++it)
        if(get<1>(*it) != -1)
            jobs_done.push_back(get<1>(*it));
    vector <int> breaks = node.breaks;
    vector<int> m2_ending = node.m_2_ending_times;
    if(get<1>(idx) != -1){  //aktualnie wprowadzany nie jest przerwa
        jobs_done.push_back(get<1>(idx));
        if(m2_ending.size() != 0)
            lowerbound += m2_ending.back() + m2.jobs[get<1>(idx)];
        else{
            if(node.m_1_ending_times.size() != 0){
                lowerbound += node.m_1_ending_times.back() + m2.jobs[get<1>(idx)];
                m2_ending.push_back(node.m_1_ending_times.back() + m2.jobs[get<1>(idx)]);
            }
            else{
                m2_ending.push_back(0 + m2.jobs[get<1>(idx)]);
                lowerbound += m2.jobs[get<1>(idx)];
            }
        }
    }
    if(get<1>(idx) == -1){    //aktualnie wprowadzany to przerwa
        if(m2_ending.size() != 0){
            /*if(node.m_1_ending_times.back() != m2_ending.back())
                lowerbound += 1;
            else
                lowerbound += 0;*/
            breaks.push_back(m2_ending.back() + t);
            m2_ending.push_back(m2_ending.back() + t);
        }
        else{
            //lowerbound += t;
            if(node.m_1_ending_times.size() != 0){
                breaks.push_back(node.m_1_ending_times.back() + t);
                m2_ending.push_back(node.m_1_ending_times.back() + t);
            }
            else{
                 m2_ending.push_back(m2.jobs[get<1>(idx)]);
                 breaks.push_back(m2.jobs[get<1>(idx)]);
            }
        }
    }
    vector <int> v = m2.jobs;
    //for(int i=0; i<m2.jobs_n; i++){  //dalsze uporzadkowanie
    for(auto i: sort_indexes(v)){
        if (std::find(jobs_done.begin(), jobs_done.end(),i) != jobs_done.end())
            continue;
        if(breaks.size() != 0){
            if(breaks.back() != m2_ending.back()){
                int space = breaks.back() + tau - m2_ending.back();
                if(space >= m2.jobs[i]){
                    jobs_done.push_back(i);
                    m2_ending.push_back(m2_ending.back() + space);
                    lowerbound += m2_ending.back();
                }
                else if(space < m2.jobs[i]){
                    jobs_done.push_back(i);
                    breaks.push_back(m2_ending.back() + space + t);
                    m2_ending.push_back(m2_ending.back() + space + t + m2.jobs[i] - space);
                    lowerbound += m2_ending.back();
                }
                else{
                    breaks.push_back(m2_ending.back() + t);
                    m2_ending.push_back(m2_ending.back() + t);
                }
            }
            else{
                jobs_done.push_back(i);
                m2_ending.push_back(m2_ending.back() + m2.jobs[i]);
                lowerbound += m2_ending.back();
            }
        }
        else{
            int space = tau - m2_ending.back();
            if(space > m2.jobs[i]){
                jobs_done.push_back(i);
                m2_ending.push_back(m2_ending.back() + m2.jobs[i]);
                lowerbound += m2_ending.back();
            }
            else if(space == m2.jobs[i]){
                jobs_done.push_back(i);
                m2_ending.push_back(m2_ending.back() + m2.jobs[i]);
                lowerbound += m2_ending.back();
                breaks.push_back(m2_ending.back() + t);
                m2_ending.push_back(m2_ending.back() + t);
            }
            else if(space < m2.jobs[i]){
                jobs_done.push_back(i);
                breaks.push_back(m2_ending.back() + space + t);
                m2_ending.push_back(m2_ending.back() + space + t + m2.jobs[i] - space);
                lowerbound += m2_ending.back();
            }
            else{
                breaks.push_back(m2_ending.back() + t);
                m2_ending.push_back(m2_ending.back() + t );
                jobs_done.push_back(i);
                m2_ending.push_back(m2_ending.back() + m2.jobs[i] );
                lowerbound += m2_ending.back();
            }
        }
    }
    return lowerbound;
}

void FlowShop(Machine m1, Machine m2, vector <tuple<int, int>> jobs_list, int c, vector <int> m_1_e, map <int, int> m_1_e_d, vector <int> m_2_e, vector<int> br){
    Node node(jobs_list, c, m_1_e, m_1_e_d, m_2_e, br);
    node.find_adjacjent(m1, m2, tau, t);
    vector<tuple<int, int>>::iterator adj;
    /*if ((node.jobs).size() != 0)
        cout << "adj for " << (get<0>((node.jobs).back())) << ", " << (get<1>((node.jobs).back())) <<endl;
    for (adj = node.adjacjent.begin(); adj != node.adjacjent.end(); ++adj){
            cout << get<0>(*adj);
            cout << get<1>(*adj);
            cout<< "\n";
        }
    cout << "\n";*/
    int cj;
    if(node.adjacjent.size() == 0){   //leaf
        Flag = true;
        //cout <<"\nCj liscia: " <<node.Cj <<"\n";
        //return;
        if(solution_cj == 0 || node.Cj < solution_cj){
            solution_cj = node.Cj;
            solution_jobs = node.jobs;
        }
    }
    else{
        vector<tuple<int, int>>::iterator adj;
        for (adj = node.adjacjent.begin(); adj != node.adjacjent.end(); ++adj){
            if(Flag){
                int lowerbound = Count_bound(*adj, node, m1, m2);
                if(lowerbound > solution_cj)
                    continue;
            }
            vector <tuple<int, int>> jobs = node.jobs;
            jobs.push_back(*adj);
            vector <int> ends = node.m_1_ending_times;
            vector <int> ends_2 = node.m_2_ending_times;
            map <int, int> ends_dict(node.m_1_ending_times_dic);
            if(get<0>(*adj) != -2){
                if(ends.size() == 0)
                    ends.push_back(m1.jobs[get<0>(*adj)]);
                else
                    ends.push_back(ends.back() + m1.jobs[get<0>(*adj)]);
                ends_dict.insert({get<0>(*adj), ends.back()});
            }
            vector <int> breaks = node.breaks;
            if(get<0>(*adj) == -2 && get<1>(*adj) != -1){
                cj = node.Cj + m2.jobs[get<1>(*adj)] + ends_2.back();
                ends_2.push_back(ends.back() + m2.jobs[get<1>(*adj)]);
                //cout << "CJ: " << cj <<endl;
            }
            if(get<1>(*adj) == -1){
                if(ends_2.size() != 0){
                    if(ends.back() <= ends_2.back()){
                        breaks.push_back(ends_2.back() + t);
                        ends_2.push_back(ends_2.back() + t);
                    }
                    else{
                        if(ends.back() <= ends_2.back() + t){
                            breaks.push_back(ends.back());
                            ends_2.push_back(ends.back());
                        }
                        else{
                            breaks.push_back(ends_2.back() + t);
                            ends_2.push_back(ends_2.back() + t);
                        }
                    }
                }
                else{
                    if(t < ends.back()){
                        breaks.push_back(ends.back());
                        ends_2.push_back(ends.back());
                    }
                    else{
                        breaks.push_back(t);
                        ends_2.push_back(t);
                    }
                }
                /*if(ends.back() != ends_2.back()){
                    cj = node.Cj + (ends_2.back() - ends.back());
                    cout << "CJ: " << cj <<endl;
                }
                else{
                    cj = node.Cj;
                    cout << "CJ: " << cj <<endl;
                }
                */
                cj = node.Cj;
            }
            if(get<0>(*adj) != -2 && get<1>(*adj) != -1){
                //cj = node.Cj + m2.jobs[get<1>(*adj)] + ends_dict.find(get<1>(*adj))->second;
                if(ends_2.size() == 0){
                    cj = node.Cj + ends.back() + m2.jobs[get<1>(*adj)];
                    ends_2.push_back(ends.back() + m2.jobs[get<1>(*adj)]);
                    //cout << "CJ: " << cj <<endl;
                }
                else{
                    //cout << "SLOWNIK::::: " << ends_dict.find(get<1>(*adj))->second << endl;
                    if((ends_dict.find(get<1>(*adj))->second) > ends_2.back()){
                        cj = node.Cj + ends_dict.find(get<1>(*adj))->second + m2.jobs[get<1>(*adj)];
                        ends_2.push_back(ends_dict.find(get<1>(*adj))->second + m2.jobs[get<1>(*adj)]);
                        //cout << "CJ: " << cj <<endl;
                    }
                    else{
                        cj = node.Cj + ends_2.back() + m2.jobs[get<1>(*adj)];
                        ends_2.push_back(ends_2.back() + m2.jobs[get<1>(*adj)]);
                        //cout << "CJ: " << cj <<endl;
                    }
                }
            }
            FlowShop(m1, m2, jobs, cj, ends, ends_dict, ends_2, breaks);
        }
    }
}


int main()
{
    Machine m1(vector <int>{3,1,3,5,7,10,7,5,1,4,5,4,3});
    Machine m2(vector <int>{2,2,3,3,4,9,10,7,1,5,5,5,3});
    cout << "Rozwiazanie" << "\n";
    auto start = chrono::steady_clock::now();
    FlowShop(m1, m2, vector <tuple<int, int>>{}, 0, vector <int>{}, map <int, int>{}, vector <int>{}, vector<int>{});
    auto end = chrono::steady_clock::now();
    for (int i = 0; i < solution_jobs.size(); i++){
        cout << get<0>(solution_jobs[i]) << " ";
        cout << get<1>(solution_jobs[i]) << " ";
    }
    cout << "\nWynik: " <<solution_cj << "\n";
    cout << "Czas w sekundach: "
        << chrono::duration_cast<chrono::seconds>(end - start).count()
        << " sec";
    return 0;
}





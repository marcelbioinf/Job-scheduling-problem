class Machine:
    def __init__(self, l):
        self.jobs = l
        self.n_jobs = len(l)

class Node:
    def __init__(self, jobs_list, cj, m1_endings, m1_endings_dict, m2_endings, breaks):
        self.jobs = jobs_list
        self.Cj = cj
        self.adjacjent = []
        self.m1_ending_times = m1_endings
        self.m1_ending_times_dic = m1_endings_dict
        self.m2_ending_tmes = m2_endings
        self.break_times = breaks

    def find_adjacjent(self, m1, m2, tau, t):
        m1_used = [i for i, j in self.jobs if i != -2]
        m2_used = [j for i, j in self.jobs]
        if len(self.jobs) == 0:    #1st level
            for i in range(len(m1.jobs)):
                if not m2.jobs[i] + m1.jobs[i] > tau:
                    self.adjacjent.append((i, i))
                self.adjacjent.append((i, -1))
        else:
            if len(m1_used) == m1.n_jobs:   #jesli maszyna 1 pelna
                for j in range(m2.n_jobs):
                    if j in m2_used:
                        continue
                    if self.m2_ending_tmes[-1] + m2.jobs[j] > self.break_times[-1] + tau:
                        self.adjacjent.append((-2, -1))
                        continue
                    elif self.m1_ending_times_dic.get(j) and self.m1_ending_times_dic.get(j) > self.m2_ending_tmes[-1]:
                        self.adjacjent.append((-2, -1))
                        continue
                    self.adjacjent.append((-2, j))
            else:
                for i in range(m1.n_jobs):
                    if i in m1_used:
                        continue
                    if len(self.m2_ending_tmes) != 0:               #####ZMIENIC??
                        if self.m2_ending_tmes[-1] == tau and len(self.break_times) == 0:
                            self.adjacjent.append((i, -1))
                            continue
                    for j in range(m2.n_jobs):
                        if j in m2_used:
                            continue
                        if i < j and not self.m1_ending_times_dic.get(j):
                            continue
                        if i > j and not self.m1_ending_times_dic.get(j):
                            continue
                        if i == j:
                            if len(self.m2_ending_tmes) != 0 and len(self.m1_ending_times) != 0:
                                if self.m2_ending_tmes[-1] + tau < self.m1_ending_times[-1] + m1.jobs[i] + m2.jobs[j]:   #jesli nowowdowawane zadanie na m2 mialoby trwac gdzies gdzie powinna byc przewrqa
                                    continue
                        if len(self.m2_ending_tmes) != 0:
                            if len(self.break_times) != 0:
                                if self.m2_ending_tmes[-1] + m2.jobs[j] > self.break_times[-1] + tau:
                                    self.adjacjent.append((i, -1))
                                    continue
                            elif self.m2_ending_tmes[-1] + m2.jobs[j] > tau:
                                self.adjacjent.append((i, -1))
                                continue
                        if (i, j) not in self.adjacjent:
                            self.adjacjent.append((i, j))
                        if (i, -1) not in self.adjacjent:
                            self.adjacjent.append((i, -1))
        self.adjacjent = list(dict.fromkeys(self.adjacjent))


def count_bound(idx, node):
    lower_bound = node.Cj
    jobs_done = [j for i, j in node.jobs if j != -1]
    breaks = list(node.break_times)
    m2_ending = list(node.m2_ending_tmes)
    if not idx[1] == -1:
        jobs_done.append(idx[1])
        if len(m2_ending) != 0:
            lower_bound += m2_ending[-1] + m2.jobs[idx[1]]
        else:
            if len(node.m1_ending_times) != 0:
                lower_bound += node.m1_ending_times[-1] + m2.jobs[idx[1]]
                m2_ending.append(node.m1_ending_times[-1] + m2.jobs[idx[1]])
            else:
                m2_ending.append(0 + m2.jobs[idx[1]])
                lower_bound += m2.jobs[idx[1]]
    if idx[1] == -1:
        if len(m2_ending) != 0:
            lower_bound += 1 if node.m1_ending_times[-1] != m2_ending[-1] else 0
            breaks.append(m2_ending[-1] + t)
            m2_ending.append(m2_ending[-1] + t)
        else:
            lower_bound += t
            if len(node.m1_ending_times) != 0:
                breaks.append(node.m1_ending_times[-1] + t)
                m2_ending.append(node.m1_ending_times[-1] + t)
            else:
                m2_ending.append(m2.jobs[idx[1]])
                breaks.append(m2.jobs[idx[1]])
    for i in range(m2.n_jobs):
        if not i in jobs_done:
            if len(breaks) != 0:
                if not breaks[-1] == m2_ending[-1]:    #jesli ostatnie zadanie nie jest przerwą a jest jakas przerwa już
                    space = breaks[-1] + tau - m2_ending[-1]
                    if space >= m2.jobs[i]:
                        jobs_done.append(i)
                        m2_ending.append(m2_ending[-1] + space)
                        lower_bound += m2_ending[-1]
                    elif space < m2.jobs[i]:
                        jobs_done.append(i)
                        breaks.append(m2_ending[-1] + space + t)
                        m2_ending.append(m2_ending[-1] + space + t + m2.jobs[i] - space)
                        lower_bound += m2_ending[-1]
                    else:   #space == 0
                        breaks.append(m2_ending[-1] + t)
                        m2_ending.append(m2_ending[-1] + t)
                else:                                #jesli ostatnie zadanie to jest przerwa to po prostu dodaj za nia
                    jobs_done.append(i)
                    m2_ending.append(m2_ending[-1] + m2.jobs[i])
                    lower_bound += m2_ending[-1]
            else:            #jesli nie ma zadnej przerwy
                space = tau - m2_ending[-1]
                if space > m2.jobs[i]:
                    jobs_done.append(i)
                    m2_ending.append(m2_ending[-1] + m2.jobs[i])
                    lower_bound += m2_ending[-1]
                elif space == m2.jobs[i]:
                    jobs_done.append(i)
                    m2_ending.append(m2_ending[-1] + m2.jobs[i])
                    lower_bound += m2_ending[-1]
                    breaks.append(m2_ending[-1] + t)
                    m2_ending.append(m2_ending[-1] + t)
                elif space < m2.jobs[i]:
                    jobs_done.append(i)
                    breaks.append(m2_ending[-1] + space + t)
                    m2_ending.append(m2_ending[-1] + space + t + m2.jobs[i] - space)
                    lower_bound += m2_ending[-1]
                else:
                    breaks.append(m2_ending[-1] + t)
                    m2_ending.append(m2_ending[-1] + t)
                    jobs_done.append(i)
                    m2_ending.append(m2_ending[-1] + m2.jobs[i])
                    lower_bound += m2_ending[-1]
    return lower_bound


def Flow_shop(jobs_list, cj, m1_endings, m1_endings_dict, m2_endings, breaks):
    global flag
    global solution_cj
    global solution_jobs
    node = Node(jobs_list, cj, m1_endings, m1_endings_dict, m2_endings, breaks)
    node.find_adjacjent(m1, m2, tau, t)
    if len(node.adjacjent) == 0:   #leaf
        flag = True
        if solution_cj is None or node.Cj < solution_cj:
            solution_cj = node.Cj
            solution_jobs = node.jobs
    else:
        for adj in node.adjacjent:
            lower_bound = count_bound(adj, node)
            if flag and lower_bound and lower_bound > solution_cj:
                continue
            jobs = list(node.jobs)
            jobs.append(adj)
            ends = list(node.m1_ending_times)
            ends_2 = list(node.m2_ending_tmes)
            ends_dict = node.m1_ending_times_dic.copy()
            if adj[0] != -2:
                ends.append(m1.jobs[adj[0]]) if len(ends) == 0 else ends.append(ends[-1] + m1.jobs[adj[0]])
                ends_dict[adj[0]] = ends[-1]
            breaks = list(node.break_times)
            if adj[0] == -2 and adj[1] != -1:
                cj = node.Cj + m2.jobs[adj[1]] + ends_2[-1]
                ends_2.append(ends[-1] + m2.jobs[adj[1]])
            if adj[1] == -1:
                breaks.append(ends_2[-1] + t) if len(ends_2) != 0 else breaks.append(ends[-1] + t)
                ends_2.append(ends_2[-1] + t) if len(ends_2) != 0 else ends_2.append(ends[-1] + t)   ####>???????
                cj = node.Cj + 1 if ends[-1] != ends_2[-1] else node.Cj
            if adj[0] != -2 and adj[1] != -1:
                cj = node.Cj + m2.jobs[adj[1]] + ends_dict[adj[1]]  #to co juz mam + dlugosc zadania + moment rozpocz
                ends_2.append(ends[-1] + m2.jobs[adj[1]]) if len(ends_2) == 0 else ends_2.append(ends_2[-1] + m2.jobs[adj[1]])
            Flow_shop(jobs, cj, ends, ends_dict, ends_2, breaks)


def main():
    Flow_shop([], 0, [], {}, [], [])  #root
    print(solution_jobs)
    print(solution_cj)

flag = False
solution_cj = None
solution_jobs = None
tau = 3
t = 1

# m1 = Machine([2,1,3,1,3,3,1])
# m2 = Machine([1,2,2,2,1,2,1])
# m1 = Machine([2,1,1,2])
# m2 = Machine([3,2,1,1])

# m1 = Machine([2,1,1])
# m2 = Machine([3,2,1])


# m1 = Machine([2,1,3,1,3,3,1])
# m2 = Machine([1,2,2,2,1,2,2])

m1 = Machine([2,1,3])
m2 = Machine([1,2,2])
main()








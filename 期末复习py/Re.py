INDEX = 0
re = '(aa|bb)*(a|b)(aa|bb)*$'
TOKEN = re[0]

# 动作集合
ACTIONS = set()

CURRENT_STATE = 0
# NFA状态转换表，映射套映射map<int, map<char, set<int>>>
NTT: dict[int, dict[str, set[int]]] = {}

# DFA开始状态默认为0
# DFA终止状态集合
DES = set()
# DFA状态转换表，映射套映射map<int, map<char, int>>
DTT: dict[int, dict[str, int]] = {}

# MFA开始状态
MSS = 0
# MFA终止状态集合
MES = set()
# MFA状态转换表，映射套映射map<int, map<char, int>>
MTT: dict[int, dict[str, int]] = {}


def match(token):
    global TOKEN, INDEX
    if TOKEN == token:
        INDEX = INDEX + 1
        TOKEN = re[INDEX]
    else:
        raise Exception("未匹配的符号" + TOKEN)


def create_nfa():
    global CURRENT_STATE
    ret = [CURRENT_STATE, CURRENT_STATE + 1]
    CURRENT_STATE += 2
    return ret


def connect(a, token, b):
    if a not in NTT:
        NTT[a] = {}
    if token not in NTT[a]:
        NTT[a][token] = set()
    NTT[a][token].add(b)


def expression():
    t1 = term()
    while TOKEN == '|':
        match('|')
        t2 = term()
        t = create_nfa()
        connect(t[0], 'ε', t1[0])
        connect(t[0], 'ε', t2[0])
        connect(t1[1], 'ε', t[1])
        connect(t2[1], 'ε', t[1])
        t1 = t
    return t1


def term():
    f1 = factor()
    while TOKEN == '(' or str.islower(TOKEN):
        f2 = factor()
        connect(f1[1], 'ε', f2[0])
        f1[1] = f2[1]
    return f1


def factor():
    p = part()
    if TOKEN == '*' or TOKEN == '+' or TOKEN == '?':
        if TOKEN == '*':
            connect(p[0], 'ε', p[1])
            connect(p[1], 'ε', p[0])
        elif TOKEN == '+':
            connect(p[1], 'ε', p[0])
        elif TOKEN == '?':
            connect(p[0], 'ε', p[1])
        match(TOKEN)
    return p


def part():
    if TOKEN == '(':
        match('(')
        e = expression()
        match(')')
    elif str.isalnum(TOKEN):
        e = create_nfa()
        connect(e[0], TOKEN, e[1])
        ACTIONS.add(TOKEN)
        match(TOKEN)
    else:
        raise Exception('未知的标识符' + TOKEN)
    return e


NFA = expression()


def closure(states):
    result = list(states)
    for state in result:
        if state in NTT and 'ε' in NTT[state]:
            for next_state in NTT[state]['ε']:
                if next_state not in result:
                    result.append(next_state)
    return set(result)


def move(states, ch):
    result = set()
    for state in states:
        if state in NTT and ch in NTT[state]:
            result |= NTT[state][ch]
    return closure(result)


def nfa_to_dfa():
    dfa_state_list = [closure({NFA[0]})]
    i = 0
    while i < len(dfa_state_list):
        if NFA[1] in dfa_state_list[i]:
            DES.add(i)
        DTT[i] = {}
        for action in ACTIONS:
            move_set = move(dfa_state_list[i], action)
            if move_set:
                if move_set not in dfa_state_list:
                    dfa_state_list.append(move_set)
                DTT[i][action] = dfa_state_list.index(move_set)
        i += 1


nfa_to_dfa()


def get_source_set(target_set, action):
    result = set()
    for state in DTT:
        if action in DTT[state] and DTT[state][action] in target_set:
            result.add(state)
    return result


def hopcroft():
    partition = [DES]
    dfa_non_term = DTT.keys() - DES
    if dfa_non_term:
        partition.append(dfa_non_term)
    stack = partition[:]
    while stack:
        cur = stack[-1]
        stack.pop()
        for action in ACTIONS:
            source_set = get_source_set(cur, action)
            new_partition = []
            for p in partition:
                s1 = p & source_set
                s2 = p - s1
                if s1 and s2:
                    new_partition.extend([s1, s2])
                    if p in stack:
                        stack.remove(p)
                        stack.extend([s1, s2])
                    elif len(s1) <= len(s2):
                        stack.append(s1)
                    else:
                        stack.append(s2)
                else:
                    new_partition.append(p)
            partition = new_partition
    return partition


def dfa_to_mfa():
    global MSS
    partition = hopcroft()
    for i in range(len(partition)):
        p = partition[i]
        if 0 in p:
            MSS = i
        if p <= DES:
            MES.add(i)
        MTT[i] = {}
        for action, to in DTT[list(p)[0]].items():
            for j in range(len(partition)):
                if to in partition[j]:
                    MTT[i][action] = j
                    break


dfa_to_mfa()


def mfa_to_code():
    code = f'''string GetToken(string str) {{
    int state = {MSS};
    for (const auto &ch : str) {{
        switch (state) {{'''
    for state in MTT:
        code += f'''
        case {state}:'''
        for action, to in MTT[state].items():
            code += f'''
            if (ch == {action}) {{ state = {to}; break; }}'''
        code += '''
            ERROR();'''
    code += f'''
        }}
    }}'''
    for state in MES:
        code += f'''
    if (state == {state}) return str;'''
    code += '''
    ERROR();
}'''
    return code


print(mfa_to_code())
